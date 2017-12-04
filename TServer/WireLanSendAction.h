#ifndef WIRELANSENDACTION_H
#define WIRELANSENDACTION_H

#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

/*from server to client*/
void wlCallAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{

		std::string callCommand = CRpcJsonParser::mergeCommand("wlCall", callId, param.c_str(), type.c_str());
		int ret = CBroker::instance()->getRpcServer()->sendRequest(callCommand.c_str(),
			callId,
			pRemote,
			[&](const char* pResponse, void* data){

			//CBroker::instance()->getRadioClient()-
			/*CRemotePeer* pCommandSender = (CRemotePeer*)data;
			pCommandSender->sendResponse(pResponse, strlen(pResponse));*/
		}, nullptr);
		if (-1 == ret)
		{
			// remote error or disconnected
			std::map<std::string, std::string> args;
			std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 404, "", ArgumentType());
			pRemote->sendResponse(strResp.c_str(), strResp.size());
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}

void wlCallStatusAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{

		std::string callCommand = CRpcJsonParser::mergeCommand("wlCallStatus", callId, param.c_str(), type.c_str());
		int ret = CBroker::instance()->getRpcServer()->sendRequest(callCommand.c_str(),
			callId,
			pRemote,
			[&](const char* pResponse, void* data){

			//CBroker::instance()->getRadioClient()-
			/*CRemotePeer* pCommandSender = (CRemotePeer*)data;
			pCommandSender->sendResponse(pResponse, strlen(pResponse));*/
		}, nullptr);
		if (-1 == ret)
		{
			// remote error or disconnected
			std::map<std::string, std::string> args;
			std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 404, "", ArgumentType());
			pRemote->sendResponse(strResp.c_str(), strResp.size());
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}

void wlInfoAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;
	std::lock_guard<std::mutex> locker(lock);
	try
	{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("getType") && d["getType"].IsInt())
		{
			int getType = d["getType"].GetInt();
			switch (getType)
			{
				//#define GET_TYPE_SYSTEM_STATUS 0x04
			case 0x04:
			{
						 if (d.HasMember("info") && d["info"].IsObject())
						 {
							 Value json = d["info"].GetObject();
							 if (json.HasMember("DongleCount") && json["DongleCount"].IsInt())
							 {
								 int value = json["DongleCount"].GetInt();
								 /*更新DongleCount*/
								 CBroker::instance()->setDeviceStatusByType(System_DongleCount, value);
							 }
							 if (json.HasMember("MicphoneStatus") && json["MicphoneStatus"].IsInt())
							 {
								 int value = json["MicphoneStatus"].GetInt();
								 /*更新MicphoneStatus*/
								 CBroker::instance()->setDeviceStatusByType(System_MicphoneStatus, value);
							 }
							 if (json.HasMember("SpeakerStatus") && json["SpeakerStatus"].IsInt())
							 {
								 int value = json["SpeakerStatus"].GetInt();
								 /*更新SpeakerStatus*/
								 CBroker::instance()->setDeviceStatusByType(System_SpeakerStatus, value);
							 }
							 if (json.HasMember("LEStatus") && json["LEStatus"].IsInt())
							 {
								 int value = json["LEStatus"].GetInt();
								 /*更新LEStatus*/
								 CBroker::instance()->setDeviceStatusByType(System_LEStatus, value);
							 }
							 if (json.HasMember("WireLanStatus") && json["WireLanStatus"].IsInt())
							 {
								 int value = json["WireLanStatus"].GetInt();
								 /*更新WireLanStatus*/
								 CBroker::instance()->setDeviceStatusByType(System_WireLanStatus, value);
							 }
							 if (json.HasMember("DeviceInfoStatus") && json["DeviceInfoStatus"].IsInt())
							 {
								 int value = json["DeviceInfoStatus"].GetInt();
								 /*更新DeviceInfoStatus*/
								 CBroker::instance()->setDeviceStatusByType(System_DeviceInfoStatus, value);
							 }
						 }
			}
				break;
				//#define GET_TYPE_CONN 0x01
			case 0x01:
			{
						 if (d.HasMember("info") && d["info"].IsInt())
						 {
							 int value = d["info"].GetInt();
							 /*更新DeviceStatus*/
							 CBroker::instance()->setDeviceStatusByType(System_DeviceStatus, value);
						 }
			}
				break;
				//#define GET_TYPE_ONLINE_DEVICES 0x02
			case 0x02:
			{
			}
				break;
				//#define GET_TYPE_SESSION_STATUS 0x03
			case 0x03:
			{
			}
				break;
			default:
				break;
			}
			if (4 != getType)
			{
				std::string callCommand = CRpcJsonParser::mergeCommand("wlInfo", callId, param.c_str(), type.c_str());
				int ret = CBroker::instance()->getRpcServer()->sendRequest(callCommand.c_str(),
					callId,
					pRemote,
					[&](const char* pResponse, void* data){

					//CBroker::instance()->getRadioClient()-
					/*CRemotePeer* pCommandSender = (CRemotePeer*)data;
					pCommandSender->sendResponse(pResponse, strlen(pResponse));*/
				}, nullptr);
				if (-1 == ret)
				{
					// remote error or disconnected
					std::map<std::string, std::string> args;
					std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 404, "", ArgumentType());
					pRemote->sendResponse(strResp.c_str(), strResp.size());
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

void wlPlayStatusAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{

		std::string callCommand = CRpcJsonParser::mergeCommand("wlPlayStatus", callId, param.c_str(), type.c_str());
		int ret = CBroker::instance()->getRpcServer()->sendRequest(callCommand.c_str(),
			callId,
			pRemote,
			[&](const char* pResponse, void* data){

			//CBroker::instance()->getRadioClient()-
			/*CRemotePeer* pCommandSender = (CRemotePeer*)data;
			pCommandSender->sendResponse(pResponse, strlen(pResponse));*/
		}, nullptr);
		if (-1 == ret)
		{
			// remote error or disconnected
			std::map<std::string, std::string> args;
			std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 404, "", ArgumentType());
			pRemote->sendResponse(strResp.c_str(), strResp.size());
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}

void wlGetConfigAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		CBroker::instance()->sendWirelanConfig();
		//CBroker::instance()->sendLoactionIndoorConfigToWl();
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}
}
void wlReadSerialAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
 {
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{

		static std::mutex lock;
		std::lock_guard<std::mutex> locker(lock);
		Document d;
		d.Parse(param.c_str());
		LOG(INFO) << "wlReadSerialAction";
		if (d.HasMember("serial") && d["serial"].IsString())
		{
			SerialInformation s = CBroker::instance()->getSerialInformation();
			memcpy(s.licType, "TrboX 3.0", 12);
			std::string serial = d["serial"].GetString();
			if (type == "radio")
			{
				s.deviceType = CRADIO;    //DeviceType：设备类型(1:车载台，2：中继台，3：对讲机， 4：PC)
				if (serial.length() == 10)
				{
					memcpy(s.radioSerial, serial.c_str(), 16);
				}
				else if (serial.length() == 12)
				{
					memcpy(s.radioMode, serial.c_str(), 16);
				}

				CBroker::instance()->setSerialInformation(s);
			}
			else if (type == "wl")
			{
				s.deviceType = REPEATER;
				if (serial.length() == 10)
				{
					memcpy(s.repeaterSerial, serial.c_str(), 16);
					memcpy(s.repeaterMode, "AZH69JDC9KA2AN", 16);
				}
				
				CBroker::instance()->setSerialInformation(s);
			}

			std::string strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			readSerial();
		}

	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}

#endif