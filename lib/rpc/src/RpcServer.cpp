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

int CRpcServer::onReceive(unsigned char* pData, int dataLen)
{
	std::string str((char*)pData, dataLen);

	std::map<std::string, std::string> args;
	CRpcJsonParser parser;
	std::string call = parser.parseCall(str.c_str(), args);

	std::lock_guard<std::mutex> lock(m_actionsLocker);
	if (m_mpActions.find(call) != m_mpActions.end())
	{
		m_mpActions[call]->run(m_pConnector, args);
	}

	printf("received data:%s", str.c_str());
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
