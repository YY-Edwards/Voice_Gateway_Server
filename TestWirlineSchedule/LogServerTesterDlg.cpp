
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


unsigned long long g_sn = 0;
HWND g_hwnd = NULL;
SERVER_REQUEST* g_pNewRequest = NULL;
std::list<SERVER_REQUEST*> g_requests;
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



TestWirlineScheduleDlg::TestWirlineScheduleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(TestWirlineScheduleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_hwnd = GetSafeHwnd();
	m_bRunHandleServerProc = false;
	m_bupdateData = false;
}

void TestWirlineScheduleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, TARGET_ID, m_targetId);
	DDX_Control(pDX, CALL_TYPE, m_callType);
	DDX_Control(pDX, STOP_CALL, m_stopCall);
	DDX_Control(pDX, INIT_CALL, m_call);
	DDX_Control(pDX, IDC_COMBO2, m_careCalls);
}

BEGIN_MESSAGE_MAP(TestWirlineScheduleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CONNECT, &TestWirlineScheduleDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_CONFIG, &TestWirlineScheduleDlg::OnBnClickedBtnConfig)
	ON_BN_CLICKED(IDC_BTN_STOP, &TestWirlineScheduleDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_CALL_APP_EVENT, &TestWirlineScheduleDlg::OnBnClickedBtnCallAppEvent)
	ON_BN_CLICKED(IDC_RADIOBTN, &TestWirlineScheduleDlg::OnBnClickedRadiobtn)
	ON_BN_CLICKED(IDC_CALLBTN, &TestWirlineScheduleDlg::OnBnClickedCallbtn)
	ON_BN_CLICKED(IDC_MSGBTN, &TestWirlineScheduleDlg::OnBnClickedMsgbtn)
	ON_BN_CLICKED(IDC_STOPCALLBTN, &TestWirlineScheduleDlg::OnBnClickedStopcallbtn)
	ON_BN_CLICKED(IDC_POWERON, &TestWirlineScheduleDlg::OnBnClickedPoweron)
	ON_BN_CLICKED(IDC_GROUPMSG, &TestWirlineScheduleDlg::OnBnClickedGroupmsg)
	ON_BN_CLICKED(IDC_POWEROFF, &TestWirlineScheduleDlg::OnBnClickedPoweroff)
	ON_BN_CLICKED(IDC_ONLINE, &TestWirlineScheduleDlg::OnBnClickedOnline)
	ON_BN_CLICKED(IDC_WIRETAP, &TestWirlineScheduleDlg::OnBnClickedWiretap)
	ON_BN_CLICKED(INIT_CALL, &TestWirlineScheduleDlg::OnBnClickedCall)
	ON_BN_CLICKED(STOP_CALL, &TestWirlineScheduleDlg::OnBnClickedStopCall)
	ON_BN_CLICKED(IDC_BUTTON1, &TestWirlineScheduleDlg::OnBnClickedSetPlayCallOfCare)
	ON_MESSAGE(WM_UPDATE_DATA,OnUpdateData)
END_MESSAGE_MAP()


// CLogServerTesterDlg message handlers

BOOL TestWirlineScheduleDlg::OnInitDialog()
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

void TestWirlineScheduleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void TestWirlineScheduleDlg::OnPaint()
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
HCURSOR TestWirlineScheduleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


#define CONNSTR  "tcp://127.0.0.1:9002"
void TestWirlineScheduleDlg::OnBnClickedBtnConnect()
{
	//m_callNames["Send_CARE_CALL_STATUS"] = CALL_NAME_Send_CARE_CALL_STATUS;
	if (!m_bRunHandleServerProc)
	{
		m_bRunHandleServerProc = true;
		AfxBeginThread(HandleServerRequestProc, this);
	}
	m_rpcClient.setIncomeDataHandler(OnServerRequest);
	m_rpcClient.start(CONNSTR);
}


void TestWirlineScheduleDlg::OnBnClickedBtnStop()
{
	m_rpcClient.stop();
}


void TestWirlineScheduleDlg::OnBnClickedBtnCallAppEvent()
{
	std::map<std::string, std::string> args;
	args["ip"] = "10.2.9.11";
	std::string callJsonStr = CRpcJsonParser::buildCall("setRadioIp", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, [](const char* pResponse){

		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}


void TestWirlineScheduleDlg::OnBnClickedRadiobtn()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strConenct = "{\"call\":\"connect\",\"param\":{\"radioIP\":\"192.168.10.2\",\"mnisIP\":\"\"},\"callId\":1}";
	std::map<std::string, std::string> args;
	args["radioIP"] = "192.168.10.2";
	args["mnisIP"] = "";
	std::string callJsonStr = CRpcJsonParser::buildCall("connect", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, [](const char* pResponse){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
	//m_rpcClient.send((PBYTE)strConenct.c_str(), strConenct.size() + 1);
}


void TestWirlineScheduleDlg::OnBnClickedCallbtn()
{
	// TODO:  在此添加控件通知处理程序代码
	
		//std::string strCall = "{\"call\":\"call\",\"param\":{\"id\":\"10\"},\"callId\":1}";
		//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("call", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, [](const char* pResponse){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();

	
}

void TestWirlineScheduleDlg::OnBnClickedMsgbtn()
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
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, [](const char* pResponse){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}
void TestWirlineScheduleDlg::OnBnClickedGroupmsg()
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
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, [](const char* pResponse){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}

void TestWirlineScheduleDlg::OnBnClickedStopcallbtn()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strCall = "{\"call\":\"stopCall\",\"param\":{\"id\":\"10\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("stopCall", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, [](const char* pResponse){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();

}


void TestWirlineScheduleDlg::OnBnClickedPoweron()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strCall = "{\"call\":\"remotePowerOn\",\"param\":{\"id\":\"10\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("remotePowerOn", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, [](const char* pResponse){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}





void TestWirlineScheduleDlg::OnBnClickedPoweroff()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strCall = "{\"call\":\"remotePowerOff\",\"param\":{\"id\":\"10\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("remotePowerOff", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, [](const char* pResponse){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}


void TestWirlineScheduleDlg::OnBnClickedOnline()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strCall = "{\"call\":\"radioCheck\",\"param\":{\"id\":\"10\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("radioCheck", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, [](const char* pResponse){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}


void TestWirlineScheduleDlg::OnBnClickedWiretap()
{
	// TODO:  在此添加控件通知处理程序代码
	//std::string strCall = "{\"call\":\"wiretap\",\"param\":{\"id\":\"10\"},\"callId\":1}";
	//m_rpcClient.send((PBYTE)strCall.c_str(), strCall.size() + 1);
	std::map<std::string, std::string> args;
	args["id"] = "10";
	std::string callJsonStr = CRpcJsonParser::buildCall("wiretap", 223, args);
	//std::string strCall = "{\"call\":\"appEvent\",\"param\":{\"name\":23, \"dest\":234, \"content\":\"group\"}}";
	//m_rpcClient.send(callJsonStr.c_str(), callJsonStr.size());
	m_rpcClient.sendRequest(callJsonStr.c_str(), 223, [](const char* pResponse){
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}

void TestWirlineScheduleDlg::OnBnClickedBtnConfig()
{
	// TODO:  在此添加控件通知处理程序代码
	std::map<std::string, std::string> args;
	args["module"] = "wl";
	args["MASTER_IP"] = "192.168.2.121";
	args["MASTER_PORT"] = "50000";
	args["LOCAL_PEER_ID"] = "120";
	args["LOCAL_RADIO_ID"] = "5";
	args["RECORD_TYPE"] = "CPC";
	args["DEFAULT_GROUP"] = "9";
	args["DONGLE_PORT"] = "7";
	args["HUNG_TIME"] = "4";
	args["MASTER_HEART_TIME"] = "59";
	args["PEER_HEART_TIME"] = "59";
	args["DEFAULT_SLOT"] = "1";
	std::string callJsonStr = CRpcJsonParser::buildCall("config", ++g_sn, args);
	m_rpcClient.sendRequest(callJsonStr.c_str(), g_sn, [](const char* pResponse){
		std::string strResp = pResponse;
		TRACE("%s\r", strResp.c_str());
		//std::wstring wstr;
		//utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		//TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
}




void TestWirlineScheduleDlg::OnBnClickedCall()
{
	// TODO:  在此添加控件通知处理程序代码
	char temp[128] = { 0 };
	int callType = m_callType.GetCurSel();
	CString str;
	unsigned long tartgetId = 0;
	try
	{

		m_targetId.GetWindowText(str);
		tartgetId = (unsigned long)_ttoll(str);
		if (0 == tartgetId)
		{
			AfxMessageBox(_T("错误的target id"));
			return;
		}
	}
	catch (CMemoryException* e)
	{
		AfxMessageBox(_T("错误的target id"));
		return;
	}
	catch (CFileException* e)
	{
		AfxMessageBox(_T("错误的target id"));
		return;
	}
	catch (CException* e)
	{
		AfxMessageBox(_T("错误的target id"));
		return;
	}

	if (callType == 0)
	{
		callType = 0x4f;
	}
	else if (callType == 1)
	{
		callType = 0x50;
	}
	else if (callType == 2)
	{
		callType = 0x53;
	}
	else
	{
		AfxMessageBox(_T("尚未选择通话类型"));
		return;
	}
	/*构建json*/
	std::map<std::string, std::string> args;
	args["module"] = "wl";
	sprintf_s(temp, "%d", callType);
	args["callType"] = temp;
	sprintf_s(temp, "%lu", tartgetId);
	args["tartgetId"] = temp;
	/*发送请求*/
	std::string callJsonStr = CRpcJsonParser::buildCall("initialCall", ++g_sn, args);
	m_rpcClient.sendRequest(callJsonStr.c_str(), g_sn, [](const char* pResponse){
		/*处理返回值*/
		std::string strResp = pResponse;
		TRACE("%s\r",strResp.c_str());
		//std::wstring wstr;
		//utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		//TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
	m_call.EnableWindow(FALSE);
	m_stopCall.EnableWindow(FALSE);
}


void TestWirlineScheduleDlg::OnBnClickedStopCall()
{
	// TODO:  在此添加控件通知处理程序代码
	/*构建json*/
	std::map<std::string, std::string> args;
	args["module"] = "wl";
	/*发送请求*/
	std::string callJsonStr = CRpcJsonParser::buildCall("stopCall", ++g_sn, args);
	m_rpcClient.sendRequest(callJsonStr.c_str(), g_sn, [](const char* pResponse){
		/*处理返回值*/
		std::string strResp = pResponse;
		TRACE("%s\r", strResp.c_str());
		//std::wstring wstr;
		//utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		//TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
	m_call.EnableWindow(FALSE);
	m_stopCall.EnableWindow(FALSE);
}


void TestWirlineScheduleDlg::OnBnClickedSetPlayCallOfCare()
{
	// TODO:  在此添加控件通知处理程序代码
	//AfxMessageBox(L"OnBnClickedSetPlayCallOfCare");
	int index = m_careCalls.GetCurSel();
	if (-1 == index)
	{
		AfxMessageBox(L"no selected call");
		return;
	}
	CString selectedInfo = L"";
	m_careCalls.GetLBText(index, selectedInfo);
	char temp[128] = { 0 };
	if (selectedInfo == L"")
	{
		AfxMessageBox(L"no care call");
		return;
	}
	//selectedInfo = 
	unsigned int callType = 0;
	unsigned long srcId = 0;
	unsigned long tgtId = 0;
	/*构建json*/
	std::map<std::string, std::string> args;
	swscanf_s(selectedInfo, L"%u;%lu;%lu", &callType, &srcId, &tgtId);
	sprintf_s(temp, "%u", callType);
	args["callType"] = temp;
	sprintf_s(temp, "%lu", srcId);
	args["srcId"] = temp;
	sprintf_s(temp, "%lu", tgtId);
	args["tgtId"] = temp;
	args["module"] = "wl";
	/*发送请求*/
	std::string callJsonStr = CRpcJsonParser::buildCall("setPlayCallOfCare", ++g_sn, args);
	m_rpcClient.sendRequest(callJsonStr.c_str(), g_sn, [](const char* pResponse){
		/*处理返回值*/
		std::string strResp = pResponse;
		std::wstring wstr;
		utf8::utf8to16(strResp.begin(), strResp.end(), std::back_inserter(wstr));
		TRACE("received:%s\r", wstr.c_str());
	});
	callJsonStr.clear();
	m_call.EnableWindow(TRUE);
	m_stopCall.EnableWindow(FALSE);
}

void TestWirlineScheduleDlg::OnServerRequest(CBaseConnector* pServer, const char* pData, int dataLen)
{
	try
	{
		std::string str(pData, dataLen);
		TRACE("received data:%s\r", str.c_str());
		std::string callName;
		std::string type;
		uint64_t callId = 0;
		std::string param = "";
		if (0 != CRpcJsonParser::getRequest(str, callName, callId, param, type))
		{
			// send error response
			std::string response = CRpcJsonParser::buildResponse("failed", callId, 404, "Invalid request");
			pServer->send(response.c_str(), response.size());
			throw std::exception("invalid request");
		}
		g_sn = callId;
		Document d;
		d.Parse(param.c_str());
		if (0 == strcmp(callName.c_str(),"Send_CARE_CALL_STATUS"))
		{
			g_pNewRequest = new SERVER_REQUEST;
			g_pNewRequest->cmd = WM_UPDATE_DATA;
			g_pNewRequest->info.callInfo.callType = atoi(d["callType"].GetString());
			g_pNewRequest->info.callInfo.src = (unsigned long)atoll(d["srcId"].GetString());
			g_pNewRequest->info.callInfo.status = atoi(d["status"].GetString());
			g_pNewRequest->info.callInfo.tgt = (unsigned long)atoll(d["tgtId"].GetString());
			g_requests.push_back(g_pNewRequest);
		}
	}
	catch (std::exception& e)
	{
		printf("exception: %s\r\n", e.what());
	}
	catch (...)
	{
		printf("unknow error! \r\n");
	}
}

UINT TestWirlineScheduleDlg::HandleServerRequestProc(LPVOID pParam)
{
	TestWirlineScheduleDlg* p = (TestWirlineScheduleDlg*)pParam;
	if (p)
	{
		p->HandleServerRequest();
	}
	return 0;
}

void TestWirlineScheduleDlg::HandleServerRequest()
{
	while (m_bRunHandleServerProc)
	{
		if (g_requests.size() > 0)
		{
			//UpdateData(FALSE);
			m_bupdateData = FALSE;
			PostMessage(WM_UPDATE_DATA, 0, 0);
			SERVER_REQUEST *p = new SERVER_REQUEST;
			SERVER_REQUEST *temp = g_requests.front();
			memcpy(p, temp, sizeof(SERVER_REQUEST));
			g_requests.pop_front();
			delete temp;
			temp = NULL;
			wchar_t callInfo[256] = { 0 };
			swprintf_s(callInfo, L"%d;%lu;%lu", p->info.callInfo.callType, p->info.callInfo.src, p->info.callInfo.tgt);
			int count = m_careCalls.GetCount();
			bool bHave = false;
			int haveIndex = -1;
			for (int index = 0; index < count;index++)
			{
				wchar_t indexInfo[256] = { 0 };
				m_careCalls.GetLBText(index, indexInfo);
				if (0 == wcscmp(callInfo,indexInfo))
				{
					bHave = true;
					haveIndex = index;
					break;
				}
			}
			switch (p->info.callInfo.status)
			{
			case HAVE_CALL_END_PLAY:
			{

									   GetDlgItem(CURRENT_INFO)->SetWindowText(L"no call");
			}
				break;
			case HAVE_CALL_NO_PLAY:
			{
									  if (!bHave)
									  {
										  m_careCalls.InsertString(0, callInfo);
									  }
			}
				break;
			case HAVE_CALL_START_PLAY:
			{
										 GetDlgItem(CURRENT_INFO)->SetWindowText(callInfo);
			}
				break;
			case END_CALL_NO_PLAY:
			{
									 if (bHave)
									 {
										 m_careCalls.DeleteString(haveIndex);
									 }
			}
				break;
			case NEW_CALL_END:
			{
								 m_stopCall.EnableWindow(FALSE);
								 m_call.EnableWindow(TRUE);
			}
				break;
			case NEW_CALL_START:
			{
								   m_stopCall.EnableWindow(TRUE);
								   m_call.EnableWindow(FALSE);
			}
				break;
			default:
				break;
			}
			//UpdateData(TRUE);
			m_bupdateData = TRUE;
			PostMessage(WM_UPDATE_DATA, 0, 0);
		}
		else
		{
			Sleep(50);
		}
	}
}

LRESULT  TestWirlineScheduleDlg::OnUpdateData(WPARAM w, LPARAM l)
{
	UpdateData(m_bupdateData);
	return 0;
}
