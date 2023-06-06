/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "stdafx.h"

#include "my_cfg_var.h"

//struct service_properties
const GUID			service_properties::default_service_guid = pfc::guid_null;
const char* const	service_properties::default_label = "New Service";
const char* const	service_properties::default_path = "";
const t_int32		service_properties::default_minimize_window = 0;
const t_int32		service_properties::default_wait_process = 0;
const t_uint32		service_properties::default_thread_count = 1;
const char * const	service_properties::default_album_grouping_pattern = "%album artist%|%date%|%album%";
const char * const	service_properties::default_filter = "";
//from version 2
const t_int32		service_properties::default_reload_info = 0;

const t_uint32		service_properties::max_thread_count = 32;
const t_uint32		service_properties::version = 2;	//since ver 1.03


//cfg_service_properties class
void cfg_service_properties_list::get_data_raw(stream_writer * p_stream, abort_callback & p_abort)
{
	stream_writer_formatter<> out(*p_stream, p_abort);
	out << pfc::downcast_guarded<t_uint32>(get_size());
	out << service_properties::version;

	for (t_size walk = 0; walk < get_size(); ++walk)
	{
		const service_properties & current = (*this)[walk];
		out << current.service_guid;
		out << current.label;
		out << current.path;
		out << current.minimize_window;
		out << current.wait_process;
		out << current.thread_count;
		out << current.album_grouping_pattern;
		out << current.filter;
		out << current.reload_info;
	}
}

void cfg_service_properties_list::set_data_raw(stream_reader * p_stream, t_size p_sizehint, abort_callback & p_abort)
{
	try 
	{
		stream_reader_formatter<> in(*p_stream, p_abort);
		t_uint32 count; in >> count;
		t_uint32 version; in >> version;
		set_count(count);

		for (t_uint32 walk = 0; walk < count; ++walk)
		{
			service_properties & current = (*this)[walk];
			in >> current.service_guid;
			in >> current.label;
			in >> current.path;
			in >> current.minimize_window;
			in >> current.wait_process;
			in >> current.thread_count;
			in >> current.album_grouping_pattern;
			in >> current.filter;

			//メンバーを変更したときにはversionをチェック cfgに保存されていなければdefaultの値をセットする
			if (version >= 2)
				in >> current.reload_info;
			else
				current.reload_info = service_properties::default_reload_info;
		}
	}
	catch (...) 
	{
		remove_all();
		throw;
	}
}