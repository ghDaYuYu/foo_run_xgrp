/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "../SDK/foobar2000.h"
#include "../helpers/helpers.h"
#include "resource.h"
#include "config.h"


// {9A435278-1E18-4730-BD04-A5400588987D}
static const GUID guid_preferences_page_foo_run_group =
{ 0x9a435278, 0x1e18, 0x4730, { 0xbd, 0x4, 0xa5, 0x40, 0x5, 0x88, 0x98, 0x7d } };
// {0C1F8C9C-2679-409e-928F-A80D35D89A93}
static const GUID guid_cfg_service_properties =
{ 0xc1f8c9c, 0x2679, 0x409e, { 0x92, 0x8f, 0xa8, 0xd, 0x35, 0xd8, 0x9a, 0x93 } };

cfg_service_properties_list cfg_service_properties(guid_cfg_service_properties);


class preferences_page_foo_run_group : public preferences_page_v2
{
public:
	static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	virtual HWND create(HWND parent)
	{
		return uCreateDialog(IDD_DIALOG1, parent, dialog_proc);
	}

	virtual const char * get_name()
	{
		return "Run services per group";
	}

	virtual GUID get_guid()
	{
		return guid_preferences_page_foo_run_group;
	}

	virtual GUID get_parent_guid()
	{
		return preferences_page::guid_tools;
	}

	//! Queries whether this page supports "reset page" feature.
	virtual bool reset_query()
	{
		return false;
	}

	virtual void reset()
	{
	}

	virtual bool get_help_url(pfc::string_base & p_out)
	{
		return false;
	}

private:
	static inline void add_new_service();
	static inline void remove_service(t_size index);
	static inline void swap_service(t_size index1, t_size index2);
	static void init_service_properties(HWND hwnd, t_size index);
	static void reset_service_properties(HWND hwnd);
	static void enable_move_button(HWND hwnd);
	static int cur_service_index;
};
int preferences_page_foo_run_group::cur_service_index = -1;


void preferences_page_foo_run_group::add_new_service()
{
	GUID new_guid;
	if (S_OK != CoCreateGuid(&new_guid))
	{
		console::complain("Run services per group", "Failed to create guid for new service!");
		new_guid = pfc::guid_null;
	}
	cfg_service_properties.add_item(service_properties(new_guid));
}

void preferences_page_foo_run_group::remove_service(t_size index)
{
	cfg_service_properties.remove_by_idx(index);
}

void preferences_page_foo_run_group::swap_service(t_size index1, t_size index2)
{
	cfg_service_properties.swap_items(index1, index2);
}

void preferences_page_foo_run_group::init_service_properties(HWND hwnd, t_size index)
{
	service_properties & current = cfg_service_properties[index];
	uSendDlgItemMessageText(hwnd, IDC_LABEL, WM_SETTEXT, 0, current.label);
	uSendDlgItemMessageText(hwnd, IDC_PATH, WM_SETTEXT, 0, current.path);
	CheckDlgButton(hwnd, IDC_MINIMIZE_WINDOW, (UINT)current.minimize_window);
	CheckDlgButton(hwnd, IDC_WAIT_PROCESS, (UINT)current.wait_process);
	uSendDlgItemMessage(hwnd, IDC_THREAD_COUNT_SPIN, UDM_SETPOS, 0, (LPARAM)MAKELONG(current.thread_count, 0));
	uSendDlgItemMessageText(hwnd, IDC_ALBUM_GROUPING_PATTERN, WM_SETTEXT, 0, current.album_grouping_pattern);
	uSendDlgItemMessageText(hwnd, IDC_FILTER, WM_SETTEXT, 0, current.filter);
	CheckDlgButton(hwnd, IDC_RELOAD_INFO, (UINT)current.reload_info);
	//BOOL ctrl_enable = current.wait_process ? TRUE : FALSE;
	//EnableWindow(GetDlgItem(hwnd, IDC_THREAD_COUNT_SPIN), ctrl_enable);
	EnableWindow(GetDlgItem(hwnd, IDC_RELOAD_INFO), current.wait_process ? TRUE : FALSE);
	if (current.wait_process)
	{
		//EnableWindow(GetDlgItem(hwnd, IDC_THREAD_COUNT_SPIN), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_RELOAD_INFO), TRUE);
	}
	else
	{
		////EnableWindow(GetDlgItem(hwnd, IDC_THREAD_COUNT_SPIN), FALSE);
		current.reload_info = 0;
		CheckDlgButton(hwnd, IDC_RELOAD_INFO, BST_UNCHECKED);
		EnableWindow(GetDlgItem(hwnd, IDC_RELOAD_INFO), FALSE);
	}
	
}

void preferences_page_foo_run_group::reset_service_properties(HWND hwnd)
{
	uSendDlgItemMessageText(hwnd, IDC_LABEL, WM_SETTEXT, 0, service_properties::default_label);
	uSendDlgItemMessageText(hwnd, IDC_PATH, WM_SETTEXT, 0, service_properties::default_path);
	CheckDlgButton(hwnd, IDC_MINIMIZE_WINDOW, (UINT)service_properties::default_minimize_window);
	CheckDlgButton(hwnd, IDC_WAIT_PROCESS, (UINT)service_properties::default_wait_process);
	uSendDlgItemMessage(hwnd, IDC_THREAD_COUNT_SPIN, UDM_SETPOS, 0, (LPARAM)MAKELONG(service_properties::default_thread_count, 0));
	uSendDlgItemMessageText(hwnd, IDC_ALBUM_GROUPING_PATTERN, WM_SETTEXT, 0, service_properties::default_album_grouping_pattern);
	uSendDlgItemMessageText(hwnd, IDC_FILTER, WM_SETTEXT, 0, service_properties::default_filter);
	CheckDlgButton(hwnd, IDC_RELOAD_INFO, (UINT)service_properties::default_reload_info);
	if (service_properties::default_wait_process)
	{
		//EnableWindow(GetDlgItem(hwnd, IDC_THREAD_COUNT_SPIN), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_RELOAD_INFO), TRUE);
	}
	else
	{
		////EnableWindow(GetDlgItem(hwnd, IDC_THREAD_COUNT_SPIN), FALSE);
		CheckDlgButton(hwnd, IDC_RELOAD_INFO, BST_UNCHECKED);
		EnableWindow(GetDlgItem(hwnd, IDC_RELOAD_INFO), FALSE);
	}
}

void preferences_page_foo_run_group::enable_move_button(HWND hwnd)
{
	if (cur_service_index == -1 || cfg_service_properties.get_count() < 2)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_MOVE_UP_SERVICE), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_MOVE_DOWN_SERVICE), FALSE);
		return;
	}

	if (cur_service_index == 0)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_MOVE_UP_SERVICE), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_MOVE_DOWN_SERVICE), TRUE);
	}
	else if (cur_service_index + 1 == cfg_service_properties.get_count())
	{
		EnableWindow(GetDlgItem(hwnd, IDC_MOVE_UP_SERVICE), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_MOVE_DOWN_SERVICE), FALSE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd, IDC_MOVE_UP_SERVICE), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_MOVE_DOWN_SERVICE), TRUE);
	}
}

BOOL CALLBACK preferences_page_foo_run_group::dialog_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		cur_service_index = -1;

		//spin control
		uSendDlgItemMessage(hwnd, IDC_THREAD_COUNT_SPIN, UDM_SETRANGE32, (WPARAM)0, (LPARAM)service_properties::max_thread_count);
		//EnableWindow(GetDlgItem(hwnd, IDC_THREAD_COUNT_SPIN), FALSE);

		//listview
		HWND h = uGetDlgItem(hwnd, IDC_SERVICES_LIST);

		DWORD style = ListView_GetExtendedListViewStyle(h);
		style |= LVS_EX_FULLROWSELECT;
		ListView_SetExtendedListViewStyle(h, style);

		listview_helper::insert_column(h, 0, "Services", 245);
		for (t_size i = 0, max = cfg_service_properties.get_count(); i < max; i++)
		{
			listview_helper::insert_item(h, i, cfg_service_properties[i].label, 0);
		}

		// ŠÔˆá‚¦‚Ä•ÒW‚µ‚»‚¤‚É‚È‚é‚±‚Æ‚ª‚ ‚Á‚½‚Ì‚Å
		//if (ListView_GetItemCount(h) > 0)
			//listview_helper::select_single_item(h, 0);
		//else
		{
			EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_SERVICE), FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_MOVE_UP_SERVICE), FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_MOVE_DOWN_SERVICE), FALSE);
		}
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wp))
		{
		case IDC_NEW_SERVICE:
		{
			if (HIWORD(wp) == BN_CLICKED)
			{
				add_new_service();
				
				HWND h = uGetDlgItem(hwnd, IDC_SERVICES_LIST);
				t_size index = cfg_service_properties.get_count() - 1;
				listview_helper::insert_item(h, index, cfg_service_properties[index].label, 0);
				listview_helper::select_single_item(h, index);
				//EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_SERVICE), TRUE);
			}
			break;
		}
		case IDC_REMOVE_SERVICE:
		{
			if (HIWORD(wp) == BN_CLICKED)
			{
				if (-1 != cur_service_index)
				{
					int index = cur_service_index;
					HWND h = uGetDlgItem(hwnd, IDC_SERVICES_LIST);
					ListView_DeleteItem(h, index);
					remove_service(index);

					if (cfg_service_properties.get_count())
					{
						if (index == cfg_service_properties.get_count())
							index--;
						listview_helper::select_single_item(h, index);
					}
					else
						EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_SERVICE), FALSE);
				}
			}
			break;
		}
		case IDC_MOVE_UP_SERVICE:
		{
			if (HIWORD(wp) == BN_CLICKED)
			{
				if (cur_service_index > 0)
				{
					int index = cur_service_index;
					swap_service(index, index - 1);

					HWND h = uGetDlgItem(hwnd, IDC_SERVICES_LIST);
					listview_helper::set_item_text(h, index - 1, 0, cfg_service_properties[index - 1].label);
					listview_helper::set_item_text(h, index, 0, cfg_service_properties[index].label);

					index--;
					listview_helper::select_single_item(h, index);
				}
			}
			break;
		}
		case IDC_MOVE_DOWN_SERVICE:
		{
			if (HIWORD(wp) == BN_CLICKED)
			{
				if (-1 != cur_service_index)
				{
					int index = cur_service_index;
					if (cfg_service_properties.get_count() < 2 || cfg_service_properties.get_count() == index + 1)
						break;
						
					swap_service(index, index + 1);
					HWND h = uGetDlgItem(hwnd, IDC_SERVICES_LIST);
					listview_helper::set_item_text(h, index, 0, cfg_service_properties[index].label);
					listview_helper::set_item_text(h, index + 1, 0, cfg_service_properties[index + 1].label);

					index++;
					listview_helper::select_single_item(h, index);
				}
			}
			break;
		}
		case IDC_LABEL:
		{
			if (HIWORD(wp) == EN_UPDATE)
			{
				if (-1 != cur_service_index)
				{
					cfg_service_properties[cur_service_index].label = string_utf8_from_window(hwnd, IDC_LABEL);
					listview_helper::set_item_text(GetDlgItem(hwnd, IDC_SERVICES_LIST), cur_service_index, 0, cfg_service_properties[cur_service_index].label);
				}
			}
			break;
		}
		case IDC_PATH:
		{
			if (HIWORD(wp) == EN_UPDATE)
			{
				if (-1 != cur_service_index)
				{
					cfg_service_properties[cur_service_index].path = string_utf8_from_window(hwnd, IDC_PATH);
				}
			}
			break;
		}
		case IDC_BROWSE_PATH:
		{
			if (HIWORD(wp) == BN_CLICKED)
			{
				if (-1 != cur_service_index)
				{
					int index = cur_service_index;
					pfc::string8 tmp;
					//uGetDlgItemText(hwnd, IDC_PATH, tmp);
					if (TRUE == uGetOpenFileName(hwnd, "Executables (*.exe)|*.exe\0", OFN_PATHMUSTEXIST, "*.exe",
						"Please select service path", NULL, tmp, FALSE))
					{
						pfc::string8 out = "\""; out += tmp; out += "\"";
						uSetDlgItemText(hwnd, IDC_PATH, out);
						cfg_service_properties[index].path = out;
					}
				}
			}
			break;
		}
		case IDC_MINIMIZE_WINDOW:
		{
			if (HIWORD(wp) == BN_CLICKED)
			{
				if (-1 != cur_service_index)
				{
					cfg_service_properties[cur_service_index].minimize_window = IsDlgButtonChecked(hwnd, IDC_MINIMIZE_WINDOW);
				}
			}
			break;
		}
		case IDC_WAIT_PROCESS:
		{
			if (HIWORD(wp) == BN_CLICKED)
			{
				if (-1 != cur_service_index)
				{
					cfg_service_properties[cur_service_index].wait_process = IsDlgButtonChecked(hwnd, IDC_WAIT_PROCESS);
					if (cfg_service_properties[cur_service_index].wait_process)
					{
						//EnableWindow(GetDlgItem(hwnd, IDC_THREAD_COUNT_SPIN), TRUE);
						EnableWindow(GetDlgItem(hwnd, IDC_RELOAD_INFO), TRUE);
					}
					else
					{
						//EnableWindow(GetDlgItem(hwnd, IDC_THREAD_COUNT_SPIN), FALSE);
						cfg_service_properties[cur_service_index].reload_info = 0;
						CheckDlgButton(hwnd, IDC_RELOAD_INFO, BST_UNCHECKED);
						EnableWindow(GetDlgItem(hwnd, IDC_RELOAD_INFO), FALSE);
					}
				}
			}
			break;
		}
		case IDC_ALBUM_GROUPING_PATTERN:
		{
			if (HIWORD(wp) == EN_UPDATE)
			{
				if (-1 != cur_service_index)
				{
					cfg_service_properties[cur_service_index].album_grouping_pattern = string_utf8_from_window(hwnd, IDC_ALBUM_GROUPING_PATTERN);
				}
			}
			break;
		}
		case IDC_FILTER:
		{
			if (HIWORD(wp) == EN_UPDATE)
			{
				if (-1 != cur_service_index)
				{
					cfg_service_properties[cur_service_index].filter = string_utf8_from_window(hwnd, IDC_FILTER);
				}
			}
			break;
		}
		case IDC_RELOAD_INFO:
		{
			if (HIWORD(wp) == BN_CLICKED)
			{
				if (-1 != cur_service_index)
				{
					cfg_service_properties[cur_service_index].reload_info = IsDlgButtonChecked(hwnd, IDC_RELOAD_INFO);
				}
			}
			break;
		}
		}
		break;
	}
	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lp)->idFrom)
		{
		case IDC_THREAD_COUNT_SPIN:
		{
			if (-1 != cur_service_index)
			{
				LPNMUPDOWN lpnmud = (LPNMUPDOWN)lp;
				if (lpnmud->hdr.code == UDN_DELTAPOS)
				{
					int thread_count = uSendDlgItemMessage(hwnd, IDC_THREAD_COUNT_SPIN, UDM_GETPOS, 0, 0);
					if (lpnmud->iDelta > 0)
					{
						if (thread_count < service_properties::max_thread_count)
							thread_count++;
					}
					else if (thread_count != 0)
						thread_count--;

					cfg_service_properties[cur_service_index].thread_count = thread_count;
				}
			}
			break;
		}
		case IDC_SERVICES_LIST:
		{	
			switch (((LPNMHDR)lp)->code)
			{
			case LVN_ITEMCHANGED:
			{
				LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)lp;
				if ((lpnmlv->uChanged & LVIF_STATE) &&
					(lpnmlv->uNewState & LVIS_SELECTED)!=
					(lpnmlv->uOldState & LVIS_SELECTED))
				{
					cur_service_index = ListView_GetNextItem(GetDlgItem(hwnd, IDC_SERVICES_LIST), -1, LVNI_SELECTED);
					if (cur_service_index != -1)
					{
						init_service_properties(hwnd, cur_service_index);
						EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_SERVICE), TRUE);
					}
					else
						reset_service_properties(hwnd);
					enable_move_button(hwnd);
				}
				break;
			}		
			}
			break;
		}
		}
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
}

static service_factory_single_t<preferences_page_foo_run_group> g_preferences_page_foo_run_group;