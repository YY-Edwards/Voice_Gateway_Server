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
	m_pRingBuffer = createRingBuffer(1024 * 100, 1);		// 100k buffer
}


CRpcClient::~CRpcClient()
{
	assert(NULL != m_pRingBuffer);
	if (NULL != m_pRingBuffer)
	{
		freeRingBuffer(m_pRingBuffer);
	}
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
	m_pConnector->setConnectEvent([&](CRemotePeer*){
		if (m_lstRequest.size() > 0)
		{
			auto first = m_lstRequest.begin();
			send((*first)->m_strRequest.c_str(), (*first)->m_strRequest.size());
		}
	});
	m_pConnector->start(connStr);

	// start maintain thread
	m_maintainThread = std::thread([&](){
		int nSecondCount = 0;
		while (!m_bQuit)
		{
			Sleep(1000);
			// wait quit event and timeout
			//std::unique_lock<std::mutex> lk(m_mtxQuit);
			//if (std::cv_status::timeout ==  m_evQuit.wait_for(lk, std::chrono::seconds(1)))
			{
				if (NULL == m_pConnector)
				{
					// TODO: temporary for avoid error, when stop the client
					// it should redesign the stop logic
					break;
				}
				if (!m_pConnector->isConnected())
				{
					// disconnected status, stop timeout check and don't send ping command
					continue;
				}

				if (++nSecondCount > PingTime){
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
						(*first)->nTimeoutSeconds--;
						if ((*first)->nTimeoutSeconds < 0)
						{
							// command timeout
							if (nullptr != (*first)->failed)
							{
								(*first)->failed(NULL, NULL);
							}
							delete (*first);
							m_lstRequest.pop_front();

							// send next command
							sendNextCommands();
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
	if (!m_bQuit)
	{
		m_bQuit = true;
		m_maintainThread.join();
		if (m_pConnector)
		{
			m_pConnector->stop();
			delete m_pConnector;
		}
		m_pConnector = NULL;
	}
}

bool CRpcClient::isConnected()
{
	if (m_pConnector)
	{
		return m_pConnector->isConnected();
	}
	return false;
}

int CRpcClient::onReceive(CRemotePeer* pRemote, char* pData, int dataLen)
{
	char buffer[102400];
	try{
		for (int i = 0; i < dataLen; i++)
		{
			push(m_pRingBuffer, &pData[i]);
		}
		int len = 0;
		while (getRingBufferSize(m_pRingBuffer) > 0)
		{
			memset(buffer, 0, sizeof(buffer));
			len = dumpBuffer(m_pRingBuffer, buffer);


			std::string str;// (pData, dataLen);
			int idx = JsonUtil::getJsonStr(buffer, len, str);
			if (idx <= 0)
			{
				// no valid json
				break;
			}
			// pop string from ring buffer
			for (int i = 0; i <= idx; i++)
			{
				char ch;
				pop(m_pRingBuffer, &ch);
			}

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

						// send next command
						sendNextCommands();
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
		} // end while (getRingBufferSize(m_pRingBuffer) > 0)

	}
	catch (std::exception& e)
	{
		printf("exception: %s\r\n", e.what());
	}
	catch (...)
	{
		printf("unknown error! \r\n");
	}
	return 0;
}

int CRpcClient::sendNextCommands()
{
	for (auto j = m_lstRequest.begin(); j != m_lstRequest.end();)
	{
		if ((*j)->m_bNeedResponse)
		{
			this->send((*j)->m_strRequest.c_str(), (*j)->m_strRequest.size());
			break;
		}
		else
		{
			this->send((*j)->m_strRequest.c_str(), (*j)->m_strRequest.size());
			delete *j;
			j = m_lstRequest.erase(j);
		}
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
						int nTimeoutSeconds,
						bool bNeedResponse)
{
	int ret = 0;

	if (NULL == pRequest || !m_pConnector->isConnected())
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
	pReq->m_bNeedResponse = bNeedResponse;

	if (0 == m_lstRequest.size())
	{
		// send request immediately
		ret = this->send(pReq->m_strRequest.c_str(), pReq->m_strRequest.size());
		if (bNeedResponse)
		{
			m_lstRequest.push_back(pReq);
		}
	}
	else
	{
		m_lstRequest.push_back(pReq);
	}
	

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