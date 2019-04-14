#include "stdafx.h"
#include "ShortcutProvider.h"
#include <Windows.h>
#include <shlobj.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>

void ShortcutProvider::SetDesktopShortcut(bool enable, LPSTR pszShotcutName, LPSTR pszDescription = "") {
	TCHAR szCurrentDirectory[MAX_PATH];
	TCHAR szExeFileName[MAX_PATH];
	GetModuleFileName(NULL, szExeFileName, MAX_PATH);
	 GetCurrentDirectory(MAX_PATH, szCurrentDirectory);

	CString		pszLinkfile;
	GetDesktopPath( pszLinkfile);

	pszLinkfile += pszShotcutName;

	pszLinkfile  += ".lnk";

	int nLen = pszLinkfile.GetLength();
	LPTSTR lpszLinkFileName = pszLinkfile.GetBuffer(nLen);

	if (enable) {
		CreateShortcut(szExeFileName, "",
			lpszLinkFileName, pszDescription
		);
	}
	else {
		DeleteFile(pszLinkfile);
	}

}

void ShortcutProvider::GetDesktopPath(CString &m_strDesktopPath)
{
	HRESULT hr = NOERROR;
	LPITEMIDLIST pidl = 0;
	if (SUCCEEDED(hr = ::SHGetSpecialFolderLocation(m_hWnd, CSIDL_DESKTOP, &pidl)))
	{
		if (pidl)
		{
			::SHGetPathFromIDList(pidl, m_strDesktopPath.GetBuffer(MAX_PATH));
			m_strDesktopPath.ReleaseBuffer();

			if (m_strDesktopPath.GetLength() == 0)
				m_strDesktopPath = _T("C:\\");

			if (m_strDesktopPath.Right(1) != _T('\\'))
				m_strDesktopPath += _T('\\');

		}
	}
}

BOOL ShortcutProvider::IsDesktopShortcut(LPSTR pszShotcutName) {
	CString		pszLinkfile;
	GetDesktopPath(pszLinkfile);

	pszLinkfile += pszShotcutName;

	pszLinkfile += ".lnk";

	int nLen = pszLinkfile.GetLength();
	LPTSTR lpszLinkFileName = pszLinkfile.GetBuffer(nLen);

	return PathFileExistsA(lpszLinkFileName);
}

HRESULT ShortcutProvider::CreateShortcut(LPSTR pszTargetfile, LPSTR pszTargetargs,
	LPSTR pszLinkfile, LPSTR pszDescription ,
	int iShowmode , LPSTR pszCurdir,
	LPSTR pszIconfile , int iIconindex )
{
	HRESULT       hRes;                  /* Returned COM result code */
	IShellLink*   pShellLink;            /* IShellLink object pointer */
	IPersistFile* pPersistFile;          /* IPersistFile object pointer */
	WCHAR wszLinkfile[MAX_PATH]; /* pszLinkfile as Unicode
											string */
	int           iWideCharsWritten;     /* Number of wide characters
											written */
	CoInitialize(NULL);
	hRes = E_INVALIDARG;
	if (
		(pszTargetfile != NULL) && (strlen(pszTargetfile) > 0) &&
		(pszLinkfile != NULL) && (strlen(pszLinkfile) > 0) 
		)
	{
		hRes = CoCreateInstance(
			CLSID_ShellLink,     /* pre-defined CLSID of the IShellLink
									 object */
			NULL,                 /* pointer to parent interface if part of
									 aggregate */
			CLSCTX_INPROC_SERVER, /* caller and called code are in same
									 process */
			IID_IShellLink,      /* pre-defined interface of the
									 IShellLink object */
			(LPVOID*)&pShellLink);         /* Returns a pointer to the IShellLink
									 object */
		if (SUCCEEDED(hRes))
		{
			/* Set the fields in the IShellLink object */
			hRes = pShellLink->SetPath(pszTargetfile);
			hRes = pShellLink->SetArguments(pszTargetargs);
			if (strlen(pszDescription) > 0)
			{
				hRes = pShellLink->SetDescription(pszDescription);
			}
			if (iShowmode > 0)
			{
				hRes = pShellLink->SetShowCmd(iShowmode);
			}
			if (strlen(pszCurdir) > 0)
			{
				hRes = pShellLink->SetWorkingDirectory(pszCurdir);
			}
			if (strlen(pszIconfile) > 0 && iIconindex >= 0)
			{
				hRes = pShellLink->SetIconLocation(pszIconfile, iIconindex);
			}

			/* Use the IPersistFile object to save the shell link */
			hRes = pShellLink->QueryInterface(
				IID_IPersistFile,         /* pre-defined interface of the
											  IPersistFile object */
				(LPVOID*)&pPersistFile);            /* returns a pointer to the
											  IPersistFile object */
			if (SUCCEEDED(hRes))
			{
				iWideCharsWritten = MultiByteToWideChar(CP_ACP, 0,
					pszLinkfile, -1,
					wszLinkfile, MAX_PATH);
				hRes = pPersistFile->Save(wszLinkfile, TRUE);
				pPersistFile->Release();
			}
			pShellLink->Release();
		}

	}
	CoUninitialize();
	return (hRes);
}

ShortcutProvider::ShortcutProvider(HWND hWnd)
{
	m_hWnd = hWnd;
}

ShortcutProvider::~ShortcutProvider()
{
}
