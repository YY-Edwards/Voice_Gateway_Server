#pragma once

/**
* 启动工作进程
*
* 命令: start
* 参数:
*	radio:{ip:10.0.0.2}
*	wirelan:{ip:192.168.2.100}
* JSON parameter example:
* param:{radio:{ip:10.0.0.2}， wirelan:{ip:192.168.2.100}}
*/

#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "Broker.h"

void startAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;
	std::lock_guard<std::mutex> locker(lock);

	try
	{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("radio") && d["radio"].HasMember("ip"))
		{
			// 判断进程是否已启动
			HANDLE m_hMutex = CreateMutex(NULL, FALSE, _T("Trbox-Radio-Process"));
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				// 启动车台进程
			}

			// 发送消息到进程
			std::string ip = d["radio"]["ip"].GetString();
			std::map<std::string, std::string> args;
			args["ip"] = ip;
			int clientCallId = CBroker::instance()->getCallId();
			std::string callJsonStr = CRpcJsonParser::buildCall("start", clientCallId, args);

			int ret = CBroker::instance()->getRadioClient()->sendRequest(callJsonStr.c_str(),
				clientCallId,
				pRemote,
				[&](const char* pResponse, void*){
					std::map<std::string, std::string> args;
					std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
					pRemote->sendResponse(strResp.c_str(), strResp.size());
			}, nullptr);

			if (-1 == ret)
			{
				// remote error or disconnected
				std::map<std::string, std::string> args;
				std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 404, "", args);
				pRemote->sendResponse(strResp.c_str(), strResp.size());
			}
		}
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}

}