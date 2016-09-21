#pragma once

#include <mutex>

#include "AbstractAction.h"
#include "BaseConnector.h"

class CRpcServer : public OnConnectorData
{
public:
	CRpcServer();
	~CRpcServer();

public: // dereived from OnConnectorData
	virtual int onReceive(unsigned char* pData, int dataLen);

public:
	int start();
	void stop();
	void addActionHandler(CAbstractAction* pAction);

protected:
	std::map<std::string, CAbstractAction*>  m_mpActions;
	CBaseConnector* m_pConnector;
	std::mutex m_actionsLocker;
};

