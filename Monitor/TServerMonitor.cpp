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
	m_handle = CreateThread(NULL, 0, monitorThread, this, THREAD_PRIORITY_NORMAL, NULL);
}


CTServerMonitor::~CTServerMonitor()
{
}


void CTServerMonitor::StartMonitor()
{
	//memcpy(serverName,lpName,300);
	memcpy(serverName, _T("Trbox.TServer"), 300);
	isStart = true;
	
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
		
		Sleep(10000);
	}
}