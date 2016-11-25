#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void  controlAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;
	std::lock_guard<std::mutex> locker(lock);
	try{
		g_sn = callId;
		std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", ArgumentType());
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	
		Document d;
		d.Parse(param.c_str());
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
			int  opterateType;
			int id;
			if (d.HasMember("Type") && d["Type"].IsInt())
			{
				opterateType = d["Type"].GetInt();
			}
			if (d.HasMember("Target") && d["Target"].IsInt())
			{
				id = d["Target"].GetInt();
			}
			if (isTcpConnect)
			{
				dis.control( opterateType, id);
			}
			else
			{
				try
				{
					ArgumentType args;
					args["Status"] = FieldValue(REMOTE_CONNECT_FAILED);
					args["Target"] = FieldValue(id);
					args["Type"] = FieldValue(opterateType);
					std::string callJsonStr = CRpcJsonParser::buildCall("controlStatus", ++g_sn, args, "radio");
					if (client != NULL)
					{
						client->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
					}

				}
				catch (std::exception e)
				{
#if DEBUG_LOG
					LOG(INFO) << "tcp连接不存在！";
#endif
				}
			}
		}
		else
		{

		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
