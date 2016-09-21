#pragma once

#ifndef _ABSTRACT_CONNECTOR
#define _ABSTRACT_CONNECTOR

class CRemotePeer{
public:
	virtual int sendResponse(char* pData, int dataLen) = 0;
};

class OnConnectorData{
public:
	virtual int onReceive(CRemotePeer* pRemote, char* pData, int dataLen) = 0;
};

class CBaseConnector
{
public:
	virtual int start(const char* connStr = NULL) = 0;
	virtual void stop() = 0;
	virtual int send(unsigned char* pData, int dataLen) = 0;
	virtual int connect(const char* connStr) = 0;

	void setReceiveDataHandler(OnConnectorData* handler){
		m_hReceiveData = handler;
	}

protected:
	OnConnectorData* m_hReceiveData;
};

#endif