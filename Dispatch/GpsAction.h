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
		g_sn = callId;
		std::string strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
		pRemote->sendResponse(strResp.c_str(), strResp.size());

		Document d;
		d.Parse(param.c_str());
		TcpClient * client = new TcpClient();
		SOCKET s = client->s = ((TcpClient *)pRemote)->s;
		client->addr = ((TcpClient *)pRemote)->addr;
		bool isHave = false;
		for (auto i = rmtPeerList.begin(); i != rmtPeerList.end(); i++)
		{
			TcpClient * t = *i;
			if (t->s == client->s)
			{
				isHave = true;
				break;
			}
		}
		if (isHave && param != "")
		{
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
				if (!dis.getGps(id, querymode, cycle))
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
			}
			else  if (operate == STOP)
			{
				char *buf = new char();
				itoa(id, buf, 10);
				if (g_radioStatus.find(buf) != g_radioStatus.end())
				{
					querymode = g_radioStatus[buf].gpsQueryMode;
				}
				if(!dis.stopGps( id, querymode))
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
			
			}
		}
		else
		{
#if DEBUG_LOG
			LOG(INFO) << "tcp连接不存在！ ";
#endif
		}	
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
