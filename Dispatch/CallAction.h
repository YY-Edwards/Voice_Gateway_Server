#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

map<SOCKET, DispatchOperate*>  m_dispatchOperate;
#define START                   0
#define STOP                    1
#define NONE                    0
#define ALL                     1
#define GROUP                   2
#define PRIVATE                 3

void  callAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
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
			if (operate == START)
			{
				if (opterateType == ALL)
				{
					m_dispatchOperate[s]->AddAllCommand(client,s, ALL_CALL, "", "", "", id, _T(""), 0, 0, callId);
				}
				else if (opterateType == GROUP)
				{
					m_dispatchOperate[s]->AddAllCommand(client, s,GROUP_CALL, "", "", "", id, _T(""), 0, 0, callId);
				}
				else if (opterateType == PRIVATE)
				{
					m_dispatchOperate[s]->AddAllCommand(client, s,PRIVATE_CALL, "", "", "", id, _T(""), 0, 0, callId);
				}
				else
				{
#if DEBUG_LOG
					LOG(INFO) << "opterateType 参数不对 ";
#endif
				}
			}
			else  if (operate == STOP)
			{
				m_dispatchOperate[s]->AddAllCommand(client,s, STOP_CALL, "", "", "", id, _T(""), 0, 0, callId);
			}
			else
			{
#if DEBUG_LOG
				LOG(INFO) << "Operate 参数不对 ";
#endif
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
