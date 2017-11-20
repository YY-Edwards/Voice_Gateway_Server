#pragma once

/**
* 设置车台的IP地址
*
* 命令: setRadioIp
* 参数: 
*	ip: string, example: "10.0.0.2"
*	json example: param:{ip:"10.0.0.2"}
*/

#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "Settings.h"

void sendResponse(CRemotePeer* pRemote, const char* pData, int dataLen)
{
	pRemote->sendResponse(pData, dataLen);
	
}

void setBaseAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		Document d;
		d.Parse(param.c_str());
		CSettings::instance()->setValue("base",d.GetObject());
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", "");
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}

void getBaseAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", CSettings::instance()->getValue("base"));
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}

void setRadioAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		Document d;
		d.Parse(param.c_str());
		CSettings::instance()->setValue("radio", d.GetObject());
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", "");
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}

void getRadioAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", CSettings::instance()->getValue("radio"));
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}

void setRepeaterAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		Document d;
		d.Parse(param.c_str());
		CSettings::instance()->setValue("repeater", d.GetObject());
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", "");
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}

void getRepeaterAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", CSettings::instance()->getValue("repeater"));
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}
void setMnisAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		Document d;
		d.Parse(param.c_str());
		CSettings::instance()->setValue("mnis", d.GetObject());
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", "");
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}
void getMnisAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", CSettings::instance()->getValue("mnis"));
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}
void setLocationAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		Document d;
		d.Parse(param.c_str());
		CSettings::instance()->setValue("location", d.GetObject());
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", "");
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}
void getLocationAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", CSettings::instance()->getValue("location"));
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}
void setLocationInDoorAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		Document d;
		d.Parse(param.c_str());
		CSettings::instance()->setValue("locationIndoor", d.GetObject());
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", "");
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}
void getLocationInDoorAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try
	{
		std::string strResp = CSettings::instance()->getResponse("success", callId, 200, "", CSettings::instance()->getValue("locationIndoor"));
		sendResponse(pRemote, strResp.c_str(), strResp.size());
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
}
