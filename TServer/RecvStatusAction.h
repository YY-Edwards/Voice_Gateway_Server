#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void recvStatusAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("getType") && d["getType"].IsInt())
		{
			int getType = d["getType"].GetInt();
			if (getType == 1)
			{
				if (type == "wl")
				{
					if (d.HasMember("info") && d["info"].IsInt())
					{
						CBroker::instance()->setDeviceStatusByType(System_MnisStatus, d["info"].GetInt());
					}
				}
				else if (type == "radio")
				{
					if (d.HasMember("info") && d["info"].IsInt())
					{
						int info = d["info"].GetInt();
						switch (info)
						{
						case 0:
							CBroker::instance()->setDeviceStatus(true, true);
							break;
						case 1:
							CBroker::instance()->setDeviceStatus(false, true);
							break;
						case 2:
							CBroker::instance()->setDeviceStatus(true, false);
							break;
						case 3:
							CBroker::instance()->setDeviceStatus(false, false);
							break;
						}
						//CBroker::instance()->sendSystemStatusToClient("", pRemote, callId);
					}
				}
				
			}
		}
		std::string callCommand = CRpcJsonParser::mergeCommand("status", callId, param.c_str(), type.c_str());
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
