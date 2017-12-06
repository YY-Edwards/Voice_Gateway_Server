#include "stdafx.h"

#include <string>
#include <sstream>
#include <system_error>
#include <Shlwapi.h>
#include <shlobj.h>  
#include <tchar.h>
#include <WinSvc.h>
#include <Dbt.h>
#include <initguid.h>
#include <Ndisguid.h>
#include "service.h"

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "Shlwapi.lib")

std::auto_ptr<CService> CService::m_instance;
static const int cPreshutdownInterval = 180000;
static const ULONGLONG cThirtySeconds = 30 * 1000;

CService::CService()
{
	ZeroMemory(&m_ServiceStatus, sizeof(m_ServiceStatus));
	m_ServiceStopEvent = INVALID_HANDLE_VALUE;
	m_ServiceStoppedEvent = INVALID_HANDLE_VALUE;

	m_bServiceStopped = FALSE;
}

CService::~CService()
{

}
void CService::SetServiceNameAndDescription(LPCTSTR pServiceName, LPCTSTR pServiceDescription)
{
	StrCpy(m_szServiceName, pServiceName);
	StrCpy(m_szServiceRun, _T("run"));
	StrCpy(m_szServiceDescription, pServiceDescription);
}

void CService::InstallService(){
	TCHAR szPath[MAX_PATH];
	std::wstring userName = _T("NT AUTHORITY\\NetworkService");

	// build arguments to pass to service when it auto starts
	if (GetModuleFileName(NULL, szPath, MAX_PATH) == 0) {
		throw std::system_error(GetLastError(), std::system_category(), ("ServiceInstall: GetModuleFileNameA failed"));
	}

	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCM)
	{
		throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");
	}

	if (GetModuleFileName(NULL, szPath, MAX_PATH) == 0) {
		throw std::system_error(GetLastError(), std::system_category(), "ServiceInstall: GetModuleFileNameA failed");
	}

	std::wstringstream args;
	args << "\"" << szPath << "\" ";
	args << m_szServiceRun;

	SC_HANDLE hService = CreateService(
		hSCM,
		m_szServiceName,
		m_szServiceName,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		args.str().c_str(),
		NULL, NULL, NULL,
		NULL,//userName.c_str(),
		NULL);


	if (NULL == hService) {
		if (ERROR_SERVICE_EXISTS == GetLastError())
		{
			printf("Service already installed");
			exit(1);
		}
		else {
			throw std::system_error(GetLastError(), std::system_category(), "CreateService failed");
		}
	}

	SERVICE_DESCRIPTION desc;
	desc.lpDescription = m_szServiceDescription;
	if (FALSE == ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &desc)) {
		throw std::system_error(GetLastError(), std::system_category(), "ChangeServiceConfig2 failed");
	}

	SERVICE_PRESHUTDOWN_INFO preshutdownInfo;
	preshutdownInfo.dwPreshutdownTimeout = cPreshutdownInterval;
	if (FALSE == ChangeServiceConfig2(hService, SERVICE_CONFIG_PRESHUTDOWN_INFO, &preshutdownInfo)) {
		throw std::system_error(GetLastError(), std::system_category(), "ChangeServiceConfig2 failed");
	}

	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);
}

void CService::UninstallService(){
	SC_HANDLE hSCM;
	SC_HANDLE hService;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCM) {
		throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");
	}
	hService = OpenService(hSCM, m_szServiceName, SERVICE_ALL_ACCESS);
	if (NULL != hService) {
		if (FALSE == DeleteService(hService)) {
			throw std::system_error(GetLastError(), std::system_category(), "DeleteService failed");
		}
	}
}


DWORD WINAPI CService::ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) 
{
	switch (dwControl) 
	{
		case SERVICE_CONTROL_PRESHUTDOWN:
		{
			SetEvent(CService::instance()->m_ServiceStopEvent); //(g_ServiceStopEvent);
		
			CService::instance()->m_ServiceStatus.dwControlsAccepted = 0;
			CService::instance()->m_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			CService::instance()->m_ServiceStatus.dwWin32ExitCode = 0;
			CService::instance()->m_ServiceStatus.dwCheckPoint = 4;

			if (SetServiceStatus(CService::instance()->m_StatusHandle, &CService::instance()->m_ServiceStatus) == FALSE)
			{
				throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
			}

			break;
		}

		case SERVICE_CONTROL_STOP:
		{
			DWORD start = GetTickCount();
			while (GetTickCount() - start > cPreshutdownInterval) {
				if (WaitForSingleObject(CService::instance()->m_ServiceStopEvent, cPreshutdownInterval / 10) == WAIT_OBJECT_0) {
					break;
				}

				CService::instance()->m_ServiceStatus.dwControlsAccepted = 0;
				CService::instance()->m_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
				CService::instance()->m_ServiceStatus.dwWin32ExitCode = 0;
				CService::instance()->m_ServiceStatus.dwCheckPoint = 4;

				if (SetServiceStatus(CService::instance()->m_StatusHandle, &CService::instance()->m_ServiceStatus) == FALSE)
				{
					throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
				}
			}

			CService::instance()->m_ServiceStatus.dwControlsAccepted = 0;
			CService::instance()->m_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
			CService::instance()->m_ServiceStatus.dwWin32ExitCode = 0;
			CService::instance()->m_ServiceStatus.dwCheckPoint = 4;

			if (SetServiceStatus(CService::instance()->m_StatusHandle, &CService::instance()->m_ServiceStatus) == FALSE)
			{
				throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
			}

			CService::instance()->m_bServiceStopped = TRUE;

			break;
		}
			/*radio usb ¼à²â     add by wx */
		case SERVICE_CONTROL_DEVICEEVENT:
		{

			switch (dwEventType)
			{
			case DBT_DEVICEREMOVECOMPLETE:
			{
				CService::instance()->m_radioUsb(false);
			}
				break;
			case DBT_DEVICEARRIVAL:
			{
				CService::instance()->m_radioUsb(true);
			}
				break;
			}
		}
			break;
		default:
		{
			break;
		}
	}

	return NO_ERROR;
}
DWORD WINAPI CService::ServiceWorkerThread(LPVOID lpParam)
{
	try {
		OutputDebugString(_T("service work thread runned!\r\n"));

		if (nullptr != CService::instance()->m_fnServiceCode)
		{
			CService::instance()->m_fnServiceCode();
		}
		while (WaitForSingleObject(CService::instance()->m_ServiceStopEvent, 0) != WAIT_OBJECT_0);
		//SetEvent(CService::instance()->m_ServiceStopEvent);

		return ERROR_SUCCESS;
	}
	catch (std::system_error syserr) {
		std::stringstream err;
		err << "ServiceWorkerThread: system error caught. error code=0x" << syserr.code().value() << ", message = " << syserr.what() << std::endl;
		OutputDebugStringA(err.str().c_str());
	}
	catch (std::runtime_error runerr) {
		std::stringstream err;
		err << "runtime error caught. message=" << runerr.what() << std::endl;
		OutputDebugStringA(err.str().c_str());
	}
	catch (...) {
		OutputDebugStringA("ServiceWorkerThread: other exception caught.\n");
	}

	return  ERROR_PROCESS_ABORTED;
}

VOID WINAPI CService::ServiceMain(DWORD argc, LPTSTR *argv)
{
	DWORD Status = E_FAIL;
	
	CService::instance()->m_StatusHandle = RegisterServiceCtrlHandlerEx(CService::instance()->m_szServiceName, ServiceCtrlHandler, NULL);
	if (CService::instance()->m_StatusHandle == NULL) {
		return;
	}
	
	ZeroMemory(&CService::instance()->m_ServiceStatus, sizeof(SERVICE_STATUS));
	CService::instance()->m_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	CService::instance()->m_ServiceStatus.dwControlsAccepted = 0;
	CService::instance()->m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	CService::instance()->m_ServiceStatus.dwWin32ExitCode = 0;
	CService::instance()->m_ServiceStatus.dwServiceSpecificExitCode = 0;
	CService::instance()->m_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(CService::instance()->m_StatusHandle, &CService::instance()->m_ServiceStatus) == FALSE)
	{
		throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
	}
	
	CService::instance()->m_ServiceStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	CService::instance()->m_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (CService::instance()->m_ServiceStopEvent == NULL) {
		CService::instance()->m_ServiceStatus.dwControlsAccepted = 0;
		CService::instance()->m_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		CService::instance()->m_ServiceStatus.dwWin32ExitCode = GetLastError();
		CService::instance()->m_ServiceStatus.dwCheckPoint = 1;

		if (SetServiceStatus(CService::instance()->m_StatusHandle, &CService::instance()->m_ServiceStatus) == FALSE)
		{
			throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
		}

		return;
	}

	CService::instance()->m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PRESHUTDOWN;
	CService::instance()->m_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	CService::instance()->m_ServiceStatus.dwWin32ExitCode = 0;
	CService::instance()->m_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(CService::instance()->m_StatusHandle, &CService::instance()->m_ServiceStatus) == FALSE)
	{
		throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
	}

	HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(CService::instance()->m_ServiceStopEvent);

	CService::instance()->m_ServiceStatus.dwControlsAccepted = 0;
	CService::instance()->m_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	CService::instance()->m_ServiceStatus.dwWin32ExitCode = 0;
	CService::instance()->m_ServiceStatus.dwCheckPoint = 3;

	if (SetServiceStatus(CService::instance()->m_StatusHandle, &CService::instance()->m_ServiceStatus) == FALSE)
	{
		throw std::system_error(GetLastError(), std::system_category(), "SetServiceStatus failed");
	}
}

void CService::RunService()
{
	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{ m_szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};

	if (StartServiceCtrlDispatcher(ServiceTable) == FALSE) 
	{
		throw std::system_error(GetLastError(), std::system_category(), "StartServiceCtrlDispatcherA failed");
	}
}

void CService::StartWindowsService()
{
	SC_HANDLE hSCM;
	SC_HANDLE shService;
	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwBytesNeeded;
	DWORD dwOldCheckPoint;
	DWORD dwStartTickCount;
	DWORD dwWaitTime;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCM) {
		throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");
	}
	shService = OpenService(hSCM, m_szServiceName, SERVICE_ALL_ACCESS);
	if (NULL == shService)
	{
		throw std::system_error(GetLastError(), std::system_category(), "OpenService failed");
	}
	if (!QueryServiceStatusEx(
		shService,                     // handle to service 
		SC_STATUS_PROCESS_INFO,         // information level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded))              // size needed if buffer is too small
	{
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(shService);
		CloseServiceHandle(hSCM);
		return;
	}

	// Check if the service is already running. It would be possible 
	// to stop the service here, but for simplicity this example just returns. 

	if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		printf("Cannot start the service because it is already running\n");
		CloseServiceHandle(shService);
		CloseServiceHandle(hSCM);
		return;
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	// Wait for the service to stop before attempting to start it.

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds. 

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status until the service is no longer stop pending. 

		if (!QueryServiceStatusEx(
			shService,                     // handle to service 
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded))              // size needed if buffer is too small
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			CloseServiceHandle(shService);
			CloseServiceHandle(hSCM);
			return;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// Continue to wait and check.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				printf("Timeout waiting for service to stop\n");
				CloseServiceHandle(shService);
				CloseServiceHandle(hSCM);
				return;
			}
		}
	}


	if (FALSE == ::StartService(shService, 0, NULL)) {
		throw std::system_error(GetLastError(), std::system_category(), "StartService failed");
	}

	// it will take at least a couple of seconds for the service to start.
	Sleep(2000);

	SERVICE_STATUS status;
	DWORD start = GetTickCount();
	while (QueryServiceStatus(shService, &status) == TRUE) {
		if (status.dwCurrentState == SERVICE_RUNNING) {
			break;
		}
		DWORD current = GetTickCount();
		if (current - start >= cThirtySeconds) {
			OutputDebugStringA("Service start timed out.");
			break;
		}
	}

	::CloseServiceHandle(shService);
	::CloseServiceHandle(hSCM);
}

void CService::StopService()
{
	SC_HANDLE hSCM;
	SC_HANDLE shService;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCM) {
		throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");
	}
	shService = OpenService(hSCM, m_szServiceName, SERVICE_ALL_ACCESS);
	if (NULL == shService) {
		throw std::system_error(GetLastError(), std::system_category(), "OpenService failed");
	}
	SERVICE_STATUS status;
	if (FALSE == ControlService(shService, SERVICE_CONTROL_STOP, &status)) {
		throw std::system_error(GetLastError(), std::system_category(), "ControlService failed");
	}

	DWORD start = GetTickCount();
	while (QueryServiceStatus(shService, &status) == TRUE) {
		if (status.dwCurrentState == SERVICE_STOPPED) {
			OutputDebugStringA("Service stopped.");
			break;
		}
		DWORD current = GetTickCount();
		if (current - start >= cThirtySeconds) {
			OutputDebugStringA("Service stop timed out.");
			break;
		}
	}
}

void CService::SetServiceCode(std::function<void(void)> fn)
{
	m_fnServiceCode = fn;
}
void CService::SetRadioUsb(std::function < void(bool)>radioUsb)
{
	m_radioUsb = radioUsb;
}