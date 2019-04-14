/*

MIT License

Copyright (C) 2005-2019 by Vladimir Novick

			  http://www.linkedin.com/in/vladimirnovick

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "stdafx.h"
#include "ScreenshotTools.h"
#include "Canvas.h"
#include "afxdialogex.h"
#include "Message_IDs.h"
#include "CanvasOperation.h"

IMPLEMENT_DYNAMIC(Canvas, CDialog)

Canvas* Canvas::m_pThis = NULL;

Canvas::Canvas(CanvasOperation SetOperation /*= CanvasOperation::getRegion */,
	CWnd *mainDlg /*= nullptr */,
	CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CANVAS_DIALOG, pParent)
{
	Operation = SetOperation;
	mainDialog = mainDlg;
}

Canvas::~Canvas()
{
}

BOOL Canvas::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			m_bCropScreenshot = false;
			::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
			return TRUE;                
		}
	}

	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		///etc, etc
	{
		// might want other tests
		return TRUE;
	} break;
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void Canvas::OnClose()
{
	
}

BOOL Canvas::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pThis = this;
	m_bCropScreenshot = true; // crop screenshot

	//// Set WS_EX_LAYERED on this window 
	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED );
	//// Make this window 30% alpha
	SetLayeredWindowAttributes(0, (255 * 30) / 100, LWA_ALPHA);

	return 1;
}

void Canvas::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void Canvas::OnKillFocus(CWnd *pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);
}

void Canvas::OnPaint()
{

}
void Canvas::OnSetFocus(CWnd * pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);
}

BEGIN_MESSAGE_MAP(Canvas, CDialog)
	ON_MESSAGE(WM_ACTIVATE, OnActive)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
void Canvas::PostNcDestroy()
{
	
	if (m_bCropScreenshot) {
		::PostMessage(mainDialog->m_hWnd, WM_CROP_SCREENSHOT, 0, 0);
	}

	CDialog::PostNcDestroy();
	delete this;
}

afx_msg LRESULT Canvas::OnActive(WPARAM wParam, LPARAM lParam) {
	if (LOWORD(wParam) != WA_ACTIVE) {
	 	EndDialog(0);
	}

	if (LOWORD(wParam) == WA_ACTIVE) {
		::PostMessage(mainDialog->m_hWnd, WM_START_SELECT_REGION, 0, 0);
	}

	return 1;
}

BOOL Canvas::OnEraseBkgnd(CDC *pDC)
{
	CRect clientRect;

	GetClientRect(&clientRect);
	pDC->FillSolidRect(clientRect, RGB(255, 255, 255));  

	return true;
}

