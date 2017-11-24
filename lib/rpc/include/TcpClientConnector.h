#pragma once
#include "BaseConnector.h"

#define			Connected			1
#define			NotConnect			0

#define		ClientRunning		1
#define		ClientNotRunning	0

class CRemoteServer : public CRemotePeer
{
public:
	SOCKET		s;
public:
	int sendResponse(const char* pData, int dataLen)
	{
		printf("server side send data£º%s\r\n", pData);

		return ::send(s, pData, dataLen, 0);
	}

	int sendCommand(const char* pData, int dataLen)
	{
		printf("server side send data£º%s\r\n", pData);

		return ::send(s, pData, dataLen, 0);
	}
};

class CTcpClientConnector :
	public CBaseConnector
{
public:
	CTcpClientConnector();
	~CTcpClientConnector();

public: // dereived from CAbstractConnector
	virtual int start(const char* connStr);
	virtual void stop();
	virtual int send(const char* pData, int dataLen);
	virtual int connect(const char* connStr);
	virtual bool isConnected();

protected:
	static DWORD WINAPI NetThread(LPVOID pVoid);
	DWORD netHandler();

protected:
	SOCKET m_clientSocket;
	int m_nConnected;
	HANDLE m_recvThread;
	int m_nClientRunning;
	std::string m_strConnStr;
	bool m_bQuit;

	CRemotePeer* m_pRemoteServer;
};

