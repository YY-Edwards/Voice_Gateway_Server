#pragma once

#include <mutex>
#include <map>
#include <list>

#include "BaseConnector.h"

#define		ThreadCountInPool			4

class ThreadPool;
class CRequest;
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
	int sendRequest(const char* pRequest,
		uint64_t nCallId,
		void* data,
		std::function<void(const char* pResponse, void*)> success = nullptr,
		std::function<void(const char* pResponse, void*)> failed = nullptr,
		int nTimeoutSeconds = 10);

protected:
	std::map<std::string, ACTION>  m_mpActions;
	CBaseConnector* m_pConnector;
	ThreadPool* m_thdPool;

	std::list<CRequest*> m_lstRequest;
	std::mutex m_mtxRequest;
};

