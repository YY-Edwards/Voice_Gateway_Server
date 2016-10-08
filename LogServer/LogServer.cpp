// LogServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../lib/rpc/include/RpcServer.h"
#include "AppEventAction.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CRpcServer rpcServer;
	rpcServer.addActionHandler("appEvent", appEventAction);
	rpcServer.start();
	while (1);
	return 0;
}

