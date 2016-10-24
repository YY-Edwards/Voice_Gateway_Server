// TServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../lib/rpc/include/RpcServer.h"
#include "../lib/rpc/include/RpcClient.h"
#include "Settings.h"

#include "StartAction.h"
#include "SetRadioIpAction.h"
#include "ConenctRadioAction.h"

#include"AllCallAction.h"
#include"CallAction.h"
#include "CancelGpsAction.h"
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

#define			SelfPort			9000


int _tmain(int argc, _TCHAR* argv[])
{
	//CSettings::instance()->setRadioIp("192.168.2.1");
	//std::string radioIp = CSettings::instance()->getRadioIp();

	CBroker::instance()->connect();

	CRpcServer rpcServer;
	
	rpcServer.addActionHandler("start", startAction);
	rpcServer.addActionHandler("setRadioIp", setRadioIpAction);
	//radio
	rpcServer.addActionHandler("allCall", allCallAction);
	rpcServer.addActionHandler("call", callAction);
	rpcServer.addActionHandler("cancelGps", cancelGpsAction);
	rpcServer.addActionHandler("connect", connectRadioAction);
	rpcServer.addActionHandler("getGps", getGpsAction);
	rpcServer.addActionHandler("getOverTurnGps",getOverTurnGpsAction);
	rpcServer.addActionHandler("groupCall", groupCallAction);
	rpcServer.addActionHandler("radioCheck", radioCheckAction);
	rpcServer.addActionHandler("remotePowerOn", remotePowerOnAction);
	rpcServer.addActionHandler("remotePowerOff", remotePowerOffAction);
	rpcServer.addActionHandler("sendGroupSms", sendGroupSmsAction);
	rpcServer.addActionHandler("sendSms", sendSmsAction);
	rpcServer.addActionHandler("stopCall", stopCallAction);
	rpcServer.addActionHandler("wiretap", wiretapAction);
	rpcServer.start();

	rpcServer.start(SelfPort, CRpcServer::TCP);

	while (1);
	return 0;
}

