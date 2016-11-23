#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void recvMsgAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		
		std::string callCommand = CRpcJsonParser::mergeCommand("message", callId, param.c_str(), type.c_str());
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
		//Ð´Èëlog

		Document d;
		d.Parse(param.c_str());
		int source = -1;
		int destination = -1;
		std::string message = "";
		if (d.HasMember("Target") && d["Target"].IsInt())
		{
			destination = d["Target"].GetInt();
		}
		if (d.HasMember("Source") && d["Source"].IsInt())
		{
			source = d["Source"].GetInt();
		}
		if (d.HasMember("Contents") && d["Contents"].IsString())
		{
			message = d["Contents"].GetString();
		}
		ArgumentType args;
		FieldValue f(FieldValue::TArray);
		FieldValue element(FieldValue::TObject);
		element.setKeyVal("source", FieldValue(source));
		element.setKeyVal("destination", FieldValue(destination));
		element.setKeyVal("message", FieldValue(message.c_str()));
		f.push(element);
		args["operation"] = FieldValue("add");
		args["sms"] = FieldValue(f);

		std::string callJsonStr = CRpcJsonParser::buildCall("smslog", callId, args, "radio");
		int result = CBroker::instance()->getLogClient()->sendRequest(callJsonStr.c_str(), callId,
			pRemote,
			[&](const char* pResponse, void* data){
		}, nullptr);
		if (-1 == result)
		{
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
