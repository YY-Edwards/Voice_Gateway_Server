#pragma once

#ifndef _ABSTRACT_CONNECTOR
#define _ABSTRACT_CONNECTOR

class CRemotePeer{
public:
	virtual int sendResponse(const char* pData, int dataLen) = 0;
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

protected:
	OnConnectorData* m_hReceiveData;
};

#endif