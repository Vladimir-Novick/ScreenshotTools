#ifndef ___CANVAS_H___
#define ___CANVAS_H___

#pragma once

// Canvas dialog

#include "CanvasOperation.h"

class Canvas : public CDialog
{
	DECLARE_DYNAMIC(Canvas)

public:
	Canvas(CanvasOperation Operation = CanvasOperation::getRegion, CWnd *mainDlg = nullptr, CWnd* pParent = nullptr);   // standard constructor
	virtual ~Canvas();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnInitDialog();
	CanvasOperation Operation;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CANVAS_DIALOG };
#endif

protected:
	bool m_bCropScreenshot;
	CWnd *mainDialog;
	static Canvas* m_pThis;

	afx_msg void Canvas::OnClose();

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	virtual void PostNcDestroy();
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	LRESULT OnActive(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#endif