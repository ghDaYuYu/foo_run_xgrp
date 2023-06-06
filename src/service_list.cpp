/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "stdafx.h"
#include "preferences.h"
#include "service_list.h"

void ServiceList::AddNewService()
{
	GUID new_guid;
	
	if (S_OK != CoCreateGuid(&new_guid))
	{
		console::complain(PLUGIN_NAME, "Failed to create guid for new service!");
		new_guid = pfc::guid_null;
	}

	m_cfg_service_properties.add_item(service_properties(new_guid));

	auto newsize = m_cfg_service_properties.get_count();
	m_cfg_service_prop_changed_msk.resize(newsize);
	m_cfg_service_prop_changed_msk.set(newsize - 1, true);
	
	UpdateItem(--newsize);
}

void ServiceList::RemoveService(size_t index) {
	
	auto oldcount = m_cfg_service_properties.get_count();

	if (oldcount > 1) {
		if (oldcount - index >= 1) {

			for (auto w = index; w < oldcount - 1; w++) {
				SetChangedMaskVal(w, GetChangedMaskVal(w + 1));
			}

			auto last_val = GetChangedMaskVal(oldcount - 1);
			resize_changed_mask(oldcount - 1);
			SetChangedMaskVal(oldcount - 2, last_val);
		}

		m_cfg_service_properties.remove_by_idx(index);
	}
	else {
		m_cfg_service_properties.set_size(0);		
		resize_changed_mask(0);
	}
}

void ServiceList::Reset() {
	SelectAll();
	RequestRemoveSelection();
}

void ServiceList::ItemAction(size_t item)
{
	//..
}

void ServiceList::Context_ShowContextMenu(pfc::bit_array_bittable selmask, const CPoint& point)
{
	CPoint pnt = point;
	ClientToScreen(&pnt);

	CMenu menuPopup;
	menuPopup.CreatePopupMenu();

	Context_AppendServiceItems(menuPopup, GetSelectedCount() == 1);

	UINT uCmdID = menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_NONOTIFY | TPM_RETURNCMD, pnt.x, pnt.y, GetActiveWindow());

	switch (uCmdID)
	{
	case 0:
		return;

	case menuItemRemove:
		RemoveMask(GetSelectionMask());
		break;

	default:
		;
	}
}

void ServiceList::Context_AppendServiceItems(CMenu& menuPopup, const bool single_sel)
{
	menuPopup.AppendMenu(MF_STRING | MF_BYCOMMAND,
		static_cast<UINT_PTR>(menuItemRemove), L"Remove");

}

void ServiceList::OnContextMenu(CWindow wnd, CPoint point)
{
	if (GetSelectedCount() == 0)
		return;

	if (point.x < 0 && point.y < 0)
	{
		CRect itemRect = GetItemRect(GetFirstSelected());
		point.x = itemRect.left;
		point.y = itemRect.bottom;
	}
	else
		ScreenToClient(&point);

	pfc::bit_array_bittable selmask = GetSelectionMask();
	Context_ShowContextMenu(selmask, point);
}

void ServiceList::InitColumns()
{
	InitializeHeaderCtrl(HDS_DRAGDROP);
	AddColumn("Services", UINT32_MAX);
}

void ServiceList::SetCellCheckState(size_t item, size_t subItem, bool value) {
	//..
}

// remove mask

void ServiceList::RequestRemoveSelection() {
	// Delete key etc
	RemoveMask(GetSelectionMask());

	m_pref_owner->DetailsEnabled(GetItemCount(), 0);
	m_pref_owner->OnChanged();

	NMLISTVIEW nmlv = {};
	nmlv.hdr.hwndFrom = m_hWnd;
	nmlv.iItem = -1;
	nmlv.uChanged = LVIF_STATE;
	nmlv.uNewState = LVIS_SELECTED | LVIS_FOCUSED;
	nmlv.uOldState = 0;

	m_pref_owner->ListItemChanged((LPNMHDR)&nmlv);
}

void ServiceList::RemoveMask(pfc::bit_array const& mask) {

	if (mask.get(footerRow())) {
		return; // footer row cannot be removed
	}

	bit_array_bittable new_change_mask;
	t_size new_mask_size = 0;

	for (auto w = mask.find_first(false, 0, GetItemCount()); w < GetItemCount(); w = mask.find_next(false, w, GetItemCount())) {
		new_mask_size = new_change_mask.size() + 1;
		new_change_mask.resize(new_mask_size);
		new_change_mask.set(new_mask_size - 1, m_cfg_service_prop_changed_msk.get(w));
	}
	if (new_mask_size) {
		m_cfg_service_prop_changed_msk = new_change_mask;
	}

	m_cfg_service_properties.remove_mask(mask);

	auto oldCount = GetItemCount();

	NMLISTVIEW nmlv = {};
	nmlv.hdr.hwndFrom = m_hWnd;
	nmlv.iItem = -1;
	nmlv.uChanged = LVIF_STATE;
	nmlv.uNewState = LVIS_SELECTED | LVIS_FOCUSED;
	nmlv.uOldState = 0;

	this->OnItemsRemoved(mask, oldCount);

	m_pref_owner->ListItemChanged((LPNMHDR)&nmlv);
}

// request reorder

void ServiceList::RequestReorder(size_t const* order, size_t count) {
	// we've been asked to reorder the items, by either drag&drop or cursors+modifiers
	// we can either reorder as requested, reorder partially if some of the items aren't moveable, or reject the request

	PFC_ASSERT(count == GetItemCount());

	// Footer row cannot be moved

	if (GetItemCount() > m_cfg_service_properties.size() || order[footerRow()] != footerRow())
	{
		return;
	}

	// reorder services

	m_cfg_service_properties.reorder(order);

	// reorder changed mask

	pfc::array_t<bool> arr_changed;
	arr_changed.set_size(m_cfg_service_prop_changed_msk.size());
	arr_changed.fill(false);

	size_t iCount = m_cfg_service_prop_changed_msk.size();
	size_t wtrue = m_cfg_service_prop_changed_msk.find_first(true, 0, iCount);
	
	for (wtrue; wtrue < iCount; wtrue = m_cfg_service_prop_changed_msk.find_next(true, wtrue, iCount)) {
		arr_changed[wtrue] = true;
	}

	pfc::reorder_t(arr_changed, order, m_cfg_service_properties.size());

	for (auto w = 0; w < arr_changed.size(); w++) {
		m_cfg_service_prop_changed_msk.set(w, arr_changed[w]);
	}

	// change mask reordered

	this->OnItemsReordered(order, count);
}

void ServiceList::OnSelectionChanged(pfc::bit_array const& affected, pfc::bit_array const& status) {

	const auto waff = affected.find_first(true, 0, GetItemCount());

	if (waff >= GetItemCount()) {
		return;
	}

	auto isel = GetSingleSel();

	NMLISTVIEW nmlv = {};
	nmlv.hdr.hwndFrom = m_hWnd;
	nmlv.iItem = static_cast<int>(isel);
	nmlv.uChanged = LVIF_STATE;
	nmlv.uNewState = LVIS_SELECTED | LVIS_FOCUSED;
	nmlv.uOldState = 0;

	m_pref_owner->ListItemChanged((LPNMHDR) &nmlv);
	
}