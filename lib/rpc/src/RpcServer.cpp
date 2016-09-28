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

		std::string callName;
		uint64_t callId = 0;
		std::string param = "";
		if (0 != CRpcJsonParser::getRequest(str, callName, callId, param))
		{
			// send error response
			std::string response = CRpcJsonParser::buildResponse("failed", callId, 404, "Invalid request");
			pRemote->sendResponse(response.c_str(), response.size());
			throw std::exception("invalid request");
		}
		CAbstractAction* pAction = NULL;
		pAction = (m_mpActions.find(callName) != m_mpActions.end()) ? m_mpActions[callName] : NULL;

		if (pAction)
		{
			pAction->run(pRemote, param, callId);
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
	m_mpActions[pAction->getName()] = pAction;
}
