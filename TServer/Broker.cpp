#include "stdafx.h"
#include "../lib/rpc/include/RpcClient.h"
#include "Broker.h"

#define				RadioProcessConnector			"tcp://127.0.0.1:9001"
#define				WirelanProcessConnector			"tcp://127.0.0.1:9002"
#define				LogProcessConnector				"tcp://127.0.0.1:9003"

std::auto_ptr<CBroker> CBroker::m_instance;

CBroker::CBroker()
	: m_wirelanClient(NULL)
	, callId(1)
{
	m_radioClient = new CRpcClient();
}


CBroker::~CBroker()
{
	if (m_radioClient)
	{
		m_radioClient->stop();
		delete m_radioClient;
	}
}


int CBroker::connect()
{
	return m_radioClient->start(RadioProcessConnector);
}