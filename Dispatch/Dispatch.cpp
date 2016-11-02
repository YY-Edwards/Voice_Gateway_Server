// Dispatch.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <sstream>
//#include <system_error>
#include <Shlwapi.h>
#include <shlobj.h>  
#include "Util.h"
#include "../lib/rpc/include/RpcServer.h"
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




//void StartService()
//{
//	SC_HANDLE hSCM;
//	SC_HANDLE shService;
//	SERVICE_STATUS_PROCESS ssStatus;
//	DWORD dwBytecallIdeeded;
//	DWORD dwOldCheckPoint;
//	DWORD dwStartTickCount;
//	DWORD dwWaitTime;
//
//	LOG(INFO) << "StartService Entry";
//
//	hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
//	if (NULL == hSCM) {
//		throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");
//	}
//	shService = OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS);
//	if (NULL == shService) {
//		throw std::system_error(GetLastError(), std::system_category(), "OpenService failed");
//	}
//	if (!QueryServiceStatusEx(
//		shService,                     // handle to service 
//		SC_STATUS_PROCESS_INFO,         // information level
//		(LPBYTE)&ssStatus,             // address of structure
//		sizeof(SERVICE_STATUS_PROCESS), // size of structure
//		&dwBytecallIdeeded))              // size needed if buffer is too small
//	{
//		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
//		CloseServiceHandle(shService);
//		CloseServiceHandle(hSCM);
//		return;
//	}
//
//	// Check if the service is already running. It would be possible 
//	// to stop the service here, but for simplicity this example just returns. 
//
//	if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
//	{
//		printf("Cannot start the service because it is already running\n");
//		CloseServiceHandle(shService);
//		CloseServiceHandle(hSCM);
//		return;
//	}
//
//	// Save the tick count and initial checkpoint.
//
//	dwStartTickCount = GetTickCount();
//	dwOldCheckPoint = ssStatus.dwCheckPoint;
//
//	// Wait for the service to stop before attempting to start it.
//
//	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
//	{
//		// Do not wait longer than the wait hint. A good interval is 
//		// one-tenth of the wait hint but not less than 1 second  
//		// and not more than 10 seconds. 
//
//		dwWaitTime = ssStatus.dwWaitHint / 10;
//
//		if (dwWaitTime < 1000)
//			dwWaitTime = 1000;
//		else if (dwWaitTime > 10000)
//			dwWaitTime = 10000;
//
//		Sleep(dwWaitTime);
//
//		// Check the status until the service is no longer stop pending. 
//
//		if (!QueryServiceStatusEx(
//			shService,                     // handle to service 
//			SC_STATUS_PROCESS_INFO,         // information level
//			(LPBYTE)&ssStatus,             // address of structure
//			sizeof(SERVICE_STATUS_PROCESS), // size of structure
//			&dwBytecallIdeeded))              // size needed if buffer is too small
//		{
//			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
//			CloseServiceHandle(shService);
//			CloseServiceHandle(hSCM);
//			return;
//		}
//
//		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
//		{
//			// Continue to wait and check.
//
//			dwStartTickCount = GetTickCount();
//			dwOldCheckPoint = ssStatus.dwCheckPoint;
//		}
//		else
//		{
//			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
//			{
//				printf("Timeout waiting for service to stop\n");
//				CloseServiceHandle(shService);
//				CloseServiceHandle(hSCM);
//				return;
//			}
//		}
//	}
//
//
//	if (FALSE == StartService(shService, 0, NULL)) {
//	
//		throw std::system_error(GetLastError(), std::system_category(), "StartService failed");
//	}
//
//	// it will take at least a couple of seconds for the service to start.
//	Sleep(2000);
//
//	SERVICE_STATUS status;
//	DWORD start = GetTickCount();
//	while (QueryServiceStatus(shService, &status) == TRUE) {
//		if (status.dwCurrentState == SERVICE_RUNNING) {
//			break;
//		}
//		DWORD current = GetTickCount();
//		if (current - start >= cThirtySeconds) {
//			OutputDebugStringA("Service start timed out.");
//			LOG(ERROR) << "Service start timed out.";
//			break;
//		}
//	}
//
//	LOG(INFO) << "StartService Exit";
//
//	::CloseServiceHandle(shService);
//	::CloseServiceHandle(hSCM);
//}
//void InstallService()
//{
//	TCHAR szPath[MAX_PATH];
//	std::wstring userName = _T("NT AUTHORITY\\NetworkService");
//
//	// build arguments to pass to service when it auto starts
//	if (GetModuleFileName(NULL,szPath, MAX_PATH) == 0) {
//		throw std::system_error(GetLastError(), std::system_category(), ("ServiceInstall: GetModuleFileNameA failed"));
//	}
//
//	SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
//	if (NULL == hSCM)
//	{
//
//		throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");
//	}
//
//	if (GetModuleFileName(NULL,szPath, MAX_PATH) == 0) {
//		throw std::system_error(GetLastError(), std::system_category(), "ServiceInstall: GetModuleFileNameA failed");
//	}
//
//	std::wstringstream args;
//	args << "\"" << szPath << "\" ";
//	args << szServiceRun;
//
//	SC_HANDLE hService = CreateService(
//		hSCM,
//		szServiceName,
//		szServiceName,
//		SERVICE_ALL_ACCESS,
//		SERVICE_WIN32_OWN_PROCESS,
//		SERVICE_AUTO_START,
//		SERVICE_ERROR_NORMAL,
//		args.str().c_str(),
//		"","",""
//		userName.c_str(),
//		NULL);
//
//
//	if (NULL == hService) {
//		if (ERROR_SERVICE_EXISTS == GetLastError())
//		{
//			printf("Service already installed");
//			exit(1);
//		}
//		else {
//			throw std::system_error(GetLastError(), std::system_category(), "CreateService failed");
//		}
//	}
//
//	SERVICE_DESCRIPTION desc;
//	desc.lpDescription = _T("上海济华信息系统有限公司，调度系统");
//	if (FALSE == ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &desc)) {
//		throw std::system_error(GetLastError(), std::system_category(), "ChangeServiceConfig2 failed");
//	}
//
//	SERVICE_PRESHUTDOWN_INFO preshutdownInfo;
//	preshutdownInfo.dwPreshutdownTimeout = cPreshutdownInterval;
//	if (FALSE == ChangeServiceConfig2(hService, SERVICE_CONFIG_PRESHUTDOWN_INFO, &preshutdownInfo)) {
//		throw std::system_error(GetLastError(), std::system_category(), "ChangeServiceConfig2 failed");
//	}
//
//	::CloseServiceHandle(hService);
//	::CloseServiceHandle(hSCM);
//}
//std::wstring getAppdataPath(){
//	TCHAR szBuffer[MAX_PATH];
//	SHGetSpecialFolderPath(NULL,szBuffer, CSIDL_APPDATA, FALSE);
//
//	return std::wstring(szBuffer);
//}
//void UninstallService(){
//	SC_HANDLE hSCM;
//	SC_HANDLE hService;
//
//	hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
//	if (NULL == hSCM) {
//		throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");
//	}
//	hService = OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS);
//	if (NULL != hService) {
//		if (FALSE == DeleteService(hService)) {
//			throw std::system_error(GetLastError(), std::system_category(), "DeleteService failed");
//		}
//	}
//}
//void StopService(){
//	SC_HANDLE hSCM;
//	SC_HANDLE shService;
//
//	hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
//	if (NULL == hSCM) {
//		throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");
//	}
//	shService = OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS);
//	if (NULL == shService) {
//		throw std::system_error(GetLastError(), std::system_category(), "OpenService failed");
//	}
//	SERVICE_STATUS status;
//	if (FALSE == ControlService(shService, SERVICE_CONTROL_STOP, &status)) {
//		throw std::system_error(GetLastError(), std::system_category(), "ControlService failed");
//	}
//
//	DWORD start = GetTickCount();
//	while (QueryServiceStatus(shService, &status) == TRUE) {
//		if (status.dwCurrentState == SERVICE_STOPPED) {
//			OutputDebugStringA("Service stopped.");
//			break;
//		}
//		DWORD current = GetTickCount();
//		if (current - start >= cThirtySeconds) {
//			OutputDebugStringA("Service stop timed out.");
//			break;
//		}
//	}
//}
//
//DWORD WINAPI ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) {
//	switch (dwControl) {
//	case SERVICE_CONTROL_PRESHUTDOWN:
//	{
//										SetEvent(g_ServiceStopEvent);
//
//										g_ServiceStatus.dwControlsAccepted = 0;
//										g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
//										g_ServiceStatus.dwWin32ExitCode = 0;
//										g_ServiceStatus.dwCheckPoint = 4;
//
//										if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
//											throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
//										}
//
//										break;
//	}
//
//	case SERVICE_CONTROL_STOP:
//	{
//								 DWORD start = GetTickCount();
//								 while (GetTickCount() - start > cPreshutdownInterval) {
//									 if (WaitForSingleObject(g_ServiceStoppedEvent, cPreshutdownInterval / 10) == WAIT_OBJECT_0) {
//										 break;
//									 }
//
//									 g_ServiceStatus.dwControlsAccepted = 0;
//									 g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
//									 g_ServiceStatus.dwWin32ExitCode = 0;
//									 g_ServiceStatus.dwCheckPoint = 4;
//
//									 if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
//										 throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
//									 }
//								 }
//
//								 g_ServiceStatus.dwControlsAccepted = 0;
//								 g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
//								 g_ServiceStatus.dwWin32ExitCode = 0;
//								 g_ServiceStatus.dwCheckPoint = 4;
//
//								 if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
//									 throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
//								 }
//								 break;
//	}
//
//	default:
//	{
//			   break;
//	}
//	}
//
//	return NO_ERROR;
//}
//DWORD WINAPI ServiceWorkerThread(LPVOID lpParam) {
//	LOG(INFO) << "service work thread runned!";
//	try {
//		OutputDebugString(_T("service work thread runned!\r\n"));
//
//		/*FILE *f = fopen("e:\\log.txt", "w");
//		fwrite("sss", 3, 1, f);
//		fclose(f);
//		*/
//		// When the service starts the current directory is %systemdir%. If the launching user does not have permission there(i.e., NETWORK SERVICE), the 
//		// memory mapped file will not be able to be created. Thus Redis will fail to start. Setting the current directory to the executable directory
//		// should fix this.
//		//char szFilePath[MAX_PATH];
//		//if (GetModuleFileNameA(NULL,szFilePath, MAX_PATH) == 0) {
//		//	throw std::system_error(GetLastError(), std::system_category(), "ServiceWrokerThread: GetModuleFileName failed");
//		//}
//		//std::string currentDir = szFilePath;
//		//auto pos = currentDir.rfind("\\");
//		//currentDir.erase(pos);
//
//		//if (FALSE == SetCurrentDirectoryA(currentDir.c_str())) {
//		//	throw std::system_error(GetLastError(), std::system_category(), "SetCurrentDirectory failed");
//		//}
//		//CGatewayControl *pGateway = new CGatewayControl();
//		//pGateway->Start();
//		/*DispatchOperate *pDispatchOperate = new DispatchOperate();
//		pDispatchOperate->connect("192.168.10.2","192.168.10.2");*/
//		std::wstring		m_strFcsServerIP = _T("0.0.0.0");
//		DWORD fcsIp = inet_addr(CUtil::Utf16ToUtf8(m_strFcsServerIP.c_str()).c_str());
//		pDispatchPort->Start(fcsIp, 5000);
//		while (1){
//
//		}
//		//pGateway->Stop();
//
//		//delete pGateway;
//
//		SetEvent(g_ServiceStoppedEvent);
//
//		return ERROR_SUCCESS;
//	}
//	catch (std::system_error syserr) {
//		std::stringstream err;
//		err << "ServiceWorkerThread: system error caught. error code=0x" << syserr.code().value() << ", message = " << syserr.what() << std::endl;
//		OutputDebugStringA(err.str().c_str());
//	}
//	catch (std::runtime_error runerr) {
//		std::stringstream err;
//		err << "runtime error caught. message=" << runerr.what() << std::endl;
//		OutputDebugStringA(err.str().c_str());
//	}
//	catch (...) {
//		OutputDebugStringA("ServiceWorkerThread: other exception caught.\n");
//	}
//
//	return  ERROR_PROCESS_ABORTED;
//}
//VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv) {
//	LOG(INFO) << "ServiceMain Entry";
//	DWORD Status = E_FAIL;
//
//	g_StatusHandle = RegisterServiceCtrlHandlerEx(szServiceName, ServiceCtrlHandler, NULL);
//	if (g_StatusHandle == NULL) {
//		return;
//	}
//
//	ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
//	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
//	g_ServiceStatus.dwControlsAccepted = 0;
//	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
//	g_ServiceStatus.dwWin32ExitCode = 0;
//	g_ServiceStatus.dwServiceSpecificExitCode = 0;
//	g_ServiceStatus.dwCheckPoint = 0;
//
//	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
//		throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
//	}
//
//	g_ServiceStoppedEvent = CreateEvent(NULL,TRUE, FALSE, NULL);
//	g_ServiceStopEvent = CreateEvent(NULL,TRUE, FALSE, NULL);
//	if (g_ServiceStopEvent == NULL) {
//		g_ServiceStatus.dwControlsAccepted = 0;
//		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
//		g_ServiceStatus.dwWin32ExitCode = GetLastError();
//		g_ServiceStatus.dwCheckPoint = 1;
//
//		if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
//			LOG(ERROR) << "SetServiceStatus failed";
//			throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
//		}
//
//		return;
//	}
//
//	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PRESHUTDOWN;
//	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
//	g_ServiceStatus.dwWin32ExitCode = 0;
//	g_ServiceStatus.dwCheckPoint = 0;
//
//	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
//		throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
//	}
//
//	HANDLE hThread = CreateThread(NULL,0, ServiceWorkerThread, NULL,0, NULL);
//
//	WaitForSingleObject(hThread, INFINITE);
//
//	CloseHandle(g_ServiceStopEvent);
//
//	g_ServiceStatus.dwControlsAccepted = 0;
//	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
//	g_ServiceStatus.dwWin32ExitCode = 0;
//	g_ServiceStatus.dwCheckPoint = 3;
//
//	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
//		throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
//	}
//	LOG(INFO) << "ServiceMain Exit";
//}
//void RunService(){
//	LOG(INFO) << "RunService Entry";
//	SERVICE_TABLE_ENTRY ServiceTable[] =
//	{
//		{ szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
//		{ NULL,NULL }
//	};
//
//	if (StartServiceCtrlDispatcher(ServiceTable) == FALSE) {
//		throw std::system_error(GetLastError(), std::system_category(), "StartServiceCtrlDispatcherA failed");
//	}
//	LOG(INFO) << "RunService Exit";
//}
int _tmain(int argc, _TCHAR* argv[])
{


	/*std::wstring appFolder = getAppdataPath() + _T("\\Dispatch");
	if (!PathFileExists(appFolder.c_str()))
	{
		_wmkdir(appFolder.c_str());
	}

	std::wstring logFolder = appFolder + _T("\\log");
	if (!PathFileExists(logFolder.c_str()))
	{
		_wmkdir(logFolder.c_str());
	}

	std::wstring pathLogInfo = logFolder + _T("/info_");
	std::wstring pathLogError = logFolder + _T("/error_");
	std::wstring pathLogWarning = logFolder + _T("/warning_");

	google::InitGoogleLogging("");
	google::SetLogDestination(google::GLOG_INFO, CUtil::Utf16ToUtf8(pathLogInfo.c_str()).c_str());
	google::SetLogDestination(google::GLOG_ERROR, CUtil::Utf16ToUtf8(pathLogError.c_str()).c_str());
	google::SetLogDestination(google::GLOG_WARNING, CUtil::Utf16ToUtf8(pathLogWarning.c_str()).c_str());
	google::SetLogFilenameExtension("log");


	std::wstring strArg = argv[1];
	try{

		if (0 == strArg.compare(_T("install")))
		{
			InstallService();
			LOG(INFO) << "Install Service";
		}
		else if (0 == strArg.compare(_T("uninstall")))
		{
			UninstallService();
			LOG(INFO) << "UnInstall Service";
		}
		else if (0 == strArg.compare(_T("start")))
		{
			StartService();
			LOG(INFO) << "Start Service";
		}
		else if (0 == strArg.compare(_T("stop")))
		{
			StopService();
			LOG(INFO) << "Stop Service";
		}
		else if (0 == strArg.compare(_T("run")))
		{
			RunService();
		}
	}
	catch (std::system_error syserr) {
		std::stringstream ss;
		ss << "HandleServiceCommands: system error caught. error code=" << syserr.code().value() << ", message = " << syserr.what() << std::endl;
		LOG(ERROR) << ss.str();
		exit(1);
	}
	catch (std::runtime_error runerr) {
		std::stringstream err;
		err << "HandleServiceCommands: runtime error caught. message=" << runerr.what() << std::endl;
		LOG(ERROR) << err.str();
		exit(1);
	}
	catch (...) {
		std::stringstream ss;
		ss << "HandleServiceCommands: other exception caught." << std::endl;
		LOG(ERROR) << ss.str();
		exit(1);
	}

	wprintf(argv[1]);*/
	
	CRpcServer rpcServer;
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
	rpcServer.addActionHandler("connect",connectAction);
	rpcServer.addActionHandler("call", callAction);
	rpcServer.addActionHandler("control", controlAction);
	rpcServer.addActionHandler("queryGps", gpsAction);
	rpcServer.addActionHandler("message", msgAction);

	rpcServer.start(TCP_PORT, rpcServer.TCP);
	
	while (1){
		
	};
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);                    //检查内存泄漏

	rpcServer.start();
	while (1);

	return 0;
}



