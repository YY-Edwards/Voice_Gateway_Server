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
		if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
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
					m_dispatchOperate[pRemote]->AddAllCommand(pRemote, GPS_IMME_COMM, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				case GPS_TRIGG_COMM:
					m_dispatchOperate[pRemote]->AddAllCommand(pRemote, GPS_TRIGG_COMM, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				case GPS_IMME_CSBK:
					m_dispatchOperate[pRemote]->AddAllCommand(pRemote, GPS_IMME_CSBK, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				case GPS_TRIGG_CSBK:
					m_dispatchOperate[pRemote]->AddAllCommand(pRemote, GPS_TRIGG_CSBK, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				case GPS_IMME_CSBK_EGPS:
					m_dispatchOperate[pRemote]->AddAllCommand(pRemote, GPS_IMME_CSBK_EGPS, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				case GPS_TRIGG_CSBK_EGPS:
					m_dispatchOperate[pRemote]->AddAllCommand(pRemote, GPS_TRIGG_CSBK_EGPS, "", "", "", id, _T(""), cycle, querymode, callId);
					break;
				default:
#if DEBUG_LOG
					LOG(INFO) << "querymode 参数不对 ";
#endif
					break;
				}
			}
			else  if (operate == STOP)
			{
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, STOP_QUERY_GPS, "", "", "", id, _T(""), 0, 0, callId);
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
			LOG(INFO) << " 请先确保tcp连接已经建立" ;
#endif
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
