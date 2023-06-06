/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <libPPUI/CListControlComplete.h>
#include <libPPUI/CListControl-Cells.h>

#include "my_cfg_var.h"

extern cfg_service_properties_list g_cfg_service_properties;

class CMyPreferences;

class ServiceList : public CListControlComplete
{
public:

	ServiceList(CMyPreferences* pref_owner) : m_pref_owner(pref_owner) {
		m_cfg_service_properties.add_items(g_cfg_service_properties);
		m_cfg_service_prop_changed_msk.resize(m_cfg_service_properties.get_count());
	}

~ServiceList()	{
		m_cfg_service_properties.remove_all();
		m_cfg_service_prop_changed_msk.resize(0);
	}

	// PUBLIC SERVICES & DETAILS

	void AddNewService();
	void RemoveService(size_t index);
	void Reset();

	void SetServiceDetails(size_t index, const service_properties & service_details) {
		m_cfg_service_properties.replace_item(index, service_details);
		UpdateItem(index);
	}

	void GetServiceDetails(size_t index, service_properties & service_details) {
		service_details = m_cfg_service_properties[index];
	}

	bool GetCurrentDetails(service_properties& current_srv_details) {
		auto index = GetSingleSel();
		if (index == ~0) {
			current_srv_details = service_properties();
		}
		else
		{
			current_srv_details = m_cfg_service_properties[index];
		}
		return (index != ~0);
	}

	// PUBLIC CHANGE MASK

	void SetChangedMaskVal(size_t pos, bool val) {
		if (pos >= m_cfg_service_prop_changed_msk.size()) {
			m_cfg_service_prop_changed_msk.resize(pos + 1);
		}
		m_cfg_service_prop_changed_msk.set(pos, val);
	}

	bool GetChangedMaskVal(size_t pos) {
		return m_cfg_service_prop_changed_msk.get(pos);
	}

	bool AnyChangedMaskVal() {
		auto cmask = m_cfg_service_prop_changed_msk.size();
		auto pos = m_cfg_service_prop_changed_msk.find_first(true, 0, cmask);
		return pos < cmask;
	}

	void SetItemPath(size_t index, pfc::string8 path) {
			m_cfg_service_properties[index].path = path;
	}

	void ToGlobal() {
		g_cfg_service_properties.remove_all(); g_cfg_service_properties.set_size(0);
		g_cfg_service_properties.add_items(m_cfg_service_properties);
		clear_changed_mask();
	}

	// list overrides

	virtual groupID_t GetItemGroup(t_size p_item) const override {
		(void)p_item; return 0;
	}

	// item count

	t_size GetItemCount() const override {
		return m_cfg_service_properties.size(); // footer
	}

	// can select

	bool CanSelectItem(size_t row) const override {
		// can not select footer
		return row != footerRow();
	}

	// footer 

	size_t footerRow() const {
		return m_cfg_service_properties.size();
	}

	void onFooterClicked() {
		SelectNone();
	}

	// allow scrollbar

	bool AllowScrollbar(bool vertical) const override {
		return true;
	}

	// selection and reorder

	void OnSelectionChanged(pfc::bit_array const& affected, pfc::bit_array const& status) override;

	t_size InsertIndexFromPointEx(const CPoint& pt, bool& bInside) const override {
		// Drag&drop insertion point hook, for reordering only
		auto ret = __super::InsertIndexFromPointEx(pt, bInside);
		bInside = false; // never drop *into* an item, only between, as we only allow reorder
		if (ret > m_cfg_service_properties.size()) {
			ret = m_cfg_service_properties.size(); // never allow drop beyond footer
		}
		return ret;
	}

	// request reorder

	void RequestReorder(size_t const* order, size_t count) override;

	// remove mask

	void RemoveMask(pfc::bit_array const& mask);

	// request remove

	void RequestRemoveSelection() override;

	// default action

	void ExecuteDefaultAction(t_size index) override {
		// double click, enter key, etc
		//..
	}

	// set text

	bool GetSubItemText(t_size item, t_size subItem, pfc::string_base& out) const override {
		if (item == footerRow()) {
			if (subItem == 0) {
				out = "+ add new";
				return true;
			}
			return false;
		}

		switch (subItem) {
		case 0:
			// pass blank string or return false to create a checkbox only column
			out = m_cfg_service_properties[item].label;
			return true;
		default:
			return false;
		}
	}

	size_t GetSubItemSpan(size_t row, size_t column) const override {
		if (row == footerRow() && column == 0) {
			return GetColumnCount();
		}
		return 1;
	}

	cellType_t GetCellType(size_t item, size_t subItem) const override {
		// cellType_t is a pointer to a cell class object supplying cell behavior specification & rendering methods
		// use PFC_SINGLETON to provide static instances of used cells
		if (item == footerRow()) {
			if (subItem == 0) {
				return &PFC_SINGLETON(CListCell_Button);
			}
			else {
				return nullptr;
			}
		}
		switch (subItem) {
		case 0:
			//return &PFC_SINGLETON(CListCell_Checkbox);
		default:
			return &PFC_SINGLETON(CListCell_Text);
		}
	}

	bool GetCellTypeSupported() const override {
		return true;
	}

	bool GetCellCheckState(size_t item, size_t subItem) const override {
		//..
		return false;
	}
	void SetCellCheckState(size_t item, size_t subItem, bool value) override;

	uint32_t QueryDragDropTypes() const override { return dragDrop_reorder; }

	// Inplace edit handlers
	// Overrides of CTableEditHelperV2 methods

	void TableEdit_SetField(t_size item, t_size subItem, const char* value) override {};
	void TableEdit_GetField(t_size item, t_size subItem, pfc::string_base& out, t_size& lineCount) override {};
	bool TableEdit_IsColumnEditable(t_size subItem) const override {
		return false;
	}

private:

	void InitColumns();

	void Context_ShowContextMenu(pfc::bit_array_bittable selmask, const CPoint& point);
	void Context_AppendServiceItems(CMenu& menuPopup, const bool single_sel);

private:
	
	typedef CListControlComplete TParent;
	BEGIN_MSG_MAP(ServiceList)

		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CONTEXTMENU(OnContextMenu)

		CHAIN_MSG_MAP(TParent)

	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct) {
		InitColumns(); // set up header control with columns
		SetWindowText(L"Service list window"); // screen reader will see this
		return 0;
	}

	void OnDestroy() {
		//..
	}

	void OnContextMenu(CWindow wnd, CPoint point);
	void ItemAction(size_t item);

	void resize_changed_mask(size_t newsize) { m_cfg_service_prop_changed_msk.resize(newsize); }
	void clear_changed_mask() { m_cfg_service_prop_changed_msk = pfc::bit_array_bittable(m_cfg_service_properties.get_count()); }

private:

	enum EPopupMenuItems
	{
		menuItemRemove = 1000 
		//menuItemEdit,
        //menuItemDuplicate,
	};

	CMyPreferences* m_pref_owner;
	pfc::list_t<service_properties> m_cfg_service_properties;
	pfc::bit_array_bittable m_cfg_service_prop_changed_msk;

};

