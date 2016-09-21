#include "stdafx.h"
#include "../../utf8/utf8.h"
#include "../include/TcpClientConnector.h"
#include "../include/RpcClient.h"


CRpcClient::CRpcClient()
	: m_pConnector(NULL)
{
}


CRpcClient::~CRpcClient()
{
	stop();
}


int CRpcClient::start(const char* connStr)
{
	m_pConnector = new CTcpClientConnector();
	m_pConnector->setReceiveDataHandler(this);
	m_pConnector->start(connStr);
	return 0;
}

void CRpcClient::stop()
{
	if (m_pConnector)
	{
		m_pConnector->stop();
		delete m_pConnector;
	}
	m_pConnector = NULL;
}

int CRpcClient::onReceive(CRemotePeer* pRemote, char* pData, int dataLen)
{
	std::string str(pData, dataLen);
	std::wstring wStr;
	utf8::utf8to16(str.begin(), str.end(), std::back_inserter(wStr));
	TRACE(_T("received str:%s\r\n"), wStr.c_str());
	return 0;
}

int CRpcClient::send(unsigned char* pData, int dataLen)
{
	if (NULL == m_pConnector)
	{
		return 0;
	}
	return m_pConnector->send(pData, dataLen);
}