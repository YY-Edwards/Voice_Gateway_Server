#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void  controlAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;
	std::lock_guard<std::mutex> locker(lock);
	try{
		Document d;
		d.Parse(param.c_str());
		if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
		{
			ArgumentType args;
			args["message"] = "control";
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			string opterateType;
			int id;
			if (d.HasMember("Type") && d["Type"].IsString())
			{
				opterateType = d["Type"].GetString();
			}
			if (d.HasMember("Target") && d["Target"].IsInt())
			{
				id = d["Target"].GetInt();
			}
			if (opterateType == "Startup")
			{
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, REMOTE_OPEN, "", "", "", id, _T(""), 0, 0, callId);
			}
			else if (opterateType == "Off")
			{
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, REMOTE_CLOSE, "", "", "", id, _T(""), 0, 0, callId);
			}
			else if (opterateType == "Check")
			{
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, CHECK_RADIO_ONLINE, "", "", "", id, _T(""), 0, 0, callId);
			}
			else if (opterateType == "Wiretap")
			{
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, REMOTE_MONITOR, "", "", "", id, _T(""), 0, 0, callId);
			}
		}
		else
		{
#if DEBUG_LOG
			LOG(INFO) << " 请先确保tcp连接已经建立";
#endif
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
