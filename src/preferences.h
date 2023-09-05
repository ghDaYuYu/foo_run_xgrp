/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once
#include "resource.h"
#include "atlframe.h"
#include "atlddx.h"
#include "header_static.h"
#include "helpers/DarkMode.h"
#include "my_cfg_var.h"
#include "service_list.h"


// {09E7539C-F52D-4B2F-B96F-CF0C464FAA68}
static constexpr GUID guid_preferences_page_foo_run_group =
{ 0x9e7539c, 0xf52d, 0x4b2f, { 0xb9, 0x6f, 0xcf, 0xc, 0x46, 0x4f, 0xaa, 0x68 } }; //1.05

class CMyPreferences : public CDialogImpl<CMyPreferences>,
	public CDialogResize<CMyPreferences>,
	public preferences_page_instance,
	public CWinDataExchange<CMyPreferences> {

public:

	CMyPreferences(preferences_page_callback::ptr callback) : m_callback(callback), m_serviceList(this) {
		//..
	}

	~CMyPreferences() {

		m_staticPrefHeaderSrv.Detach();
		m_staticPrefHeaderSrvDet.Detach();

		DeleteObject(m_font);
	}

	//dialog resource ID
	enum { IDD = IDD_PREF_PAGE };

	// preferences_page_instance methods
	t_uint32 get_state() override;
	void apply() override;
	void reset() override ;

	bool HasChanged();
	void OnChanged();
	void OnChanged(size_t index, const service_properties& service_details);

private:

	BEGIN_MSG_MAP_EX(CMyPreferences)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_HANDLER_EX(IDC_BROWSE_PATH, BN_CLICKED, OnBtnBrowseFilesClicked)
		COMMAND_HANDLER_EX(IDC_NEW_SERVICE, BN_CLICKED, OnBtnNewServiceClicked)
		COMMAND_HANDLER_EX(IDC_REMOVE_SERVICE, BN_CLICKED, OnBtnRemoveServiceClicked)
		COMMAND_HANDLER_EX(IDC_MINIMIZE_WINDOW, BN_CLICKED, OnBtnCheckBoxServiceClicked)
		COMMAND_HANDLER_EX(IDC_WAIT_PROCESS, BN_CLICKED, OnBtnCheckBoxServiceClicked)
		COMMAND_HANDLER_EX(IDC_RELOAD_INFO, BN_CLICKED, OnBtnCheckBoxServiceClicked)

		NOTIFY_HANDLER_EX(IDC_THREAD_COUNT_SPIN, UDN_DELTAPOS, OnBtnCountSpinNotifyClicked)
		CHAIN_MSG_MAP(CDialogResize<CMyPreferences>)

		MSG_WM_COMMAND(OnCommand)
	END_MSG_MAP()


	BEGIN_DLGRESIZE_MAP(CMyPreferences)
		DLGRESIZE_CONTROL(IDC_SERVICE_LIST_HEADER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_SERVICES_LIST, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_SERVICE_DETAILS_HEADER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LABEL, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_PATH, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_ALBUM_GROUPING_PATTERN, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_FILTER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_NEW_SERVICE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_REMOVE_SERVICE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BROWSE_PATH, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_MINIMIZE_WINDOW, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_WAIT_PROCESS, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_RELOAD_INFO, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(CMyPreferences)
		DDX_CONTROL(IDC_SERVICE_LIST_HEADER, m_staticPrefHeaderSrv)
		DDX_CONTROL(IDC_SERVICE_DETAILS_HEADER, m_staticPrefHeaderSrvDet)
	END_DDX_MAP()

	void DlgResize_UpdateLayout(int cxWidth, int cyHeight) {
		CDialogResize<CMyPreferences>::DlgResize_UpdateLayout(cxWidth, cyHeight);
	}

	BOOL OnInitDialog(CWindow, LPARAM);
	void SetThemeFont();

	void OnBtnNewServiceClicked(UINT, int, CWindow);
	void OnBtnRemoveServiceClicked(UINT, int, CWindow);
	void OnBtnCheckBoxServiceClicked(UINT, int, CWindow);
	void OnBtnBrowseFilesClicked(UINT, int, CWindow);
	
	LRESULT OnBtnCountSpinNotifyClicked(LPNMHDR lpmhdr);

	void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);

	LRESULT ListItemChanged(LPNMHDR lpmhdr);
	void DetailsEnabled(bool benabled, size_t csel);

	void add_new_service();
	void init_service_properties(HWND hwndParent, t_size index, const service_properties& current);
	void reset_service_properties(HWND hwndParent, t_size index);

	// Dark mode
	fb2k::CDarkModeHooks m_dark;
	HFONT m_font;

	HeaderStatic m_staticPrefHeaderSrv;
	HeaderStatic m_staticPrefHeaderSrvDet;
	
	ServiceList m_serviceList;
	service_properties m_current_srv_details;
	
	const preferences_page_callback::ptr m_callback;

	bool m_is_initiating = false;

};
