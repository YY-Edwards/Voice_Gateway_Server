#pragma once
#include "BaseConnector.h"

#define			Connected			1
#define			NotConnect			0

#define		ClientRunning		1
#define		ClientNotRunning	0

class CTcpClientConnector :
	public CBaseConnector
{
public:
	CTcpClientConnector();
	~CTcpClientConnector();

public: // dereived from CAbstractConnector
	virtual int start(const char* connStr);
	virtual void stop();
	virtual int send(unsigned char* pData, int dataLen);
	virtual int connect(const char* connStr);

protected:
	static DWORD WINAPI NetThread(LPVOID pVoid);
	DWORD netHandler();

protected:
	SOCKET m_clientSocket;
	int m_nConnected;
	HANDLE m_recvThread;
	int m_nClientRunning;
	std::string m_strConnStr;
};

