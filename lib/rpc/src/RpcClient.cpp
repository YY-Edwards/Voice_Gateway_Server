#include "stdafx.h"

#include <chrono>

#include "../../utf8/utf8.h"
#include "../include/TcpClientConnector.h"
#include "../include/RpcClient.h"
#include "../include/RpcJsonParser.h"


CRpcClient::CRpcClient()
	: m_pConnector(NULL)
	, m_bQuit(false)
	, m_nCallId(0)
{
}


CRpcClient::~CRpcClient()
{
	stop();
}

uint64_t CRpcClient::getCallId()
{
	return (++m_nCallId);
}

int CRpcClient::start(const char* connStr)
{
	m_pConnector = new CTcpClientConnector();
	m_pConnector->setReceiveDataHandler(this);
	m_pConnector->start(connStr);

	// start maintain thread
	m_maintainThread = std::thread([&](){
		int nSecondCount = 0;
		while (!m_bQuit)
		{
			// wait quit event and timeout
			std::unique_lock<std::mutex> lk(m_mtxQuit);
			if (std::cv_status::timeout ==  m_evQuit.wait_for(lk, std::chrono::seconds(1)))
			{
				if (!m_pConnector->isConnected())
				{
					// disconnected status, stop timeout check and don't send ping command
					continue;
				}

				if (++nSecondCount > 30){
					nSecondCount = 0;
					// send ping command
					uint64_t callId = getCallId();
					std::string pingRequest = CRpcJsonParser::buildCall("ping", callId, ArgumentType());
					sendRequest(pingRequest.c_str(), callId, NULL, nullptr, nullptr);
				}

				// check timeout in command queue
				{
					std::lock_guard<std::mutex> lock(m_mtxRequest);
					if (m_lstRequest.size() > 0)
					{
						auto first = m_lstRequest.begin();
						if ((*first)->nTimeoutSeconds < 0)
						{
							// command timeout
							if (nullptr != (*first)->failed)
							{
								(*first)->failed(NULL, NULL);
							}
							delete (*first);
							m_lstRequest.pop_front();

							// TODO: send next command
							if (m_lstRequest.size() > 0)
							{
								auto head = m_lstRequest.begin();
								this->send((*head)->m_strRequest.c_str(), (*head)->m_strRequest.size());
							}
						}
					}
				}
			}
		}
	});

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
		bool bHandled = false;

		std::string callName, arguments, type;   // for handle request

		if (0 == parser.getResponse(str, status, statusText, errCode, callId, content))
		{
			// it's valid response
			std::lock_guard<std::mutex> lock(m_mtxRequest);
			// remove the front request in list, if the call id is match
			for (auto itr = m_lstRequest.begin(); itr != m_lstRequest.end(); ++itr){
				if (callId == (*itr)->m_nCallId)
				{
					bHandled = true;
					// handle response
					if (nullptr != (*itr)->success)
					{
						(*itr)->success(str.c_str(), (*itr)->data);
					}

					delete *itr;
					m_lstRequest.erase(itr);
					break;
				}
			}

		}
		else if (0 == parser.getRequest(str, callName, callId, arguments, type))
		{
			auto actionFn = (m_mpActions.find(callName) != m_mpActions.end()) ? m_mpActions[callName] : nullptr;
			if (nullptr != actionFn)
			{
				bHandled = true;
				actionFn(pRemote, arguments, callId, type);
				//m_thdPool->enqueue(actionFn, pRemote, param, callId, type);
			}
		}

		if (!bHandled)
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
		// send request immediately
		ret = this->send(pReq->m_strRequest.c_str(), pReq->m_strRequest.size());
	}
	m_lstRequest.push_back(pReq);

	return ret;
}


void CRpcClient::addActionHandler(const char* pName, ACTION action)
{
	if (nullptr == action)
	{
		return;
	}
	m_mpActions[pName] = action;
}