#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "../lib/rpc/include/TcpServer.h"

void  statusAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
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
			if (d.HasMember("getType") && d["getType"].IsInt())
			{
				int type = d["getType"].GetInt();
				switch (type)
				{
				case CONNECT_STATUS:
					m_dispatchOperate[s]->AddAllCommand(client, s, RADIO_CONNECT, "", "", "", 0, _T(""), 0, 0, callId);
					break;
				case RADIO_STATUS:
					m_dispatchOperate[s]->AddAllCommand(client, s, RADIO_ARS, "", "", "", 0, _T(""), 0, 0, callId);
					break;
				}
				
			}
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}