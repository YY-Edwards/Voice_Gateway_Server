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
#include "../lib/utf8/utf8.h"
//#include"AllCallEventAction.h"
//#include"CallEventAction.h"
//#include"CancelgpspEventAction.h"
//#include"ConnectEventAction.h"
//#include"GetGpsEventAction.h"
//#include "GetOverTurnGpsEventAction.h"
//#include"GroupCallEventAction.h"
//#include "RadioCheckEventAction.h"
//#include "RemotePowerOffEventAction.h"
//#include "RemotePowerOnEventAction.h"
//#include "SendGroupSmsEventAction.h"
//#include "SendSmsEventAction.h"
//#include "StopCallEventAction.h"
//#include "WiretapEventAction.h"



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
	


	//CService::instance()->SetServiceNameAndDescription(_T("Trbox.Dispatch"), _T("Trbox Dispatch Server"));
	//CService::instance()->SetServiceCode([&](){

		/*初始化变量 开始工作*/
		CRpcServer rpcServer;
		rpcServer.setOnConnectHandler(DispatchOperate::OnConnect);
		rpcServer.setOnDisconnectHandler(DispatchOperate::OnDisConnect);

		//rpcServer.addActionHandler("allCall",allCallEventAction);
		//rpcServer.addActionHandler("call",callEventAction);
		//rpcServer.addActionHandler("cancelGps",cancelgpspEventAction);
		//rpcServer.addActionHandler("connect",connectEventAction);
		//rpcServer.addActionHandler("getGps",getGpsEventAction);
		//rpcServer.addActionHandler("getOverTurnGps",getOverTurnGpsEventAction);
		//rpcServer.addActionHandler("groupCall",groupCallEventAction);
		//rpcServer.addActionHandler("radioCheck",radioCheckEventAction);
		//rpcServer.addActionHandler("remotePowerOn",remotePowerOnEventAction);
		//rpcServer.addActionHandler("remotePowerOff",remotePowerOffEventAction);
		//rpcServer.addActionHandler("sendGroupSms", sendGroupSmsEventAction);
		//rpcServer.addActionHandler("sendSms", sendSmsEventAction);
		//rpcServer.addActionHandler("stopCall",stopCallEventAction);
		//rpcServer.addActionHandler("wiretap",wiretapEventAction);
		rpcServer.addActionHandler("connect", connectAction);
		rpcServer.addActionHandler("call", callAction);
		rpcServer.addActionHandler("control", controlAction);
		rpcServer.addActionHandler("queryGps", gpsAction);
		rpcServer.addActionHandler("message", msgAction);
		rpcServer.addActionHandler("status", statusAction);



		rpcServer.start(TCP_PORT, rpcServer.TCP);

		dis.setCallBack();

		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);                    //检查内存泄漏
		//dis = new DispatchOperate();
		//cs.setCallBackFunc(DispatchOperate::OnData);
		/*while (1){ Sleep(1); };*/
		/*等待结束标识*/
		char temp = 0x00;
		printf("press any key to end\r\n");
		scanf_s("%c", &temp, 1);

		/*释放资源*/
		dis.disConnect();
//	});

	
	//std::wstring strArg = argv[1];
	//try{
	//	if (0 == strArg.compare(_T("install")))
	//	{
	//		CService::instance()->InstallService();
	//		//InstallService();
	//	}
	//	else if (0 == strArg.compare(_T("uninstall")))
	//	{
	//		CService::instance()->UninstallService();
	//		//LOG(INFO) << "UnInstall Service";
	//	}
	//	else if (0 == strArg.compare(_T("start")))
	//	{
	//		CService::instance()->StartWindowsService();
	//		//LOG(INFO) << "Start Service";
	//	}
	//	else if (0 == strArg.compare(_T("stop")))
	//	{
	//		CService::instance()->StopService();
	//		//LOG(INFO) << "Stop Service";
	//	}
	//	else if (0 == strArg.compare(_T("run")))
	//	{
	//		CService::instance()->RunService();
	//	}
	//}
	//catch (std::system_error syserr) {
	//	exit(1);
	//}
	//catch (std::runtime_error runerr) {
	//	exit(1);
	//}
	//catch (...) {
	//	exit(1);
	//}

	//wprintf(argv[1]);
	return 0;
}



