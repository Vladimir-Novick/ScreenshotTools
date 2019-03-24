// TransparentWindowDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScreenshotTools.h"
#include "CTransparentWindowDlg.h"
#include "afxdialogex.h"


// TransparentWindowDlg dialog

IMPLEMENT_DYNAMIC(CTransparentWindowDlg, CDialogEx)

CTransparentWindowDlg::CTransparentWindowDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TRANSPARENTWINDOW_DIALOG, pParent)
{

}

CTransparentWindowDlg::~CTransparentWindowDlg()
{
}

void CTransparentWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}



BOOL CTransparentWindowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();



	SetBackgroundColor(RGB(1, 11, 21));
	LONG ExtendedStyle = GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE);
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ExtendedStyle | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(GetSafeHwnd(), RGB(1, 11, 21), 255, LWA_COLORKEY);

	ShowWindow(SW_MAXIMIZE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CTransparentWindowDlg, CDialogEx)
END_MESSAGE_MAP()


// TransparentWindowDlg message handlers
