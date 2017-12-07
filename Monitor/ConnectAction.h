#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
std::string serverName;
CMonitorServer m_monitorServer;
void ConnectAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;
	std::lock_guard<std::mutex> locker(lock);
	try{
		std::string strResp = CRpcJsonParser::buildResponse("success", callId, 200, "success", ArgumentType());
		pRemote->sendResponse(strResp.c_str(), strResp.size());
		rapidjson::Document doc;
		doc.Parse(param.c_str());
		if (doc.IsObject() && doc.HasMember("IsEnable") && doc["IsEnable"].IsBool())
		{
			if (true == doc["IsEnable"].GetBool() && type == "radio")
			{
				serverName = "Trbox.Dispatch";
			}
			else if (true == doc["IsEnable"].GetBool() && type == "wl")
			{
				serverName = "Trbox.Wirelan";
			}

		}
		std::wstring wstr(serverName.length(), L' ');
		std::copy(serverName.begin(), serverName.end(), wstr.begin());
		if (serverName != "")
		{
			m_monitorServer.startMonitor(wstr.c_str());

		}
	}

	catch (std::exception e){

	}
	catch (...)
	{

	}

}