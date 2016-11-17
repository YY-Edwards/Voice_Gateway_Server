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
		//if (m_dispatchOperate.find(s) != m_dispatchOperate.end())
		{
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", ArgumentType());
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			
			int operate = -1;
			int  querymode = -1;
			int id = -1;;
			double cycle = -1;
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
			if (d.HasMember("Cycle") && d["Cycle"].IsDouble())
			{
				cycle = d["Cycle"].GetDouble();
			}
			if (operate == START)
			{
				//if (!cs.radioGetGps(client, id, querymode, cycle, callId))
				{
					ArgumentType args;
					args["Target"] = FieldValue(id);
					args["Type"] = FieldValue(querymode);
					args["Cycle"] = FieldValue(cycle);
					args["Operate"] = FieldValue(operate);
					args["Status"] = FieldValue(UNSUCESS);
					std::string callJsonStrRes = CRpcJsonParser::buildCall("sendGpsStatus", callId, args, "radio");
					if (client != NULL)
					{
						client->sendResponse((const char *)callJsonStrRes.c_str(), callJsonStrRes.size());
					}
				}
				//char *buf = new char();
				//itoa(id, buf, 10);
				//if (radioStatus.find(buf) == radioStatus.end())
				//{
				//	status st;
				//	st.id = id;
				//	st.status = RADIO_STATUS_OFFLINE;
				//	st.gpsQueryMode = querymode;
				//	radioStatus[buf] = st;
				//}
				//else
				//{
				//	//radioStatus[buf].status = RADIO_STATUS_OFFLINE;
				//	radioStatus[buf].gpsQueryMode = querymode;
				//}
			}
			else  if (operate == STOP)
			{
				char *buf = new char();
				itoa(id, buf, 10);
				if (radioStatus.find(buf) != radioStatus.end())
				{
					querymode = radioStatus[buf].gpsQueryMode;
				}	
				//if(!cs.radioStopGps(client, id, querymode, callId))
				{
					ArgumentType args;
					args["Target"] = FieldValue(id);
					args["Type"] = FieldValue(querymode);
					args["Cycle"] = FieldValue(cycle);
					args["Operate"] = FieldValue(operate);
					args["Status"] = FieldValue(UNSUCESS);
					std::string callJsonStrRes = CRpcJsonParser::buildCall("sendGpsStatus", callId, args, "radio");
					if (client != NULL)
					{
						client->sendResponse((const char *)callJsonStrRes.c_str(), callJsonStrRes.size());
					}
				}
				//m_dispatchOperate[s]->AddAllCommand(client, s, STOP_QUERY_GPS, "", "", "", id, _T(""), 0, querymode, callId);
			}
			else
			{
#if DEBUG_LOG
				LOG(INFO) << "Operate 参数不对 ";
#endif
			}

		}
			/*else
			{
				try
				{
					ArgumentType args;
					args["Target"] = FieldValue(id);
					args["Type"] = FieldValue(querymode);
					args["Cycle"] = FieldValue(cycle);
					args["Operate"] = FieldValue(operate);
					args["Status"] = FieldValue(REMOTE_FAILED);
					std::string callJsonStrRes = CRpcJsonParser::buildCall("sendGpsStatus", callId, args, "radio");
					if (client != NULL)
					{
						client->sendResponse((const char *)callJsonStrRes.c_str(), callJsonStrRes.size());
					}
				}
				catch (std::exception e)
				{

				}
			}*/
		
		}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
