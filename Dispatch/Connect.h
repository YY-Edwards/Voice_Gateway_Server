#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void connectEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	Document d;
	try{
		d.Parse(param.c_str());
		DispatchOperate  * pDispatchOperate = new DispatchOperate();
		m_dispatchOperate[pRemote] = pDispatchOperate;
		std::map<std::string, std::string> args;
		args["message"] = "connect";
		std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
		string radioIP = d["radioIP"].GetString();
		string mnisIP = d["mnisIP"].GetString();
		m_dispatchOperate[pRemote]->AddAllCommand(pRemote, RADIO_CONNECT, radioIP, mnisIP, "", 0, _T(""), 0, 0, callId);
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}
	
}
