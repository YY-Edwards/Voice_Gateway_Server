#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void wiretapEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
{
	Document d;
	try{
		d.Parse(param.c_str());
		if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
		{
			std::map<std::string, std::string> args;
			args["message"] = "wiretap";
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			string temp = d["id"].GetString();
			int id = atoi(temp.c_str());
			//	int result = m_dispatchOperate[pRemote]->wiretap(pRemote, id, callId);
			//AddAllCommand(pRemote,callId, REMOTE_MONITOR);
			m_dispatchOperate[pRemote]->AddAllCommand(pRemote, REMOTE_MONITOR, "","","",id,_T(""),0, 0, callId);
		}

	}
	catch (std::exception e){

	}
	catch (...)
	{

	}



}
