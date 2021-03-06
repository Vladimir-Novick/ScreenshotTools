// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define _ATL_APARTMENT_THREADED 
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Set Windows Version To WinXP
#define _WIN32_WINNT 0x0501  

#define _CRT_SECURE_NO_DEPRECATE 

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <gdiplus.h>
#include <afxcontrolbars.h>
#include <afxwin.h>
using namespace Gdiplus;
#pragma comment (lib, "Gdiplus.lib")
typedef GdiplusStartupInput GDIPSI;

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
