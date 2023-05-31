/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "stdafx.h"

#include <process.h>

#include "my_threaded_process_callback.h"
#include "my_titleformat_hook.h"
#include "config.h"

threaded_process_callback_foo_run_group::threaded_process_callback_foo_run_group(t_size service_index) :
m_service_index(service_index),
m_next_job_index(0), 
m_abort(0), 
m_status(0), 
m_cur_progress_bar(0),
m_max_progress_bar(0)
{}

void threaded_process_callback_foo_run_group::init()
{
	m_input_files.reset(new pfc::list_t<metadb_handle_list>);
}

void threaded_process_callback_foo_run_group::add_input_files(metadb_handle_list_cref input_files)
{
	m_input_files->add_item(input_files);
}

void threaded_process_callback_foo_run_group::run(threaded_process_status & p_status, abort_callback & p_abort)
{
	if (cfg_service_properties[m_service_index].path.is_empty())
	{
		m_popup_message += "Commandline is empty.\n";
		return;
	}

	if (!m_input_files->get_count())
	{
		m_popup_message += "No input items.\n";
		return;
	}

	m_next_job_index = 0;
	m_cur_progress_bar = 0;
	m_max_progress_bar = m_input_files->get_count();
	m_abort = &p_abort;
	m_status = &p_status;

	t_uint32 thread_count = cfg_service_properties[m_service_index].thread_count;
	if (thread_count == 0)
		thread_count = pfc::getOptimalWorkerThreadCountEx(service_properties::max_thread_count);
	
	try {
		m_thread.set_size(thread_count);
		for (t_size i = 0; i < thread_count; i++) m_thread[i] = NULL;

		for (t_size i = 0; i < thread_count; i++)
		{
			m_thread[i] = (HANDLE)_beginthreadex(NULL, 0, thread_proc, (void*)this, 0, NULL);
			if (NULL == m_thread[i])
				throw std::runtime_error("_beginthreadex error!\n");
		}
	}
	catch (const std::exception & e)	{
		try	{
			m_popup_message += e.what();
		}
		catch (...)	{}
	}
	catch (...)	{

	}

	//FINALLY
	for (t_size i = 0; i < thread_count; i++)
	{
		if (NULL != m_thread[i])
		{
			WaitForSingleObject(m_thread[i], INFINITE);
			CloseHandle(m_thread[i]);
			m_thread[i] = NULL;
		}
	}
}

//main thread callback
class main_thread_callback_foo_run_group : public main_thread_callback
{
private:
	std::shared_ptr<pfc::list_t<metadb_handle_list>> m_input_files;
public:
	main_thread_callback_foo_run_group(const std::shared_ptr<pfc::list_t<metadb_handle_list>> & input_files) : m_input_files(input_files)
	{}

	virtual void callback_run()
	{
		static_api_ptr_t<metadb_io_v2> mio_v2;
		for (t_size i = 0; i < m_input_files->get_count(); i++)
		{
			mio_v2->load_info_async(m_input_files->get_item(i), metadb_io::load_info_force, core_api::get_main_window(), metadb_io_v2::op_flag_background, 0);
			//mio_v2->dispatch_refresh(m_input_files->get_item(i));
		}
	}
};

void threaded_process_callback_foo_run_group::on_done(HWND p_wnd, bool p_was_aborted)
{
	if (cfg_service_properties[m_service_index].reload_info && cfg_service_properties[m_service_index].wait_process)
	{
		//Legal to call from main thread only
		main_thread_callback_spawn<main_thread_callback_foo_run_group, std::shared_ptr<pfc::list_t<metadb_handle_list>>>(m_input_files);
	}

	if (!m_popup_message.is_empty())
		popup_message::g_show(m_popup_message, "Run services per group");
}

unsigned __stdcall threaded_process_callback_foo_run_group::thread_proc(void *p)
{
	unsigned exit_code = 0;
	threaded_process_callback_foo_run_group *this_ = reinterpret_cast<threaded_process_callback_foo_run_group*>(p);

	try	{
		service_ptr_t<titleformat_object> p_script;
		if (!static_api_ptr_t<titleformat_compiler>()->compile(p_script, cfg_service_properties[this_->m_service_index].path))
			throw std::runtime_error("Titleformat compile error!");

		while (true)
		{
			t_size cur_job_index = 0;
			{
				insync(this_->m_lock_next_jpb);
				cur_job_index = this_->m_next_job_index;
				if (cur_job_index >= this_->m_input_files->get_count())
					break;

				this_->m_next_job_index++;
			}

			this_->m_abort->check();
			metadb_handle_list_cref handle_list = this_->m_input_files->get_item(cur_job_index);
			
			pfc::string8 commandline;
			if (!handle_list[0]->format_title(&title_format_hook_foo_run_group(handle_list, cur_job_index), commandline, p_script, NULL))
				throw std::runtime_error("Titleformat compile error!");

			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			memset(&si, 0, sizeof(STARTUPINFO));
			memset(&pi, 0, sizeof(PROCESS_INFORMATION));

			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = cfg_service_properties[this_->m_service_index].minimize_window ? SW_MINIMIZE : SW_SHOW;
			pfc::stringcvt::string_os_from_utf8 commandline_converted(commandline);
			pfc::array_t<TCHAR> commandline_mod; commandline_mod.set_size(commandline_converted.length() + 1);
			_tcscpy_s(commandline_mod.get_ptr(), commandline_mod.get_count(), commandline_converted.get_ptr());

			if (!CreateProcess(
				NULL,
				commandline_mod.get_ptr(),
				NULL,
				NULL,
				FALSE,
				NORMAL_PRIORITY_CLASS,
				NULL,
				NULL,
				&si,
				&pi
				))
			{
				this_->m_popup_message = "Failed to execute service!\n";
				this_->m_popup_message += "Commandline: "; this_->m_popup_message += commandline; this_->m_popup_message += "\n";
				throw  std::logic_error("");
			}

			if (cfg_service_properties[this_->m_service_index].wait_process)
			{
				WaitForSingleObject(pi.hProcess, INFINITE);
				DWORD exit_code = 0;
				GetExitCodeProcess(pi.hProcess, &exit_code);
				if (exit_code && exit_code != STILL_ACTIVE)
				{
					try	{
						this_->m_popup_message += cfg_service_properties[this_->m_service_index].label;
						this_->m_popup_message += " returned error code.\n";
						this_->m_popup_message += "Commandline: "; this_->m_popup_message += commandline; this_->m_popup_message += "\n";
					}
					catch (...)	{

					}
				}
			}
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);

			//update progress bar
			insync(this_->m_lock_progress);
			this_->m_cur_progress_bar++;
			this_->m_status->set_progress(this_->m_cur_progress_bar, this_->m_max_progress_bar);
		}
	}
	catch (const foobar2000_io::exception_aborted /*& e*/)	{

	}
	catch (const std::exception & e)	{
		try	{
			this_->m_popup_message += e.what();
		}
		catch (...)	{}
	}
	catch (...)	{
		exit_code = 1;
	}

	return exit_code;
}
