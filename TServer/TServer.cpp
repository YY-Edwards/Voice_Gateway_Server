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


#include"AllCallAction.h"
#include"CallAction.h"
#include "CancelGpsAction.h"
#include "ConenctRadioAction.h"
#include "GetGpsAction.h"
#include"GetOverTurnGpsAction.h"
#include"GroupCallAction.h"
#include"RadioCheckAction.h"
#include"RemotePowerOffAction.h"
#include"RemotePowerOnAction.h"
#include"WiretapAction.h"
#include"SendGroupSmsAction.h"
#include"SendSmsAction.h"
#include"StopCallAction.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//CSettings::instance()->getResponse("sucess", 1, 200, "", rapidjson::Value(NULL));
	//std::string str =  CSettings::instance()->getResponse("sucess", 1, 200, "", "");
	//CSettings::instance()->setValue("tst", rapidjson::Value(NULL));

	CBroker::instance();

	CRpcServer rpcServer;

	rpcServer.addActionHandler("start", startAction);



	rpcServer.addActionHandler("setBaseSetting", setBaseAction);
	rpcServer.addActionHandler("getBaseSetting", getBaseAction);

	rpcServer.addActionHandler("setRadioSetting", setRadioAction);
	rpcServer.addActionHandler("getRadioSetting", getRadioAction);

	rpcServer.addActionHandler("setRepeaterSetting", setRepeaterAction);
	rpcServer.addActionHandler("getRepeaterSetting", getRepeaterAction);

	//radio
	rpcServer.addActionHandler("allCall", allCallAction);
	rpcServer.addActionHandler("call", callAction);
	rpcServer.addActionHandler("cancelGps", cancelGpsAction);
	rpcServer.addActionHandler("connect", connectRadioAction);
	rpcServer.addActionHandler("getGps", getGpsAction);
	rpcServer.addActionHandler("getOverTurnGps", getOverTurnGpsAction);
	rpcServer.addActionHandler("groupCall", groupCallAction);
	rpcServer.addActionHandler("radioCheck", radioCheckAction);
	rpcServer.addActionHandler("remotePowerOn", remotePowerOnAction);
	rpcServer.addActionHandler("remotePowerOff", remotePowerOffAction);
	rpcServer.addActionHandler("sendGroupSms", sendGroupSmsAction);
	rpcServer.addActionHandler("sendSms", sendSmsAction);
	rpcServer.addActionHandler("stopCall", stopCallAction);
	rpcServer.addActionHandler("wiretap", wiretapAction);

	//std::string  str = CSettings::instance()->getValue("radio");
	Sleep(1000);
	while (CBroker::instance()->getRadioClient() != nullptr){

	//CBroker::instance()->getRadioClient()->send(str.c_str(), strlen(str.c_str()));

	//std::string callJsonStr = buildCall("connect", 0, args);
	std::string callJsonStr = CSettings::instance()->getRequest("connect", "radio", 0, CSettings::instance()->getValue("radio"));


	int ret = CBroker::instance()->getRadioClient()->sendRequest(callJsonStr.c_str(),
		0,
		NULL,
		[&](const char* pResponse, void* data){
		CRemotePeer* pCommandSender = (CRemotePeer*)data;
		pCommandSender->sendResponse(pResponse, strlen(pResponse));
	}, nullptr);
	if (-1 != ret)break;
	
	Sleep(50);

}

		
	
	rpcServer.start();

	while (1);
	return 0;
}

