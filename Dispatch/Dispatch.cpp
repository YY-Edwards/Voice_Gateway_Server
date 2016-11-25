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


int _tmain(int argc, _TCHAR* argv[])
{

	google::InitGoogleLogging("");
	google::SetLogDestination(google::GLOG_INFO, "../debug/log/info");
	google::SetLogDestination(google::GLOG_WARNING, "../debug/warning/info");
	google::SetLogDestination(google::GLOG_ERROR, "../debug/error/info");

	//CService::instance()->SetServiceNameAndDescription(_T("Trbox.Dispatch"), _T("Trbox Dispatch Server"));
	//CService::instance()->SetServiceCode([&](){
	/*设置回调*/
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
	rpcServer.start(TCP_PORT, rpcServer.TCP);

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);                    //检查内存泄漏
	//dis = new DispatchOperate();
	//cs.setCallBackFunc(DispatchOperate::OnData);
	/*while (1){ Sleep(1); };*/
	/*等待结束标识*/
	char temp = 0x00;
	printf("press any key to end\r\n");
	scanf_s("%c", &temp, 1);

	/*释放资源*/
	//while (!CService::instance()->m_bServiceStopped);
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
	
	/*	});


	std::wstring strArg = argv[1];
	try{
		if (0 == strArg.compare(_T("install")))
		{
			CService::instance()->InstallService();
			
			LOG(INFO) << "Install Service";
		}
		else if (0 == strArg.compare(_T("uninstall")))
		{
			CService::instance()->UninstallService();
			LOG(INFO) << "UnInstall Service";
		}
		else if (0 == strArg.compare(_T("start")))
		{
			CService::instance()->StartWindowsService();
			LOG(INFO) << "Start Service";
		}
		else if (0 == strArg.compare(_T("stop")))
		{
			CService::instance()->StopService();
			LOG(INFO) << "Stop Service";
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

	wprintf(argv[1]);*/
	return 0;
}



