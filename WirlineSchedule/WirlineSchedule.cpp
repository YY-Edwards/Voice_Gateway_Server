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

//#ifndef DLLEXPORT
//#define DLLEXPORT extern "C" _declspec(dllexport)
//#endif
//static CManager manager;
///*播放指定的AMBE数据*/
//DLLEXPORT bool Play(unsigned int length, char* pData)
//{
//
//	if (length == 0
//		|| NULL == pData)
//	{
//		return false;
//	}
//
//	int rlt = manager.play(length, pData);
//	if (0 != rlt)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//DLLEXPORT int SetLogPtr(PLogReport logHandel)
//{
//	return manager.setLogPtr(logHandel);
//}
//
///*关闭*/
//DLLEXPORT bool DisConnect()
//{
//	int rlt = manager.disConnect();
//	if (0 != rlt)
//	{
//		return false;
//	}
//	return true;
//}
//
//#if WXJ_DLL
///************************************************************************/
///* dll for wxj
///************************************************************************/
///*连接并初始化扬声器*/
//DLLEXPORT bool Connect(HWND current_hwnd, unsigned int serial_port)
//{
//	int rlt = manager.initWnd(current_hwnd);
//	if (rlt != 0)
//	{
//		return false;
//	}
//	rlt = manager.initDongle(serial_port);
//	if (rlt != 0)
//	{
//		return false;
//	}
//	return true;
//}
//#else
//DLLEXPORT int InitSys(char* master_ip, //主中继IP
//	unsigned int master_port, //主中继UDP端口
//	unsigned int local_id, //Consle ID
//	unsigned int local_radio_id,// Consle RadioId
//	//unsigned int master_id, //主中继ID
//	unsigned int record_type, //录音模式
//	unsigned int local_slot, //Consle 信道
//	unsigned int local_group, //Consle 组
//	unsigned int serial_port //dongle 端口
//	)
//{
//	return manager.initSys(master_ip, //主中继IP
//		master_port, //主中继UDP端口
//		local_id, //Consle ID
//		local_radio_id,// Consle RadioId
//		//master_id, //主中继ID
//		record_type, //录音模式
//		local_slot, //Consle 信道
//		local_group, //Consle 组
//		serial_port //dongle 端口
//		);
//}
//
//DLLEXPORT int  InitWnd(HWND current_hwnd)
//{
//	return manager.initWnd(current_hwnd);
//}
//
//DLLEXPORT int StablePlay()
//{
//	return manager.play();
//}
//
//DLLEXPORT int Record(unsigned long tartgetId, unsigned char callType)
//{
//	return manager.initialCall(tartgetId, callType);
//}
//
//DLLEXPORT int StopRecord()
//{
//	return manager.stopRecord();
//}
//
//DLLEXPORT int SendFile(unsigned int length, char* pData)
//{
//	return manager.SendFile(length, pData);
//}
//#endif


#define DEFAULT_CALLTYPE	0x4f

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
	//std::function<void(CRemotePeer*)> fun;
	//fun = CManager::OnConnect;
	//fun(NULL);
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
	handleLog("input any for end");
	scanf_s("%c", &m_temp, 1);

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


//#pragma region 原有代码
//	/*局部变量声明*/
//	int returnValue;
//	CMySQL m_db;
//	CManager *m_pManager = new CManager(&m_db);
//	int cmd = 0;
//	bool run = true;
//	PLogReport m_report = handleLog;
//	/*配置参数*/
//	m_pManager->config("192.168.1.121", "50000", "120", "5", "CPC", "9", "7", "4000", "60000", "600000","1");
//	/*当前句柄获取*/
//	HWND m_hwnd = GetConsoleHwnd();
//	/*初始化日志回调*/
//	m_pManager->setLogPtr(m_report);
//	m_db.SetLogPtr(m_report);
//	/*初始化数据库*/
//	BOOL ret = m_db.Open(DB_HOST, DB_PORT, DB_USER, DB_PWD, DB_NAME);
//	if (!ret)
//	{
//		handleLog("open data server fail");
//		char temp = 0x00;
//		scanf_s("%c", &temp, 1);
//		return 0;
//	}
//	/*初始化网络、Dongle、Sound*/
//	returnValue = m_pManager->initWnd(m_hwnd);
//	if (returnValue != 0)
//	{
//		handleLog("Initialization system fail");
//		char temp = 0x00;
//		scanf_s("%c", &temp, 1);
//		return 0;
//	}
//	else
//	{
//		handleLog("Initialization system success");
//
//	}
//	returnValue = m_pManager->initSys();
//	if (returnValue != 0)
//	{
//		handleLog("Initialization system fail");
//		char temp = 0x00;
//		scanf_s("%c", &temp, 1);
//		return 0;
//	}
//	else
//	{
//		handleLog("Initialization system success");
//	}
//	/*进入菜单选项*/
//	while (run)
//	{
//		printf_s("/************/\n/* 1.按下PTT\n/* 2.松开PTT\n/* 3.录音回放\n/* 4.发送语音\n/* 9.清屏\n/* 0.退出\n/************/\n\r");
//		scanf_s("%d", &cmd, 1);
//		switch (cmd)
//		{
//		case 0x01:
//		{
//					 m_pManager->initialCall("9", "79");
//		}
//			break;
//		case 0x02:
//		{
//					 m_pManager->stopCall();
//		}
//			break;
//		case 0x03:
//		{
//					 m_pManager->play();
//		}
//			break;
//		case 0x04:
//		{
//					 HANDLE file = CreateFile(L"D:\\WirelineScheduleVoiceData\\201610.bit", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
//					 if (INVALID_HANDLE_VALUE == file)
//					 {
//						 handleLog("CreateFile fail");
//					 }
//					 else
//					 {
//						 DWORD len = 0;
//						 DWORD readLen = 0;
//						 len = GetFileSize(file, 0);
//						 if (len > 0)
//						 {
//							 char* pBuffer = new char[len];
//							 ReadFile(file, pBuffer, len, &readLen, NULL);
//							 CloseHandle(file);
//							 m_pManager->SendFile(readLen, pBuffer);
//							 delete[] pBuffer;
//						 }
//					 }
//
//		}
//			break;
//		case 0x09:
//		{
//					 system("cls");
//		}
//			break;
//		default:
//		{
//				   run = false;
//		}
//			break;
//		}
//	}
//	return 0;
//#pragma endregion
}