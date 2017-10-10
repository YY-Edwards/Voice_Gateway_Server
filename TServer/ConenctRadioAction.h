#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "Broker.h"

void connectRadioAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);
	
	try{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("radioIP") && d.HasMember("mnisIP"))
		{
			std::string radioIP = d["radioIP"].GetString();
			std::string mnisIP = d["mnisIP"].GetString();
			ArgumentType args;
			args["radioIP"] = FieldValue(radioIP.c_str());
			args["mnisIP"] = FieldValue(mnisIP.c_str());
			int clientCallId = CBroker::instance()->getCallId();
			std::string callJsonStr = CRpcJsonParser::buildCall("connect", clientCallId, args);

			int ret = CBroker::instance()->getRadioClient()->sendRequest(callJsonStr.c_str(),
				clientCallId,
				pRemote,
				[&](const char* pResponse, void* data){
					//ArgumentType args;
					//std::string strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);
					CRemotePeer* pCommandSender = (CRemotePeer*)data;
					pCommandSender->sendResponse(pResponse, strlen(pResponse));
			}, nullptr);
			if (-1 == ret)
			{
				// remote error or disconnected
				ArgumentType args;
				std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 404, "", args);
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
