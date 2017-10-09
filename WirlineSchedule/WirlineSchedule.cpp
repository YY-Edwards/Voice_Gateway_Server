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
#define SERVICE_CODDE FALSE

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

int _tmain(int argc, _TCHAR* argv[])
{
	/*log初始化*/
	g_pWLlog = new WLSocketLog();
	int createFileRlt = 0;
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
	std::wstring defaultAudioPath = appFolder;
	//defaultAudioPath += L"\\Voice";
	appFolder = appFolder + _T("\\WirelineSchedule");
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
	google::SetLogDestination(google::GLOG_INFO, g_tool.UnicodeToUTF8(pathLogInfo).c_str());
	google::SetLogDestination(google::GLOG_ERROR, g_tool.UnicodeToUTF8(pathLogError).c_str());
	google::SetLogDestination(google::GLOG_WARNING, g_tool.UnicodeToUTF8(pathLogWarning).c_str());
	google::SetLogFilenameExtension("log");

#if SERVICE_CODDE
	CService::instance()->SetServiceNameAndDescription(_T("Trbox.Wirelan"), _T("Trbox Wirelan Server"));
	CService::instance()->SetServiceCode([&](){
		/************************************************************************/
		/* 运行代码
		/************************************************************************/
		LOG(INFO) << "================Service started================";
#endif

		/*声明变量并初始化*/
		CMySQL *m_pDb = new CMySQL();
		CDataScheduling *m_pMnis = new CDataScheduling();
		CManager *m_pManager = new CManager(m_pDb, m_pMnis,defaultAudioPath);
		BOOL m_ret = FALSE;
		PLogReport m_report = NULL;
		HWND m_hwnd = NULL;
		char m_temp = 0x00;
		int cmd = 0;
		CRpcServer *m_pRpcServer = new CRpcServer();

		/*设置回调*/
		m_pMnis->setCallBackFunc(CManager::OnMnisCallBack);
		m_pRpcServer->setOnConnectHandler(CManager::OnConnect);
		m_pRpcServer->setOnDisconnectHandler(CManager::OnDisConnect);

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
		m_pRpcServer->addActionHandler("queryGps", wlMnisQueryGpsActionHandler);
		m_pRpcServer->addActionHandler("message", wlMnisMessageHandler);
		m_pRpcServer->addActionHandler("status", wlMnisStatusHandler);
		m_pRpcServer->addActionHandler("locationIndoor", wlMnisLocationIndoorHandler);
		m_pRpcServer->start(WL_SERVER_PORT);

		/*初始化数据库*/
		m_ret = m_pDb->Open(DB_HOST, DB_PORT, DB_USER, DB_PWD, DB_NAME);

#if SERVICE_CODDE
		while (!CService::instance()->m_bServiceStopped)
		{
			Sleep(100);
		}
#else
		handleLog("input 0 for end");
		scanf_s("%d", &cmd);
		/*等待用户选择退出*/
		while (cmd != 0)
		{
			switch (cmd)
			{
			case 1:
			{
					  REMOTE_TASK *pTask = new REMOTE_TASK;
					  memset(pTask, 0, sizeof(REMOTE_TASK));
					  pTask->cmd = REMOTE_CMD_CALL;
					  pTask->param.info.callParam.operateInfo.callType = GROUP_CALL;
					  pTask->param.info.callParam.operateInfo.isCurrent = 1;
					  pTask->param.info.callParam.operateInfo.operate = 0;
					  pTask->param.info.callParam.operateInfo.source = 5;
					  pTask->param.info.callParam.operateInfo.tartgetId = 9;
					  push_back_task(pTask);
			}
				break;
			case 2:
			{
					  REMOTE_TASK *pTask = new REMOTE_TASK;
					  memset(pTask, 0, sizeof(REMOTE_TASK));
					  pTask->cmd = REMOTE_CMD_STOP_CALL;
					  push_back_task(pTask);
			}
				break;
			default:
				break;
			}

			handleLog("input 0 for end");
			scanf_s("%d", &cmd);
		}
#endif
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