#include "stdafx.h"

#include "../include/request.h"
#include "../../threadpool/threadpool.h"
#include "../include/TcpServer.h"
#include "../include/RpcServer.h"
#include "../include/RpcJsonParser.h"

CRpcServer::CRpcServer()
	: m_pConnector(NULL)
	, m_thdPool(NULL)
{
}


CRpcServer::~CRpcServer()
{
	if (NULL != m_thdPool)
	{
		delete m_thdPool;
	}
}

int CRpcServer::onReceive(CRemotePeer* pRemote, char* pData, int dataLen)
{
	try{
		std::string str(pData, dataLen);
		printf("received data:%s", str.c_str());

		std::string callName;
		uint64_t callId = 0;
		std::string param = "";
		std::string type = "";

		std::string status, statusText, content;
		int errCode = 0;

		if (0 == CRpcJsonParser::getRequest(str, callName, callId, param, type))
		{
			// test if it's ping command, direct send response when receive ping command
			if (0 == callName.compare("ping"))
			{
				std::string pingResponse = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
				pRemote->sendResponse(pingResponse.c_str(), pingResponse.size());
			}
			else // other command, call command handler
			{
				auto actionFn = (m_mpActions.find(callName) != m_mpActions.end()) ? m_mpActions[callName] : nullptr;

				if (nullptr != actionFn)
				{
					m_thdPool->enqueue(actionFn, pRemote, param, callId, type);
				}
			}
		}
		else if (0 == CRpcJsonParser::getResponse(str, status, statusText, errCode, callId, content))
		{
			// find which client send the response
			if (m_Clients.end() != m_Clients.find(pRemote))
			{
				std::lock_guard<std::mutex> lk(m_Clients[pRemote]->m_mtxRequest);

				auto firstRequest = m_Clients[pRemote]->m_lstRequest.begin();
				if (callId == (*firstRequest)->m_nCallId)
				{
					if (nullptr != (*firstRequest)->success)
					{
						(*firstRequest)->success(str.c_str(), (*firstRequest)->data);
					}
					delete *firstRequest;
					m_Clients[pRemote]->m_lstRequest.pop_front();

					// send next command
					if (m_Clients[pRemote]->m_lstRequest.size() > 0){
						auto head = m_Clients[pRemote]->m_lstRequest.front();
						pRemote->sendCommand(head->m_strRequest.c_str(), head->m_strRequest.size());
					}
				}
			}
		}
		
	}
	catch (std::exception& e)
	{
		printf("exception: %s\r\n", e.what());
	}
	catch (...)
	{
		printf("unknow error! \r\n");
	}
	return 0;
}

int CRpcServer::start(unsigned short port, int type)
{
	m_thdPool = new ThreadPool(ThreadCountInPool);

	if (CRpcServer::TCP == type)
	{
		m_pConnector = new CTcpServer();
		m_pConnector->setReceiveDataHandler(this);
		m_pConnector->start(std::to_string(port).c_str());

		m_pConnector->setConnectEvent([&](CRemotePeer* remotePeer){
			auto itr = m_Clients.find(remotePeer);
			if (m_Clients.end() == itr)
			{
				CClient* client = new CClient();
				m_Clients[remotePeer] = client;
			}
		});
		m_pConnector->setDisconnectEvent([&](CRemotePeer* remotePeer){
			auto itr = m_Clients.find(remotePeer);
			if (m_Clients.end() != itr)
			{
				delete itr->second;
				m_Clients.erase(itr);
			}
		});
	}

	return 0;
}

void CRpcServer::stop()
{
	if (m_pConnector)
	{
		delete m_pConnector;
	}

	m_mpActions.erase(m_mpActions.begin(), m_mpActions.end());
}

void CRpcServer::addActionHandler(const char* pName, ACTION action)
{
	if (nullptr == action)
	{
		return;
	}
	m_mpActions[pName] = action;
}

int CRpcServer::sendRequest(const char* pRequest,
	uint64_t nCallId,
	void* data,
	std::function<void(const char* pResponse, void*)> success,
	std::function<void(const char* pResponse, void*)> failed,
	int nTimeoutSeconds)
{
	int ret = -1;

	if (NULL == pRequest)
	{
		return ret;
	}

	// send to all clients
	for (auto i = m_Clients.begin(); i != m_Clients.end(); i++)
	{
		std::lock_guard<std::mutex> lock(i->second->m_mtxRequest);

		CRequest* pReq = new CRequest();
		pReq->m_strRequest = pRequest;
		pReq->m_nCallId = nCallId;
		pReq->success = success;
		pReq->failed = failed;
		pReq->data = data;
		pReq->nTimeoutSeconds = nTimeoutSeconds;

		if (0 == i->second->m_lstRequest.size())
		{
			ret = i->first->sendCommand(pReq->m_strRequest.c_str(), pReq->m_strRequest.size());
		}

		i->second->m_lstRequest.push_back(pReq);
	}

	return ret;
}
