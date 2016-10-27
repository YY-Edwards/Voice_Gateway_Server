#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"


void connectAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);
	try{
		std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "sucess", ArgumentType());
		pRemote->sendResponse(strResp.c_str(), strResp.size());

		string radioIP = "";
		string mnisIP = "";
		Document d;
		d.Parse(param.c_str());
	/*	if (doc.HasMember("param"))
		if (doc["param"].IsObject())*/
		{
			//Value obj = doc["param"].GetObject();

			//radioIP
			if (d.HasMember("Radio") && d["Radio"].IsObject())
			{
				Value objRadio = d["Radio"].GetObject();
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
			DispatchOperate  * pDispatchOperate = new DispatchOperate();
			m_dispatchOperate[pRemote] = pDispatchOperate;
			m_dispatchOperate[pRemote]->AddAllCommand(pRemote, RADIO_CONNECT, radioIP, mnisIP, "", 0, _T(""), 0, 0, callId);
		}

	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
