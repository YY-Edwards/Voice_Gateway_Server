
// LogServerTesterDlg.h : header file
//

#pragma once

#include "../lib/rpc/include/rpcclient.h"
#include "afxwin.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "../lib/utf8/utf8.h"
#define StartCall 0x01
#define StopCall 0x02
#define SetPlayCall 0x03
#define SendCallStatus 0x04

extern unsigned long long g_sn;
extern HWND g_hwnd;

#define WM_UPDATE_DATA WM_USER+100
typedef struct
{
	int callType;
	unsigned long src;
	unsigned long tgt;
	int status;
}CALL_INFO;
typedef struct
{
	int cmd;
	union
	{
		CALL_INFO callInfo;
	}info;
}SERVER_REQUEST;
extern SERVER_REQUEST* g_pNewRequest;
extern std::list<SERVER_REQUEST*> g_requests;

#define HAVE_CALL_NO_PLAY 4
#define HAVE_CALL_START_PLAY 5
#define HAVE_CALL_END_PLAY 6
#define END_CALL_NO_PLAY 7
#define NEW_CALL_START 8
#define NEW_CALL_END 9

// CLogServerTesterDlg dialog
class TestWirlineScheduleDlg : public CDialogEx
{
// Construction
public:
	TestWirlineScheduleDlg(CWnd* pParent = NULL);	// standard constructor

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
	afx_msg void OnBnClickedBtnConfig();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnCallAppEvent();
	afx_msg void OnBnClickedRadiobtn();
	afx_msg void OnBnClickedCallbtn();
	afx_msg void OnBnClickedMsgbtn();
	afx_msg void OnBnClickedStopcallbtn();
	afx_msg void OnBnClickedPoweron();
	afx_msg void OnBnClickedGroupmsg();
	afx_msg void OnBnClickedPoweroff();
	afx_msg void OnBnClickedOnline();
	afx_msg void OnBnClickedWiretap();
	
	afx_msg void OnBnClickedCall();
	CEdit m_targetId;
	CComboBox m_callType;
	CButton m_stopCall;
	afx_msg void OnBnClickedStopCall();
	CButton m_call;
	CComboBox m_careCalls;
	afx_msg void OnBnClickedSetPlayCallOfCare();
	afx_msg LRESULT OnUpdateData(WPARAM w, LPARAM l);
	void HandleServerRequest();
private:
	static void OnServerRequest(CBaseConnector* pServer, const char* pData, int dataLen);
	static UINT HandleServerRequestProc(LPVOID pParam);
	bool m_bRunHandleServerProc;
	//std::map<std::string, int> m_callNames;
	BOOL m_bupdateData;
};
