#pragma once

#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void appEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
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
	ArgumentType args;
	FieldValue fMsg(FieldValue::TString);
	fMsg.setString("this is first message");
	args["message"] = fMsg;
	std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
	pRemote->sendResponse(strResp.c_str(), strResp.size());
	printf("send response:%s\r", strResp.c_str());
}