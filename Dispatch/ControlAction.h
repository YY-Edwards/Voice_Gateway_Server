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
		Document d;
		d.Parse(param.c_str());
		TcpClient * client = new TcpClient();
		SOCKET s = client->s = ((TcpClient *)pRemote)->s;
		client->addr = ((TcpClient *)pRemote)->addr;
		if (m_dispatchOperate.find(s) != m_dispatchOperate.end())
		{
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", ArgumentType());
			pRemote->sendResponse(strResp.c_str(), strResp.size());
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
				
				if (opterateType == ON)
				{
					m_dispatchOperate[s]->AddAllCommand(client, s, REMOTE_OPEN, "", "", "", id, _T(""), 0, 0, callId);
				}
				else if (opterateType == OFF)
				{
					m_dispatchOperate[s]->AddAllCommand(client, s, REMOTE_CLOSE, "", "", "", id, _T(""), 0, 0, callId);
				}
				else if (opterateType == RADIOCHECK)
				{
					m_dispatchOperate[s]->AddAllCommand(client, s, CHECK_RADIO_ONLINE, "", "", "", id, _T(""), 0, 0, callId);
				}
				else if (opterateType == MONITOR)
				{
					m_dispatchOperate[s]->AddAllCommand(client, s, REMOTE_MONITOR, "", "", "", id, _T(""), 0, 0, callId);
				}
			}
			else
			{
				try
				{
					ArgumentType args;
					args["Status"] = FieldValue(REMOTE_CONNECT_FAILED);
					args["Target"] = FieldValue(id);
					args["Type"] = FieldValue(opterateType);
					std::string callJsonStr = CRpcJsonParser::buildCall("controlStatus", ++seq, args, "radio");
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
			LOG(INFO) << " 请先确保tcp连接已经建立";
#endif
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
