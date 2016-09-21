
// LogServerTesterDlg.h : header file
//

#pragma once

#include "../lib/rpc/include/rpcclient.h"

// CLogServerTesterDlg dialog
class CLogServerTesterDlg : public CDialogEx
{
// Construction
public:
	CLogServerTesterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LOGSERVERTESTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CRpcClient m_rpcClient;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnCallAppEvent();
};
