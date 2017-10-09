#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void queryLicenseAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	LOG(INFO) << "queryLicenseAction";
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		int status = CBroker::instance()->getLicenseStatus();
		SerialInformation license = CBroker::instance()->getLicenseInformation();
		SerialInformation s = CBroker::instance()->getSerialInformation();
		ArgumentType args;
		std::string strResp;
		switch (status)
		{
		case 0:
		case 2:
			args["DeviceType"] = s.deviceType;
			args["RadioSerial"] = s.radioSerial;
			args["RadioMode"] = s.radioMode;
			args["RepeaterSerial"] = s.repeaterSerial;
			args["RepeaterMode"] = s.repeaterMode;
		    strResp = CRpcJsonParser::buildResponse("faliure", callId, 201, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			break;
		case 1:
			args["DeviceType"] = license.deviceType;
			args["RadioSerial"] = license.radioSerial;
			args["RadioMode"] = license.radioMode;
			args["RepeaterSerial"] = license.repeaterSerial;
			args["RepeaterMode"] = license.repeaterMode;
			args["Time"] = license.time;
			args["IsEver"] = license.isEver;
			args["Expiration"] = license.expiration;
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			break;
		default:
			
			
			break;
		}
		
	/*	if (status)
		{
			SerialInformation license = CBroker::instance()->getLicenseInformation();
			ArgumentType args;
			args["DeviceType"] = license.deviceType;
			args["RadioSerial"] = license.radioSerial;
			args["RadioMode"] = license.radioMode;
			args["RepeaterSerial"] = license.repeaterSerial;
			args["RepeaterMode"] = license.repeaterMode;
			args["Time"] = license.time;
			args["IsEver"] = license.isEver;
			args["Expiration"] = license.expiration;
			std::string strResp = CRpcJsonParser::buildResponse("success", callId, 200, "",args );
			pRemote->sendResponse(strResp.c_str(), strResp.size());
		}
		else
		{
			SerialInformation s = CBroker::instance()->getSerialInformation();
			ArgumentType args;
			args["DeviceType"] = s.deviceType;
			args["RadioSerial"] = s.radioSerial;
			args["RadioMode"] = s.radioMode;
			args["RepeaterSerial"] = s.repeaterSerial;
			args["RepeaterMode"] = s.repeaterMode;
			std::string strResp = CRpcJsonParser::buildResponse("faliure", callId, 201, "",args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
		}
		*/
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
