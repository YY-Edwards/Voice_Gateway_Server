#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void connectEventAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);
	try{
		string radioIP = "";
		string mnisIP = "";

		Document doc;
		doc.Parse(param.c_str());
		//if (type == "radio")
		{
			if (doc.HasMember("param"))
			if (doc["param"].IsObject())
			{
				Value obj = doc["param"].GetObject();

				//radioIP
				if (obj.HasMember("Ride") && obj["Ride"].IsObject())
				{
					Value objRadio = obj["Ride"].GetObject();
					if (objRadio.HasMember("Ip") && objRadio["Ip"].IsString())
					{
						radioIP = objRadio["Ip"].GetString();
						
					}
				}
				//mnisIP
				if (obj.HasMember("Mnis") && obj["Mnis"].IsObject())
				{
					Value objRadio = obj["Mnis"].GetObject();
					if (objRadio.HasMember("Ip") && objRadio["Ip"].IsString())
					{
						mnisIP = objRadio["Ip"].GetString();


					}
				}
				//GPS ·­×ªIP
				if (obj.HasMember("Gps") && obj["Gps"].IsObject())
				{
					Value objRadio = obj["Gps"].GetObject();
					if (objRadio.HasMember("Ip") && objRadio["Ip"].IsString())
					{
						//radioIP = objRadio["Ip"].GetString();

					}
				}
				DispatchOperate  * pDispatchOperate = new DispatchOperate();
				m_dispatchOperate[pRemote] = pDispatchOperate;
				m_dispatchOperate[pRemote]->AddAllCommand(pRemote, RADIO_CONNECT, radioIP, mnisIP, "", 0, _T(""), 0, 0, callId);

				ArgumentType args;
				args["message"] = "connect";
				std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "sucess", args);
				pRemote->sendResponse(strResp.c_str(), strResp.size());

				

			}
		}
		
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}
	
}
