#pragma once

#ifndef _ABSTRACT_CONNECTOR
#define _ABSTRACT_CONNECTOR

#include <string>
#include <functional>

class CRemotePeer{
public:
	virtual int sendResponse(const char* pData, int dataLen) = 0;
	virtual int sendCommand(const char* pData, int dataLen) = 0;
};

class OnConnectorData{
public:
	virtual int onReceive(CRemotePeer* pRemote, char* pData, int dataLen) = 0;
};

class CBaseConnector
{
public:
	virtual ~CBaseConnector(){}
	virtual int start(const char* connStr = NULL) = 0;
	virtual void stop() = 0;
	virtual int send(const char* pData, int dataLen) = 0;
	virtual int connect(const char* connStr) = 0;

	void setReceiveDataHandler(OnConnectorData* handler){
		m_hReceiveData = handler;
	}

	void setConnectEvent(std::function<void(CRemotePeer*)> fnEvent)
	{
		m_fnConnectEvent = fnEvent;
	}

	void setDisconnectEvent(std::function<void(CRemotePeer*)> fnEvent)
	{
		m_fnDisconnectEvent = fnEvent;
	}

	virtual bool isConnected() = 0;

protected:
	OnConnectorData* m_hReceiveData;
	std::function<void(CRemotePeer*)> m_fnDisconnectEvent;
	std::function<void(CRemotePeer*)> m_fnConnectEvent;
};

#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

#ifndef ACTION
typedef std::function<void(CRemotePeer*, const std::string&, uint64_t, const std::string&)> ACTION;
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR            (-1)
#endif

#endif