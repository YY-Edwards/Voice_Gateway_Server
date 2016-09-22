#include "stdafx.h"
#include "../include/TcpServer.h"
#include "../include/RpcServer.h"
#include "../include/RpcJsonParser.h"

CRpcServer::CRpcServer()
	:m_pConnector(NULL)
{
}


CRpcServer::~CRpcServer()
{
}

int CRpcServer::onReceive(CRemotePeer* pRemote, char* pData, int dataLen)
{
	try{
		std::string str(pData, dataLen);
		printf("received data:%s", str.c_str());

		std::map<std::string, std::string> args;
		CRpcJsonParser parser;
		std::string callName;
		uint64_t callId = 0;
		if (0 != parser.getCallName(str, callName, callId))
		{
			// send error response
			std::string response = CRpcJsonParser::buildResponse("failed", callId, 404, "Invalid request");
			pRemote->sendResponse(response.c_str(), response.size());
			throw std::exception("invalid request");
		}

		std::lock_guard<std::mutex> lock(m_actionsLocker);

		if (m_mpActions.find(callName) != m_mpActions.end())
		{
			std::list<std::string> argList = m_mpActions[callName]->getArgNames();
			if (0 != parser.getArgs(str, argList, args))
			{
				// invalid parameter, send error response
				std::string response = CRpcJsonParser::buildResponse("failed", callId, 404, "Invalid parameter");
				pRemote->sendResponse(response.c_str(), response.size());
				throw std::exception("invalid parameter number");
			}
			m_mpActions[callName]->run(pRemote, args, callId);
		}
	}
	catch (std::exception& e)
	{
		printf("exception: %s\r\n", e.what());
	}
	catch (...)
	{
		printf("unknow error! \r\n");
	}
	return 0;
}

int CRpcServer::start()
{
	m_pConnector = new CTcpServer();
	m_pConnector->setReceiveDataHandler(this);
	m_pConnector->start();

	return 0;
}

void CRpcServer::stop()
{
	if (m_pConnector)
	{
		delete m_pConnector;
	}

	std::lock_guard<std::mutex> lock(m_actionsLocker);

	// close and free all actions memory
	for (auto i = m_mpActions.begin(); i != m_mpActions.end(); i++)
	{
		delete i->second;
	}

	m_mpActions.erase(m_mpActions.begin(), m_mpActions.end());
}

void CRpcServer::addActionHandler(CAbstractAction* pAction)
{
	if ( NULL == pAction)
	{
		return;
	}
	std::lock_guard<std::mutex> lock(m_actionsLocker);
	m_mpActions[pAction->getName()] = pAction;
}
