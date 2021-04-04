/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

class contextmenu_item_foo_run_group : public contextmenu_item_v2
{
public:
	virtual unsigned get_num_items();
	virtual contextmenu_item_node_root * instantiate_item(unsigned p_index, metadb_handle_list_cref p_data, const GUID & p_caller);
	virtual GUID get_item_guid(unsigned p_index);
	virtual void get_item_name(unsigned p_index, pfc::string_base & p_out);
	virtual void get_item_default_path(unsigned p_index, pfc::string_base & p_out);
	virtual bool get_item_description(unsigned p_index, pfc::string_base & p_out);
	virtual t_enabled_state get_enabled_state(unsigned p_index);
	virtual void item_execute_simple(unsigned p_index, const GUID & p_node, metadb_handle_list_cref p_data, const GUID & p_caller);
	virtual double get_sort_priority();
	virtual GUID get_parent();
	
	static void g_context_command(unsigned p_index, metadb_handle_list_cref p_data, const GUID& p_caller);
	static void g_get_item_name(unsigned p_index, pfc::string_base & p_out);
private:
	static const char* const item_name[];
	static const GUID item_guid[];
	static pfc::string8 fb2k_path;
};

class contextmenu_item_node_root_popup_foo_run_group : public contextmenu_item_node_root_popup
{
private:
	unsigned m_index;
public:
	contextmenu_item_node_root_popup_foo_run_group(unsigned p_index);
	bool get_display_data(pfc::string_base & p_out, unsigned & p_displayflags, metadb_handle_list_cref p_data, const GUID & p_caller);
	t_size get_children_count();
	contextmenu_item_node * get_child(t_size p_index);
	GUID get_guid();
	bool is_mappable_shortcut();
};

class contextmenu_item_node_leaf_foo_run_group : public contextmenu_item_node_leaf
{
private:
	unsigned m_index;
public:
	contextmenu_item_node_leaf_foo_run_group(unsigned p_index);
	bool get_display_data(pfc::string_base & p_out, unsigned & p_displayflags, metadb_handle_list_cref p_data, const GUID & p_caller);
	void execute(metadb_handle_list_cref p_data, const GUID & p_caller);
	bool get_description(pfc::string_base & p_out);
	GUID get_guid();
	bool is_mappable_shortcut();
};