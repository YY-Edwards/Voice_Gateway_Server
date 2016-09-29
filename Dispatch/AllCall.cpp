#include "stdafx.h"
#include "AllCall.h"


CAllCall::CAllCall()
{
}


CAllCall::~CAllCall()
{
}
std::list<std::string> CAllCall::getArgNames()
{
	std::list<std::string> args;

	/*args.push_back("name");
	args.push_back("content");*/

	return args;
}
std::string CAllCall::getName()
{

	return "allCall";
}

int CAllCall::run(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
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
		int result = m_dispatchOperate[pRemote]->allCall(pRemote, callId);
	}
	return 0;
}
