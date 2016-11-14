#pragma once

#include <mutex>
#include <map>
#include <list>
#include <thread>
#include <condition_variable>

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

	class CClient{
	public:
		CClient(){}
	public:
		std::mutex m_mtxRequest;
		std::list<CRequest*> m_lstRequest;
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
		int nTimeoutSeconds = 10,
		bool bNeedResponse = true);

	void setOnConnectHandler(std::function<void(CRemotePeer*)> fnEvent);
	void setOnDisconnectHandler(std::function<void(CRemotePeer*)> fnEvent);

protected:
	int sendNextCommands(CRemotePeer* remote, std::list<CRequest*>& lstCommands);

protected:
	std::map<std::string, ACTION>  m_mpActions;
	CBaseConnector* m_pConnector;
	ThreadPool* m_thdPool;

	std::map<CRemotePeer*, CClient*> m_Clients;

	std::thread	m_maintainThread;
	bool m_bQuit;
	std::mutex m_mtxQuit;
	std::condition_variable m_evQuit;

	std::function<void(CRemotePeer*)> m_fnNewClient;
	std::function<void(CRemotePeer*)> m_fnClientDisconnect;
};

