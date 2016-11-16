#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "../lib/rpc/include/TcpServer.h"
CDataScheduling cs;
void connectAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);
	try{
		std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "sucess", ArgumentType());
		pRemote->sendResponse(strResp.c_str(), strResp.size());

		TcpClient * client = new TcpClient();
		SOCKET s = client->s = ((TcpClient *)pRemote)->s;
		client->addr = ((TcpClient *)pRemote)->addr;

		string radioIP = "";
		string mnisIP = "";
		Document d;
		d.Parse(param.c_str());
	/*	if (doc.HasMember("param"))
		if (doc["param"].IsObject())*/
		{
			//Value obj = doc["param"].GetObject();

			//radioIP
			if (d.HasMember("Ride") && d["Ride"].IsObject())
			{
				Value objRadio = d["Ride"].GetObject();
				if (objRadio.HasMember("Ip") && objRadio["Ip"].IsString())
				{
					radioIP = objRadio["Ip"].GetString();

				}
			}
			//mnisIP
			if (d.HasMember("Mnis") && d["Mnis"].IsObject())
			{
				Value objRadio = d["Mnis"].GetObject();
				if (objRadio.HasMember("Ip") && objRadio["Ip"].IsString())
				{
					mnisIP = objRadio["Ip"].GetString();


				}
			}
			//GPS ·­×ªIP
			if (d.HasMember("Gps") && d["Gps"].IsObject())
			{
				Value objRadio = d["Gps"].GetObject();
				if (objRadio.HasMember("Ip") && objRadio["Ip"].IsString())
				{
					//radioIP = objRadio["Ip"].GetString();

				}
			}
			if (radioIP!=m_radioIP && mnisIP !=m_mnisIP)
			{
				if (mnisIP != "")
				{
					cs.radioConnect(client, mnisIP.c_str(), callId);
					m_mnisIP = mnisIP;
				}
				else if (radioIP != "")
				{
					m_radioIP = radioIP;
					cs.radioConnect(client,  radioIP.c_str(), callId);
					m_dispatchOperate[s]->AddAllCommand(client, s, RADIO_CONNECT, radioIP, mnisIP, "", 0, _T(""), 0, 0, callId);
				}
				
			}
			else
			{

			}
			
		}

	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
