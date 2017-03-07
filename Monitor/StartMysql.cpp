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
		// �򿪷���������
		SC_HANDLE hSC = ::OpenSCManager(NULL,
			NULL, GENERIC_EXECUTE);
		if (hSC == NULL)
		{
			//return;
		}
		// ��mysql����
		SC_HANDLE hSvc = ::OpenService(hSC, L"wampmysqld",
			SERVICE_ALL_ACCESS);
		if (hSvc == NULL)
		{
			::CloseServiceHandle(hSC);
			//return;
		}
		// ��÷����״̬
		SERVICE_STATUS status;
		if (::QueryServiceStatus(hSvc, &status) == FALSE)
		{
			::CloseServiceHandle(hSvc);
			::CloseServiceHandle(hSC);
			//return;
		}
		//�������ֹͣ״̬����������
		if (status.dwCurrentState == SERVICE_STOPPED)
		{
			// ��������
			if (::StartService(hSvc, NULL, NULL) == FALSE)
			{
				
				::CloseServiceHandle(hSvc);
				::CloseServiceHandle(hSC);
				//return;
			}
			// �ȴ���������
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