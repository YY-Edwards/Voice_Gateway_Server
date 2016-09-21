// LogServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "AppEventLog.h"
#include "../lib/rpc/include/RpcServer.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CAppEventLog* pEventLogAction = new CAppEventLog();
	CRpcServer rpcServer;
	rpcServer.addActionHandler(pEventLogAction);
	rpcServer.start();
	while (1);
	return 0;
}

