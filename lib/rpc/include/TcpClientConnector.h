#pragma once
#include "BaseConnector.h"

#define			Connected			1
#define			NotConnect			0

class CTcpClientConnector :
	public CBaseConnector
{
public:
	CTcpClientConnector();
	~CTcpClientConnector();

public: // dereived from CAbstractConnector
	virtual int start();
	virtual void stop();
	virtual int send(unsigned char* pData, int dataLen);
	virtual int connect(char* connStr);

protected:
	SOCKET m_clientSocket;
	int m_nConnected;

};

