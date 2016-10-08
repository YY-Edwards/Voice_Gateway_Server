#include "stdafx.h"
#include "Connect.h"
#include "DispatchOperate.h"

CConnect::CConnect()
{
}


CConnect::~CConnect()
{
}
std::list<std::string> CConnect::getArgNames()
{
	std::list<std::string> args;

	args.push_back("radioIP");
	args.push_back("mnisIP");

	return args;
}
std::string CConnect::getName()
{

	return "connect";
}

int CConnect::run(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
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
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	m_dispatchOperate[pRemote] = pDispatchOperate;
	std::map<std::string, std::string> args;
	args["message"] = "connect";
	std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
	pRemote->sendResponse(strResp.c_str(), strResp.size());
	string radioIP = d["radioIP"].GetString();
	string mnisIP = d["mnisIP"].GetString();
	int result = pDispatchOperate->Connect(pRemote, radioIP.c_str(), mnisIP.c_str(), callId);
	return 0;
}