#pragma once
#include <map>
#include <string>
#include "BaseConnector.h"

class CRpcClient : public OnConnectorData
{
public:
	CRpcClient();
	~CRpcClient();

public: // dereived from OnConnectorData
	virtual int onReceive(unsigned char* pData, int dataLen);

public:
	int start();
	void stop();

protected:
	CBaseConnector* m_pConnector;
};

