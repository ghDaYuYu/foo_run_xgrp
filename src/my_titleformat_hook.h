/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

class title_format_hook_foo_run_group : public titleformat_hook
{
public:
	title_format_hook_foo_run_group(metadb_handle_list_cref p_data, t_size group_index);
	virtual bool process_field(titleformat_text_out * p_out, const char * p_name, t_size p_name_length, bool & p_found_flag);
	virtual bool process_function(titleformat_text_out * p_out, const char * p_name, t_size p_name_length, titleformat_hook_function_params * p_params, bool & p_found_flag);

private:
	metadb_handle_list_cref m_metadb_handle_list;
	pfc::string8 m_path_group;
	t_size m_group_index;
};