#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void getGpsEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
{
	Document d;
	try{
		d.Parse(param.c_str());
		if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
		{
			std::map<std::string, std::string> args;
			args["message"] = "Connect";
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			int id = d["id"].GetInt();
			int cycle = d["cycyle"].GetInt();
			int querymode = d["querymode"].GetInt();
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
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, GPS_TRIGG_CSBK, "","","",id,_T(""),cycle, querymode, callId);
				break;
			case GPS_IMME_CSBK_EGPS:
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, GPS_IMME_CSBK_EGPS, "","","",id,_T(""),cycle, querymode, callId);
				break;
			case GPS_TRIGG_CSBK_EGPS:
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, GPS_TRIGG_CSBK_EGPS, "","","",id,_T(""),cycle, querymode, callId);
				break;
			}
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}



}

