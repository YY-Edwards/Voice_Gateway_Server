#include "stdafx.h"
#include "GetGps.h"


CGetGps::CGetGps()
{
}


CGetGps::~CGetGps()
{
}
std::list<std::string> CGetGps::getArgNames()
{
	std::list<std::string> args;
	args.push_back("id");
	args.push_back("cycle");
	args.push_back("querymode");
	return args;
}
std::string CGetGps::getName()
{
	return "getGps";
}

int CGetGps::run(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
{
	Document d;
	try{
		d.Parse(param.c_str());
		if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
		{
			std::map<std::string, std::string> args;
			args["message"] = "Connect";
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			int id = d["id"].GetInt();
			int cycle = d["cycyle"].GetInt();
			int querymode = d["querymode"].GetInt();
			int result = m_dispatchOperate[pRemote]->getGps(pRemote, id, querymode, cycle, callId);
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

	return 0;
}