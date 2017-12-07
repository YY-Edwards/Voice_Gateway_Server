#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

void recvGetConfigAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		if (type == "radio")
		{
			CBroker::instance()->sendRadioConfig();
		}
		else if (type == "monitor")
		{
			CBroker::instance()->sendRadioConfig();
			Sleep(1000);
			CBroker::instance()->sendWirelanConfig();
		}
		
	
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
