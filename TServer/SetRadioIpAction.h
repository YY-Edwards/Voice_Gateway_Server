#pragma once

/**
* 设置车台的IP地址
*
* 命令: setRadioIp
* 参数: 
*	ip: string, example: "10.0.0.2"
*	json example: param:{ip:"10.0.0.2"}
*/

#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "Settings.h"

void setRadioIpAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("ip"))
		{
			CSettings::instance()->setRadioIp(d["ip"].GetString());
			std::map<std::string, std::string> args;
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
		}
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}

}