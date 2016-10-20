#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void getoverturngpsEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	Document d;
	try{
		d.Parse(param.c_str());
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}
	if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
	{
		std::map<std::string, std::string> args;
		args["message"] = "getOverturnGps";
		std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
		string ip = d["ip"].GetString();
		//int result = m_dispatchOperate[pRemote]->getOverturnGps(pRemote, ip.c_str(), callId);
		m_dispatchOperate[pRemote]->AddAllCommand(pRemote, GPS_TRIGG_CSBK_EGPS, "","",ip, 0, _T(""),0, 0, callId);
	}



}
