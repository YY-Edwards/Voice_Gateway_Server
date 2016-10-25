#pragma once

#include <mutex>
#include <map>

#include "BaseConnector.h"

#define		ThreadCountInPool			4

typedef std::function<void(CRemotePeer*, const std::string&, uint64_t, const std::string&)> ACTION;

class ThreadPool;
class CRpcServer : public OnConnectorData
{
public:
	CRpcServer();
	~CRpcServer();

	enum _ConnectorType{
		TCP = 1,
	};

public: // dereived from OnConnectorData
	virtual int onReceive(CRemotePeer* pRemote, char* pData, int dataLen);

public:
	int start(unsigned short port = 8000, int type = CRpcServer::TCP);
	void stop();
	void addActionHandler(const char* pName, ACTION action);

protected:
	std::map<std::string, ACTION>  m_mpActions;
	CBaseConnector* m_pConnector;
	ThreadPool* m_thdPool;
};

