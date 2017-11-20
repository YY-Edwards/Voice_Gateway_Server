#pragma once
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "../lib/rpc/include/TcpServer.h"
#include "extern.h"


void parseRadioCfg(radio_t &cfg,Value json) 
{
	if (json.HasMember("IsEnable") && json["IsEnable"].IsBool())
	{
		cfg.IsEnable = json["IsEnable"].GetBool();
	}
		
	if (json.HasMember("TomeoutSeconds") && json["TomeoutSeconds"].IsInt())
	{
		cfg.TomeoutSeconds = json["TomeoutSeconds"].GetInt();
	}
	if (json.HasMember("Ride") && json["Ride"].IsObject())
		{
		Value objRide = json["Ride"].GetObject();
			if (objRide.HasMember("ID") && objRide["ID"].IsInt())
			{
				cfg.ID = objRide["ID"].GetInt();
			}
			if (objRide.HasMember("Host") && objRide["Host"].IsString())
			{
				strcpy_s(cfg.Host, objRide["Host"].GetString());
			}
			if (objRide.HasMember("MessagePort") && objRide["MessagePort"].IsInt())
			{
				cfg.MessagePort = objRide["MessagePort"].GetInt();
			}
			if (objRide.HasMember("ArsPort") && objRide["ArsPort"].IsInt())
			{
				cfg.ArsPort = objRide["ArsPort"].GetInt();
			}
			if (objRide.HasMember("GpsPort") && objRide["GpsPort"].IsInt())
			{
				cfg.GpsPort = objRide["GpsPort"].GetInt();
			}
			if (objRide.HasMember("XnlPort") && objRide["XnlPort"].IsInt())
			{
				cfg.XnlPort = objRide["XnlPort"].GetInt();
			}
			if (objRide.HasMember("Mode") && objRide["Mode"].IsInt())
			{
				cfg.Mode = objRide["Mode"].GetInt();
			}
		}
	if (json.HasMember("CAI") && json["CAI"].IsInt())
	{
		cfg.CAI = json["CAI"].GetInt();
	}
	if (json.HasMember("GroupCAI") && json["GroupCAI"].IsInt())
	{
		cfg.GroupCAI = json["GroupCAI"].GetInt();
	}
	if (json.HasMember("LocationType") && json["LocationType"].IsInt())
	{
		cfg.LocationType = json["LocationType"].GetInt();
	}
}
void parseMnisCfg(mnis_t &cfg, Value json)
{
	if (json.HasMember("IsEnable") && json["IsEnable"].IsBool())
	{
		cfg.IsEnable = json["IsEnable"].GetBool();
	}
	if (json.HasMember("TomeoutSeconds") && json["TomeoutSeconds"].IsInt())
	{
		cfg.TomeoutSeconds = json["TomeoutSeconds"].GetInt();
	}
	if (json.HasMember("ID") && json["ID"].IsInt())
	{
		cfg.ID = json["ID"].GetInt();
	}
	if (json.HasMember("Host") && json["Host"].IsString())
	{
		strcpy_s(cfg.Host, json["Host"].GetString());
	}
	if (json.HasMember("MessagePort") && json["MessagePort"].IsInt())
	{
		cfg.MessagePort = json["MessagePort"].GetInt();
	}
	if (json.HasMember("ArsPort") && json["ArsPort"].IsInt())
	{
		cfg.ArsPort = json["ArsPort"].GetInt();
	}
	if (json.HasMember("GpsPort") && json["GpsPort"].IsInt())
	{
		cfg.GpsPort = json["GpsPort"].GetInt();
	}
	if (json.HasMember("XnlPort") && json["XnlPort"].IsInt())
	{
		cfg.XnlPort = json["XnlPort"].GetInt();
	}
	if (json.HasMember("CAI") && json["CAI"].IsInt())
	{
		cfg.CAI = json["CAI"].GetInt();
	}
	if (json.HasMember("GroupCAI") && json["GroupCAI"].IsInt())
	{
		cfg.GroupCAI = json["GroupCAI"].GetInt();
	}
	if (json.HasMember("LocationType") && json["LocationType"].IsInt())
	{
		cfg.LocationType = json["LocationType"].GetInt();
	}
}
void parseLocationCfg(location_t &cfg, Value json)
{
	Value tempJson;
	if (json.HasMember("IsEnable") && json["IsEnable"].IsBool())
	{
		cfg.IsEnable = json["IsEnable"].GetBool();
	}
	if (json.HasMember("Interval") && json["Interval"].IsDouble())
	{
		cfg.Interval = json["Interval"].GetDouble();
	}
	if (json.HasMember("IsEnableGpsC") && json["IsEnableGpsC"].IsBool())
	{
		cfg.IsEnableGpsC = json["IsEnableGpsC"].GetBool();
	}
	if (json["GpsC"].IsObject())
	{
		tempJson = json["GpsC"].GetObject();
		if (tempJson.HasMember("Ip") && tempJson["Ip"].IsString())
		{
			strcpy_s(cfg.GpsC.Ip, tempJson["Ip"].GetString());
		}
		if (tempJson.HasMember("Port") && tempJson["Port"].IsInt())
		{
			cfg.GpsC.Port = tempJson["Port"].GetInt();
		}
	}
}
 void parseLocationIndoorCfg(locationindoor_t &cfg, Value json)
{
	if (json.HasMember("IsEnable") && json["IsEnable"].IsBool())
	{
		cfg.IsEnable = json["IsEnable"].GetBool();
	}
	if (json.HasMember("Interval") && json["Interval"].IsDouble())
	{
		cfg.Interval = json["Interval"].GetDouble();
	}
	if (json.HasMember("iBeaconNumber") && json["iBeaconNumber"].IsInt())
	{
		cfg.iBeaconNumber = json["iBeaconNumber"].GetInt();
	}
	if (json.HasMember("IsEmergency") && json["IsEmergency"].IsBool())
	{
		cfg.IsEmergency = json["IsEmergency"].GetBool();
	}
}
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
			radio_t radioCfg = { 0 };
			mnis_t  mnisCfg = { 0 };
			location_t locationCfg = { 0 };
			locationindoor_t locationIndoorCfg = { 0 };
			Document d;
			d.Parse(param.c_str());
			if (d.IsObject() && d.HasMember("radio") && d["radio"].IsObject())
			{
				parseRadioCfg(radioCfg, d["radio"].GetObject());
			}
			if (d.IsObject() && d.HasMember("mnis") && d["mnis"].IsObject())
			{
				parseMnisCfg(mnisCfg, d["mnis"].GetObject());
			}
			if (d.IsObject() && d.HasMember("location") && d["location"].IsObject())
			{
				parseLocationCfg(locationCfg, d["location"].GetObject());
			}
			if (d.IsObject() && d.HasMember("locationIndoor") && d["locationIndoor"].IsObject())
			{
				parseLocationIndoorCfg(locationIndoorCfg, d["locationIndoor"].GetObject());
			}
			if (radioCfg.IsEnable)
			{
				dis.connect(radioCfg, mnisCfg, locationCfg, locationIndoorCfg);
			}
			else
			{
				dis.disConnect();
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

