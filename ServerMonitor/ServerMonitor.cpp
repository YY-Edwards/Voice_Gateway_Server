// ServerMonitor.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<stdlib.h>
#include "TServerMonitor.h"
#include "WriteLog.h"
#include "../lib/service/service.h"
#include <sstream>
#define SERVER_CODE  TRUE 
int _tmain(int argc, _TCHAR* argv[])
{
#if SERVER_CODE 
	CService::instance()->SetServiceNameAndDescription(_T("Trbox.Monitor"), _T("Trbox Monitor Server"));
	CService::instance()->SetServiceCode([&](){
#endif
	/*CTServerMonitor monitor;
	CWriteLog::instance()->WriteLog("startTserver\r\n");
	monitor.StartMonitor(_T("Trbox.TServer"));*/
	//WinExec("InstallSvrA.bat", SW_HIDE);
		//system("net start Trbox.TServer ");
		system("monitorShell.sh");

#if SERVER_CODE 
	});

	std::wstring strArg = argv[1];
	try{
		if (0 == strArg.compare(_T("install")))
		{
			CService::instance()->InstallService();
			//InstallService();
		}
		else if (0 == strArg.compare(_T("uninstall")))
		{
			CService::instance()->UninstallService();
			//LOG(INFO) << "UnInstall Service";
		}
		else if (0 == strArg.compare(_T("start")))
		{
			CService::instance()->StartWindowsService();
			
			//LOG(INFO) << "Start Service";
		}
		else if (0 == strArg.compare(_T("stop")))
		{
			CService::instance()->StopService();
			//LOG(INFO) << "Stop Service";
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
	system("net start Trbox.TServer ");
	wprintf(argv[1]);
#else
		char temp = 0x00;
		printf("press any key to end\r\n");
		scanf_s("%c", &temp, 1);
#endif
		
	return 0;

}

