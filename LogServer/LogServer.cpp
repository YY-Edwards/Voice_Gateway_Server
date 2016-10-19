// LogServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../lib/rpc/include/RpcServer.h"
#include "AppEventAction.h"
#include "AddStaffAction.h"
#include "Db.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CDb::instance()->open("localhost", 3306, "root", "", "tbx");
	CDb::instance()->insertUser("xy", "123", "xy", "111");

	CRpcServer rpcServer;
	rpcServer.addActionHandler("appEvent", appEventAction);
	rpcServer.addActionHandler("addStaff", addStaffAction);
	rpcServer.start();
	while (1);
	return 0;
}

