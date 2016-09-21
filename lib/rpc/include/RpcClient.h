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
	virtual int onReceive(CRemotePeer* pRemote, char* pData, int dataLen);

public:
	int start();
	void stop();
	int send(unsigned char* pData, int dataLen);

protected:
	CBaseConnector* m_pConnector;
};

