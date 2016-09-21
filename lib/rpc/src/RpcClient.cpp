#include "stdafx.h"
#include "../include/TcpClientConnector.h"
#include "../include/RpcClient.h"


CRpcClient::CRpcClient()
	: m_pConnector(NULL)
{
}


CRpcClient::~CRpcClient()
{
	if (m_pConnector)
	{
		m_pConnector->stop();
		delete m_pConnector;
	}
}


int CRpcClient::start()
{
	m_pConnector = new CTcpClientConnector();
	m_pConnector->start();
	m_pConnector->connect("tcp://localhost:8000");
	return 0;
}

void CRpcClient::stop()
{
	m_pConnector->stop();
}

int CRpcClient::onReceive(unsigned char* pData, int dataLen)
{
	return 0;
}