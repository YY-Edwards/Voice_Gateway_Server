#include "stdafx.h"
#include "../../utf8/utf8.h"
#include "../include/TcpClientConnector.h"
#include "../include/RpcClient.h"
#include "../include/RpcJsonParser.h"


CRpcClient::CRpcClient()
	: m_pConnector(NULL)
{
}


CRpcClient::~CRpcClient()
{
	stop();
}


int CRpcClient::start(const char* connStr)
{
	m_pConnector = new CTcpClientConnector();
	m_pConnector->setReceiveDataHandler(this);
	m_pConnector->start(connStr);
	return 0;
}

void CRpcClient::stop()
{
	if (m_pConnector)
	{
		m_pConnector->stop();
		delete m_pConnector;
	}
	m_pConnector = NULL;
}

int CRpcClient::onReceive(CRemotePeer* pRemote, char* pData, int dataLen)
{
	try{
		std::string str(pData, dataLen);

		std::map<std::string, std::string> args;
		CRpcJsonParser parser;
		std::string status, statusText;
		int errCode = 0;
		uint64_t callId = 0;
		std::string content;
		bool isResponse = false;
		if (0 == parser.getResponse(str, status, statusText, errCode, callId, content))
		{
			// it's valid response
			std::lock_guard<std::mutex> lock(m_mtxRequest);
			// remove the front request in list, if the call id is match
			for (auto itr = m_lstRequest.begin(); itr != m_lstRequest.end(); ++itr){
				if (callId == (*itr)->m_nCallId)
				{
					isResponse = true;
					// handle response
					(*itr)->success(str.c_str(), (*itr)->data);

					delete *itr;
					m_lstRequest.remove(*itr);
					break;
				}
			}

		}

		if (!isResponse)
		{
			if (nullptr != m_fnIncomeHandler)
			{
				m_fnIncomeHandler(m_pConnector, pData, dataLen);
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

void CRpcClient::setIncomeDataHandler(IncomeDataHandler handler)
{
	m_fnIncomeHandler = handler;
}


int CRpcClient::send(const char* pData, int dataLen)
{
	if (NULL == m_pConnector)
	{
		return -1;
	}
	return m_pConnector->send(pData, dataLen);
}

int CRpcClient::sendRequest(const char* pRequest,
						uint64_t nCallId,
						void* data,
						std::function<void(const char* pResponse, void*)> success,
						std::function<void(const char* pResponse, void*)> failed)
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

	if (0 == m_lstRequest.size())
	{
		// send request immediately
		ret = this->send(pReq->m_strRequest.c_str(), pReq->m_strRequest.size());
	}
	m_lstRequest.push_back(pReq);

	return ret;
}
