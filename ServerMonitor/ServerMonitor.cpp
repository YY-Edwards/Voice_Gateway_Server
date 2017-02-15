// ServerMonitor.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Shlwapi.h>
#include <shlobj.h>  
#include "Tool.h"
#include "TServerMonitor.h"
#include "WriteLog.h"
#include "../lib/service/service.h"
#include <sstream>
#define SERVER_CODE  FALSE

std::wstring getAppdataPath(){
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);

	return std::wstring(szBuffer);
}
int _tmain(int argc, _TCHAR* argv[])
{
	int createFileRlt = 0;
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);
	std::wstring appFolder = getAppdataPath() + _T("\\Jihua Information");
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}
	appFolder = appFolder + _T("\\Trbox");
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}
	appFolder = appFolder + _T("\\3.0");
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}
	appFolder = appFolder + _T("\\ServerMonitor");
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}

	std::wstring logFolder = appFolder + _T("\\log");
	if (!PathFileExists(logFolder.c_str()))
	{
		createFileRlt = _wmkdir(logFolder.c_str());
	}

	std::wstring pathLogInfo = logFolder + _T("/info_");
	std::wstring pathLogError = logFolder + _T("/error_");
	std::wstring pathLogWarning = logFolder + _T("/warning_");

	//FLAGS_log_dir = "./";
	google::InitGoogleLogging("");
	google::SetLogDestination(google::GLOG_INFO, CTool::UnicodeToUTF8(pathLogInfo).c_str());
	google::SetLogDestination(google::GLOG_ERROR, CTool::UnicodeToUTF8(pathLogError).c_str());
	google::SetLogDestination(google::GLOG_WARNING, CTool::UnicodeToUTF8(pathLogWarning).c_str());
	google::SetLogFilenameExtension("log");

#if SERVER_CODE 
	CService::instance()->SetServiceNameAndDescription(_T("Trbox.Monitor"), _T("Trbox Monitor Server"));
	CService::instance()->SetServiceCode([&](){
#endif
	CTServerMonitor monitor;
	LOG(INFO) << "Start Service";
	monitor.StartMonitor(_T("Trbox.TServer"));
	LOG(INFO) << "end Service ";
	//WinExec("InstallSvrA.bat", SW_HIDE);
		LOG(INFO) << "Start Service";
		int status = system("net start Trbox.TServer ");

		std::string str = std::to_string(status);
		CWriteLog::instance()->WriteLog(str);
		//system("monitorShell.sh");

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

	wprintf(argv[1]);
#else
		char temp = 0x00;
		printf("press any key to end\r\n");
		scanf_s("%c", &temp, 1);
#endif
		
	return 0;

}

