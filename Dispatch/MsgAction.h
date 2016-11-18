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
		if (isHave)
		{
			int opterateType = -1;
			int id = -1;
			std::string msg = "";
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
			wchar_t* text = new wchar_t[MSG_SIZE];
			MultiByteToWideChar(CP_ACP, 0, msg.c_str(), -1, text, MSG_SIZE);
			if (!dis.sendMsg(client, text, id, callId, opterateType))
			{
				ArgumentType args;
				args["Target"] = FieldValue(id);
				args["contents"] = FieldValue(msg.c_str());
				args["status"] = FieldValue(REMOTE_FAILED);
				args["type"] = FieldValue(opterateType);
				std::string callJsonStr = CRpcJsonParser::buildCall("messageStatus", ++seq, args, "radio");
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