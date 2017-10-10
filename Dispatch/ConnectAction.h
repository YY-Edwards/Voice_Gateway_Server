#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "../lib/rpc/include/TcpServer.h"
#include "extern.h"
void connectAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;
	std::lock_guard<std::mutex> locker(lock);
	try{
		g_sn = callId;
		std::string strResp = CRpcJsonParser::buildResponse("success", callId, 200, "success", ArgumentType());
		pRemote->sendResponse(strResp.c_str(), strResp.size());

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
		if (isHave && param !="")
		{
			std::string radioIP = "";
			std::string mnisIP = "";
			std::string gpsIP = "";
			Document d;
			d.Parse(param.c_str());
			if (d.IsObject() && d.HasMember("IsEnable") && d["IsEnable"].IsBool())
			if (d.HasMember("IsOnlyRide")&& d["IsOnlyRide"].IsBool())
			{
				if (true == d["IsEnable"].GetBool() )
				{
					if (d.HasMember("Ride") && d["Ride"].IsObject())
					{
						Value objRadio = d["Ride"].GetObject();
						if (objRadio.HasMember("Ip") && objRadio["Ip"].IsString())
						{
							radioIP = objRadio["Ip"].GetString();

						}
					}
					//mnisIP
					if(false == d["IsOnlyRide"].GetBool())
					if (d.HasMember("Mnis") && d["Mnis"].IsObject())
					{
						Value objRadio = d["Mnis"].GetObject();
						if (objRadio.HasMember("Ip") && objRadio["Ip"].IsString())
						{
							mnisIP = objRadio["Ip"].GetString();


						}
					}
					//GPS 翻转IP
					if (d.HasMember("Gps") && d["Gps"].IsObject())
					{
						Value objRadio = d["Gps"].GetObject();
						if (objRadio.HasMember("Ip") && objRadio["Ip"].IsString())
						{
							gpsIP = objRadio["Ip"].GetString();

						}
					}
					dis.connect(radioIP.c_str(), mnisIP.c_str(), gpsIP.c_str());
				}
				else
				{
					dis.disConnect();
				}
			}
		}
		else
		{
#if DEBUG_LOG
			LOG(INFO) << "tcp连接不存在！";
#endif
		}
		
	}

	catch (std::exception e){

	}
	catch (...)
	{

	}

}
