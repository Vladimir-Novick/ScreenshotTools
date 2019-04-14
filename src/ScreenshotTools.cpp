/*

MIT License

Copyright (c) 2019 Vladimir Novick Copyright (C) 2005-2019 by Vladimir Novick

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
#include "MainDlg.h"
#include <initguid.h>
#include "ScreenshotTools_i.c"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



class CScreenshotToolsModule :
	public ATL::CAtlMfcModule
{
public:
	DECLARE_LIBID(LIBID_ScreenshotToolsLib);
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SCREENSHOTTOOLS, "{E37A72CF-9095-40D5-861C-9A9C683C82DF}");
};

CScreenshotToolsModule _AtlModule;

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	//ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

//=================================================
// CApp construction
//=================================================
CApp::CApp()
{
}

//=================================================
// Overrides
//=================================================
BOOL CApp::InitInstance()
{
	AfxOleInit();
	// Allow only 1 instance of this app
	HWND hWnd = NULL;
	if(hWnd = ::FindWindow(NULL, _T("Screenshot Tools")))
	{
		if((hWnd) && (::IsWindow(hWnd)))
		{
			if (!::IsWindowVisible(hWnd)) {
				::ShowWindow(hWnd, SW_RESTORE);
			}
//			if(::IsIconic(hWnd))
//				::ShowWindow(hWnd, SW_RESTORE);
			::SetForegroundWindow(hWnd);
			::BringWindowToTop(hWnd);
			return FALSE;
		}
		
	}

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	#if !defined(_WIN32_WCE) || defined(_CE_DCOM)
	// Register class factories via CoRegisterClassObject().
	if (FAILED(_AtlModule.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE)))
		return FALSE;
	#endif // !defined(_WIN32_WCE) || defined(_CE_DCOM)
	// App was launched with /Embedding or /Automation switch.
	// Run app as automation server.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Don't show the main window
		return TRUE;
	}
	// App was launched with /Unregserver or /Unregister switch.
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		_AtlModule.UpdateRegistryAppId(FALSE);
		_AtlModule.UnregisterServer(TRUE);
		return FALSE;
	}
	// App was launched with /Register or /Regserver switch.
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister)
	{
		_AtlModule.UpdateRegistryAppId(TRUE);
		_AtlModule.RegisterServer(TRUE);
		return FALSE;
	}

	// Startup GDI+
	GdiplusStartup(&m_GDIPlusToken, &m_GDIPSI, NULL);

	// Create main dialog and show it modal
	CMainDlg dlg;
	dlg.ReadRegistrySettings();
	m_pMainWnd = &dlg;

	dlg.DoModal();
	dlg.WriteRegistrySettings();
	
	// Cleanup GDI+
	GdiplusShutdown(m_GDIPlusToken);
    
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

//=================================================
// The one and only CApp object
//=================================================
CApp theApp;


BOOL CApp::ExitInstance()
{
#if !defined(_WIN32_WCE) || defined(_CE_DCOM)
	_AtlModule.RevokeClassObjects();
#endif
	return CWinApp::ExitInstance();
}
