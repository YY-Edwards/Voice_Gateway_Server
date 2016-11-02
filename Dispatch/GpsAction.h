#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"



void  gpsAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
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
			int  querymode = -1;
			int id = -1;;
			int cycle = -1;
			if (d.HasMember("Operate") && d["Operate"].IsInt())
			{
				operate = d["Operate"].GetInt();
			}
			if (d.HasMember("Type") && d["Type"].IsInt())
			{
				querymode = d["Type"].GetInt();
			}
			if (d.HasMember("Target") && d["Target"].IsInt())
			{
				id = d["Target"].GetInt();
			}
			if (d.HasMember("Cycle") && d["Cycle"].IsInt())
			{
				id = d["Cycle"].GetInt();
			}
			if (operate == START)
			{
				switch (querymode)
				{
				case GPS_IMME_COMM:
					m_dispatchOperate[s]->AddAllCommand(client,s, GPS_IMME_COMM, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				case GPS_TRIGG_COMM:
					m_dispatchOperate[s]->AddAllCommand(client,s, GPS_TRIGG_COMM, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				case GPS_IMME_CSBK:
					m_dispatchOperate[s]->AddAllCommand(client, s,GPS_IMME_CSBK, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				case GPS_TRIGG_CSBK:
					m_dispatchOperate[s]->AddAllCommand(client,s, GPS_TRIGG_CSBK, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				case GPS_IMME_CSBK_EGPS:
					m_dispatchOperate[s]->AddAllCommand(client,s, GPS_IMME_CSBK_EGPS, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				case GPS_TRIGG_CSBK_EGPS:
					m_dispatchOperate[s]->AddAllCommand(client,s, GPS_TRIGG_CSBK_EGPS, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				default:
#if DEBUG_LOG
					LOG(INFO) << "querymode �������� ";
#endif
					break;
				}
				char *buf = new char();
				itoa(id, buf, 10); 
				if (radioStatus.find(buf) == radioStatus.end())
				{
					status st;
					st.gpsQueryMode = querymode;
					radioStatus[buf] = st;
				}
				else
				{
					radioStatus[buf].gpsQueryMode = querymode;
				}
			}
			else  if (operate == STOP)
			{
				char *buf = new char();
				itoa(id, buf, 10);
				if (radioStatus.find(buf) != radioStatus.end())
				{
					querymode = radioStatus[buf].gpsQueryMode;
				}
			
				m_dispatchOperate[s]->AddAllCommand(client, s, STOP_QUERY_GPS, "", "", "", id, _T(""), 0, querymode, callId);
			}
			else
			{
#if DEBUG_LOG
				LOG(INFO) << "Operate �������� ";
#endif
			}
		
		}
		else
		{
#if DEBUG_LOG
			LOG(INFO) << " ����ȷ��tcp�����Ѿ�����" ;
#endif
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
