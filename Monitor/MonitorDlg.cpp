
// MonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Monitor.h"
#include "MonitorDlg.h"
#include "afxdialogex.h"
#include "ConnectAction.h"
#include "Tool.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TCP_PORT 9004
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMonitorDlg 对话框



CMonitorDlg::CMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMonitorDlg, CDialogEx)
	ON_MESSAGE(WM_TRAYICON, OnSysTrayMsg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CMonitorDlg 消息处理程序

BOOL CMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hWnd = this->m_hWnd;
	m_nid.uID = IDR_MAINFRAME;
	m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_nid.uCallbackMessage = WM_TRAYICON;
	m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	wcscpy_s(m_nid.szTip, _T("Trbox.Monitor"));
	wcscpy_s(m_nid.szInfoTitle, _T("Trbox.Monitor"));
	wcscpy_s(m_nid.szInfo, _T("监测程序已启动!"));
	m_nid.uTimeout = 2000;
	m_nid.dwInfoFlags = NIIF_INFO;
	Shell_NotifyIcon(NIM_ADD, &m_nid);

	writeLog();

	//WINDOWPLACEMENT wp;
	//wp.length = sizeof(WINDOWPLACEMENT);
	//wp.flags = WPF_RESTORETOMAXIMIZED;
	//wp.showCmd = SW_HIDE;
	//SetWindowPlacement(&wp);             //隐藏窗口
	//ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW, 1);     //隐藏任务栏
	//AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWMINIMIZED); //隐藏窗口
	int nFullWidth = GetSystemMetrics(SM_CXSCREEN);
	int nFullHeight = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(NULL, nFullWidth, nFullHeight, 0, 0, SWP_NOZORDER);  //设置0像素,移到最角落  或者:MoveWindow(0,0,0,0);
	ShowWindow(SW_HIDE);
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);  //移除任务栏图标显示
	// TODO:  在此添加额外的初始化代码
	m_tserver.StartMonitor();
	rpcServer.setOnConnectHandler(CMonitorDlg::OnConnect);
	rpcServer.addActionHandler("connect", ConnectAction);
	rpcServer.start(TCP_PORT, rpcServer.TCP);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMonitorDlg::OnConnect(CRemotePeer* pRemotePeer)
{

	if (pRemotePeer)
	{
		std::string strRequest = CRpcJsonParser::buildCall("getSettingConfig", 1, ArgumentType(), "radio");
		pRemotePeer->sendResponse((const char *)strRequest.c_str(), strRequest.size());

	}
}
void CMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMonitorDlg::OnSysTrayMsg(WPARAM w, LPARAM l)
{
	UINT uID;//发出该消息的图标的ID 
	UINT uMouseMsg;//鼠标动作 
	//POINT pt;
	uID = (UINT)w;
	uMouseMsg = (UINT)l;

	switch (uMouseMsg)
	{
	case WM_LBUTTONDBLCLK:
		//ShowWindow(SW_SHOWNORMAL);
		break;
	case WM_RBUTTONDOWN:
	{
						 ///*  CMenu menu;
						 //  menu.LoadMenu(IDR_MENU1);
						 //  CMenu*pMenu = menu.GetSubMenu(0);*/
						 //  CPoint pos;
						 //  GetCursorPos(&pos);
						 //  //加入SetForegroundWindow的目的为使用户点菜单之外时菜单可以消失
						 //  ::SetForegroundWindow(m_hWnd);
						 //  pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd());
						 //  menu.DestroyMenu();
	}
		break;
	default:
		break;
	}
	return 0;
}
void CMonitorDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanging(lpwndpos);

	// TODO:  在此处添加消息处理程序代码

	lpwndpos->flags &= ~SWP_SHOWWINDOW;
	CDialog::OnWindowPosChanging(lpwndpos);
}
void CMonitorDlg::OnClose()
{
	ShowWindow(SW_HIDE);
}
std::string CMonitorDlg::getServerName()
{
	std::string serverName = "";
	std::string strConfig = CSettings::instance()->getValue("radio");
	//std::string strConfig = "{\"IsEnable\":true,\"IsOnlyRide\":true,\"Svr\":{\"Ip\":\"127.0.0.1\",\"Port\":9001},\"Ride\":{\"Ip\":\"192.168.10.2\",\"Port\":0},\"Mnis\":{\"Ip\":null,\"Port\":0},\"GpsC\":{\"Ip\":null,\"Port\":0},\"Ars\":{\"Ip\":null,\"Port\":4007},\"Message\":{\"Ip\":null,\"Port\":4005},\"Gps\":null,\"Xnl\":null}";
	rapidjson::Document doc;
	doc.Parse(strConfig.c_str());
	if (doc.IsObject() && doc.HasMember("IsEnable") && doc["IsEnable"].IsBool())
	{
		if (true == doc["IsEnable"].GetBool())
		{
			serverName = "Trbox.Dispatch";
		}
		{
			serverName = "Trbox.Wirelan";
		}

	}
	return  serverName;
}
std::wstring CMonitorDlg::getAppdataPath()
{
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);

	return std::wstring(szBuffer);
}
void CMonitorDlg::writeLog()
{
	int createFileRlt = 0;
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);
	std::wstring appFolder = getAppdataPath() + _T("\\Jihua Information");
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}
	appFolder = appFolder + _T("\\Trbox");
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}
	appFolder = appFolder + _T("\\3.0");
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}
	appFolder = appFolder + _T("\\Monitor");
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}

	std::wstring logFolder = appFolder + _T("\\log");
	if (!PathFileExists(logFolder.c_str()))
	{
		createFileRlt = _wmkdir(logFolder.c_str());
	}

	std::wstring pathLogInfo = logFolder + _T("/info_");
	std::wstring pathLogError = logFolder + _T("/error_");
	std::wstring pathLogWarning = logFolder + _T("/warning_");

	//FLAGS_log_dir = "./";
	google::InitGoogleLogging("");
	google::SetLogDestination(google::GLOG_INFO, CTool::UnicodeToUTF8(pathLogInfo).c_str());
	google::SetLogDestination(google::GLOG_ERROR, CTool::UnicodeToUTF8(pathLogError).c_str());
	google::SetLogDestination(google::GLOG_WARNING, CTool::UnicodeToUTF8(pathLogWarning).c_str());
	google::SetLogFilenameExtension("log");
}