#pragma once

#ifndef _TCP_SERVER_
#define _TCP_SERVER_

#ifdef _WIN32

#define _WINSOCKAPI_
#endif

#include <map>
#include <mutex>

#ifndef SOCKET_ERROR
#define SOCKET_ERROR            (-1)
#endif

#include "BaseConnector.h"

#define		ServerRunning		1
#define		ServerNotRunning	0

#define		ServerPORT			8000
#define		MaxClientSupport	10

class TcpClient : public CRemotePeer{
public:
	SOCKET		s;
	SOCKADDR_IN addr;
public:
	int sendResponse(const char* pData, int dataLen)
	{
		return ::send(s, pData, dataLen, 0);
	}
};

class CTcpServer :
	public CBaseConnector
{
public:
	CTcpServer();
	~CTcpServer();

public: // dereived from CAbstractConnector
	virtual int start(const char* connStr = NULL);
	virtual void stop();
	virtual int send(const char* pData, int dataLen);
	virtual int connect(const char* connStr);

protected:
	static DWORD WINAPI AcceptThread(LPVOID pVoid);
	DWORD connectHandler();
	void addClient(SOCKET s, SOCKADDR_IN addr);
	void removeClient(SOCKET s);

protected:
	int								m_nServerIsRunning;
	SOCKET							m_serverSocket;
	HANDLE							m_hAccetpThread;
	std::map<SOCKET, TcpClient*>	m_mpClients;
	std::mutex						m_clientLocker;
};

#endif
