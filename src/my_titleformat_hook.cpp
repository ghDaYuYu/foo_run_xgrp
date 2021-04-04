/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "../SDK/foobar2000.h"
#include "my_titleformat_hook.h"

title_format_hook_foo_run_group::title_format_hook_foo_run_group(metadb_handle_list_cref p_data, t_size group_index) :
m_metadb_handle_list(p_data), 
m_path_group(""), 
m_group_index(group_index)
{}

bool title_format_hook_foo_run_group::process_field(titleformat_text_out * p_out, const char * p_name, t_size p_name_length, bool & p_found_flag)
{
	p_found_flag = false;
	if (!stricmp_utf8(p_name, "path_group"))
	{
		if (m_metadb_handle_list.get_count() < 1)
			return true;

		if (m_path_group.is_empty())
		{
			for (t_size i = 0, max = m_metadb_handle_list.get_count(); i < max; i++)
			{
				const char *path = m_metadb_handle_list[i]->get_path();
				if (!path) continue;
				pfc::string8 display_path;
				filesystem::g_get_display_path(path, display_path);
				m_path_group += "\"";
				m_path_group += display_path;
				m_path_group += "\" ";
			}
			m_path_group.truncate(m_path_group.length() - 1);	//cut last space
		}

		if (!m_path_group.is_empty())
		{
			p_found_flag = true;
			p_out->write(titleformat_inputtypes::unknown, m_path_group);
		}
	}
	else if (!stricmp_utf8(p_name, "group_index"))
	{
		p_found_flag = true;
		p_out->write_int(titleformat_inputtypes::unknown, m_group_index);
	}
	else
		return false;

	return true;
}

bool title_format_hook_foo_run_group::process_function(titleformat_text_out * p_out, const char * p_name, t_size p_name_length, titleformat_hook_function_params * p_params, bool & p_found_flag)
{
	return false;
}