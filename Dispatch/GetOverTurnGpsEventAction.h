#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void getOverTurnGpsEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;
	std::lock_guard<std::mutex> locker(lock);

	try{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("ip"))
		{
			if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
			{
				ArgumentType args;
				args["message"] = "getOverturnGps";
				std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
				pRemote->sendResponse(strResp.c_str(), strResp.size());
				string ip = d["ip"].GetString();
				//int result = m_dispatchOperate[pRemote]->getOverturnGps(pRemote, ip.c_str(), callId);
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, GPS_TRIGG_CSBK_EGPS, "", "", ip, 0, _T(""), 0, 0, callId);
			}
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}
}
