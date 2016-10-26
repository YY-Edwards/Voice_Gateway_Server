#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void getGpsEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);
	
	try{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("id") && d.HasMember("cycle") && d.HasMember("querymode"))
		{
			if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
			{
				ArgumentType args;
				args["message"] = "getGps";
				std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
				pRemote->sendResponse(strResp.c_str(), strResp.size());
				string temp = d["id"].GetString();
				int id = atoi(temp.c_str());
				string tempCycle = d["cycle"].GetString();
				int cycle = atoi(tempCycle.c_str());
				string tempMode = d["queryMode"].GetString();
				int querymode = atoi(tempMode.c_str());
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
		}

	}
	catch (std::exception e){

	}
	catch (...)
	{

	}



}

