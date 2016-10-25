#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void sendSmsEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);
	
	try{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("id") && d.HasMember("msg"))
		{
			if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
			{
				std::map<std::string, std::string> args;
				args["message"] = "sendSms";
				std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
				pRemote->sendResponse(strResp.c_str(), strResp.size());
				string temp = d["id"].GetString();
				int id = atoi(temp.c_str());
				string msg = d["msg"].GetString();
				int msgSize = (int)(msg.length() + 1);
				wchar_t* text = new wchar_t[msgSize];
				MultiByteToWideChar(CP_ACP, 0, msg.c_str(), -1, text, msgSize);
				//int result = m_dispatchOperate[pRemote]->sendSms(pRemote, id, text, callId);
				//	AddAllCommand(pRemote,callId, SEND_PRIVATE_MSG);
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, SEND_PRIVATE_MSG, "", "", "", id, text, 0, 0, callId);
				delete[] text;
			}
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}



}