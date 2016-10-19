// TServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../lib/rpc/include/RpcServer.h"
#include "../lib/rpc/include/RpcClient.h"
#include "Settings.h"

#include "StartAction.h"
#include "SettingAction.h"


#include <shlobj.h> 
#include <Shlwapi.h>

int _tmain(int argc, _TCHAR* argv[])
{
	//CSettings::instance()->getResponse("sucess", 1, 200, "", rapidjson::Value(NULL));
	//std::string str =  CSettings::instance()->getResponse("sucess", 1, 200, "", "");
	//CSettings::instance()->setValue("tst", rapidjson::Value(NULL));

	CRpcServer rpcServer;
	
	rpcServer.addActionHandler("start", startAction);

	rpcServer.addActionHandler("setBase", setBaseAction);
	rpcServer.addActionHandler("getBase", getBaseAction);

	rpcServer.addActionHandler("setRadio", setRadioAction);
	rpcServer.addActionHandler("getRadio", getRadioAction);

	rpcServer.addActionHandler("setRepeater", setRepeaterAction);
	rpcServer.addActionHandler("getRepeater", getRepeaterAction);


	rpcServer.start();
	while (1);
	return 0;
}

