// ==========================================================================
// MainDlg.h : 
// ==========================================================================
#ifndef ___MAINDLG_H___
#define ___MAINDLG_H___
#pragma once

#include "Canvas.h"

class CMainDlg : public CDialog
{
public:
	// Construction
	CMainDlg(CWnd* pParent = NULL);

protected:	
	// Dialog Data
	enum { IDD = IDD_MAIN_DIALOG };

	bool bMinimized_;
	bool m_bToClipboardEnabled;
	HMENU popupMenu;
	bool m_bGetScreenShot;

	HICON m_hIcon; 

	HCURSOR		m_hSelectCursor;
	
	BOOL		m_bKeyboardHook;
	BOOL		m_bDrawing;
	BOOL		m_bSelecting;
	
	BOOL		m_bDitherLast;
	CPoint		m_ptFirst;
	CPoint		m_ptLast;
	HWND		m_hHilightWnd;

	int			m_iBandPx;

	CLSID		m_clsidImgType;

	// Properties
	CString		m_strImagePath;
	CString		m_strImageName;
	BOOL		m_bMinToTray;
	int			m_iNamingOpt;

	// Tray Icon 
	NOTIFYICONDATA	m_NID;

	// Hooks
	static HHOOK m_hMouseHook;
	static HHOOK m_hKeyboardHook;
	static CMainDlg* m_pThis;

public:
	// Overrides
	virtual BOOL OnInitDialog();
	BOOL		m_bMouseHook;

	// Persistence
	void ReadRegistrySettings();
	void WriteRegistrySettings();

protected:
	Canvas* m_pCanvasDialog;
	// Mouse Hook Notifications

	void OnMouseHookLButtonDown(UINT nFlags, CPoint point);
	void OnMouseHookLButtonUp(UINT nFlags, CPoint point);
	void OnMouseHookMouseMove(UINT nFlags, CPoint point);
	void OnMouseHookRButtonDown(UINT nFlags, CPoint point);

	LRESULT OnMouseHookCancel(WPARAM wParam, LPARAM lParam);
	LRESULT OnStartSelectRegion(WPARAM wParam, LPARAM lParam);

	CString GetOutputFileName();

	LRESULT LatestRectangularSnapshot();
	LRESULT SnipDeskScreenshot();

	LRESULT CutRegionToFile(CPoint ptFirst, CPoint ptLast);

	BOOL ActivateSelectionHook(BOOL bActivate);

	BOOL ActivateKeyboardHook(BOOL bActivate);
	void DrawFocusRect(CPoint pt, BOOL bRemoveRect = FALSE);
	HBITMAP GetScreenshot(HWND hWnd, CRect* pRect = NULL);
	HBITMAP CropDDB(HBITMAP hSrcBmp, CRect rCrop);
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid); 
	BOOL SaveImageAs(HBITMAP hBmp, CString strFile); 

	void UpdateToolTip(CString strToolTip);
	void HighlightWindow(HWND hWnd);
	void RefreshWindow(HWND hWnd);
	void GetControlValues();
	void SetControlValues();
	CBitmap* ConvertIconToBitmap(HICON hIcon);

	// Windows Messages
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnActive(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGropScreenshot(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnQueryEndSession(WPARAM wParm, LPARAM lParm);
	afx_msg void OnCbnSelchangeImageExt();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnBtnClickedScreenDraw();
	afx_msg void OnBtnSnipToClipboard();
	afx_msg void OnBtnClickedBrowseImgPath();
	afx_msg void OnEnKillfocusLineWidth();
	afx_msg void  OnBtnSnipDeskScreenshot();

	// Our Messages
	LRESULT OnDrawComplete(WPARAM wParam, LPARAM lParam);
	LRESULT OnSelectionComplete(WPARAM wParam, LPARAM lParam);
	LRESULT OnPrintScreen(WPARAM wParam, LPARAM lParam);
	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);

	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis);
	afx_msg void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis);
	HICON GetIconForItem(HMENU pMenu,UINT itemID) const;


	// Tray Menu Items
	afx_msg void OnTrayShow();
	afx_msg void OnShutdown();
	afx_msg void OnShowAbout();
	afx_msg void OnDrawAndSnipSelection();
	afx_msg void OnLatestRectangularSnapshot();
	afx_msg void OnRegionSelect();
	void ActivateCanvasWindow();
	BOOL ActivateSelection(BOOL bActivate);

	static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	// Hook Callback
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif // ___MAINDLG_H___
