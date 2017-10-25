#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#define MSG_SIZE   256
void  msgAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;
	std::lock_guard<std::mutex> locker(lock);
	try{
		g_sn = callId;
		std::string strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
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
			int opterateType = -1;
			int id = -1;
			std::string msg = "";
			std::string sessionId = "";
			if (d.HasMember("Type") && d["Type"].IsInt())
			{
				opterateType = d["Type"].GetInt();
			}
			if (d.HasMember("Target") && d["Target"].IsInt())
			{
				id = d["Target"].GetInt();
			}
			if (d.HasMember("Contents") && d["Contents"].IsString())
			{
				msg = d["Contents"].GetString();
			}
			//int msgSize = (int)(msg.length() + 1);
			if (d.HasMember("SessionId") && d["SessionId"].IsString())
			{
				sessionId = d["SessionId"].GetString();
			}
			if (!dis.sendMsg( msg, id,  opterateType,sessionId))
			{
				ArgumentType args;
				args["Target"] = FieldValue(id);
				args["contents"] = FieldValue(msg.c_str());
				args["status"] = FieldValue(REMOTE_FAILED);
				args["type"] = FieldValue(opterateType);
				args["SessionId"] = FieldValue(sessionId.c_str());
				std::string callJsonStr = CRpcJsonParser::buildCall("messageStatus", ++g_sn, args, "radio");
				client->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
			}
		}
		else
		{
#if DEBUG_LOG
			LOG(INFO) << "tcp连接不存在！ ";
#endif
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}