/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "stdafx.h"

#include "foo_run_group.h"
#include "config.h"
#include "my_threaded_process_callback.h"


unsigned contextmenu_item_foo_run_group::get_num_items()
{
	return 1;
}

contextmenu_item_node_root * contextmenu_item_foo_run_group::instantiate_item(unsigned p_index, metadb_handle_list_cref p_data, const GUID & p_caller)
{
	return new contextmenu_item_node_root_popup_foo_run_group(p_index);
}

GUID contextmenu_item_foo_run_group::get_item_guid(unsigned p_index)
{
	return item_guid[p_index];
}

void contextmenu_item_foo_run_group::get_item_name(unsigned p_index, pfc::string_base & p_out)
{
	p_out = item_name[p_index];
}

void contextmenu_item_foo_run_group::get_item_default_path(unsigned p_index, pfc::string_base & p_out)
{
	p_out = "";
}

bool contextmenu_item_foo_run_group::get_item_description(unsigned p_index, pfc::string_base & p_out)
{
	p_out = item_name[p_index];
	return true;
}

contextmenu_item::t_enabled_state contextmenu_item_foo_run_group::get_enabled_state(unsigned p_index)
{
	return contextmenu_item::DEFAULT_ON;
}

void contextmenu_item_foo_run_group::item_execute_simple(unsigned p_index, const GUID & p_node, metadb_handle_list_cref p_data, const GUID & p_caller)
{
	//This function is called from toolbar button
	if (p_node == pfc::guid_null)
		return;

	for (t_size i = 0, count = cfg_service_properties.get_count(); i < count; i++)
	{
		if (p_node == cfg_service_properties[i].service_guid)
			g_context_command(static_cast<unsigned int>(i), p_data, p_caller);
	}
}

double contextmenu_item_foo_run_group::get_sort_priority()
{
	return 0;
}
GUID contextmenu_item_foo_run_group::get_parent()
{
	return contextmenu_groups::root;
}

void contextmenu_item_foo_run_group::g_context_command(unsigned p_index, metadb_handle_list_cref p_data, const GUID& p_caller)
{
	if (!p_data.get_count())
	{
		popup_message::g_show("No input items.\n", "Run services per group");
		return;
	}

	metadb_handle_list input_files = p_data;
	input_files.remove_duplicates();

	service_ptr_t<titleformat_object> p_script;
	pfc::string8 sort_format(cfg_service_properties[p_index].album_grouping_pattern);
	sort_format += "|%path_sort%";
	if (!static_api_ptr_t<titleformat_compiler>()->compile(p_script, sort_format))
	{
		popup_message::g_show("Titleformat compile error!\n", "Run services per group");
		return;
	}

	input_files.sort_by_format(p_script, NULL);

	sort_format.truncate(sort_format.length() - strlen("|%path_sort%"));
	if (!static_api_ptr_t<titleformat_compiler>()->compile(p_script, sort_format))
	{
		popup_message::g_show("Titleformat compile error!\n", "Run services per group");
		return;
	}

	//Filter
	bool apply_filter = !cfg_service_properties[p_index].filter.is_empty();
	pfc::array_t<bool> filter_result;
	if (apply_filter)
	{
		try
		{
			static_api_ptr_t<search_filter_manager> sfm;
			search_filter::ptr sf = sfm->create(cfg_service_properties[p_index].filter);
			filter_result.set_size(input_files.get_count());
			sf->test_multi(input_files, filter_result.get_ptr());
		}
		catch (...)
		{
			apply_filter = false;
			console::complain("Run services per group", "Failed to apply filter!");
		}
	}


	pfc::string8_fast current_album, temp_album;
	metadb_handle_list list;
	service_ptr_t<threaded_process_callback_foo_run_group> p_callback = new service_impl_t<threaded_process_callback_foo_run_group>(p_index);
	p_callback->init();

	for (t_size i = 0, j = input_files.get_count(); i < j; i++)
	{
		metadb_handle_ptr ptr = input_files[i];
		if (!ptr->format_title(NULL, temp_album, p_script, NULL)) continue;
		if (stricmp_utf8(current_album, temp_album))
		{
			if (list.get_count()) p_callback->add_input_files(list);
			list.remove_all();
			current_album = temp_album;
		}
		if (!apply_filter || apply_filter && filter_result[i])
			list.add_item(ptr);
	}
	if (list.get_count()) p_callback->add_input_files(list);

	//Set Current Directory
	if (fb2k_path.is_empty())
	{
		uGetModuleFileName(NULL, fb2k_path);
		fb2k_path.truncate(fb2k_path.scan_filename());
	}
	uSetCurrentDirectory(fb2k_path.get_ptr());
	
	threaded_process::g_run_modeless(p_callback, threaded_process::flag_show_abort | threaded_process::flag_show_progress, core_api::get_main_window(), "Run services per group");
}

void contextmenu_item_foo_run_group::g_get_item_name(unsigned p_index, pfc::string_base & p_out)
{
	p_out = item_name[p_index];
}

const char* const contextmenu_item_foo_run_group::item_name[] = { "Run services x group" };
// {E88A49E2-9392-4B12-9B53-3924B9A56990}
const GUID contextmenu_item_foo_run_group::item_guid[] = { 0xe88a49e2, 0x9392, 0x4b12, { 0x9b, 0x53, 0x39, 0x24, 0xb9, 0xa5, 0x69, 0x90 } }; //1.0.5

pfc::string8 contextmenu_item_foo_run_group::fb2k_path("");

//Root Popup
contextmenu_item_node_root_popup_foo_run_group::contextmenu_item_node_root_popup_foo_run_group(unsigned p_index) : m_index(p_index)
{ }

bool contextmenu_item_node_root_popup_foo_run_group::get_display_data(pfc::string_base & p_out, unsigned & p_displayflags, metadb_handle_list_cref p_data, const GUID & p_caller)
{
	if (!cfg_service_properties.get_count())
		return false;

	contextmenu_item_foo_run_group::g_get_item_name(m_index, p_out);
	return true;
}

t_size contextmenu_item_node_root_popup_foo_run_group::get_children_count()
{
	return cfg_service_properties.get_count();
}

contextmenu_item_node * contextmenu_item_node_root_popup_foo_run_group::get_child(t_size p_index)
{
	return new contextmenu_item_node_leaf_foo_run_group(static_cast<unsigned int>(p_index));
}

GUID contextmenu_item_node_root_popup_foo_run_group::get_guid()
{
	return pfc::guid_null;
}

bool contextmenu_item_node_root_popup_foo_run_group::is_mappable_shortcut()
{
	return false;
}


//Leaf
contextmenu_item_node_leaf_foo_run_group::contextmenu_item_node_leaf_foo_run_group(unsigned p_index) : m_index(p_index)
{ }

bool contextmenu_item_node_leaf_foo_run_group::get_display_data(pfc::string_base & p_out, unsigned & p_displayflags, metadb_handle_list_cref p_data, const GUID & p_caller)
{
	p_displayflags = 0;
	p_out = cfg_service_properties[m_index].label;
	return true;
}

bool contextmenu_item_node_leaf_foo_run_group::get_description(pfc::string_base & p_out)
{
	p_out = "Runs ";
	p_out += cfg_service_properties[m_index].label;
	p_out += " per group";
	return true;
}

void contextmenu_item_node_leaf_foo_run_group::execute(metadb_handle_list_cref p_data, const GUID & p_caller)
{
	contextmenu_item_foo_run_group::g_context_command(m_index, p_data, p_caller);
}

GUID contextmenu_item_node_leaf_foo_run_group::get_guid()
{
	return cfg_service_properties[m_index].service_guid;
}

bool contextmenu_item_node_leaf_foo_run_group::is_mappable_shortcut()
{
	return true;
}


//factory
static contextmenu_item_factory_t<contextmenu_item_foo_run_group> g_contextmenu_item_foo_run_group_factory;
