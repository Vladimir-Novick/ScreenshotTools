#pragma once


// TransparentWindowDlg dialog

class CTransparentWindowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTransparentWindowDlg)

public:
	CTransparentWindowDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTransparentWindowDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRANSPARENTWINDOW_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
