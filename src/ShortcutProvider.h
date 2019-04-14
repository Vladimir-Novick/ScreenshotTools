#pragma once

class ShortcutProvider
{
protected:
		void GetDesktopPath(CString &m_strImagePath);
		HWND m_hWnd;
public:
	/*
	-------------------------------------------------------------------
	Description:
	Creates the actual 'lnk' file (assumes COM has been initialized).

	Parameters:
	pszTargetfile    - File name of the link's target.
	pszTargetargs    - Command line arguments passed to link's target.
	pszLinkfile      - File name of the actual link file being created.
	pszDescription   - Description of the linked item.
	iShowmode        - ShowWindow() constant for the link's target.
	pszCurdir        - Working directory of the active link.
	pszIconfile      - File name of the icon file used for the link.
	iIconindex       - Index of the icon in the icon file.

	Returns:
	HRESULT value >= 0 for success, < 0 for failure.
	--------------------------------------------------------------------
	*/
	HRESULT CreateShortcut(LPSTR pszTargetfile, LPSTR pszTargetargs,
		LPSTR pszLinkfile, LPSTR pszDescription = "",
		int iShowmode = 0, LPSTR pszCurdir = "",
		LPSTR pszIconfile = "", int iIconindex = 0);

	void SetDesktopShortcut(bool enable, LPSTR pszShotcutName, LPSTR pszDescription);

	BOOL IsDesktopShortcut(LPSTR m_shotcutName);

	ShortcutProvider(HWND hWnd);
	~ShortcutProvider();
};

