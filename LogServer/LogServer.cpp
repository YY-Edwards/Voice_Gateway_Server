// LogServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../lib/rpc/include/RpcServer.h"
#include "../lib/service/service.h"
#include "Db.h"
#include "AppEventAction.h"
#include "DbUserAction.h"
#include "DbGroupAction.h"
#include "DbRadioAction.h"
#include "DbStaffAction.h"
#include "DbSmsLogAction.h"
#include "DbGpsAction.h"
#define SERVICE_CODE    TRUE
int _tmain(int argc, _TCHAR* argv[])
{
#if SERVICE_CODE
	CService::instance()->SetServiceNameAndDescription(_T("Trbox.Log"), _T("Trbox Log Server"));
	CService::instance()->SetServiceCode([&](){
#endif
		bool db_connected = false;
		while (!db_connected){
			db_connected = CDb::instance()->open("127.0.0.1", 3306, "root", "", "tbx");
			Sleep(1000);
		}
		
		CRpcServer rpcServer;
		rpcServer.addActionHandler("appEvent", appEventAction);
		rpcServer.addActionHandler("user", userAction);
		rpcServer.addActionHandler("radio", radioAction);
		rpcServer.addActionHandler("staff", staffAction);
		rpcServer.addActionHandler("department", groupAction);
		rpcServer.addActionHandler("smslog", smsLogAction);
		rpcServer.addActionHandler("gpslog", gpsLogAction);

		rpcServer.start(9003, CRpcServer::TCP);
#if SERVICE_CODE

		while (!CService::instance()->m_bServiceStopped){
			Sleep(1000);
		}
		rpcServer.stop();
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
#endif
	//wprintf(argv[1]);
	//return 0;

	//FieldValue record1(FieldValue::TArray);
	//FieldValue r1(FieldValue::TString);
	//r1.setString("record 1");
	//FieldValue r2(FieldValue::TInt);
	//r2.setInt(22);
	//record1.push(r1);
	//record1.push(r2);

	//FieldValue vvv(FieldValue::TObject);
	//vvv.setKeyVal("condition", record1);

	//rapidjson::Document d;
	//rapidjson::Value vl = CRpcJsonParser::toNode(vvv, d);

	//std::string str = CRpcJsonParser::toString(vl);

	//recordType r1;
	//r1["username"] = "xsd";
	//r1["password"] = "sdf";

	//recordType r2;
	//r2["username"] = "sd";
	//r2["password"] = "dfs";

	//records.push_back(r1);
	//records.push_back(r2);

	//std::string v = CRpcJsonParser::listToString(records);
	//std::string str = CRpcJsonParser::test();
	//std::list<recordType> records;
	//CDb::instance()->open("localhost", 3306, "root", "", "tbx");
	
	//CDb::instance()->insertDepartment("xs", 1);
	//CDb::instance()->listDepartmentStaff(1, records);
	//CDb::instance()->listUser(NULL, records);
	//int id = CDb::instance()->getUserIdByStaffId(2);
	//CDb::instance()->count("user", NULL);
	//CDb::instance()->insertUser("xywz", "123", "xywz", "111", "sms", "radio");

	//CRpcServer rpcServer;
	//rpcServer.addActionHandler("appEvent", appEventAction);
	//rpcServer.addActionHandler("user", userAction);
	//rpcServer.addActionHandler("radio", radioAction);
	//rpcServer.addActionHandler("staff", staffAction);
	//rpcServer.addActionHandler("department", groupAction);
	//rpcServer.addActionHandler("smslog", smsLogAction);
	//rpcServer.addActionHandler("gpslog", gpsLogAction);

	//rpcServer.start(9003, CRpcServer::TCP);
	while (1)
	{
		Sleep(100000000);
	}
	return 0;
}

