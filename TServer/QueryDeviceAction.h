#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void queryDeviceAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
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
			args["DeviceType"] = s.deviceType;
			args["DeviceSerial"] = s.radioSerial;
			args["DeviceMode"] = s.radioMode;
	
			strResp = CRpcJsonParser::buildResponse("success", callId, 201, "", args);
		}
		else if (!tempRepeater.empty())
		{
			args["DeviceType"] = s.deviceType;
			args["DeviceSerial"] = s.repeaterSerial;
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
