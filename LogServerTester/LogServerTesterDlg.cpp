
// LogServerTesterDlg.cpp : implementation file
//

#include "stdafx.h"
//#include <vld.h> 
#include "LogServerTester.h"
#include "LogServerTesterDlg.h"
#include "afxdialogex.h"

#include "../lib/rpc/include/RpcJsonParser.h"
#include "../lib/utf8/utf8.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLogServerTesterDlg dialog



CLogServerTesterDlg::CLogServerTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLogServerTesterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLogServerTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLogServerTesterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CLogServerTesterDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_STOP, &CLogServerTesterDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_CALL_APP_EVENT, &CLogServerTesterDlg::OnBnClickedBtnCallAppEvent)
	ON_BN_CLICKED(IDC_RADIOBTN, &CLogServerTesterDlg::OnBnClickedRadiobtn)
	ON_BN_CLICKED(IDC_CALLBTN, &CLogServerTesterDlg::OnBnClickedCallbtn)
	ON_BN_CLICKED(IDC_MSGBTN, &CLogServerTesterDlg::OnBnClickedMsgbtn)
	ON_BN_CLICKED(IDC_STOPCALLBTN, &CLogServerTesterDlg::OnBnClickedStopcallbtn)
	ON_BN_CLICKED(IDC_POWERON, &CLogServerTesterDlg::OnBnClickedPoweron)
	ON_BN_CLICKED(IDC_GROUPMSG, &CLogServerTesterDlg::OnBnClickedGroupmsg)
	ON_BN_CLICKED(IDC_POWEROFF, &CLogServerTesterDlg::OnBnClickedPoweroff)
	ON_BN_CLICKED(IDC_ONLINE, &CLogServerTesterDlg::OnBnClickedOnline)
	ON_BN_CLICKED(IDC_WIRETAP, &CLogServerTesterDlg::OnBnClickedWiretap)
	ON_BN_CLICKED(IDC_BTN_RADIO_CONNECT, &CLogServerTesterDlg::OnBnClickedBtnRadioConnect)
END_MESSAGE_MAP()


// CLogServerTesterDlg message handlers

BOOL CLogServerTesterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//m_rpcClient = new CRpcClient();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLogServerTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLogServerTesterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLogServerTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


#define CONNSTR  "tcp://127.0.0.1:9000"
void CLogServerTesterDlg::OnBnClickedBtnConnect()
{
	m_rpcClient.start(CONNSTR);
}


void CLogServerTesterDlg::OnBnClickedBtnStop()
{
	m_rpcClient.stop();
}


void CLogServerTesterDlg::OnBnClickedBtnCallAppEvent()
{
	std::map<std::string, std::string> args;
	args["ip"] = "10.2.9.11";
	std::string callJsonStr = CRpcJsonParser::buildCall("start", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223,NULL, [](const char* pResponse , void* data){

		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}


void CLogServerTesterDlg::OnBnClickedRadiobtn()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strConenct = "{\"call\":\"connect\",\"param\":{\"radioIP\":\"192.168.10.2\",\"mnisIP\":\"\"},\"callId\":1}";
	std::map<std::string, std::string> args;
	args["radioIP"] = "192.168.10.2";
	args["mnisIP"] = "";
	std::string callJsonStr = CRpcJsonParser::buildCall("connect", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, NULL, [](const char* pResponse, void* data){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
	//m_rpcClient.send((PBYTE)strConenct.c_str(), strConenct.size() + 1);
}


void CLogServerTesterDlg::OnBnClickedCallbtn()
{
	// TODO:  在此添加控件通知处理程序代码
	
		//std::string strCall = "{\"call\":\"call\",\"param\":{\"id\":\"10\"},\"callId\":1}";
		//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("call", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, NULL, [](const char* pResponse, void* data){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();

	
}

void CLogServerTesterDlg::OnBnClickedMsgbtn()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strMsg = "{\"call\":\"sendSms\",\"param\":{\"id\":\"10\",\"msg\":\"上海计划信息系统有限公司\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strMsg.c_str(), strMsg.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	args["msg"] = "上海计划信息系统有限公司--单发";
	std::string callJsonStr = CRpcJsonParser::buildCall("sendSms", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, NULL, [](const char* pResponse, void* data){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}
void CLogServerTesterDlg::OnBnClickedGroupmsg()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strMsg = "{\"call\":\"sendGroupSms\",\"param\":{\"id\":\"1\",\"msg\":\"上海计划信息系统有限公司--组发\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strMsg.c_str(), strMsg.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "1";
	args["msg"] = "上海计划信息系统有限公司--组发";
	std::string callJsonStr = CRpcJsonParser::buildCall("sendGroupSms", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, NULL, [](const char* pResponse, void* data){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}

void CLogServerTesterDlg::OnBnClickedStopcallbtn()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strCall = "{\"call\":\"stopCall\",\"param\":{\"id\":\"10\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("stopCall", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, NULL,[](const char* pResponse, void* data){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();

}


void CLogServerTesterDlg::OnBnClickedPoweron()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strCall = "{\"call\":\"remotePowerOn\",\"param\":{\"id\":\"10\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("remotePowerOn", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, NULL, [](const char* pResponse, void* data){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}





void CLogServerTesterDlg::OnBnClickedPoweroff()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strCall = "{\"call\":\"remotePowerOff\",\"param\":{\"id\":\"10\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("remotePowerOff", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, NULL, [](const char* pResponse, void* data){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}


void CLogServerTesterDlg::OnBnClickedOnline()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strCall = "{\"call\":\"radioCheck\",\"param\":{\"id\":\"10\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("radioCheck", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, NULL, [](const char* pResponse, void* data){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}


void CLogServerTesterDlg::OnBnClickedWiretap()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strCall = "{\"call\":\"wiretap\",\"param\":{\"id\":\"10\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("wiretap", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, NULL, [](const char* pResponse, void* data){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}


void CLogServerTesterDlg::OnBnClickedBtnRadioConnect()
{
	std::map<std::string, std::string> args;
	args["radioIP"] = "192.168.10.2";
	args["mnisIP"] = "192.168.10.2";
	std::string callJsonStr = CRpcJsonParser::buildCall("connect", 123, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 123, NULL, [](const char* pResponse, void* data){

		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}
