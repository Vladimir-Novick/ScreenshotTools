// ==========================================================================
// AboutDlg.h
// ==========================================================================
#ifndef ___ABOUTDLG_H___
#define ___ABOUTDLG_H___
#pragma once

#include "Resource.h"

class CAboutDlg : public CDialog
{
public:
	DECLARE_DYNAMIC(CAboutDlg)

	// Construction / Destruction
	CAboutDlg(CWnd* pParent = NULL); 
	virtual ~CAboutDlg();

	LRESULT OnActive(WPARAM wParam, LPARAM lParam);

	enum { IDD = IDD_ABOUT };

protected:

public:
	// Overrides
	virtual BOOL OnInitDialog();

protected:

	// Windows Message Handlers
	DECLARE_MESSAGE_MAP()

};

#endif // ___ABOUTDLG_H___