#include "stdafx.h"
#include "Manager.h"
#include "MySQL.h"
#include "../lib/rpc/include/RpcServer.h"
#include "actionHandler.h"
#include "WLNet.h"
#include "../lib/radio/DataScheduling.h"
#include "../lib/service/service.h"

#include <shlobj.h> 

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

extern CWLNet* g_pNet;

#define DB_HOST				"127.0.0.1"
#define DB_PORT				3306
#define DB_USER				"root"
#define DB_PWD				""
#define DB_NAME				"tbx"
#define SERVICE_CODDE		TRUE

#define			ConmpanyName	L"JiHua Information"
#define			AppName			L"Trbox"
#define			AppVersion		L"3.0"
#define			AppNameSub		L"WirelineSchedule"

#define Log_log L"log"
#define Log_info L"info_"
#define Log_error L"error_"
#define Log_warning L"warning_"

HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.
	HWND hwndFound;         // This is what is returned to the caller.
	wchar_t pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
	// WindowTitle.
	wchar_t pszOldWindowTitle[MY_BUFSIZE]; // Contains original
	// WindowTitle.

	// Fetch current window title.

	GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

	// Format a "unique" NewWindowTitle.

	wsprintf(pszNewWindowTitle, L"%d/%d",
		GetTickCount(),
		GetCurrentProcessId());

	// Change current window title.

	SetConsoleTitle(pszNewWindowTitle);

	// Ensure window title has been updated.

	Sleep(40);

	// Look for NewWindowTitle.

	hwndFound = FindWindow(NULL, pszNewWindowTitle);

	// Restore original window title.

	SetConsoleTitle(pszOldWindowTitle);

	return(hwndFound);
}

void handleLog(char *pLog)
{
#if _DEBUG
	LOG(INFO) << pLog;
#if SERVICE_CODDE
#else
	SYSTEMTIME t;
	GetLocalTime(&t);
	printf_s("\r\n%04d-%02d-%02d %02d:%02d:%02d %03d %s\r\n", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, pLog);
#endif
#endif
}

std::wstring getAppdataPath(){
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);

	return std::wstring(szBuffer);
}

bool m_bCheckUsbRun = false;
CManager *m_pManager = NULL;
HDEVNOTIFY m_hDevNotify = NULL;

LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
	//char temp[512] = { 0 };
	//sprintf_s(temp, "WndProc Work  HWND:0x%08x,\tmsg:0x%04x,\tWPARAM:0x%04x\n", h, msg, wp);
	//handleLog(temp);
	switch (msg)
	{
	case WM_DEVICECHANGE:
	{
							switch ((DWORD)wp)
							{
							case DBT_DEVICEARRIVAL:
							case DBT_DEVICEREMOVECOMPLETE:
							{
															 PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lp;
															 switch (pHdr->dbch_devicetype)
															 {
															 case DBT_DEVTYP_DEVICEINTERFACE:
															 {
																								PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
																								if (m_pManager)
																								{
																									m_pManager->OnUpdateUsb((DWORD)wp);
																								}
																								break;
															 }
																 break;
															 default:
																 break;
															 }
							}
								break;
							default:
								break;
							}
	}
		return TRUE;
	default:
		return DefWindowProc(h, msg, wp, lp);
	}
}

static unsigned int __stdcall OperateProc(void* pArguments)
{
	handleLog("OperateProc Start");
	/*操作处理*/
	char temp;
	while ('0' != (temp = getchar()))
	{
	}
	m_bCheckUsbRun = false;
	//MSG msg;
	//while (m_bOperateRun)
	//{
	//	if (PeekMessage(&msg, m_hNotifyWnd, 0, 0, PM_REMOVE) != 0)
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//	else
	//	{
	//		Sleep(INTERVAL_CHECK_USB);
	//	}
	//}
	handleLog("OperateProc End");
	return 0;
}

void RegisterDevice(HWND& hwnd, HDEVNOTIFY& hDevNotify)
{
	const GUID GUID_DEVINTERFACE_LIST[] = {
		{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
		{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },
		{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } }, /* HID */
		{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } } };

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
	{
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
		hDevNotify = RegisterDeviceNotification(hwnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
		if (!hDevNotify) {
			//MessageBox(hwnd, TEXT("注册USB设备通知失败"), TEXT("错误"), MB_ICONERROR);
			handleLog("RegisterDeviceNotification fail");
			break;
		}
	}
}

void RegisterDevice(HDEVNOTIFY& hDevNotify)
{
	const GUID GUID_DEVINTERFACE_LIST[] = {
		{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
		{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },
		{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } }, /* HID */
		{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } } };

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
	{
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
		hDevNotify = RegisterDeviceNotification(CService::instance()->m_StatusHandle, &NotificationFilter, DEVICE_NOTIFY_SERVICE_HANDLE);
		if (!hDevNotify) {
			//MessageBox(hwnd, TEXT("注册USB设备通知失败"), TEXT("错误"), MB_ICONERROR);
			handleLog("RegisterDeviceNotification fail");
			break;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//utf8显示
	system("chcp 65001");
	system("cls");

	/*工具初始化*/
	g_pTool = new CTool();
	/*log初始化*/
	g_pWLlog = new WLSocketLog();
	int createFileRlt = 0;
	std::wstring appFolder = getAppdataPath() + L"\\" + ConmpanyName;
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}
	appFolder = appFolder + L"\\" + AppName;
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}
	appFolder = appFolder + L"\\" + AppVersion;
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}

	wcscpy(g_ambedata_path, appFolder.c_str());
	std::wstring defaultAudioPath = appFolder;
	//defaultAudioPath += L"\\Voice";
	appFolder = appFolder + L"\\" + AppNameSub;
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}

	std::wstring logFolder = appFolder + L"\\" + Log_log;
	if (!PathFileExists(logFolder.c_str()))
	{
		createFileRlt = _wmkdir(logFolder.c_str());
	}

	std::wstring pathLogInfo = logFolder + L"\\" + Log_info;
	std::wstring pathLogError = logFolder + L"\\" + Log_error;
	std::wstring pathLogWarning = logFolder + L"\\" + Log_warning;

	//FLAGS_log_dir = "./";
	google::InitGoogleLogging("");
	google::SetLogDestination(google::GLOG_INFO, g_pTool->UnicodeToUTF8(pathLogInfo).c_str());
	google::SetLogDestination(google::GLOG_ERROR, g_pTool->UnicodeToUTF8(pathLogError).c_str());
	google::SetLogDestination(google::GLOG_WARNING, g_pTool->UnicodeToUTF8(pathLogWarning).c_str());
	google::SetLogFilenameExtension("log");

#if SERVICE_CODDE
	CService::instance()->SetServiceNameAndDescription(_T("Trbox.Wirelan"), _T("Trbox Wirelan Server"));
	CService::instance()->SetServiceCode([&](){
		/************************************************************************/
		/* 运行代码
		/************************************************************************/
		LOG(INFO) << "================Service started================";
		RegisterDevice(m_hDevNotify);
#endif

		/*声明变量并初始化*/
		CMySQL *m_pDb = new CMySQL();
		CDataScheduling *m_pMnis = new CDataScheduling();
		m_pManager = new CManager(m_pDb, m_pMnis, defaultAudioPath);
		g_manager = m_pManager;
		BOOL m_ret = FALSE;
		PLogReport m_report = NULL;
		HWND m_hwnd = NULL;
		char m_temp = 0x00;
		int cmd = 0;
		CRpcServer *m_pRpcServer = new CRpcServer();

		/*设置回调*/
		m_pMnis->setCallBackFunc(CManager::OnData);
		m_pRpcServer->setOnConnectHandler(CManager::OnConnect);
		m_pRpcServer->setOnDisconnectHandler(CManager::OnDisConnect);
#if SERVICE_CODDE
		CService::instance()->SetRadioUsb(CManager::OnUpdateUsbService);
#endif

		/*设置基本参数*/
		m_report = handleLog;
		m_pDb->SetLogPtr(m_report);
		m_pManager->setLogPtr(m_report);
		/*开启远程任务处理线程*/
		m_pManager->startHandleRemoteTask();
		/*初始化服务部分*/
		m_pRpcServer->addActionHandler("wlConnect", wlConnectActionHandler);
		m_pRpcServer->addActionHandler("wlCall", wlCallActionHandler);
		m_pRpcServer->addActionHandler("wlCallStatus", wlCallStatusActionHandler);
		m_pRpcServer->addActionHandler("wlPlay", wlPlayActionHandler);
		m_pRpcServer->addActionHandler("wlInfo", wlInfoActionHandler);
		//m_pRpcServer->addActionHandler("queryGps", wlMnisQueryGpsActionHandler);
		m_pRpcServer->addActionHandler("location", wlMnisQueryGpsActionHandler);
		m_pRpcServer->addActionHandler("message", wlMnisMessageHandler);
		m_pRpcServer->addActionHandler("status", wlMnisStatusHandler);
		//m_pRpcServer->addActionHandler("locationIndoor", wlMnisLocationIndoorHandler);
		m_pRpcServer->start(WL_SERVER_PORT);

		/*初始化数据库*/
		m_ret = m_pDb->Open(DB_HOST, DB_PORT, DB_USER, DB_PWD, DB_NAME);
		if (m_ret)
		{
			g_pDb=m_pDb;
		}

#if SERVICE_CODDE
		while (!CService::instance()->m_bServiceStopped)
		{
			Sleep(100);
		}
#else
		handleLog("input 0 for end");
		/*注册设备插拔事件*/
		unsigned int m_pOperateThreadId = 0;
		HANDLE m_pOperateThread = NULL;
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.lpszClassName = TEXT("myusbmsg");
		wc.lpfnWndProc = WndProc;
		RegisterClass(&wc);
		HWND m_hNotifyWnd = CreateWindow(TEXT("myusbmsg"), TEXT(""), 0, 0, 0, 0, 0,
			0, 0, GetModuleHandle(0), 0);
		//初始化当前串口状态
		RegisterDevice(m_hNotifyWnd, m_hDevNotify);
		//操作测试线程
		m_pOperateThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			OperateProc,
			NULL,
			CREATE_SUSPENDED,
			&m_pOperateThreadId
			);
		if (NULL == m_pOperateThread)
		{
			handleLog("create OperateProc fail");
			goto WL_END;
		}
		m_bCheckUsbRun = true;
		ResumeThread(m_pOperateThread);
		MSG msg;
		while (m_bCheckUsbRun)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				Sleep(INTERVAL_CHECK_USB);
			}
		}
		handleLog("check usb proc exit");
		m_bCheckUsbRun = false;
		if (NULL != m_pOperateThread)
		{
			WaitForSingleObject(m_pOperateThread, 1000);
			CloseHandle(m_pOperateThread);
			m_pOperateThread = NULL;
		}
		//handleLog("input 0 for end");
		//scanf_s("%d", &cmd);
		///*等待用户选择退出*/
		//while (cmd != 0)
		//{
		//	switch (cmd)
		//	{
		//	case 1:
		//	{
		//			  REMOTE_TASK *pTask = new REMOTE_TASK;
		//			  memset(pTask, 0, sizeof(REMOTE_TASK));
		//			  pTask->cmd = REMOTE_CMD_CALL;
		//			  pTask->param.info.callParam.operateInfo.callType = GROUP_CALL;
		//			  pTask->param.info.callParam.operateInfo.isCurrent = 1;
		//			  pTask->param.info.callParam.operateInfo.operate = 0;
		//			  pTask->param.info.callParam.operateInfo.source = 5;
		//			  pTask->param.info.callParam.operateInfo.tartgetId = 9;
		//			  push_back_task(pTask);
		//	}
		//		break;
		//	case 2:
		//	{
		//			  REMOTE_TASK *pTask = new REMOTE_TASK;
		//			  memset(pTask, 0, sizeof(REMOTE_TASK));
		//			  pTask->cmd = REMOTE_CMD_STOP_CALL;
		//			  push_back_task(pTask);
		//	}
		//		break;
		//	default:
		//		break;
		//	}

		//	handleLog("input 0 for end");
		//	scanf_s("%d", &cmd);
		//}
#endif
	WL_END:
		/************************************************************************/
		/* 资源释放
		/************************************************************************/
		if (m_pManager)
		{
			//m_pManager->stop();
			delete m_pManager;
			m_pManager = NULL;
		}
		if (NULL != m_pMnis)
		{
			m_pMnis->radioDisConnect();
			delete m_pMnis;
			m_pMnis = NULL;
		}
		if (g_pNet)
		{
			//g_pNet->stop();
			delete g_pNet;
			g_pNet = NULL;
		}
		while (g_onLineClients.size() > 0)
		{
			TcpClient *p = g_onLineClients.front();
			g_onLineClients.pop_front();
			if (p)
			{
				delete p;
				p = NULL;
			}
		}
		if (m_pRpcServer)
		{
			m_pRpcServer->stop();
			delete m_pRpcServer;
			m_pRpcServer = NULL;
		}

		if (g_pSound)
		{
			//g_pSound->stop();
			delete g_pSound;
			g_pSound = NULL;
		}

		if (g_pDongle)
		{
			//g_pDongle->stop();
			delete g_pDongle;
			g_pDongle = NULL;
		}

		if (m_pDb)
		{
			//m_pDb->stop();
			delete m_pDb;
			m_pDb = NULL;
		}
		if (g_pWLlog)
		{
			delete g_pWLlog;
			g_pWLlog = NULL;
		}
#if SERVICE_CODDE
	});
	std::wstring strArg = argv[1];
	try{
		if (0 == strArg.compare(_T("install")))
		{
			CService::instance()->InstallService();
			//InstallService();
		}
		else if (0 == strArg.compare(_T("uninstall")))
		{
			CService::instance()->UninstallService();
			//LOG(INFO) << "UnInstall Service";
		}
		else if (0 == strArg.compare(_T("start")))
		{
			CService::instance()->StartWindowsService();
			//LOG(INFO) << "Start Service";
		}
		else if (0 == strArg.compare(_T("stop")))
		{
			CService::instance()->StopService();
			//LOG(INFO) << "Stop Service";
		}
		else if (0 == strArg.compare(_T("run")))
		{
			CService::instance()->RunService();
		}
	}
	catch (std::system_error syserr) {
		exit(1);
	}
	catch (std::runtime_error runerr) {
		exit(1);
	}
	catch (...) {
		exit(1);
	}

	wprintf(argv[1]);

#else
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		return 0;
	}