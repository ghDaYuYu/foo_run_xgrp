#include "stdafx.h"
#include "header_static.h"

#include "libPPUI/PaintUtils.h"

using PaintUtils::PaintSeparatorControl;

HeaderStatic::HeaderStatic() :
	m_iLeftSpacing(8),
	m_clrLeft(GetSysColor(COLOR_ACTIVECAPTION)),
	m_clrRight(GetSysColor(COLOR_BTNFACE)),
	m_clrText(GetSysColor(COLOR_CAPTIONTEXT))
{
}

void HeaderStatic::OnPaint(CDCHandle dcDummy)
{
	PaintSeparatorControl(*this);
}

void HeaderStatic::DrawGradRect(CPaintDC& dc, const CRect& r, COLORREF clrLeft, COLORREF clrRight)
{
	float fStep = static_cast<float>(r.Width()) / 255.0f;

	for(int iOnBand = 0; iOnBand < 255; ++iOnBand) 
	{
		CRect rectStep(
			r.left + static_cast<int>(std::floor(iOnBand * fStep + 0.5)), 
			r.top,
			r.left + static_cast<int>(std::floor((iOnBand + 1) * fStep + 0.5)), 
			r.bottom);	

		BYTE btNewR = static_cast<BYTE>(
			(GetRValue(clrRight) - GetRValue(clrLeft)) * 
			static_cast<float>(iOnBand) / 255.0f + GetRValue(clrLeft));

		BYTE btNewG = static_cast<BYTE>(
			(GetGValue(clrRight) - GetGValue(clrLeft)) * 
			static_cast<float>(iOnBand) / 255.0f + GetGValue(clrLeft));

		BYTE btNewB = static_cast<BYTE>(
			(GetBValue(clrRight) - GetBValue(clrLeft)) * 
			static_cast<float>(iOnBand) / 255.0f + GetBValue(clrLeft));

		dc.FillSolidRect(rectStep, RGB(btNewR, btNewG, btNewB));
	}
}

BOOL HeaderStatic::SubclassWindow(HWND hWnd)
{
	if (!CWindowImpl<HeaderStatic, CStatic>::SubclassWindow(hWnd))
		return FALSE;

	SetLeftGradientColor (GetSysColor(COLOR_ACTIVECAPTION));
	SetRightGradientColor(GetSysColor(COLOR_BTNFACE));
	SetTextColor         (GetSysColor(COLOR_CAPTIONTEXT));

	CLogFont logFont(GetFont());
	logFont.SetBold();
	logFont.SetHeight(9);

	m_newFont = logFont.CreateFontIndirect();
	
	SetFont(m_newFont);

	return TRUE;
}

void HeaderStatic::PaintGradientHeader()
{
	CPaintDC dc(m_hWnd);

	CRect rect;
	GetClientRect(&rect);

	DrawGradRect(dc, rect, m_clrLeft, m_clrRight);

	dc.SetTextColor(m_clrText);
	dc.SetBkMode(TRANSPARENT);

	CString strText;
	GetWindowText(strText);

	CFontHandle font(GetFont());
	CFontHandle fontOld = dc.SelectFont(font);

	DWORD dwStyle = GetStyle();
	if ((dwStyle & SS_CENTER) == SS_CENTER)
		dc.DrawText(strText, -1, rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	else if((dwStyle & SS_LEFT) == SS_LEFT)
	{
		rect.left += m_iLeftSpacing;
		dc.DrawText(strText, -1, rect, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	}
	else // Right
	{
		rect.right -= m_iLeftSpacing;
		dc.DrawText(strText, -1, rect, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
	}

	dc.SelectFont(fontOld);
}