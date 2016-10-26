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
			ArgumentType args;
			args["message"] = "queryGps";
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());

			Value obj = d["param"].GetObject();
			if (obj.HasMember("Operate") && obj["Operate"].IsString())
			{
				string operate = obj["Operate"].GetString();
				int  querymode = -1;
				int id = -1;;
				int cycle = -1;
				if (obj.HasMember("Type") && obj["Type"].IsInt())
				{
					querymode = obj["Type"].GetInt();
				}
				if (obj.HasMember("Target") && obj["Target"].IsInt())
				{
					id = obj["Target"].GetInt();
				}
				if (obj.HasMember("Cycle") && obj["Cycle"].IsInt())
				{
					id = obj["Cycle"].GetInt();
				}
				if (operate == "Start")
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
					}
				}
				else  if (operate == "Stop")
				{
					m_dispatchOperate[pRemote]->AddAllCommand(pRemote, STOP_QUERY_GPS, "", "", "", id, _T(""), 0, 0, callId);
				}

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
