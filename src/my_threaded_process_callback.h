/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once
#include <memory>

class threaded_process_callback_foo_run_group : public threaded_process_callback
{
public:
	threaded_process_callback_foo_run_group(t_size service_index);
	void init();
	void add_input_files(metadb_handle_list_cref input_files);
	virtual void run(threaded_process_status & p_status, abort_callback & p_abort);
	virtual void on_done(HWND p_wnd, bool p_was_aborted);

private:
	std::shared_ptr<pfc::list_t<metadb_handle_list>>		m_input_files;
	t_size													m_next_job_index;
	t_size													m_service_index;
	t_size													m_max_progress_bar;
	t_size													m_cur_progress_bar;
	abort_callback											*m_abort;
	threaded_process_status									*m_status;
	pfc::array_t<HANDLE>									m_thread;
	critical_section										m_lock_next_jpb;
	critical_section										m_lock_progress;
	pfc::string8											m_popup_message;

	static unsigned __stdcall thread_proc(void *p);
};