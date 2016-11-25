#include "stdafx.h"
#include "WLSocketLog.h"

WLSocketLog::WLSocketLog()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return;
	}
	m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	m_waitSendLog = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bSendLog = true;
	m_hWorkThreadId = CreateThread(NULL, 0, workThread, this, THREAD_PRIORITY_NORMAL, NULL);

	
}

WLSocketLog::~WLSocketLog()
{
	m_bSendLog = false;
	m_logs.clear();
	SetEvent(m_waitSendLog);
	WaitForSingleObject(m_hWorkThreadId, 1000);
	CloseHandle(m_hWorkThreadId);
	closesocket(m_serverSocket);
	WSACleanup();
}

void WLSocketLog::sendLog(std::string log)
{
	m_logLstLocker.lock();
	log += "\r\n";
	m_logs.push_back(log);
	m_logLstLocker.unlock();
	SetEvent(m_waitSendLog);
}

DWORD WINAPI WLSocketLog::workThread(LPVOID lpParam)
{
	WLSocketLog* p = (WLSocketLog*)lpParam;
	if (p)
	{
		p->workThreadProc();
	}
	return 0;
}

void WLSocketLog::workThreadProc()
{
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("192.168.2.106");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);
	connect(m_serverSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	while (m_bSendLog)
	{
		WaitForSingleObject(m_waitSendLog, 1000);
		while (m_logs.size() > 0)
		{
			m_logLstLocker.lock();
			std::string log = m_logs.front();
			send(m_serverSocket, log.c_str(), log.length(), 0);
			m_logs.pop_front();
			m_logLstLocker.unlock();
		}
	}
}

