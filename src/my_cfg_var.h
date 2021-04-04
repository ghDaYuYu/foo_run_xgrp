/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

struct service_properties
{
	service_properties() :
		service_guid(default_service_guid),
		label(default_label),
		path(default_path),
		minimize_window(default_minimize_window),
		wait_process(default_wait_process),
		thread_count(default_thread_count),
		album_grouping_pattern(default_album_grouping_pattern),
		filter(default_filter), 
		reload_info(default_reload_info)
	{}

	service_properties(GUID new_service_guid) :
		service_guid(new_service_guid),
		label(default_label),
		path(default_path),
		minimize_window(default_minimize_window),
		wait_process(default_wait_process),
		thread_count(default_thread_count),
		album_grouping_pattern(default_album_grouping_pattern), 
		filter(default_filter), 
		reload_info(default_reload_info)
	{}

	//メンバー
	GUID service_guid;
	pfc::string8 label;
	pfc::string8 path;
	t_int32 minimize_window;
	t_int32 wait_process;
	t_uint32 thread_count;
	pfc::string8 album_grouping_pattern;
	pfc::string8 filter;
	//from version 2
	t_int32 reload_info;

	//デフォルト値など
	static const GUID default_service_guid;
	static const char * const default_label;
	static const char* const default_path;
	static const t_int32 default_minimize_window;
	static const t_int32 default_wait_process;
	static const t_uint32 default_thread_count;
	static const char * const default_album_grouping_pattern;
	static const char * const default_filter;
	static const t_uint32 max_thread_count;
	//from version 2
	static const t_int32 default_reload_info;

	//メンバーを変更したときに合わせて変更する
	static const t_uint32 version;
};

class cfg_service_properties_list : public cfg_var, public pfc::list_t<service_properties>
{
public:
	//メンバーを変更したときに合わせて変更する
	explicit cfg_service_properties_list(const GUID& guid) : cfg_var(guid) {};
	void get_data_raw(stream_writer * p_stream, abort_callback & p_abort);
	void set_data_raw(stream_reader * p_stream, t_size p_sizehint, abort_callback & p_abort);
};