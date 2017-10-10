#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void callEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("id")&& d["id"].IsInt())
		{
			if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
			{
				ArgumentType args;
				args["message"] = "call";
				std::string strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);
				pRemote->sendResponse(strResp.c_str(), strResp.size());
				int id = d["id"].GetInt();
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, PRIVATE_CALL, "", "", "", id, _T(""), 0, 0, callId);
			}
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}


}
