#pragma once

#include <map>
#include <string>
#include "BaseConnector.h"
#include "request.h"

#include <mutex>
#include <list>

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
	int send(const char* pData, int dataLen);
	int sendRequest(const char* pRequest, 
				uint64_t nCallId, 
				std::function<void (const char* pResponse)> success, 
				std::function<void(const char* pResponse)> failed = nullptr);
	void setIncomeDataHandler(IncomeDataHandler handler);

protected:
	CBaseConnector* m_pConnector;
	std::list<CRequest*> m_lstRequest;
	std::mutex m_mtxRequest;
	IncomeDataHandler m_fnIncomeHandler;
};

