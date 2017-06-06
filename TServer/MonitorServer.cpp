#include "stdafx.h"
#include "MonitorServer.h"
#include <string>
#include <sstream>
#include <system_error>
#include <Shlwapi.h>
#include <shlobj.h>  
static const ULONGLONG cThirtySeconds = 30 * 1000;
CMonitorServer::CMonitorServer()
{
	isMonitor = true;
	memset(serverName,0,300);
	memset(logServerName,0,300);
}


CMonitorServer::~CMonitorServer()
{
}
void CMonitorServer::startMonitor(LPCTSTR lpName, LPCTSTR logServer)
{
	//int length = 0;
	//length = wcslen(lpName);
	//printf("\n%d", length);
	//wcscpy_s(serverName,(wchar_t*)lpName);
	//swprintf_s(serverName, L"%s", lpName);
	StrCpy(serverName, lpName);
	StrCpy(logServerName, logServer);
	m_handle = CreateThread(NULL, 0, monitorThread, this, THREAD_PRIORITY_NORMAL, NULL);
	m_logServerHandle = CreateThread(NULL, 0, logServerThread, this, THREAD_PRIORITY_NORMAL, NULL);
}
void CMonitorServer::stopMonitor()
{
	if (m_handle)
	{
		isMonitor = false;
		WaitForSingleObject(m_handle,1000);
		CloseHandle(m_handle);
	}
}
DWORD WINAPI CMonitorServer::monitorThread(LPVOID lpParam)
{
	CMonitorServer * p = (CMonitorServer *)(lpParam);
	if (p != NULL)
	{
		p->monitorThreadFunc();
	}
	return 1;
}
DWORD WINAPI CMonitorServer::logServerThread(LPVOID lpParam)
{
	CMonitorServer * p = (CMonitorServer *)(lpParam);
	if (p != NULL)
	{
		p->logServerThreadFunc();
	}
	return 1;
}
void CMonitorServer::logServerThreadFunc()
{
	while (isMonitor)
	{

		std::wstring userName = _T("NT AUTHORITY\\NetworkService");

		SC_HANDLE schSCManager;
		SC_HANDLE schService;

		SERVICE_STATUS_PROCESS ssStatus;
		//DWORD dwOldCheckPoint;
		//DWORD dwStartTickCount;
		//DWORD dwWaitTime;
		DWORD dwBytesNeeded;
		// Get a handle to the SCM database. 
		schSCManager = OpenSCManager(
			NULL,                    // local computer
			NULL,                    // ServicesActive database 
			SC_MANAGER_ALL_ACCESS);  // full access rights 

		if (NULL == schSCManager)
		{
			//throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");

		}
		// Get a handle to the service.
		schService = OpenService(
			schSCManager,         // SCM database 
			logServerName,            // name of service         
			SERVICE_ALL_ACCESS
			);  // full access 

		if (schService == NULL)
		{
			TCHAR szFilePath[MAX_PATH];
			::GetModuleFileName(NULL, szFilePath, MAX_PATH);
			(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
			std::wstring wstr;
			if ( 0 == wcscmp(logServerName , L"Trbox.Log"))
			{
				wstr = (std::wstring)szFilePath + _T("LogServer.exe");
			}
			else
			{
				return;
			}
			LPCWSTR pWstr = wstr.c_str();

			std::wstringstream args;
			//TCHAR m_szServiceRun[300];
			//StrCpy(m_szServiceRun, _T("run"));
			args << "\"" << pWstr << "\" ";
			args << _T("run");
			//创建服务  
			schService = ::CreateService(
				schSCManager, logServerName, logServerName,
				SERVICE_ALL_ACCESS,
				SERVICE_WIN32_OWN_PROCESS,
				SERVICE_AUTO_START,
				SERVICE_ERROR_NORMAL,
				args.str().c_str(),
				NULL, NULL, NULL,
				userName.c_str(),
				NULL);

			if (schService == NULL)
			{
				if (ERROR_SERVICE_EXISTS == GetLastError())
				{
					printf("Service already installed");
					exit(1);
				}
				else
				{
					//throw std::system_error(GetLastError(), std::system_category(), "CreateService failed");
				}
			}
		}

		// Check the status in case the service is not stopped. 

		if (!QueryServiceStatusEx(
			schService,                     // handle to service 
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded))              // size needed if buffer is too small
		{
			//printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		}
		else
		{
			switch (ssStatus.dwCurrentState)
			{
			case SERVICE_STOPPED:
			{
									if (FALSE == ::StartService(schService, NULL, NULL)) {
										int a = GetLastError();
										printf("Start LogServer failed %d \n", a);
									}

									// it will take at least a couple of seconds for the service to start.
									Sleep(2000);
									SERVICE_STATUS status;
									DWORD start = GetTickCount();
									while (QueryServiceStatus(schService, &status) == TRUE) {
										if (status.dwCurrentState == SERVICE_RUNNING) {
											break;
										}
										DWORD current = GetTickCount();
										if (current - start >= cThirtySeconds) {
											OutputDebugStringA("Service start timed out.");
											break;
										}
									}
			}
				break;
			case SERVICE_STOP_PENDING:
				break;

			}
		}
		::CloseServiceHandle(schService);
		::CloseServiceHandle(schSCManager);
		Sleep(10000);
	}
}
void CMonitorServer::monitorThreadFunc()
{
	while (isMonitor)
	{

		std::wstring userName = _T("NT AUTHORITY\\NetworkService");

		SC_HANDLE schSCManager;
		SC_HANDLE schService;

		SERVICE_STATUS_PROCESS ssStatus;
		//DWORD dwOldCheckPoint;
		//DWORD dwStartTickCount;
		//DWORD dwWaitTime;
		DWORD dwBytesNeeded;
		// Get a handle to the SCM database. 
		schSCManager = OpenSCManager(
			NULL,                    // local computer
			NULL,                    // ServicesActive database 
			SC_MANAGER_ALL_ACCESS);  // full access rights 

		if (NULL == schSCManager)
		{
			//throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");
			
		}
		// Get a handle to the service.
		schService = OpenService(
			schSCManager,         // SCM database 
			serverName,            // name of service         
			SERVICE_ALL_ACCESS
			);  // full access 

		if (schService == NULL)
		{
			TCHAR szFilePath[MAX_PATH];
			::GetModuleFileName(NULL, szFilePath, MAX_PATH);
			(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
			(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
			std::wstring wstr;
			if ( 0 == wcscmp(serverName, L"Trbox.Dispatch"))
			{
				 wstr = (std::wstring)szFilePath + _T("Device\\Dispatch.exe");
			}
			else if ( 0 == wcscmp(serverName , L"Trbox.Wirelan"))
			{
				 wstr = (std::wstring)szFilePath + _T("Device\\WirelineSchedule.exe");
			}
			else
			{
				return;
			}
			LPCWSTR pWstr = wstr.c_str();

			std::wstringstream args;
			//TCHAR m_szServiceRun[300];
			//StrCpy(m_szServiceRun, _T("run"));
			args << "\"" << pWstr << "\" ";
			args << _T("run");
			//创建服务  
			schService = ::CreateService(
				schSCManager, serverName, serverName,
				SERVICE_ALL_ACCESS,
				SERVICE_WIN32_OWN_PROCESS,
				SERVICE_AUTO_START,
				SERVICE_ERROR_NORMAL,
				args.str().c_str(),
				NULL, NULL, NULL,
				userName.c_str(),
				NULL);

			if (schService == NULL)
			{
				int a = GetLastError();
				if (ERROR_SERVICE_EXISTS == GetLastError())
				{
					printf("Service already installed");
					exit(1);
				}
				else
				{
					//throw std::system_error(GetLastError(), std::system_category(), "CreateService failed");
				}
			}
		}

		// Check the status in case the service is not stopped. 

		if (!QueryServiceStatusEx(
			schService,                     // handle to service 
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded))              // size needed if buffer is too small
		{
			//printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		}
		else
		{
			switch (ssStatus.dwCurrentState)
			{
			case SERVICE_STOPPED:
			{
									if (FALSE == ::StartService(schService, NULL, NULL)) {
										int a = GetLastError();
										printf("StartService failed %d \n", a);
									}

									// it will take at least a couple of seconds for the service to start.
									Sleep(2000);
									SERVICE_STATUS status;
									DWORD start = GetTickCount();
									while (QueryServiceStatus(schService, &status) == TRUE) {
										if (status.dwCurrentState == SERVICE_RUNNING) {
											break;
										}
										DWORD current = GetTickCount();
										if (current - start >= cThirtySeconds) {
											OutputDebugStringA("Service start timed out.");
											break;
										}
									}
			}
				break;
			case SERVICE_STOP_PENDING:
				break;

			}
		}
		::CloseServiceHandle(schService);
		::CloseServiceHandle(schSCManager);
		Sleep(10000);
	}
}