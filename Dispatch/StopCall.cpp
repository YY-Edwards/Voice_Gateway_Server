#include "stdafx.h"
#include "StopCall.h"


CStopCall::CStopCall()
{
}


CStopCall::~CStopCall()
{
}
std::list<std::string> CStopCall::getArgNames()
{
	std::list<std::string> args;

	args.push_back("id");


	return args;
}
std::string CStopCall::getName()
{

	return "stopCall";
}

int CStopCall::run(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
{
	Document d;
	try{
		d.Parse(param.c_str());
		if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
		{
			std::map<std::string, std::string> args;
			args["message"] = "stopCall";
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			string temp = d["id"].GetString();
			int id = atoi(temp.c_str());
			int result = m_dispatchOperate[pRemote]->stopCall(pRemote, callId);
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}
	
	
	return 0;
}
