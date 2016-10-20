// TServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../lib/rpc/include/RpcServer.h"
#include "../lib/rpc/include/RpcClient.h"
#include "Settings.h"

#include "StartAction.h"
#include "SetRadioIpAction.h"
#include "ConenctRadioAction.h"
int _tmain(int argc, _TCHAR* argv[])
{
	//CSettings::instance()->setRadioIp("192.168.2.1");
	//std::string radioIp = CSettings::instance()->getRadioIp();

	CRpcServer rpcServer;
	
	rpcServer.addActionHandler("start", startAction);
	rpcServer.addActionHandler("setRadioIp", setRadioIpAction);
	rpcServer.addActionHandler("connect", connectRadioAction);
	rpcServer.start();
	while (1);
	return 0;
}

