#pragma once

#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "Db.h"

void databaseStatusAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{

	std::string strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
	pRemote->sendResponse(strResp.c_str(), strResp.size());

	Document d;
	try{
		
		d.Parse(param.c_str());
		/*std::string sessionId = "";

		if (d.HasMember("SessionId") && d["SessionId"].IsString())
		{
			sessionId = d["sessionId"].GetString();
		}*/
		if (d.HasMember("getType") && d["getType"].IsInt())
		{
			if (d["getType"].GetInt() == 4)  //4system status
			{
				ArgumentType args;
				//args["SessionId"] = FieldValue(sessionId.c_str());
				if (CDb::instance()->getDatabaseStatus())
				{
					args["DatabaseStatus"] = 0;   //0:connected
				}
				else
				{
					args["DatabaseStatus"] = 1;  //1:disconnected
				}
				strResp = CRpcJsonParser::buildCall("status", ++callId, args, "radio");
			}
		}
	}
	catch (std::exception e)
	{
		strResp = CRpcJsonParser::buildResponse("failed", callId, 500, e.what(), ArgumentType());
	}
	catch (...)
	{
		strResp = CRpcJsonParser::buildResponse("failed", callId, 500, "unknow error", ArgumentType());
	}
	pRemote->sendResponse(strResp.c_str(), strResp.size());
	
}