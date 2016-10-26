#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void wiretapAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{

		std::string callCommand = CRpcJsonParser::mergeCommand("wiretap", callId, param.c_str());
		int ret = CBroker::instance()->getRadioClient()->sendRequest(callCommand.c_str(),
			callId,
			pRemote,
			[&](const char* pResponse, void* data){
			CRemotePeer* pCommandSender = (CRemotePeer*)data;
			pCommandSender->sendResponse(pResponse, strlen(pResponse));
		}, nullptr);

		if (-1 == ret)
		{
			// remote error or disconnected
			std::map<std::string, std::string> args;
			std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 404, "", ArgumentType());
			pRemote->sendResponse(strResp.c_str(), strResp.size());
		}

		//Document d;
		//d.Parse(param.c_str());
		//if (d.HasMember("id"))
		//{
		//	std::string id = d["id"].GetString();
		//	ArgumentType args;
		//	args["id"] = FieldValue( id.c_str());
		//	int clientCallId = CBroker::instance()->getCallId();
		//	std::string callJsonStr = CRpcJsonParser::buildCall("wiretap", clientCallId, args);
		//	int ret = CBroker::instance()->getRadioClient()->sendRequest(callJsonStr.c_str(),
		//		clientCallId,
		//		pRemote,
		//		[&](const char* pResponse, void* data){
		//		CRemotePeer* pCommandSender = (CRemotePeer*)data;
		//		pCommandSender->sendResponse(pResponse, strlen(pResponse));
		//	}, nullptr);
		//	if (-1 == ret)
		//	{
		//		// remote error or disconnected
		//		std::map<std::string, std::string> args;
		//		std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 404, "", ArgumentType());
		//		pRemote->sendResponse(strResp.c_str(), strResp.size());
		//	}
		//}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
