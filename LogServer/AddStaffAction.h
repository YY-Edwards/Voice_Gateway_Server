#pragma once

#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "Db.h"

/*
* 添加员工信息
* JSON format: param{name:xx,phone:123333, username:xx, password:xxxx}
*/

void addStaffAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
{
	Document d;
	std::map<std::string, std::string> args;
	std::string strResp;
	try{
		d.Parse(param.c_str());
		std::string name = d["name"].GetString();
		std::string username = d["username"].GetString();
		std::string phone = d["phone"].GetString();
		std::string password = d["password"].GetString();

		bool ret = CDb::instance()->insertUser(name.c_str(), phone.c_str(), username.c_str(), password.c_str());
		if (ret)
		{
			strResp  = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
		}
		else
		{
			args["message"] = "write record failed";
			std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 500, "", args);
			
		}
	}
	catch (std::exception e){
		args["message"] = e.what();
		std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 500, "", args);
	}
	catch (...)
	{
		args["message"] = "unknow error";
		std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 500, "", args);
	}
	pRemote->sendResponse(strResp.c_str(), strResp.size());

}