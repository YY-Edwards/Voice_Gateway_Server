#pragma once
#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void queryDeviceAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	addPeer(pRemote);
	
	LOG(INFO) << "queryDeviceAction";
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		
		SerialInformation s = CBroker::instance()->getSerialInformation();
		ArgumentType args;
		std::string strResp;
		std::string tempRadio(s.radioSerial);
		std::string tempRepeater(s.repeaterSerial);
		if (!tempRadio.empty())
		{
			if (1==CBroker::instance()->getLicenseStatus())
			{
				args["DeviceSerial"] = CBroker::instance()->getSerial();
			}
			else
			{
				args["DeviceSerial"] = s.radioSerial;
			}
			args["DeviceType"] = s.deviceType;
			
			args["DeviceMode"] = s.radioMode;
	
			strResp = CRpcJsonParser::buildResponse("success", callId, 201, "", args);
		}
		else if (!tempRepeater.empty())
		{
			if (1 == CBroker::instance()->getLicenseStatus())
			{
				args["DeviceSerial"] = CBroker::instance()->getSerial();
			}
			else
			{
				args["DeviceSerial"] = s.repeaterSerial;
			}
			args["DeviceType"] = s.deviceType;
			
			args["DeviceMode"] = s.repeaterMode;
			strResp = CRpcJsonParser::buildResponse("success", callId, 201, "", args);
		}
		else
		{
			strResp = CRpcJsonParser::buildResponse("faliure", callId, 201, "", args);
		}
		
		pRemote->sendResponse(strResp.c_str(), strResp.size());
		
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
