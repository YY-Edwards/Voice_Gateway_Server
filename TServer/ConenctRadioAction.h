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
			std::map<std::string, std::string> args;
			args["radioIP"] = radioIP;
			args["mnisIP"] = mnisIP;

			std::string strCall = CRpcJsonParser::buildCall("connect", callId, args);
			CBroker::instance()->getRadioClient()->sendRequest(strCall.c_str(), callId, pRemote, [&](const char* pResponse, void* data){
				std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
				CRemotePeer* pCommandSender = (CRemotePeer*)data;
				pCommandSender->sendResponse(pResponse, strlen(pResponse));
				printf("recevied: %s\r\n", pResponse);
			});
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
