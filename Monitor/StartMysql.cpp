#include "stdafx.h"
#include "StartMysql.h"


CStartMysql::CStartMysql()
{
	isStartMonitor = false;
}


CStartMysql::~CStartMysql()
{
}


void CStartMysql::startMysql()
{
	isStartMonitor = true;
	CreateThread(NULL, 0, monitorMysqlStatusThread, this, THREAD_PRIORITY_NORMAL, NULL);
}

DWORD WINAPI CStartMysql::monitorMysqlStatusThread(LPVOID lpParam)
{
	CStartMysql * p = (CStartMysql *)(lpParam);
	if (p != NULL)
	{
		p->monitorMysqlStatusThreadFunc();
	}
	return 1;
}
void CStartMysql::monitorMysqlStatusThreadFunc()
{
	while (isStartMonitor)
	{
		// 打开服务管理对象
		SC_HANDLE hSC = ::OpenSCManager(NULL,
			NULL, GENERIC_EXECUTE);
		if (hSC == NULL)
		{
			//return;
		}
		// 打开mysql服务。
		SC_HANDLE hSvc = ::OpenService(hSC, L"wampmysqld",
			SERVICE_ALL_ACCESS);
		if (hSvc == NULL)
		{
			::CloseServiceHandle(hSC);
			//return;
		}
		// 获得服务的状态
		SERVICE_STATUS status;
		if (::QueryServiceStatus(hSvc, &status) == FALSE)
		{
			::CloseServiceHandle(hSvc);
			::CloseServiceHandle(hSC);
			//return;
		}
		//如果处于停止状态则启动服务
		if (status.dwCurrentState == SERVICE_STOPPED)
		{
			// 启动服务
			if (::StartService(hSvc, NULL, NULL) == FALSE)
			{
				
				::CloseServiceHandle(hSvc);
				::CloseServiceHandle(hSC);
				//return;
			}
			// 等待服务启动
			/*while (::QueryServiceStatus(hSvc, &status) == TRUE)
			{
			::Sleep(status.dwWaitHint);
			if (status.dwCurrentState == SERVICE_RUNNING)
			{
			::CloseServiceHandle(hSvc);
			::CloseServiceHandle(hSC);
			return;
			}
			}*/
		}
		::CloseServiceHandle(hSvc);
		::CloseServiceHandle(hSC);
		
		Sleep(10000);
	}
	
	
}