// ==========================================================================
// AboutDlg.cpp :
// ==========================================================================
#include "stdafx.h"
#include "AboutDlg.h"
#include ".\aboutdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
ON_MESSAGE(WM_ACTIVATE, OnActive)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

//=================================================
// Construction / Destruction
//=================================================
CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
}

CAboutDlg::~CAboutDlg()
{
}

afx_msg LRESULT CAboutDlg::OnActive(WPARAM wParam, LPARAM lParam) {
	if (LOWORD(wParam) != WA_ACTIVE) {
		EndDialog(0);
	}

	return 1;
}

//=================================================
// Overrides
//=================================================
BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Load the icon
	HICON hIcon = NULL;
	VERIFY(hIcon = AfxGetApp()->LoadIcon(IDI_ABOUT));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	//// Make window semi-transparent
	//ModifyStyleEx(0, WS_EX_LAYERED);
	//::SetLayeredWindowAttributes(m_hWnd, 
	//	RGB(255,0,255),	215, LWA_COLORKEY|LWA_ALPHA);



	return FALSE;
}






