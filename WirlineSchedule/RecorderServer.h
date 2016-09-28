#pragma once
#undef ERROR

#include <Windows.h>
#include <tchar.h>
#include <deque>
#include <vector>
#include <string>
//#include "strutil.h"
#include "mutex.h"





class CRecorderConnection
{
public:
	CRecorderConnection(SOCKET s, const SOCKADDR_IN& addr)
		:m_socketFcsClient(s)
	{
		memcpy(&m_addr, &addr, sizeof(addr));
	}

	CRecorderConnection& operator = (const CRecorderConnection& c){
		m_socketFcsClient = c.m_socketFcsClient;
		memcpy(&m_addr, &c.m_addr, sizeof(m_addr));
		return *this;
	}

	void Close()
	{
		closesocket(m_socketFcsClient);
	}
public:
	SOCKET						m_socketFcsClient;
	SOCKADDR_IN					m_addr;


};


class CRecorderServer
{
public:
	CRecorderServer(void);
	~CRecorderServer(void);

private:
	SOCKET						m_serverSocket;
	BOOL						m_bServerIsRunning;


	// work thread
	HANDLE							m_hAccetpThread;
	XyLib::CMutex					m_connectionsLocker;
	std::deque<CRecorderConnection>		m_fcsConnections;

public:
	static DWORD WINAPI AcceptThread(LPVOID pVoid);

public:
	int Start();
	void Close();
	void CloseConnection(SOCKET s);
	void AddConnection(SOCKET s, const SOCKADDR_IN& addr);
	bool SendSignal();
};

