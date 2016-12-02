#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

/*gb2312 ◊™utf8*/
char* G2U(const char* pcGb2312)
{
	int nUnicodeLen = MultiByteToWideChar(CP_ACP, 0, pcGb2312, -1, NULL, 0);

	wchar_t * pcUnicode = new wchar_t[nUnicodeLen + 1];

	memset(pcUnicode, 0, nUnicodeLen * 2 + 2);

	MultiByteToWideChar(CP_ACP, 0, pcGb2312, -1, pcUnicode, nUnicodeLen);

	//unicode to utf8

	int nUtf8Len = WideCharToMultiByte(CP_UTF8, 0, pcUnicode, -1, NULL, 0, NULL, NULL);

	char *pcUtf8 = new char[nUtf8Len + 1];

	memset(pcUtf8, 0, nUtf8Len + 1);

	WideCharToMultiByte(CP_UTF8, 0, pcUnicode, -1, pcUtf8, nUtf8Len, NULL, NULL);

	///delete[] pcUtf8;

	delete[] pcUnicode;

	return pcUtf8;
}
void addSmsLog(std::string param, int callId, CRemotePeer* pRemote)
{
	try
	{
		Document d;
		d.Parse(param.c_str());
		int source = -1;
		int destination = -1;
		std::string pcGb2312 = "";
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
			pcGb2312 = d["Contents"].GetString();
		}
		/*gb2312 to utf8*/
		std::string message = G2U(pcGb2312.c_str());

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
			CRemotePeer* pCommandSender = (CRemotePeer*)data;
			pCommandSender->sendResponse(pResponse, strlen(pResponse));
		}, nullptr);
		if (-1 == result)
		{
			std::map<std::string, std::string> args;
			std::string strResp = CRpcJsonParser::buildResponse("failed", callId, 404, "", ArgumentType());
			pRemote->sendResponse(strResp.c_str(), strResp.size());
		}

	}

	catch (std::exception e)
	{

	}


}
void msgAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		std::string callCommand = CRpcJsonParser::mergeCommand("message", callId, param.c_str());
		CRpcClient* pDstServer = NULL;
		if ("wl" == type)
		{
			pDstServer = CBroker::instance()->getWireLanClient();
		}
		else
		{
			pDstServer = CBroker::instance()->getRadioClient();
		}
		int ret = pDstServer->sendRequest(callCommand.c_str(),
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

		//–¥»Îlog
		addSmsLog(param, callId, pRemote);
	}
	catch (std::exception e)
	{

	}
	catch (...)
	{

	}
}


