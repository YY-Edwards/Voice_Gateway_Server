// Dispatch.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <sstream>
//#include <system_error>
#include <Shlwapi.h>
#include <shlobj.h>  
#include "Util.h"
#include "../lib/rpc/include/RpcServer.h"
#include "../lib/service/service.h"
#include "DispatchOperate.h"

#include "ConnectAction.h"
#include "CallAction.h"
#include "ControlAction.h"
#include "GpsAction.h"
#include "MsgAction.h"
#include "StatusAction.h"
#include "LocationInDoorAction.h"
#include "Tool.h"

#define TCP_PORT 9001

#pragma comment(lib, "Shlwapi.lib")
static TCHAR szServiceName[] = _T("Dispatch");
static TCHAR szServiceRun[] = _T("run");
static const int cPreshutdownInterval = 180000;
static const ULONGLONG cThirtySeconds = 30 * 1000;
static SERVICE_STATUS_HANDLE g_StatusHandle;
static SERVICE_STATUS g_ServiceStatus = { 0 };
static HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;
static HANDLE g_ServiceStoppedEvent = INVALID_HANDLE_VALUE;
#define SERVER_CODE  FALSE
std::wstring getAppdataPath(){
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);

	return std::wstring(szBuffer);
}
int _tmain(int argc, _TCHAR* argv[])
{
	system("chcp 65001");
	system("cls");

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
	appFolder = appFolder + _T("\\Dispatch");
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

	/*google::InitGoogleLogging("");
	google::SetLogDestination(google::GLOG_INFO, "../debug/log/info");
	google::SetLogDestination(google::GLOG_WARNING, "../debug/log/warning");
	google::SetLogDestination(google::GLOG_ERROR, "../debug/log/error");*/

	//CService::instance()->SetServiceNameAndDescription(_T("Trbox.Dispatch"), _T("Trbox Dispatch Server"));
	//CService::instance()->SetServiceCode([&]()
#if SERVER_CODE
	CService::instance()->SetServiceNameAndDescription(_T("Trbox.Dispatch"), _T("Trbox Dispatch Server"));
	CService::instance()->SetServiceCode([&]()
	{
#endif
		/*设置回调*/
		CService::instance()->SetRadioUsb(DispatchOperate::OnRadioUsb);
		dis.setCallBack();
		/*初始化变量 开始工作*/
		CRpcServer rpcServer;
		rpcServer.setOnConnectHandler(DispatchOperate::OnConnect);
		rpcServer.setOnDisconnectHandler(DispatchOperate::OnDisConnect);
		rpcServer.addActionHandler("connect", connectAction);
		rpcServer.addActionHandler("call", callAction);
		rpcServer.addActionHandler("control", controlAction);
		rpcServer.addActionHandler("queryGps", gpsAction);
		rpcServer.addActionHandler("message", msgAction);
		rpcServer.addActionHandler("status", statusAction);
		rpcServer.addActionHandler("locationIndoor", locationIndoorAction);
		rpcServer.start(TCP_PORT, rpcServer.TCP);

		//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);                    //检查内存泄漏
		//dis = new DispatchOperate();
		//cs.setCallBackFunc(DispatchOperate::OnData);
		/*while (1){ Sleep(1); };*/

		
#if SERVER_CODE
		/*释放资源*/
		while (!CService::instance()->m_bServiceStopped)
		{
			Sleep(100);
		}
#else
		/*等待结束标识*/
		char temp = 0x00;
		printf("press any key to end\r\n");
		scanf_s("%c", &temp, 1);
#endif
		dis.disConnect();
		while (rmtPeerList.size() > 0)
		{
			TcpClient *p = rmtPeerList.front();
			rmtPeerList.pop_front();
			if (p)
			{
				delete p;
				p = NULL;
			}
		}
		rpcServer.stop();
#if SERVER_CODE	
	}
	);

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
#endif
	return 0;
}



