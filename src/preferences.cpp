/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "stdafx.h"
#include "helpers/atl-misc.h"
#include "libPPUI/listview_helper.h"

#include "yesno_dialog.h"
#include "preferences.h"


// {95D75E47-EA6B-4DFF-8908-A126516552BF}
static const GUID guid_cfg_service_properties =
{ 0x95d75e47, 0xea6b, 0x4dff, { 0x89, 0x8, 0xa1, 0x26, 0x51, 0x65, 0x52, 0xbf } }; //1.05

cfg_service_properties_list g_cfg_service_properties(guid_cfg_service_properties);

BOOL CMyPreferences::OnInitDialog(CWindow, LPARAM) {

	DlgResize_Init(false, true);
	SetThemeFont();

	DoDataExchange(DDX_LOAD);

	m_staticPrefHeaderSrv.PaintGradientHeader();
	m_staticPrefHeaderSrvDet.PaintGradientHeader();

	m_serviceList.CreateInDialog(*this, IDC_SERVICES_LIST);
	if (m_serviceList.GetItemCount()) {
		m_serviceList.SelectSingle(0);
	}
	else {
		DetailsEnabled(false, 0);
	}

	m_dark.AddDialogWithControls(*this);

	return FALSE;
}

void CMyPreferences::SetThemeFont() {
	LOGFONTW lf;
	CWindowDC dc(core_api::get_main_window());
	CTheme wtheme;
	HTHEME theme = wtheme.OpenThemeData(core_api::get_main_window(), L"TEXTSTYLE");
	GetThemeFont(theme, dc, TEXT_BODYTEXT, 0, TMT_FONT, &lf);
	m_font = CreateFontIndirectW(&lf);
	SetFont(m_font, true);

	for (HWND walk = ::GetWindow(m_hWnd, GW_CHILD); walk != NULL; ) {
		HWND next = ::GetWindow(walk, GW_HWNDNEXT);
		if (::IsWindow(next)) {
			::SendMessage(next, WM_SETFONT, (WPARAM)m_font, MAKELPARAM(1/*bRedraw*/, 0));
		}
		walk = next;
	}
}

// INIT SERVICES

void CMyPreferences::init_service_properties(HWND hwndParent, t_size index, const service_properties& current)
{
	m_is_initiating = true;

	size_t csel = m_serviceList.GetSelectedCount();
	bool benabled = m_serviceList.GetSingleSel() != -1 && csel == 1;
	DetailsEnabled(benabled, csel);

	HWND wnd_ctrl = uGetDlgItem(IDC_LABEL);
	uSendDlgItemMessageText(hwndParent, IDC_LABEL, WM_SETTEXT, 0, current.label);
	uSendDlgItemMessageText(hwndParent, IDC_PATH, WM_SETTEXT, 0, current.path);
	CheckDlgButton(IDC_MINIMIZE_WINDOW, (UINT)current.minimize_window);
	CheckDlgButton(IDC_WAIT_PROCESS, (UINT)current.wait_process);
	uSendDlgItemMessage(IDC_THREAD_COUNT_SPIN, UDM_SETPOS, 0, (LPARAM)MAKELONG(current.thread_count, 0));
	uSendDlgItemMessageText(hwndParent, IDC_ALBUM_GROUPING_PATTERN, WM_SETTEXT, 0, current.album_grouping_pattern);
	uSendDlgItemMessageText(hwndParent, IDC_FILTER, WM_SETTEXT, 0, current.filter);
	CheckDlgButton(IDC_RELOAD_INFO, (UINT)current.reload_info);

	::EnableWindow(GetDlgItem(IDC_RELOAD_INFO), current.wait_process ? TRUE : FALSE);

	if (current.wait_process)
	{
		::EnableWindow(GetDlgItem(IDC_RELOAD_INFO), TRUE);
	}
	else
	{
		CheckDlgButton(IDC_RELOAD_INFO, BST_UNCHECKED);
		::EnableWindow(GetDlgItem(IDC_RELOAD_INFO), FALSE);
	}

	m_is_initiating = false;
}

// RESET SERVICE PROPERTIES

void CMyPreferences::DetailsEnabled(bool benabled, size_t csel) {
	::EnableWindow(GetDlgItem(IDC_LABEL), benabled);
	::EnableWindow(GetDlgItem(IDC_PATH), benabled);
	::EnableWindow(GetDlgItem(IDC_BROWSE_PATH), benabled);
	::EnableWindow(GetDlgItem(IDC_ALBUM_GROUPING_PATTERN), benabled);
	::EnableWindow(GetDlgItem(IDC_FILTER), benabled);
	::EnableWindow(GetDlgItem(IDC_THREAD_COUNT_SPIN), benabled);
	::EnableWindow(GetDlgItem(IDC_THREAD_COUNT_EDIT), benabled);
	::EnableWindow(GetDlgItem(IDC_MINIMIZE_WINDOW), benabled);
	::EnableWindow(GetDlgItem(IDC_WAIT_PROCESS), benabled);
	::EnableWindow(GetDlgItem(IDC_RELOAD_INFO), benabled);

	::EnableWindow(GetDlgItem(IDC_REMOVE_SERVICE), csel != 0);
}

void CMyPreferences::reset_service_properties(HWND hwndParent, t_size oldindex)
{
	m_is_initiating = true;

	bool benabled;
	size_t csel = m_serviceList.GetSelectedCount();

	if (oldindex == -1) {
		benabled = false;
	}
	else {
		benabled = m_serviceList.GetSingleSel() != -1 && csel == 1;
	}

	DetailsEnabled(benabled, csel);

	if (benabled) {
		uSendDlgItemMessageText(hwndParent, IDC_LABEL, WM_SETTEXT, 0, service_properties::default_label);
		uSendDlgItemMessageText(hwndParent, IDC_PATH, WM_SETTEXT, 0, service_properties::default_path);
		CheckDlgButton(IDC_MINIMIZE_WINDOW, (UINT)service_properties::default_minimize_window);
		CheckDlgButton(IDC_WAIT_PROCESS, (UINT)service_properties::default_wait_process);
		uSendDlgItemMessage(IDC_THREAD_COUNT_SPIN, UDM_SETPOS, 0, (LPARAM)MAKELONG(service_properties::default_thread_count, 0));
		uSendDlgItemMessageText(hwndParent, IDC_ALBUM_GROUPING_PATTERN, WM_SETTEXT, 0, service_properties::default_album_grouping_pattern);
		uSendDlgItemMessageText(hwndParent, IDC_FILTER, WM_SETTEXT, 0, service_properties::default_filter);
		CheckDlgButton(IDC_FILTER, (UINT)service_properties::default_reload_info);

		if (service_properties::default_wait_process)
		{

			::EnableWindow(GetDlgItem(IDC_RELOAD_INFO), TRUE);
		}
		else
		{

			CheckDlgButton(IDC_RELOAD_INFO, BST_UNCHECKED);
			::EnableWindow(GetDlgItem(IDC_RELOAD_INFO), FALSE);
		}
	}

	m_is_initiating = false;
}

void CMyPreferences::OnBtnNewServiceClicked(UINT, int, CWindow) {
	m_serviceList.AddNewService();
	m_serviceList.GetServiceDetails(m_serviceList.GetItemCount() - 1, m_current_srv_details);
	OnChanged();
}

void CMyPreferences::OnBtnBrowseFilesClicked(UINT, int, CWindow) {

	auto ndx = m_serviceList.GetSingleSel();

	if (ndx != ~0)
	{
		pfc::string8 buffer;

		if (TRUE == uGetOpenFileName(m_hWnd, "Executables (*.exe)|*.exe\0", OFN_PATHMUSTEXIST, "*.exe",
			"Please select service path", NULL, buffer, FALSE))
		{
			pfc::string8 out = "\""; out += buffer; out += "\"";
			uSetDlgItemText(m_hWnd, IDC_PATH, out);

			m_current_srv_details.path = out;
			m_serviceList.SetItemPath(ndx, out);
			OnChanged(ndx, m_current_srv_details);
			
		}
	}
}

void CMyPreferences::OnBtnRemoveServiceClicked(UINT, int, CWindow) {

	auto ndx = m_serviceList.GetFirstSelected();

	if (ndx != ~0)
	{
		m_serviceList.RemoveMask(m_serviceList.GetSelectionMask());

		NMLISTVIEW nmlv = {};
		nmlv.hdr.hwndFrom = m_hWnd;
		nmlv.iItem = -1;
		nmlv.uChanged = LVIF_STATE;
		nmlv.uNewState = LVIS_SELECTED | LVIS_FOCUSED;
		nmlv.uOldState = 0;

		ListItemChanged((LPNMHDR)&nmlv);
		DetailsEnabled(m_serviceList.GetFirstSelected() != -1, 0);

		return;
	}
}

void CMyPreferences::OnBtnCheckBoxServiceClicked(UINT a, int nID, CWindow hwnd) {

	auto ndx = m_serviceList.GetFirstSelected();

	if (ndx != ~0)
	{
		switch (nID) {
		
		case IDC_MINIMIZE_WINDOW:
			m_current_srv_details.minimize_window = IsDlgButtonChecked(IDC_MINIMIZE_WINDOW);
			break;
		case IDC_WAIT_PROCESS:
			m_current_srv_details.wait_process = IsDlgButtonChecked(IDC_WAIT_PROCESS);
			if (m_current_srv_details.wait_process)
			{
				::EnableWindow(GetDlgItem(IDC_RELOAD_INFO), TRUE);
			}
			else
			{
				m_current_srv_details.reload_info = 0;
				CheckDlgButton(IDC_RELOAD_INFO, BST_UNCHECKED);
				::EnableWindow(GetDlgItem(IDC_RELOAD_INFO), FALSE);
			}
			break;
		case IDC_RELOAD_INFO:
			m_current_srv_details.reload_info = IsDlgButtonChecked(IDC_RELOAD_INFO);
			break;
		default:
			return;
		}
		OnChanged(ndx, m_current_srv_details);
	}
}

LRESULT CMyPreferences::ListItemChanged(LPNMHDR lpmhdr) {

	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)lpmhdr;
	HWND hwnd = ::GetParent(lpnmlv->hdr.hwndFrom);

	if ((lpnmlv->uChanged & LVIF_STATE) &&
		(lpnmlv->uNewState & LVIS_SELECTED) !=
		(lpnmlv->uOldState & LVIS_SELECTED))
	{
		auto ndx = m_serviceList.GetSingleSel();

		if (lpnmlv->iItem != -1 && ndx != ~0 && m_serviceList.GetSelectedCount() == 1)
		{
			m_serviceList.GetCurrentDetails(m_current_srv_details);
			init_service_properties(m_hWnd, ndx, m_current_srv_details);
			::EnableWindow(GetDlgItem(IDC_REMOVE_SERVICE), TRUE);
		}
		else {
			reset_service_properties(hwnd, lpnmlv->iItem);
		}
		OnChanged();
	}
	return FALSE;
}


LRESULT CMyPreferences::OnBtnCountSpinNotifyClicked(LPNMHDR lpmhdr) {

	auto ndx = m_serviceList.GetSingleSel();

	if (ndx != ~0)
	{
		LPNMUPDOWN lpnmud = (LPNMUPDOWN)lpmhdr;
		HWND hwnd = lpmhdr->hwndFrom;
		if (lpnmud->hdr.code == UDN_DELTAPOS)
		{
			t_uint32 thread_count = static_cast<t_uint32>(uSendDlgItemMessage(IDC_THREAD_COUNT_SPIN, UDM_GETPOS, 0, 0));
			if (lpnmud->iDelta > 0)
			{
				if (thread_count != 0) {
					thread_count--;
				}
			}
			else {
				if (thread_count < service_properties::max_thread_count) {
					thread_count++;
				}
			}
			
			if (m_current_srv_details.thread_count != thread_count) {
				m_current_srv_details.thread_count = thread_count;
				lpnmud->iPos = thread_count;

				SetDlgItemInt(IDC_THREAD_COUNT_EDIT, thread_count, 0);
			}
		}
	}
	return	TRUE;
}

void CMyPreferences::OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl) {

	if (m_is_initiating) {
		return;
	}

	auto ndx = m_serviceList.GetSingleSel();

	//todo: ddx

	if (uNotifyCode == EN_UPDATE && ndx != ~0) {

		bool bcallchanged = false;

		switch (nID)
		{
		case IDC_LABEL:

			bcallchanged = true;
			m_current_srv_details.label = uGetDlgItemText(m_hWnd, IDC_LABEL);
			break;

		case IDC_PATH:

			bcallchanged = true;
			m_current_srv_details.path = uGetDlgItemText(m_hWnd, IDC_PATH);
			break;

		case IDC_THREAD_COUNT_EDIT:
		{
			bcallchanged = true;
			pfc::string8 buffer = uGetDlgItemText(m_hWnd, IDC_THREAD_COUNT_EDIT);
			auto iThreads = pfc::atodec<t_uint32>(buffer.ptr(), buffer.get_length());
			m_current_srv_details.thread_count = iThreads;
			break;
		}
		case IDC_ALBUM_GROUPING_PATTERN:

			bcallchanged = true;
			m_current_srv_details.album_grouping_pattern = uGetDlgItemText(m_hWnd, IDC_ALBUM_GROUPING_PATTERN);			
			break;

		case IDC_FILTER:

			bcallchanged = true;
			m_current_srv_details.filter = uGetDlgItemText(m_hWnd, IDC_FILTER);
			break;

		default:
			;
		}

		if (bcallchanged) {
			OnChanged(ndx, m_current_srv_details);
		}

	} // end EN_UPDATE

	//todo: revision

	switch (uNotifyCode)
	{

	case EN_KILLFOCUS:
		[[fallthrough]];
	case WM_KILLFOCUS:
		[[fallthrough]];
	case CBN_KILLFOCUS:
	{
		switch (nID) {
		case IDC_PATH:
			[[fallthrough]];
		case IDC_LABEL:
			[[fallthrough]];
		case IDC_MINIMIZE_WINDOW:
			[[fallthrough]];
		case IDC_WAIT_PROCESS:
			[[fallthrough]];
		case IDC_THREAD_COUNT_EDIT:
			[[fallthrough]];
		case IDC_RELOAD_INFO:
			[[fallthrough]];
		case IDC_ALBUM_GROUPING_PATTERN:
			[[fallthrough]];
		case IDC_FILTER:
			OnChanged();
		default:
			;
		}
	}
	default:
		;
	}
}

t_uint32 CMyPreferences::get_state() {

	t_uint32 state = preferences_state::resettable | preferences_state::dark_mode_supported;

	auto bchanged = HasChanged();
	if (bchanged) state |= preferences_state::changed;
	return state;
}

void CMyPreferences::reset() {

	CYesNoApiDialog yndlg;

	auto res = yndlg.query(m_hWnd, { "Reset","Reset component configuration ?" }, true, true);

	if (res == 1) {
		
		m_serviceList.Reset();

	}
}

void CMyPreferences::apply() {

	m_serviceList.ToGlobal();

	OnChanged();
}

void CMyPreferences::OnChanged(size_t index, const service_properties& service_details) {
	m_serviceList.SetServiceDetails(index, service_details);
	OnChanged();
}

void CMyPreferences::OnChanged() {

	m_callback->on_state_changed();
}

bool CMyPreferences::HasChanged() {

	t_size icount = m_serviceList.GetItemCount();
	auto ndx = m_serviceList.GetSingleSel();

	pfc::string8 buffer;
	bool bchanged = false;
	bool bchecked;
	t_uint32  ival;

	if (ndx != ~0 && ndx < g_cfg_service_properties.get_count()) {

		buffer = uGetDlgItemText(m_hWnd, IDC_LABEL);
		bchanged = (bool)stricmp_utf8(g_cfg_service_properties[ndx].label, buffer);

		buffer = uGetDlgItemText(m_hWnd, IDC_PATH);
		bchanged |= (bool)stricmp_utf8(g_cfg_service_properties[ndx].path, buffer);


		buffer = uGetDlgItemText(m_hWnd, IDC_ALBUM_GROUPING_PATTERN);
		bchanged |= (bool)stricmp_utf8(g_cfg_service_properties[ndx].album_grouping_pattern, buffer);

		buffer = uGetDlgItemText(m_hWnd, IDC_FILTER);
		bchanged |= (bool)stricmp_utf8(g_cfg_service_properties[ndx].filter, buffer);

		bchecked = IsDlgButtonChecked(IDC_WAIT_PROCESS);
		bchanged |= g_cfg_service_properties[ndx].wait_process != (t_int32)bchecked;


		bchecked = IsDlgButtonChecked(IDC_RELOAD_INFO);
		bchanged |= g_cfg_service_properties[ndx].reload_info != (t_int32)bchecked;

		bchecked = IsDlgButtonChecked(IDC_MINIMIZE_WINDOW);
		bchanged |= g_cfg_service_properties[ndx].minimize_window != (t_int32)bchecked;

		ival = static_cast<t_uint32>(uSendDlgItemMessage(IDC_THREAD_COUNT_SPIN, UDM_GETPOS, 0, 0));
		bchanged |= g_cfg_service_properties[ndx].thread_count != ival;

		m_serviceList.SetChangedMaskVal(ndx, bchanged);
	}

	bchanged |= g_cfg_service_properties.get_count() != icount;

	if (bchanged) {
		return bchanged;
	}

	bchanged = m_serviceList.AnyChangedMaskVal();

	return bchanged;
}

class preferences_page_myimpl : public preferences_page_impl<CMyPreferences> {
	// preferences_page_impl<> helper deals with instantiation of our dialog; inherits from preferences_page_v3.
public:

	virtual const char* get_name()
	{
		return PLUGIN_NAME;
	}

	virtual GUID get_guid()
	{
		return guid_preferences_page_foo_run_group;
	}

	virtual GUID get_parent_guid()
	{
		return preferences_page::guid_tools;
	}

	virtual bool reset_query() override {
		return false;
	}
};

static preferences_page_factory_t<preferences_page_myimpl> g_preferences_page_foo_run_group;