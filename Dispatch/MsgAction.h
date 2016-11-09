#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void  msgAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;
	std::lock_guard<std::mutex> locker(lock);
	try{
		Document d;
		d.Parse(param.c_str());
		TcpClient * client = new TcpClient();
		SOCKET s = client->s = ((TcpClient *)pRemote)->s;
		client->addr = ((TcpClient *)pRemote)->addr;
		if (m_dispatchOperate.find(s) != m_dispatchOperate.end())
		{
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", ArgumentType());
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			int opterateType = -1;
			int id = -1;
			string msg = "";
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
			if (isUdpConnect)
			{
				int msgSize = (int)(msg.length() + 1);
				wchar_t* text = new wchar_t[msgSize];
				MultiByteToWideChar(CP_ACP, 0, msg.c_str(), -1, text, msgSize);
				if (opterateType == GROUP)
				{
					m_dispatchOperate[s]->AddAllCommand(client, s, SEND_GROUP_MSG, "", "", "", id, text, 0, 0, callId);
				}
				else if (opterateType == PRIVATE)
				{
					m_dispatchOperate[s]->AddAllCommand(client, s, SEND_PRIVATE_MSG, "", "", "", id, text, 0, 0, callId);
				}
				else
				{
#if DEBUG_LOG
					LOG(INFO) << "opterateType �������� ";
#endif
				}
			}
			else
			{
				try
				{
					ArgumentType args;
					args["Target"] = FieldValue(id);
					args["contents"] = FieldValue(msg.c_str());
					args["status"] = FieldValue(REMOTE_FAILED);
					args["type"] = FieldValue(opterateType);
					std::string callJsonStr = CRpcJsonParser::buildCall("messageStatus", ++seq, args, "radio");
					client->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
				}
				catch (std::exception e)
				{

				}
			}

				
		}
		else
		{
#if DEBUG_LOG
			LOG(INFO) << " ����ȷ��tcp�����Ѿ�����";
#endif
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}