#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void recvLocationIndoorAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{

		std::string callCommand = CRpcJsonParser::mergeCommand("location", callId, param.c_str(), type.c_str());
		int ret = CBroker::instance()->getRpcServer()->sendRequest(callCommand.c_str(),
			callId,
			pRemote,
			[&](const char* pResponse, void* data){

		}, nullptr);
		if (-1 == ret)
		{
			// remote error or disconnected
			std::map<std::string, std::string> args;
			std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 404, "", ArgumentType());
			pRemote->sendResponse(strResp.c_str(), strResp.size());
		}
		//–¥»Îlog
		//–¥»Îlog

		Document d;
		d.Parse(param.c_str());
		int source = 0;
		ArgumentType args;
		FieldValue f(FieldValue::TArray);
		FieldValue element(FieldValue::TObject);
		if (d.HasMember("source") && d["source"].IsInt())
		{
			source = d["source"].GetInt();
			element.setKeyVal("source", FieldValue(source));

		}
		if (d.HasMember("bcons") && d["bcons"].IsArray())
		{
			int len = d["bcons"].Size();
			for (int i = 0; i < len; i++)
			{
		
				int major = d["bcons"][i]["major"].GetInt();
				int minor = d["bcons"][i]["minor"].GetInt();
				int time = d["bcons"][i]["timestamp"].GetInt();
				element.setKeyVal("major", FieldValue(major));
				element.setKeyVal("minor", FieldValue(minor));
				element.setKeyVal("timestamp", FieldValue(time));
				f.push(element);
			}
		}
		args["operation"] = FieldValue("add");
		args["location"] = FieldValue(f);

		std::string callJsonStr = CRpcJsonParser::buildCall("locationLog", callId, args, "radio");
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
