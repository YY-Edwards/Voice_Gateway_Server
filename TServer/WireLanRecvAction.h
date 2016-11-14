#ifndef WIRELANRECVACTION_H
#define WIRELANRECVACTION_H
#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

/*from client to server*/

void wlCallActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		std::string callCommand = CRpcJsonParser::mergeCommand("wlCall", callId, param.c_str());
		int ret = CBroker::instance()->getWireLanClient()->sendRequest(callCommand.c_str(),
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
	catch (std::exception e){

	}
	catch (...)
	{

	}

}

void wlCallStatusActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		std::string callCommand = CRpcJsonParser::mergeCommand("wlCallStatus", callId, param.c_str());
		int ret = CBroker::instance()->getWireLanClient()->sendRequest(callCommand.c_str(),
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
	catch (std::exception e){

	}
	catch (...)
	{

	}

}

void wlInfoActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		std::string callCommand = CRpcJsonParser::mergeCommand("wlInfo", callId, param.c_str());
		int ret = CBroker::instance()->getWireLanClient()->sendRequest(callCommand.c_str(),
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
	catch (std::exception e){

	}
	catch (...)
	{

	}

}

void wlPlayActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{
		std::string callCommand = CRpcJsonParser::mergeCommand("wlPlay", callId, param.c_str());
		int ret = CBroker::instance()->getWireLanClient()->sendRequest(callCommand.c_str(),
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
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
#endif