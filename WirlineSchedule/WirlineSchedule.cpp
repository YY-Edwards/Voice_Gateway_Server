// WirlineSchedule.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Manager.h"
#include "MySQL.h"
#include "../lib/rpc/include/RpcServer.h"
#include "actionHandler.h"
#include "WLNet.h"
#include "../lib/radio/DataScheduling.h"

extern CWLNet* g_pNet;

#define DB_HOST				"127.0.0.1"
#define DB_PORT				3306
#define DB_USER				"root"
#define DB_PWD				""
#define DB_NAME				"WirelineMotoVoice"

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
	SYSTEMTIME now = { 0 };
	GetLocalTime(&now);
	printf_s("\r\n%04u-%02u-%02u %02u:%02u:%02u %03u %s\r\n", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds, pLog);
}

int main()
{
	//_CrtSetBreakAlloc(358);
	/*声明变量并初始化*/
	CMySQL *m_pDb = new CMySQL();
	CDataScheduling *m_pMnis = new CDataScheduling();
	CManager *m_pManager = new CManager(m_pDb, m_pMnis);
	BOOL m_ret = FALSE;
	PLogReport m_report = NULL;
	HWND m_hwnd = NULL;
	char m_temp = 0x00;
	CRpcServer *m_pRpcServer = new CRpcServer();

	/*设置回调*/
	m_pMnis->setCallBackFunc(CManager::OnMnisCallBack);
	m_pRpcServer->setOnConnectHandler(CManager::OnConnect);
	m_pRpcServer->setOnDisconnectHandler(CManager::OnDisConnect);

	/*设置基本参数*/
	m_report = handleLog;
	m_hwnd = GetConsoleHwnd();
	m_pManager->initWnd(m_hwnd);
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
	m_pRpcServer->start(WL_SERVER_PORT);

	/*初始化数据库*/
	m_ret = m_pDb->Open(DB_HOST, DB_PORT, DB_USER, DB_PWD, DB_NAME);
	if (!m_ret)
	{
		handleLog("open data server fail");
		scanf_s("%c", &m_temp, 1);
		return 0;
	}

	/*等待用户选择退出*/
	handleLog("input any for end");
	scanf_s("%c", &m_temp, 1);

	/*释放资源*/
	if (m_pManager)
	{
		m_pManager->stop();
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
		g_pNet->stop();
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
		g_pSound->stop();
		delete g_pSound;
		g_pSound = NULL;
	}

	if (g_pDongle)
	{
		g_pDongle->stop();
		delete g_pDongle;
		g_pDongle = NULL;
	}

	if (m_pDb)
	{
		m_pDb->stop();
		delete m_pDb;
		m_pDb = NULL;
	}

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	return 0;
}