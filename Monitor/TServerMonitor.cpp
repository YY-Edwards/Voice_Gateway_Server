#include "stdafx.h"
#include "TServerMonitor.h"
#include <string>
#include <sstream>
#include <system_error>
#include <Shlwapi.h>
#include <shlobj.h>  
static const ULONGLONG cThirtySeconds = 30 * 1000;
CTServerMonitor::CTServerMonitor()
{
	isMonitor = true;
	isStart = false;
	
}


CTServerMonitor::~CTServerMonitor()
{
}


void CTServerMonitor::StartMonitor()
{
	//memcpy(serverName,lpName,300);
	memset(logServerName, 0, 300);
	StrCpy(logServerName, _T("Trbox.Log"));
	memcpy(serverName, _T("Trbox.TServer"), 300);
	if (NULL == m_handle)
	{
		m_handle = CreateThread(NULL, 0, monitorThread, this, THREAD_PRIORITY_NORMAL, NULL);
	}
	if (NULL == m_logHandle)
	{
		m_logHandle = CreateThread(NULL, 0, logServerThread, this, THREAD_PRIORITY_NORMAL, NULL);
	}
	isStart = true;
	isMonitor = true;
}
void CTServerMonitor::stopMonitor()
{
	if (m_handle)
	{
		isStart = false;
		isMonitor = false;
		WaitForSingleObject(m_handle, 1000);
		CloseHandle(m_handle);
		m_handle = NULL;
	}
	if (m_logHandle)
	{
		isStart = false;
		isMonitor = false;
		WaitForSingleObject(m_logHandle, 1000);
		CloseHandle(m_logHandle);
		m_logHandle = NULL;
	}
}
DWORD WINAPI CTServerMonitor::monitorThread(LPVOID lpParam)
{
	CTServerMonitor * p = (CTServerMonitor *)(lpParam);
	if (p != NULL)
	{
		p->monitorThreadFunc();
	}
	return 1;
}
void CTServerMonitor::monitorThreadFunc()
{
	while (isMonitor)
	{
		if (isStart)
		{
			//LOG(INFO) << " startMonitor ";
			std::wstring userName = _T("NT AUTHORITY\\NetworkService");

			SC_HANDLE schSCManager;
			SC_HANDLE schService;

			SERVICE_STATUS_PROCESS ssStatus;

			DWORD dwBytesNeeded;
			// Get a handle to the SCM database. 
			schSCManager = OpenSCManager(
				NULL,                    // local computer
				NULL,                    // ServicesActive database 
				SC_MANAGER_ALL_ACCESS);  // full access rights 

			if (NULL == schSCManager)
			{
				//throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");
				std::string str = std::to_string(GetLastError());
				LOG(INFO) << " tServer OpenSCManager failed" +  str;
			}
			// Get a handle to the service.
			schService = OpenService(
				schSCManager,         // SCM database 
				_T("Trbox.TServer"),            // name of service         
				SERVICE_ALL_ACCESS
				);  // full access 

			if (schService == NULL)
			{
				TCHAR szFilePath[MAX_PATH];
				::GetModuleFileName(NULL, szFilePath, MAX_PATH);
				(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
				std::wstring wstr;
				if (0 == wcscmp(_T("Trbox.TServer"), L"Trbox.TServer"))
				{
					wstr = (std::wstring)szFilePath + _T("TServer.exe");
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
					NULL,
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
						std::string str = std::to_string(GetLastError());
						LOG(INFO) << " TServer CreateService failed" + str;
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
				std::string str = std::to_string(GetLastError());
				LOG(INFO) << " TServer QueryServiceStatusEx failed" + str;;
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
											std::string str = std::to_string(GetLastError());
											LOG(INFO) << " TServer StartService failed" + str;										}

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
		}
		
		Sleep(30000);
	}
}
void CTServerMonitor::stopServer(LPCTSTR lpName)
{

	SC_HANDLE hSC = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
	SC_HANDLE	hSvc = OpenService(
		hSC,         // SCM database 
		lpName,            // name of service         
		SERVICE_ALL_ACCESS
		);  // full access 
	if (hSvc != NULL)
	{
		SERVICE_STATUS status;
		if (::QueryServiceStatus(hSvc, &status) == FALSE)
		{
			TRACE(_T("Get Service state error。"));
			::CloseServiceHandle(hSvc);
			::CloseServiceHandle(hSC);
		}
		//如果处于停止状态则启动服务，否则停止服务。
		if (status.dwCurrentState == SERVICE_RUNNING || status.dwCurrentState == SERVICE_START_PENDING)
		{
			if (::ControlService(hSvc,
				SERVICE_CONTROL_STOP, &status) == FALSE)
			{

				::CloseServiceHandle(hSvc);
				::CloseServiceHandle(hSC);
				return;
			}
			// 等待服务停止
			DWORD start = GetTickCount();
			while (::QueryServiceStatus(hSvc, &status) == TRUE)
			{
				::Sleep(status.dwWaitHint);
				if (status.dwCurrentState == SERVICE_STOPPED)
				{

					::CloseServiceHandle(hSvc);
					::CloseServiceHandle(hSC);
					break;
				}
				DWORD current = GetTickCount();
				if (current - start >= cThirtySeconds) {
					//OutputDebugStringA("Service stop timed out.");
					break;
				}
			}
		}
	}
}
DWORD WINAPI CTServerMonitor::logServerThread(LPVOID lpParam)
{
	CTServerMonitor * p = (CTServerMonitor *)(lpParam);
	if (p != NULL)
	{
		p->logServerThreadFunc();
	}
	return 1;
}
void CTServerMonitor::logServerThreadFunc()
{
	while (isMonitor)
	{
		if (isStart)
		{
			std::wstring userName = _T("NT AUTHORITY\\NetworkService");

			SC_HANDLE schSCManager;
			SC_HANDLE schService;

			SERVICE_STATUS_PROCESS ssStatus;
			DWORD dwOldCheckPoint;
			DWORD dwStartTickCount;
			DWORD dwWaitTime;
			DWORD dwBytesNeeded;
			// Get a handle to the SCM database. 
			schSCManager = OpenSCManager(
				NULL,                    // local computer
				NULL,                    // ServicesActive database 
				SC_MANAGER_ALL_ACCESS);  // full access rights 

			if (NULL == schSCManager)
			{
				//throw std::system_error(GetLastError(), std::system_category(), "OpenSCManager failed");
				std::string str = std::to_string(GetLastError());
				LOG(INFO) << " Log Server OpenSCManager failed" + str;
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
				if (0 == wcscmp(logServerName, L"Trbox.Log"))
				{
					wstr = (std::wstring)szFilePath + _T("LogServer.exe");
				}
				else
				{
					LOG(INFO) << "logServerName != Trbox.log";
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
					NULL,
					NULL);

				if (schService == NULL)
				{
					if (ERROR_SERVICE_EXISTS == GetLastError())
					{
						//printf("Service already installed");
						LOG(INFO) << " Service already installed";
						exit(1);
					}
					else
					{
						//throw std::system_error(GetLastError(), std::system_category(), "CreateService failed");
						std::string str = std::to_string(GetLastError());
						LOG(INFO) << " Log Server CreateService failed" + str;
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
				std::string str = std::to_string(GetLastError());
				LOG(INFO) << " Log Server QueryServiceStatusEx failed" + str;
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
											//printf("Start LogServer failed %d \n", a);
											std::string str = std::to_string(GetLastError());
											LOG(INFO) << " Log Server Start failed" + str;
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
		}

		Sleep(30000);
	}
}