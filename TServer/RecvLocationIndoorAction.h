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

		std::string callCommand = CRpcJsonParser::mergeCommand("sendBeacons", callId, param.c_str(), type.c_str());
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
		//FieldValue f(FieldValue::TArray);
		FieldValue element(FieldValue::TObject);
		if (d.HasMember("source") && d["source"].IsInt())
		{
			source = d["source"].GetInt();
			element.setKeyVal("source", FieldValue(source));

		}
		if (d.HasMember("bcon") && d["bcon"].IsObject())
		{
		/*	int len = d["bcons"].Size();
			for (int i = 0; i < len; i++)
			{
		
				int major = d["bcons"][i]["major"].GetInt();
				int minor = d["bcons"][i]["minor"].GetInt();
				int time = d["bcons"][i]["timestamp"].GetInt();
				int rssi = d["bcons"][i]["rssi"].GetInt();
				int txpower = d["bcons"][i]["txpower"].GetInt();
				FieldValue uuid(FieldValue::TArray);
				for (int j = 0; j < d["bcons"][i]["uuid"].GetArray().Size(); j++)
				{
					FieldValue temp(FieldValue::TInt);
					temp.setInt(d["bcons"][i]["uuid"][j].GetInt());
					uuid.push(temp);
				}
			
				element.setKeyVal("major", FieldValue(major));
				element.setKeyVal("minor", FieldValue(minor));
				element.setKeyVal("timestamp", FieldValue(time));
				element.setKeyVal("rssi", FieldValue(rssi));
				element.setKeyVal("txpower", FieldValue(txpower));
				element.setKeyVal("uuid", FieldValue(uuid));
				f.push(element);
			}*/

			int major = d["bcon"]["major"].GetInt();
			int minor = d["bcon"]["minor"].GetInt();
			int time = d["bcon"]["timestamp"].GetInt();
			int rssi = d["bcon"]["rssi"].GetInt();
			int txpower = d["bcon"]["txpower"].GetInt();
			FieldValue uuid(FieldValue::TArray);
			for (int j = 0; j < d["bcon"]["uuid"].GetArray().Size(); j++)
			{
				FieldValue temp(FieldValue::TInt);
				temp.setInt(d["bcon"]["uuid"][j].GetInt());
				uuid.push(temp);
			}

			element.setKeyVal("major", FieldValue(major));
			element.setKeyVal("minor", FieldValue(minor));
			element.setKeyVal("timestamp", FieldValue(time));
			element.setKeyVal("rssi", FieldValue(rssi));
			element.setKeyVal("txpower", FieldValue(txpower));
			element.setKeyVal("uuid", FieldValue(uuid));
		}
		args["operation"] = FieldValue("add");
		args["location"] = FieldValue(element);

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
