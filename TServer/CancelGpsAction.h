#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void cancelGpsAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("id"))
		{
			std::string id = d["id"].GetString();
			std::map<std::string, std::string> args;
			args["id"] = id;
			int clientCallId = CBroker::instance()->getCallId();
			std::string callJsonStr = CRpcJsonParser::buildCall("cancelGps", clientCallId, args);

			int ret = CBroker::instance()->getRadioClient()->sendRequest(callJsonStr.c_str(),
				clientCallId,
				pRemote,
				[&](const char* pResponse, void*){
				std::map<std::string, std::string> args;
				std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
				pRemote->sendResponse(strResp.c_str(), strResp.size());
			}, nullptr);

			if (-1 == ret)
			{
				// remote error or disconnected
				std::map<std::string, std::string> args;
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
