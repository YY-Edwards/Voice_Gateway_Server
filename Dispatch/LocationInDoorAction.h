#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "../lib/rpc/include/TcpServer.h"
#include "extern.h"
void locationIndoorAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;
	std::lock_guard<std::mutex> locker(lock);
	try{
		g_sn = callId;
		std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "sucess", ArgumentType());
		pRemote->sendResponse(strResp.c_str(), strResp.size());

		TcpClient * client = new TcpClient();
		SOCKET s = client->s = ((TcpClient *)pRemote)->s;
		client->addr = ((TcpClient *)pRemote)->addr;
		bool isHave = false;
		for (auto i = rmtPeerList.begin(); i != rmtPeerList.end(); i++)
		{
			TcpClient * t = *i;
			if (t->s == client->s)
			{
				isHave = true;
				break;
			}
		}
		if (isHave && param != "")
		{
	
			Document d;
			d.Parse(param.c_str());
			if (d.IsObject() && d.HasMember("IsEnable") && d["IsEnable"].IsBool())
			{
				if (true == d["IsEnable"].GetBool())
				{
					if (d.HasMember("Interval") && d["Interval"].IsInt())
					{


					}
					if (d.HasMember("iBeaconNumber") && d["iBeaconNumber"].IsInt())
					{


					}
					if (d.HasMember("IsEmergency") && d["IsEmergency"].IsBool())
					{


					}
				}
			}
		}
		else
		{
#if DEBUG_LOG
			LOG(INFO) << "tcp连接不存在！";
#endif
		}

	}

	catch (std::exception e){

	}
	catch (...)
	{

	}

}
