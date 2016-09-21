#pragma once

#ifndef _ABSTRACT_CONNECTOR
#define _ABSTRACT_CONNECTOR

class OnConnectorData{
public:
	virtual int onReceive(unsigned char* pData, int dataLen) = 0;
};

class CBaseConnector
{
public:
	virtual int start() = 0;
	virtual void stop() = 0;
	virtual int send(unsigned char* pData, int dataLen) = 0;
	virtual int connect(char* connStr) = 0;

	void setReceiveDataHandler(OnConnectorData* handler){
		m_hReceiveData = handler;
	}

protected:
	OnConnectorData* m_hReceiveData;
};

#endif