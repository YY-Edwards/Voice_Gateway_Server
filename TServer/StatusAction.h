#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void statusAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("getType") && d["getType"].IsInt())
		{
			int getType = d["getType"].GetInt();
			if (getType == 4)
			{
				if (d.HasMember("SessionId") && d["SessionId"].IsString())
				{
					CBroker::instance()->sendSystemStatusToClient(d["SessionId"].GetString(), pRemote,callId);
				}
			}
			else
			{
				std::string callCommand = CRpcJsonParser::mergeCommand("status", callId, param.c_str());
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
			}
		}
	
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
