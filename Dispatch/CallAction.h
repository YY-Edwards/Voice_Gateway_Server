#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "TcpScheduling.h"
#include "extern.h"
long long g_sn;
void  callAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
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
			if (t -> s == client->s)
			{
				isHave = true;
				break;
			}
		}
		if (isHave && param != "")
		{
			int operate = -1;
			int opterateType = -1;;
			int id = -1;;
			if (d.HasMember("Operate") && d["Operate"].IsInt())
			{
				operate = d["Operate"].GetInt();
			}
			if (d.HasMember("Type") && d["Type"].IsInt())
			{
				opterateType = d["Type"].GetInt();
			}
			if (d.HasMember("Target") && d["Target"].IsInt())
			{
				id = d["Target"].GetInt();
			}
			if (dis.isTcpConnect)
			{
				dis.call( opterateType, id, operate);
			}
			else
			{
				try
				{
					ArgumentType args;
					args["Status"] = FieldValue(REMOTE_FAILED);
					args["Target"] = FieldValue(id);
					args["Operate"] = FieldValue(operate);
					args["Type"] = FieldValue(opterateType);
					std::string callJsonStr = CRpcJsonParser::buildCall("callStatus", ++num, args, "radio");
					if (client != NULL)
					{
						client->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
					}

				}
				catch (std::exception e)
				{

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
