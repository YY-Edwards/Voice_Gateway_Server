#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void getGpsAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("id") && d.HasMember("cycle") && d.HasMember("querymode"))
		{
			std::string id = d["id"].GetString();
			std::string cycle = d["cycle"].GetString();
			std::string queryMode = d["queryMode"].GetString();
			ArgumentType args;
			args["id"] = FieldValue(id.c_str());
			args["cycle"] = FieldValue( cycle.c_str());
			args["queryMode"] = FieldValue( queryMode.c_str());
			int clientCallId = CBroker::instance()->getCallId();
			std::string callJsonStr = CRpcJsonParser::buildCall("getGps", clientCallId, args);

			int ret = CBroker::instance()->getRadioClient()->sendRequest(callJsonStr.c_str(),
				clientCallId,
				pRemote,
				[&](const char* pResponse, void*){
				pRemote->sendResponse(pResponse, strlen(pResponse));
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
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
