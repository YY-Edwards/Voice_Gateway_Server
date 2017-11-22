#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
void addLocationLog(std::string param, int callId, CRemotePeer* pRemote)
{
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
	if (d.HasMember("Report") && d["Report"].IsObject())
	{
		if (d.HasMember("Type") && d["Type"].IsInt())
		{
			int type = d["Type"].GetInt();
			if (type == 0)//0:gps
			{
				double lat = d["Report"]["Lat"].GetDouble();
				double lon = d["Report"]["Lon"].GetDouble();
				double alt = d["Report"]["Alt"].GetDouble();
				double speed = d["Report"]["Speed"].GetDouble();
				int valid = d["Report"]["Valid"].GetDouble();
				element.setKeyVal("latitude", FieldValue(lat));
				element.setKeyVal("longitude", FieldValue(lon));
				element.setKeyVal("velocity", FieldValue(speed));
				f.push(element);
			}
			args["operation"] = FieldValue("add");
			args["gps"] = FieldValue(f);

			std::string callJsonStr = CRpcJsonParser::buildCall("gpsLog", callId, args, "radio");
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
			else if (type == 1) //1:becon
			{
				int major = d["Report"]["major"].GetInt();
				int minor = d["Report"]["minor"].GetInt();
				int time = d["Report"]["timestamp"].GetInt();
				int rssi = d["Report"]["rssi"].GetInt();
				int txpower = d["Report"]["txpower"].GetInt();
				FieldValue uuid(FieldValue::TArray);
				for (int j = 0; j < d["Report"]["uuid"].GetArray().Size(); j++)
				{
					FieldValue temp(FieldValue::TInt);
					temp.setInt(d["Report"]["uuid"][j].GetInt());
					uuid.push(temp);
				}

				element.setKeyVal("major", FieldValue(major));
				element.setKeyVal("minor", FieldValue(minor));
				element.setKeyVal("timestamp", FieldValue(time));
				element.setKeyVal("rssi", FieldValue(rssi));
				element.setKeyVal("txpower", FieldValue(txpower));
				element.setKeyVal("uuid", FieldValue(uuid));

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
		}
	}
}
void recvSendGpsStatusAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{

		std::string callCommand = CRpcJsonParser::mergeCommand("locationStatus", callId, param.c_str(), type.c_str());
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
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
