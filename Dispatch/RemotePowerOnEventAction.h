#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void remotePowerOnEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);
	try{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("id"))
		{
			if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
			{
				std::map<std::string, std::string> args;
				args["message"] = "remotePowerOn";
				std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
				pRemote->sendResponse(strResp.c_str(), strResp.size());
				string temp = d["id"].GetString();
				int id = atoi(temp.c_str());
				//AddAllCommand(pRemote, callId, REMOTE_OPEN);
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, REMOTE_OPEN, "", "", "", id, _T(""), 0, 0, callId);
			}
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}



}
