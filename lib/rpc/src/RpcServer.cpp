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
		if (0 != CRpcJsonParser::getRequest(str, callName, callId, param, type))
		{
			// send error response
			std::string response = CRpcJsonParser::buildResponse("failed", callId, 404, "Invalid request");
			pRemote->sendResponse(response.c_str(), response.size());
			throw std::exception("invalid request");
		}

		// test if it's ping command, direct send response when receive ping command
		if (0==callName.compare("ping"))
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

	std::lock_guard<std::mutex> lock(m_mtxRequest);

	CRequest* pReq = new CRequest();
	pReq->m_strRequest = pRequest;
	pReq->m_nCallId = nCallId;
	pReq->success = success;
	pReq->failed = failed;
	pReq->data = data;
	pReq->nTimeoutSeconds = nTimeoutSeconds;

	if (0 == m_lstRequest.size())
	{
		// send request immediately, to all clients
		//ret = this->send(pReq->m_strRequest.c_str(), pReq->m_strRequest.size());
		ret = m_pConnector->send(pReq->m_strRequest.c_str(), pReq->m_strRequest.size());
	}
	m_lstRequest.push_back(pReq);

	return ret;
}