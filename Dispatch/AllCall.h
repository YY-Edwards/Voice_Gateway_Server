#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

map<CRemotePeer *, DispatchOperate*>  m_dispatchOperate;
void allCallEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
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
		args["message"] = "allCall";
		std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
		m_dispatchOperate[pRemote]->AddAllCommand(pRemote, ALL_CALL, "", "", "", 0,_T(""), 0, 0, callId);
		
		//int result = m_dispatchOperate[pRemote]->allCall(pRemote, callId);
	}
}
