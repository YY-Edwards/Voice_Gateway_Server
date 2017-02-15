#include "stdafx.h"
#include "TServerMonitor.h"

#include <string>
#include <sstream>
#include <system_error>
#include <Shlwapi.h>
#include <shlobj.h>  
#include "WriteLog.h"
static const ULONGLONG cThirtySeconds = 30 * 1000;
CTServerMonitor::CTServerMonitor()
{
	isMonitor = true;
}


CTServerMonitor::~CTServerMonitor()
{
}


void CTServerMonitor::StartMonitor(LPCTSTR lpName)
{
	//memcpy(serverName,lpName,300);
	memcpy(serverName, _T("Trbox.TServer"), 300);
	CWriteLog::instance()->WriteLog("Trbox.TServer\r\n");
	
	//StrCpy(serverName, lpName);
	m_handle = CreateThread(NULL, 0, monitorThread, this, THREAD_PRIORITY_NORMAL, NULL);
}
void CTServerMonitor::stopMonitor()
{
	if (m_handle)
	{
		isMonitor = false;
		WaitForSingleObject(m_handle, 1000);
		CloseHandle(m_handle);
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
		CWriteLog::instance()->WriteLog("startMonitor thread\r\n");
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
			CWriteLog::instance()->WriteLog("OpenSCManager failed\r\n");
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
			std::wstring wstr;
			if (0 == wcscmp(serverName, L"Trbox.TServer"))
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
			//��������  
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
					CWriteLog::instance()->WriteLog("CreateService  failed\r\n");
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
			CWriteLog::instance()->WriteLog("QueryServiceStatusEx failed\r\n");
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