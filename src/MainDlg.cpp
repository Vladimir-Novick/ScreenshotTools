/*


MIT License

		 Copyright (C) 2005-2019 by Vladimir Novick http://www.linkedin.com/in/vladimirnovick

		Permission is hereby granted, free of charge, to any person obtaining a copy
		of this software and associated documentation files (the "Software"), to deal
		in the Software without restriction, including without limitation the rights
		to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
		copies of the Software, and to permit persons to whom the Software is
		furnished to do so, subject to the following conditions:

		The above copyright notice and this permission notice shall be included in
		all copies or substantial portions of the Software.

		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
		IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
		FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
		AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
		LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
		OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
		THE SOFTWARE.

*/
#include "stdafx.h"
#include "ScreenshotTools.h"
#include "MainDlg.h"
#include "AboutDlg.h"
#include "Registry.h"
#include "GetDataDlg.h"
#include <Windows.h>
#include "Canvas.h"
#include "Message_IDs.h"

HHOOK CMainDlg::m_hMouseHook = NULL;
HHOOK CMainDlg::m_hKeyboardHook = NULL;
CMainDlg* CMainDlg::m_pThis = NULL;

#define  HOTKEY_100_C 100
#define  HOTKEY_200_c 200
#define  HOTKEY_300_W 300
#define  HOTKEY_400_w 400
#define  HOTKEY_500_S 500
#define  HOTKEY_600_s 600

static int __stdcall BrowseCallbackProc(HWND, UINT, LPARAM, LPARAM);

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SYSCOMMAND()

	ON_CBN_SELCHANGE(IDC_IMAGE_EXT, OnCbnSelchangeImageExt)
	ON_BN_CLICKED(IDC_SCREEN_DRAW, OnBnClickedScreenDraw)
	ON_BN_CLICKED(IDC_BROWSE_IMG_PATH, OnBnClickedBrowseImgPath)
	ON_EN_KILLFOCUS(IDC_LINE_WIDTH, OnEnKillfocusLineWidth)
	ON_MESSAGE(WM_SCREENDRAW_DONE, OnDrawComplete)
	ON_MESSAGE(WM_MOUSEHOOK_CANCEL, OnMouseHookCancel)
	ON_MESSAGE(WM_START_SELECT_REGION, OnStartSelectRegion)
	ON_MESSAGE(WM_CROP_SCREENSHOT, OnGropScreenshot)
	ON_MESSAGE(WM_PRINTSCREEN, OnPrintScreen)
	ON_MESSAGE(WM_TRAY_NOTIFICATION, OnTrayNotification)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(WM_ACTIVATE, OnActive)
	ON_MESSAGE(WM_QUERYENDSESSION, OnQueryEndSession)
	ON_COMMAND(ID_SHOW, OnTrayShow)
	ON_COMMAND(ID_SHUTDOWN, OnShutdown)
	ON_COMMAND(ID_SHOW_ABOUT, OnShowAbout)
	ON_COMMAND(ID_DRAW_SEL, OnDrawSelection)
	ON_COMMAND(ID_LATEST_RECTANGULAR, OnLatestRectangularSnapshot)
	ON_COMMAND(ID__RECTANGLE_SELECT, OnRegionSelect)
	ON_COMMAND(IDC_REGION_SELECTION, OnRegionSelect)

END_MESSAGE_MAP()


LRESULT CMainDlg::OnQueryEndSession(WPARAM wParm, LPARAM lParm)
{
		PostMessage(WM_QUIT);
		return TRUE;
}

afx_msg LRESULT CMainDlg::OnActive(WPARAM wParam, LPARAM lParam) {
	if (LOWORD(wParam) != WA_ACTIVE) {
		ShowWindow(SW_HIDE);
	}

	return 1;
}

afx_msg LRESULT CMainDlg::OnGropScreenshot(WPARAM wParam, LPARAM lParam)
{
	return LatestRectangularSnapshot();
}

LRESULT CMainDlg::LatestRectangularSnapshot()
{
	return CutRegionToFile(m_ptFirst, m_ptLast);
}



LRESULT CMainDlg::GropDeskScreenshot()
{
	int width = (int)GetSystemMetrics(SM_CXSCREEN);
	int height = (int)GetSystemMetrics(SM_CYSCREEN);
	CPoint ptFirst;

	ptFirst.x = 0;
	ptFirst.y = 0;

	CPoint ptLast;
	ptFirst.x = width;
	ptFirst.y = height;

	return CutRegionToFile(ptFirst, ptLast);
}



LRESULT CMainDlg::CutRegionToFile(CPoint ptFirst, CPoint ptLast)
{
	if (!m_bGetScreenShot) {
		m_bGetScreenShot = true;
		return 1;
	}
	HWND hDesktop = ::GetDesktopWindow();
		ASSERT(hDesktop);

		::InvalidateRect(hDesktop, NULL, TRUE);
		::UpdateWindow(hDesktop);

		HBITMAP m_hTmpDraw = GetScreenshot(hDesktop);
		if (!m_hTmpDraw)
			return FALSE;

	CRect rScreenshot(ptFirst, ptLast);
	rScreenshot.NormalizeRect();

	HBITMAP hScreenshotBmp = CropDDB(m_hTmpDraw, rScreenshot);

	if (!hScreenshotBmp)
		return -1;

	CString strFile = GetOutputFileName();

	CString strFilePath = m_strImagePath;
	strFilePath += strFile;

	SaveImageAs(hScreenshotBmp, strFilePath);

	::DeleteObject(hScreenshotBmp);
	hScreenshotBmp = NULL;

	::DeleteObject(m_hTmpDraw);
	m_hTmpDraw = NULL;

	return 1;
}

afx_msg LRESULT CMainDlg::OnHotKey(WPARAM wParam, LPARAM lParam) {
	switch (wParam)
	{
	case HOTKEY_100_C:
	case HOTKEY_200_c:
		OnDrawSelection();
		break;

	case HOTKEY_300_W:
	case HOTKEY_400_w:
		LatestRectangularSnapshot();
		break;

	case HOTKEY_500_S:
	case  HOTKEY_600_s:
		GropDeskScreenshot();
	}
	return 1;
}

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hSelectCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);

	m_bKeyboardHook = FALSE;
	m_pThis = this;
	m_bDrawing = FALSE;
	m_bSelecting = FALSE;

	m_iBandPx = 3;
	m_bDitherLast = FALSE;
	m_hHilightWnd = NULL;
	m_bMouseHook = FALSE;
	m_bMinToTray = TRUE;
	m_iNamingOpt = 2;
	m_strImageName = _T("");

	if (!m_strImagePath.IsEmpty())
		return;

	LPITEMIDLIST pidl = 0;
	HRESULT hr = NOERROR;
	if (SUCCEEDED(hr = ::SHGetSpecialFolderLocation(m_hWnd, CSIDL_DESKTOP, &pidl)))
	{
		if (pidl)
		{
			::SHGetPathFromIDList(pidl, m_strImagePath.GetBuffer(MAX_PATH));
			m_strImagePath.ReleaseBuffer();

			LPMALLOC pMalloc;
			hr = ::SHGetMalloc(&pMalloc);
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}

	if (m_strImagePath.GetLength() == 0)
		m_strImagePath = _T("C:\\");

	if (m_strImagePath.Right(1) != _T('\\'))
		m_strImagePath += _T('\\');
}

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pCanvasDialog = nullptr;
	m_bGetScreenShot = true;

	RegisterHotKey(m_hWnd, HOTKEY_100_C, MOD_CONTROL | MOD_SHIFT, 'C');
	RegisterHotKey(m_hWnd, HOTKEY_200_c, MOD_CONTROL | MOD_SHIFT, 'c');
	RegisterHotKey(m_hWnd, HOTKEY_300_W, MOD_CONTROL | MOD_SHIFT, 'W');
	RegisterHotKey(m_hWnd, HOTKEY_400_w, MOD_CONTROL | MOD_SHIFT, 'w');
	RegisterHotKey(m_hWnd, HOTKEY_500_S, MOD_CONTROL | MOD_SHIFT, 'S');
	RegisterHotKey(m_hWnd, HOTKEY_600_s, MOD_CONTROL | MOD_SHIFT, 's');

	memset(&m_NID, 0, sizeof(m_NID));
	m_NID.cbSize = sizeof(m_NID);
	m_NID.uID = IDR_TRAY_MENU;
	m_NID.hWnd = this->m_hWnd;
	m_NID.uCallbackMessage = WM_TRAY_NOTIFICATION;
	m_NID.hIcon = m_hIcon;
	strncpy(m_NID.szTip, _T("Screenshot Tools"), sizeof(m_NID.szTip));
	m_NID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	if (!Shell_NotifyIcon(NIM_ADD, &m_NID))
	{
		TRACE(_T("Could not create system tray icon\n"));

	}

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu = _T("About...");
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CEdit* pEdit = NULL;
	VERIFY(pEdit = (CEdit*)GetDlgItem(IDC_IMAGE_NAME));
	pEdit->SetLimitText(50);

	CComboBox* pCombo = NULL;
	VERIFY(pCombo = (CComboBox*)GetDlgItem(IDC_IMAGE_EXT));
	ImageCodecInfo* pImageCodecInfo = NULL;
	UINT nCount = 0; // number of image encoders
	UINT nSize = 0;  // size of the image encoder array in bytes
	GetImageEncodersSize(&nCount, &nSize);
	if (nSize > 0)
	{
		pImageCodecInfo = (ImageCodecInfo*)(malloc(nSize));
		if (pImageCodecInfo != NULL)
		{
			GetImageEncoders(nCount, nSize, pImageCodecInfo);
			for (UINT x = 0; x < nCount; x++)
			{
				CString strFormat(pImageCodecInfo[x].FormatDescription);
				int iItem = pCombo->AddString(strFormat);
				ASSERT(iItem != -1);
				CLSID* pCLSID = new CLSID;
				ASSERT(pCLSID);
				*pCLSID = pImageCodecInfo[x].Clsid;
				pCombo->SetItemData(iItem, (DWORD_PTR)pCLSID);

				if (strFormat.CompareNoCase(_T("jpeg")) == 0)
				{
					m_clsidImgType = pImageCodecInfo[x].Clsid;
					pCombo->SetCurSel(iItem);
				}
			}
			free(pImageCodecInfo);
		}
	}

	SetControlValues();

	ActivateKeyboardHook(TRUE);

	return TRUE;
}

void CMainDlg::OnDestroy()
{
	CDialog::OnDestroy();
	GetControlValues();

	CComboBox* pCombo = NULL;
	VERIFY(pCombo = (CComboBox*)GetDlgItem(IDC_IMAGE_EXT));
	while (pCombo->GetCount() > 0)
	{
		CLSID* pCLSID = (CLSID*)pCombo->GetItemData(0);
		delete pCLSID;
		pCombo->DeleteString(0);
	}
}

void CMainDlg::OnClose()
{

	ActivateKeyboardHook(0);
	ActivateSelectionHook(0);
	ActivateSelection(false);
	UnregisterHotKey(m_hWnd, HOTKEY_100_C);
	UnregisterHotKey(m_hWnd, HOTKEY_200_c);
	UnregisterHotKey(m_hWnd, HOTKEY_300_W);
	UnregisterHotKey(m_hWnd, HOTKEY_400_w);
	UnregisterHotKey(m_hWnd, HOTKEY_500_S);
	UnregisterHotKey(m_hWnd, HOTKEY_600_s);

	m_NID.hIcon = NULL;
	m_NID.uFlags = NIF_ICON;
	::Shell_NotifyIcon(NIM_DELETE, &m_NID);

	DestroyCursor(m_hSelectCursor);

	if (m_pCanvasDialog != nullptr) {
		delete m_pCanvasDialog;
	}

	CDialog::OnClose();
}

void CMainDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED)
	{

		ShowWindow(SW_HIDE);
	}

}

void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	if ((nID & 0xFFF0) == SC_CLOSE)
	{
		ShowWindow(SW_HIDE);
	}
	else {

		if ((nID & 0xFFF0) == IDM_ABOUTBOX)
		{
			CAboutDlg dlg;
			dlg.DoModal();
		}
		else {

			CDialog::OnSysCommand(nID, lParam);
		}
	}

}

LRESULT CMainDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	UINT uID = (UINT)wParam;
	UINT uMsg = (UINT)lParam;

	if (uID != IDR_TRAY_MENU)
		return 0;

	if (uMsg == WM_LBUTTONDBLCLK)
	{

		ShowWindow(SW_RESTORE);

		return 1;
	}

	if (uMsg == WM_RBUTTONUP)
	{

		CMenu TrayMenu;
		CMenu* pSubMenu;
		if (!TrayMenu.LoadMenu(IDR_TRAY_MENU))
			return 0;
		if (!(pSubMenu = TrayMenu.GetSubMenu(0)))
			return 0;

		pSubMenu->SetDefaultItem(ID_SHOW);

		CPoint pos;
		GetCursorPos(&pos);
		SetForegroundWindow();
		::TrackPopupMenu(pSubMenu->m_hMenu, 0, pos.x, pos.y, 0, m_hWnd, NULL);

		PostMessage(WM_NULL, 0, 0);

		TrayMenu.DestroyMenu();

		return 1;
	}

	return 0;
}

void CMainDlg::OnCbnSelchangeImageExt()
{
	CComboBox* pCombo = NULL;
	VERIFY(pCombo = (CComboBox*)GetDlgItem(IDC_IMAGE_EXT));
	int iSel = pCombo->GetCurSel();
	ASSERT(iSel != -1);
	m_clsidImgType = *(CLSID*)pCombo->GetItemData(iSel);
}

void CMainDlg::OnBnClickedScreenDraw()
{
	m_bSelecting = FALSE;

	ShowWindow(SW_HIDE);
	ActivateSelectionHook(TRUE);
}

void CMainDlg::OnBnClickedBrowseImgPath()
{

	LPMALLOC pMalloc;
	if (!(SUCCEEDED(::SHGetMalloc(&pMalloc))))
		return;

	CWnd* pWnd = GetDlgItem(IDC_IMAGE_PATH);
	ASSERT(pWnd);
	CString strOldPath = _T("");
	pWnd->GetWindowText(strOldPath);

	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));
	bi.hwndOwner = m_hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN;
	CString strTitle = _T("Select Image Directoy");
	bi.lpszTitle = strTitle;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)pWnd;

	LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
	if (pidl != NULL)
	{
		CString strNewPath = _T("");
		::SHGetPathFromIDList(pidl, strNewPath.GetBuffer(255));
		strNewPath.ReleaseBuffer();
		if (strNewPath.Right(1) != _T("\\"))
			strNewPath += _T("\\");
		if (strOldPath.CompareNoCase(strNewPath) != 0)
			pWnd->SetWindowText(strNewPath);
		m_strImagePath = strNewPath;

		pMalloc->Free(pidl);
	}
	pMalloc->Release();
}

void CMainDlg::OnEnKillfocusLineWidth()
{
	CEdit* pEdit = NULL;
	VERIFY(pEdit = (CEdit*)GetDlgItem(IDC_LINE_WIDTH));
	CString strVal;
	pEdit->GetWindowText(strVal);
	int iVal = _tstoi(strVal);
	if ((iVal < 1) || (iVal > 15))
	{
		MessageBox(_T("Line Width must be between 1 and 15"),
			_T("Screen Tool"), MB_ICONERROR);
		pEdit->SetSel(0, -1);
		pEdit->SetFocus();
	}
	m_iBandPx = iVal;
}

LRESULT CMainDlg::OnDrawComplete(WPARAM wParam, LPARAM lParam)
{
	USES_CONVERSION;

	ActivateSelectionHook(FALSE);
	ActivateSelection(FALSE);

	return 0;
}

LRESULT CMainDlg::OnMouseHookCancel(WPARAM wParam, LPARAM lParam)
{

	ActivateSelectionHook(FALSE);
	return 0;
}

LRESULT CMainDlg::OnStartSelectRegion(WPARAM wParam, LPARAM lParam)
{

	ActivateSelection(TRUE);
	return 0;
}

LRESULT CMainDlg::OnSelectionComplete(WPARAM wParam, LPARAM lParam)
{

	ActivateSelectionHook(FALSE);

	HBITMAP hScreenshotBmp = NULL;
	if ((m_hHilightWnd) && (::IsWindow(m_hHilightWnd)))
		hScreenshotBmp = GetScreenshot(m_hHilightWnd, NULL);

	if (!hScreenshotBmp)
		return -1;

	CString strFile = GetOutputFileName();

	CString strFilePath = m_strImagePath;
	strFilePath += strFile;

	SaveImageAs(hScreenshotBmp, strFilePath);

	::DeleteObject(hScreenshotBmp);

	return 0;
}

LRESULT CMainDlg::OnPrintScreen(WPARAM wParam, LPARAM lParam)
{
	GropDeskScreenshot();

	return 0;
}

void CMainDlg::OnTrayShow()
{

	ShowWindow(SW_RESTORE);
}

void CMainDlg::OnShutdown()
{

	m_NID.hIcon = NULL;
	m_NID.uFlags = NIF_ICON;
	::Shell_NotifyIcon(NIM_DELETE, &m_NID);

	PostMessage(WM_CLOSE);
}

void CMainDlg::OnShowAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CMainDlg::OnDrawSelection()
{

	ActivateSelectionHook(TRUE);
}

void CMainDlg::ActivateCanvasWindow() {

	if (m_pCanvasDialog == nullptr) {

		m_pCanvasDialog = new Canvas(CanvasOperation::getRegion, this);

		if (nullptr == m_pCanvasDialog->m_hWnd) {
			m_pCanvasDialog->Create(IDD_CANVAS_DIALOG, CWnd::GetDesktopWindow());
		}
		m_pCanvasDialog->ShowWindow(SW_MAXIMIZE);
	}
	else {
		if (m_pCanvasDialog == nullptr) {
			return;
		}
		m_pCanvasDialog->DestroyWindow();
		m_pCanvasDialog = nullptr;
	}

}

afx_msg void CMainDlg::OnLatestRectangularSnapshot()
{
	LatestRectangularSnapshot();
}

afx_msg void CMainDlg::OnRegionSelect() {
	m_bGetScreenShot = false;
	ActivateCanvasWindow();
}

void CMainDlg::OnMouseHookLButtonDown(UINT nFlags, CPoint point)
{

	if (m_bSelecting)
		return;

	m_ptFirst = point;
	m_ptLast = point;

	m_bDitherLast = FALSE;

	m_bDrawing = TRUE;
}

void CMainDlg::OnMouseHookMouseMove(UINT nFlags, CPoint point)
{

	::SetCursor(m_hSelectCursor);

	if (m_bSelecting)
	{

		HWND hFound = ::WindowFromPoint(point);

		{
			HWND hOld = m_hHilightWnd;
			m_hHilightWnd = hFound;

			if ((hOld) && (::IsWindow(hOld)))
				RefreshWindow(hOld);

			HighlightWindow(m_hHilightWnd);
		}
	}

	if (m_bDrawing)
	{

		DrawFocusRect(point);

		m_ptLast = point;
	}
}

void CMainDlg::OnMouseHookLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bSelecting)
	{

		HWND hFound = ::WindowFromPoint(point);
		if (m_hHilightWnd != hFound)
		{
			HWND hOld = m_hHilightWnd;
			m_hHilightWnd = hFound;

			if ((hOld) && (::IsWindow(hOld)))
				RefreshWindow(hOld);
		}
		else
		{
			if ((m_hHilightWnd) && (::IsWindow(m_hHilightWnd)))
				RefreshWindow(m_hHilightWnd);
		}

		m_bSelecting = FALSE;

		PostMessage(WM_WINSELECT_DONE, (WPARAM)0, (LPARAM)0);
	}

	if (m_bDrawing)
	{

		DrawFocusRect(point, TRUE);

		m_bDrawing = FALSE;

		PostMessage(WM_SCREENDRAW_DONE, (WPARAM)0, (LPARAM)0);
	}
}

void CMainDlg::OnMouseHookRButtonDown(UINT nFlags, CPoint point)
{

	if (m_bSelecting)
	{
		RefreshWindow(m_hHilightWnd);
		m_bSelecting = FALSE;
	}
	if (m_bDrawing)
	{
		DrawFocusRect(point, TRUE);
		m_bDrawing = FALSE;
	}
	PostMessage(WM_MOUSEHOOK_CANCEL, (WPARAM)0, (LPARAM)0);
}
CString CMainDlg::GetOutputFileName()
{
	CString strFile = _T("");

	GetControlValues();

	switch (m_iNamingOpt)
	{
	case 0: // Use entered prefix
	{
		strFile = m_strImageName;
		break;
	}
	case 1: // Always ask for prefix
	{
		CGetDataDlg dlg;
		dlg.SetTitle(_T("Enter File Prefix"));
		if (dlg.DoModal() == IDOK)
			strFile = dlg.GetData();
		break;
	}
	case 2: // No prefix
		break;
	}
	if (strFile.GetLength() > 0)
		strFile += _T("_");
	COleDateTime dtNow = COleDateTime::GetCurrentTime();
	CString strDT = dtNow.Format(_T("%d-%m-%y_%H-%M-%S"));
	strFile += strDT;

	CComboBox* pCombo = NULL;
	VERIFY(pCombo = (CComboBox*)GetDlgItem(IDC_IMAGE_EXT));
	strFile += _T(".");
	CString strExt = _T("");
	pCombo->GetWindowText(strExt);
	strFile += strExt;

	return strFile;
}

BOOL CMainDlg::ActivateSelectionHook(BOOL bActivate)
{

	ActivateCanvasWindow();

	return TRUE;
}

BOOL CMainDlg::ActivateSelection(BOOL bActivate)
{

	if (bActivate)
	{
		m_hMouseHook = ::SetWindowsHookEx(WH_MOUSE_LL,
			(HOOKPROC)LowLevelMouseProc, GetModuleHandle(NULL), 0);
		if (!m_hMouseHook)
			return FALSE;

	}
	else
	{
		if (m_hMouseHook)
		{
			::UnhookWindowsHookEx(m_hMouseHook);
			m_hMouseHook = NULL;
		}
	}

	return TRUE;
}

BOOL CMainDlg::ActivateKeyboardHook(BOOL bActivate)
{

	if (m_bKeyboardHook == bActivate)
		return TRUE;

	if (bActivate)
	{

		m_hKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD_LL,
			(HOOKPROC)LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
		if (!m_hKeyboardHook)
			return FALSE;
	}
	else
	{

		if (m_hKeyboardHook)
		{
			::UnhookWindowsHookEx(m_hKeyboardHook);
			m_hKeyboardHook = NULL;
		}
	}

	m_bKeyboardHook = bActivate;

	return TRUE;
}

void CMainDlg::DrawFocusRect(CPoint pt, BOOL bRemoveRect)
{
	CSize szFrame(m_iBandPx, m_iBandPx);
	CSize szLast = szFrame;
	if (bRemoveRect)
		szFrame.cx = szFrame.cy = 0;

	CBrush* pWhiteBrush = CBrush::FromHandle
	((HBRUSH)::CreateSolidBrush(RGB(255, 255, 255)));
	CBrush* pDitherBrush = CDC::GetHalftoneBrush();
	CBrush* pBrush = pWhiteBrush;

	CDC* pDC;
	CRect rNow(m_ptFirst, pt);
	rNow.NormalizeRect();
	CRect rLast(m_ptFirst, m_ptLast);
	rLast.NormalizeRect();
	HDC hDC = ::GetDC(m_pCanvasDialog->m_hWnd);
	pDC = new CDC;
	pDC->Attach(hDC);

	pDC->DrawDragRect(&rNow, szFrame, &rLast, szLast,
		pBrush, m_bDitherLast ? pDitherBrush : pWhiteBrush);

	m_bDitherLast = (pBrush == pDitherBrush);
}

HBITMAP CMainDlg::GetScreenshot(HWND hWnd, CRect* pRect/*=NULL*/)
{

	HDC hDC = ::GetWindowDC(hWnd);
	HDC hCaptureDC = ::CreateCompatibleDC(hDC);
	int nBPP = ::GetDeviceCaps(hDC, BITSPIXEL);

	CRect rCapture;
	if (pRect)
		rCapture.CopyRect(pRect);
	else
		::GetWindowRect(hWnd, rCapture);

	BITMAPINFO bmiCapture = {
		sizeof(BITMAPINFOHEADER),
		rCapture.Width(), -rCapture.Height(),
		1, nBPP, BI_RGB, 0, 0, 0, 0, 0,
	};
	LPBYTE lpCapture;
	HBITMAP hbmCapture = CreateDIBSection(hDC, &bmiCapture,
		DIB_PAL_COLORS, (LPVOID*)&lpCapture, NULL, 0);
	if (!hbmCapture)
	{
		::DeleteDC(hCaptureDC);
		::DeleteDC(hDC);
		return NULL;
	}

	int nCapture = ::SaveDC(hCaptureDC);
	::SelectObject(hCaptureDC, hbmCapture);
	if (pRect)
		::BitBlt(hCaptureDC, 0, 0, rCapture.Width(), rCapture.Height(), hDC, rCapture.left, rCapture.top, SRCCOPY);
	else
		::BitBlt(hCaptureDC, 0, 0, rCapture.Width(), rCapture.Height(), hDC, 0, 0, SRCCOPY);
	::RestoreDC(hCaptureDC, nCapture);
	::DeleteDC(hCaptureDC);
	::DeleteDC(hDC);

	return hbmCapture;
}

HBITMAP CMainDlg::CropDDB(HBITMAP hSrcBmp, CRect rCrop)
{

	HDC hDesktopDC = ::GetWindowDC(::GetDesktopWindow());
	HDC hSrcDC = ::CreateCompatibleDC(hDesktopDC);
	if (!hSrcDC)
		return NULL;
	::SelectObject(hSrcDC, hSrcBmp);

	HDC hCropDC = ::CreateCompatibleDC(hDesktopDC);
	if (!hCropDC)
	{
		::DeleteDC(hSrcDC);
		return NULL;
	}

	int nBPP = ::GetDeviceCaps(hSrcDC, BITSPIXEL);
	BITMAPINFO bmiCrop = {
		sizeof(BITMAPINFOHEADER),
		rCrop.Width(), -rCrop.Height(),
		1, nBPP, BI_RGB, 0, 0, 0, 0, 0,
	};
	LPBYTE lpCrop;
	HBITMAP hbmCrop = CreateDIBSection(hCropDC, &bmiCrop,
		DIB_PAL_COLORS, (LPVOID*)&lpCrop, NULL, 0);
	if (!hbmCrop)
	{
		::DeleteDC(hSrcDC);
		::DeleteDC(hCropDC);
		return NULL;
	}

	::SelectObject(hCropDC, hbmCrop);
	::BitBlt(hCropDC, 0, 0, rCrop.Width(), rCrop.Height(), hSrcDC, rCrop.left, rCrop.top, SRCCOPY);
	::DeleteDC(hSrcDC);
	::DeleteDC(hCropDC);

	return hbmCrop;
}

int CMainDlg::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

BOOL CMainDlg::SaveImageAs(HBITMAP hBmp, CString strFile)
{
	USES_CONVERSION;

	Bitmap* pScreenShot = new Bitmap(hBmp, (HPALETTE)NULL);

	BOOL bRes = (pScreenShot->Save(T2W(strFile), &m_clsidImgType, NULL) == Ok);

	delete pScreenShot;

	return bRes;
}

void CMainDlg::UpdateToolTip(CString strToolTip)
{
	m_NID.uFlags = NIF_TIP;
	_tcscpy(m_NID.szTip, strToolTip);
	::Shell_NotifyIcon(NIM_MODIFY, &m_NID);
}

void CMainDlg::HighlightWindow(HWND hWnd)
{

	RECT rect;
	::GetWindowRect(hWnd, &rect);

	HDC hDC = ::GetWindowDC(hWnd);
	if (!hDC)
		return;
	HPEN hPen = ::CreatePen(PS_SOLID, m_iBandPx, RGB(255, 0, 0));
	HGDIOBJ hPrevPen = ::SelectObject(hDC, hPen);
	HGDIOBJ hPrevBrush = ::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));

	::Rectangle(hDC, 0, 0, (rect.right - rect.left), (rect.bottom - rect.top));

	::SelectObject(hDC, hPrevPen);
	::SelectObject(hDC, hPrevBrush);
	::DeleteObject(hPen);
	::ReleaseDC(hWnd, hDC);
}

void CMainDlg::RefreshWindow(HWND hWnd)
{
	::InvalidateRect(hWnd, NULL, TRUE);
	::UpdateWindow(hWnd);
	::RedrawWindow(hWnd, NULL, NULL, RDW_FRAME |
		RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

LRESULT CALLBACK CMainDlg::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return ::CallNextHookEx(m_pThis->m_hMouseHook, nCode, wParam, lParam);

	MSLLHOOKSTRUCT* pMS = (MSLLHOOKSTRUCT*)lParam;
	ASSERT(pMS);
	CPoint point(pMS->pt);

	switch (wParam)
	{
	case WM_MOUSEMOVE:
		/*if (m_pThis->m_bMouseHook)*/ {
		m_pThis->OnMouseHookMouseMove(0, point);

	}
		break;
	case WM_LBUTTONDOWN:
		/*if (m_pThis->m_bMouseHook)*/ {
		m_pThis->OnMouseHookLButtonDown(0, point);

	}
		break;
	case WM_LBUTTONUP:
		/*if (m_pThis->m_bMouseHook)*/ {
		m_pThis->OnMouseHookLButtonUp(0, point);

	}
		break;
	case WM_RBUTTONDOWN:
		/*if (m_pThis->m_bMouseHook)*/ {
		m_pThis->OnMouseHookRButtonDown(0, point);

	}
		break;
	}

	return ::CallNextHookEx(m_pThis->m_hMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK CMainDlg::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return ::CallNextHookEx(m_hKeyboardHook, nCode, wParam, lParam);

	KBDLLHOOKSTRUCT* pKS = (KBDLLHOOKSTRUCT*)lParam;
	ASSERT(pKS);

	TRACE2("vkCode=%0X\nflags=%0X\n\n", pKS->vkCode, pKS->flags);

	if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN))
	{
		if (pKS->vkCode == VK_SNAPSHOT)
		{
			if (pKS->flags & LLKHF_ALTDOWN)
			{
				m_pThis->PostMessage(WM_PRINTSCREEN, 1, 0);
			}
			else
			{
				m_pThis->PostMessage(WM_PRINTSCREEN, 0, 0);
			}
			return 1;
		}
	}

	return ::CallNextHookEx(m_hKeyboardHook, nCode, wParam, lParam);
}

static int __stdcall BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	TCHAR strInitDir[MAX_PATH] = { NULL };
	if (uMsg == BFFM_INITIALIZED)
	{
		CString strInitDir = _T("");
		CWnd* pWnd = (CWnd*)lpData;
		pWnd->GetWindowText(strInitDir);
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE,
			(LPARAM)strInitDir.GetBuffer(255));
		strInitDir.ReleaseBuffer();
	}
	return 0;
}

void CMainDlg::ReadRegistrySettings()
{
	CRegistry reg(HKEY_CURRENT_USER, _T("Software\\TOOLS\\ScreenshotTools"));
	m_bMinToTray = reg.GetRegInt(_T("MinToTray"), m_bMinToTray);
	m_iBandPx = reg.GetRegInt(_T("LineWidth"), m_iBandPx);
	reg.GetRegStr(_T("ImagePath"), m_strImagePath.GetBuffer(255), m_strImagePath);
	m_strImagePath.ReleaseBuffer();
	reg.GetRegStr(_T("ImageName"), m_strImageName.GetBuffer(255), m_strImageName);
	m_strImageName.ReleaseBuffer();
	m_iNamingOpt = reg.GetRegInt(_T("NamingOption"), m_iNamingOpt);
}

void CMainDlg::WriteRegistrySettings()
{
	CRegistry reg(HKEY_CURRENT_USER, _T("Software\\TOOLS\\ScreenshotTools"));
	reg.SetRegInt(_T("MinToTray"), m_bMinToTray);
	reg.SetRegInt(_T("LineWidth"), m_iBandPx);
	reg.SetRegStr(_T("ImagePath"), (LPTSTR)(LPCTSTR)m_strImagePath);
	reg.SetRegStr(_T("ImageName"), (LPTSTR)(LPCTSTR)m_strImageName);
	reg.SetRegInt(_T("NamingOption"), m_iNamingOpt);
}

void CMainDlg::GetControlValues()
{
	CWnd* pWnd = NULL;
	VERIFY(pWnd = (CEdit*)GetDlgItem(IDC_LINE_WIDTH));
	CString strTemp;
	pWnd->GetWindowText(strTemp);
	m_iBandPx = _tstoi(strTemp);
	VERIFY(pWnd = (CEdit*)GetDlgItem(IDC_IMAGE_PATH));
	pWnd->GetWindowText(m_strImagePath);
	if (m_strImagePath.Right(1) != _T('\\'))
		m_strImagePath += _T('\\');
	VERIFY(pWnd = GetDlgItem(IDC_IMAGE_NAME));
	pWnd->GetWindowText(m_strImageName);

	if (((CButton*)GetDlgItem(IDC_OPT_IMGNAME_1))->GetCheck() == 1)
		m_iNamingOpt = 0;
	else if (((CButton*)GetDlgItem(IDC_OPT_IMGNAME_2))->GetCheck() == 1)
		m_iNamingOpt = 1;
	else
		m_iNamingOpt = 2;
}

void CMainDlg::SetControlValues()
{
	CEdit* pEdit = NULL;
	VERIFY(pEdit = (CEdit*)GetDlgItem(IDC_LINE_WIDTH));
	CString strVal;
	strVal.Format(_T("%d"), m_iBandPx);
	pEdit->SetWindowText(strVal);
	VERIFY(pEdit = (CEdit*)GetDlgItem(IDC_IMAGE_PATH));
	pEdit->SetWindowText(m_strImagePath);
	VERIFY(pEdit = (CEdit*)GetDlgItem(IDC_IMAGE_NAME));
	pEdit->SetWindowText(m_strImageName);

	switch (m_iNamingOpt) {
	case 0: ((CButton*)GetDlgItem(IDC_OPT_IMGNAME_1))->SetCheck(1);  break;
	case 1: ((CButton*)GetDlgItem(IDC_OPT_IMGNAME_2))->SetCheck(1);  break;
	case 2: ((CButton*)GetDlgItem(IDC_OPT_IMGNAME_3))->SetCheck(1);  break;
	}
}

