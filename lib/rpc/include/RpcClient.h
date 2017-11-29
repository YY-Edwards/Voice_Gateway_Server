#pragma once

#include <map>
#include <string>
#include "BaseConnector.h"
#include "request.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <list>
#include <thread>

#include "..\include\ringbuffer.h"

#ifndef IncomeDataHandler
typedef std::function<void(CBaseConnector*, const char*, int) > IncomeDataHandler;
#endif

class CRpcClient : public OnConnectorData
{
public:
	CRpcClient();
	~CRpcClient();

public: // dereived from OnConnectorData
	virtual int onReceive(CRemotePeer* pRemote, char* pData, int dataLen);

public:
	int start(const char* connStr);
	void stop();
	bool isConnected();
	int send(const char* pData, int dataLen);
	int sendRequest(const char* pRequest,
				uint64_t nCallId,
				void* data,
				std::function<void (const char* pResponse, void*)> success, 
				std::function<void(const char* pResponse, void*)> failed = nullptr,
				int nTimeoutSeconds = 10,
				bool bNeedResponse = true);
	void setIncomeDataHandler(IncomeDataHandler handler);
	uint64_t getCallId();

	void addActionHandler(const char* pName, ACTION action);

protected:
	int sendNextCommands();

protected:
	pRingBuffer		m_pRingBuffer;
	CBaseConnector* m_pConnector;
	std::list<CRequest*> m_lstRequest;
	std::mutex m_mtxRequest;
	IncomeDataHandler m_fnIncomeHandler;
	std::thread	m_maintainThread;
	//std::mutex m_mtxQuit;
	//std::condition_variable m_evQuit;
	std::atomic<bool> m_bQuit;

	uint64_t m_nCallId;

	// for handle income command
	std::map<std::string, ACTION>  m_mpActions;
};

