
// MonitorDlg.h : ͷ�ļ�
//

#pragma once
#include "MonitorServer.h"
#include "TServerMonitor.h"
#include "Settings.h"
#include <string.h>
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "../lib/rpc/include/RpcServer.h"

extern std::string serverName;

// CMonitorDlg �Ի���
class CMonitorDlg : public CDialogEx
{
// ����
public:
	CMonitorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	NOTIFYICONDATA m_nid;

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnSysTrayMsg(WPARAM w, LPARAM l);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
private:
	
	CTServerMonitor m_tserver;
	CRpcServer rpcServer;
	std::string getServerName();
	static void OnConnect(CRemotePeer* pRemotePeer);
	std::wstring getAppdataPath();
	void writeLog();
};
