#pragma once

#include <mutex>
#include <map>

#include "BaseConnector.h"

#define		ThreadCountInPool			4

typedef std::function<void(CRemotePeer*, const std::string&, uint64_t)> ACTION;

class ThreadPool;
class CRpcServer : public OnConnectorData
{
public:
	CRpcServer();
	~CRpcServer();

public: // dereived from OnConnectorData
	virtual int onReceive(CRemotePeer* pRemote, char* pData, int dataLen);

public:
	int start();
	void stop();
	void addActionHandler(const char* pName, ACTION action);

protected:
	std::map<std::string, ACTION>  m_mpActions;
	CBaseConnector* m_pConnector;
	ThreadPool* m_thdPool;
};

