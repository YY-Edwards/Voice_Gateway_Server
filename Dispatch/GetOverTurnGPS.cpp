#include "stdafx.h"
#include "GetOverTurnGPS.h"


CGetOverTurnGPS::CGetOverTurnGPS()
{
}


CGetOverTurnGPS::~CGetOverTurnGPS()
{
}
std::list<std::string> CGetOverTurnGPS::getArgNames()
{
	std::list<std::string> args;

	args.push_back("ip");

	return args;
}
std::string CGetOverTurnGPS::getName()
{

	return "getOverTurnGps";
}

int CGetOverTurnGPS::run(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
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
		args["message"] = "getOverturnGps";
		std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
		string ip = d["ip"].GetString();
		int result = m_dispatchOperate[pRemote]->getOverturnGps(pRemote, ip.c_str(), callId);
	}
	return 0;
}
