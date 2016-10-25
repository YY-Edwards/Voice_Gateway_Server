#include "stdafx.h"
#include "../lib/rpc/include/RpcClient.h"
#include "Broker.h"

std::auto_ptr<CBroker> CBroker::m_instance;

CBroker::CBroker()
	: m_wirelanClient(NULL)
	, callId(1)
{
	m_radioClient = new CRpcClient();
	m_radioClient->start("tcp://127.0.0.1:8000");

}


CBroker::~CBroker()
{
	if (m_radioClient)
	{
		m_radioClient->stop();
		delete m_radioClient;
	}
}
