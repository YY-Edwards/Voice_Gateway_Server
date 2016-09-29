#include "stdafx.h"
#include "../lib/rpc/include/TcpServer.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "AppEventLog.h"


CAppEventLog::CAppEventLog()
{
}


CAppEventLog::~CAppEventLog()
{
}

std::string CAppEventLog::getName()
{

	return "appEvent";
}

int CAppEventLog::run(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
{
	Document d;
	try{
		d.Parse(param.c_str());
		//printf("name:%s\r\n",d["name"].GetString());
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}
	std::map<std::string, std::string> args;
	args["message"] = "first temp message";
	std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
	pRemote->sendResponse(strResp.c_str(), strResp.size());
	printf("send response:%s\r", strResp.c_str());
	return 0;
}