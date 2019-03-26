
#pragma once
#ifndef ScreenshotTools_h
#define ScreenshotTools_h

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif
#include "resource.h"	
#include "ScreenshotTools_i.h"


class CApp : public CWinApp
{
public:
	CApp();

protected:
	// GDI+
	GDIPSI		m_GDIPSI;
	ULONG_PTR	m_GDIPlusToken;

public:
	// Overrides
	virtual BOOL InitInstance();

	// Messages
	DECLARE_MESSAGE_MAP()
	BOOL ExitInstance();
};

extern CApp theApp;

#endif