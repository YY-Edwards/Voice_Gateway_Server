// LogServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../lib/rpc/include/RpcServer.h"
#include "AppEventAction.h"
#include "AddStaffAction.h"
#include "Db.h"
#include "DbGetUserAction.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//std::list<recordType> records;

	FieldValue record1(FieldValue::TArray);
	FieldValue r1(FieldValue::TString);
	r1.setString("record 1");
	FieldValue r2(FieldValue::TInt);
	r2.setInt(22);
	record1.push(r1);
	record1.push(r2);

	FieldValue vvv(FieldValue::TObject);
	vvv.setKeyVal("condition", record1);

	rapidjson::Document d;
	rapidjson::Value vl = CRpcJsonParser::toNode(vvv, d);

	std::string str = CRpcJsonParser::toString(vl);

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
	CDb::instance()->open("localhost", 3306, "root", "", "tbx");
	//CDb::instance()->insertUser("xy", "123", "xy", "111");

	CRpcServer rpcServer;
	rpcServer.addActionHandler("appEvent", appEventAction);
	rpcServer.addActionHandler("addStaff", addStaffAction);
	rpcServer.addActionHandler("getUser", getUserAction);
	rpcServer.start(9003, CRpcServer::TCP);
	while (1);
	return 0;
}

