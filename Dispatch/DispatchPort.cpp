#include "stdafx.h"
#include "DispatchPort.h"
#include <mstcpip.h>

CDispatchPort::CDispatchPort(void)
:m_bServerIsRunning(FALSE)
{
}


CDispatchPort::~CDispatchPort(void)
{
	Close();
}

int CDispatchPort::Start(DWORD dwIpAddr, WORD port)
{
	if (m_bServerIsRunning)
	{
		return -1;
	}

	WSADATA			 wsda;					//   Structure   to   store   info
	int ret = WSAStartup(MAKEWORD(2, 2), &wsda);     //   Load   version   1.1   of   Winsock
	if (ret == SOCKET_ERROR)
	{
		//LOG(ERROR) << "WSAStartup Error";
		return FALSE;
	}

	m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_serverSocket == SOCKET_ERROR)
	{
		LOG(ERROR) << "create socket error";
		return SOCKET_ERROR;
	}

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = dwIpAddr;

	ret = ::bind(m_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (SOCKET_ERROR == ret)
	{
		closesocket(m_serverSocket);
		return ret;
	}

	ret = listen(m_serverSocket, 5);
	if (SOCKET_ERROR == ret)
	{
		closesocket(m_serverSocket);
		return ret;
	}

	m_bServerIsRunning = TRUE;
	m_hAccetpThread = CreateThread(NULL, 0, AcceptThread, this, 0, NULL);
	return 0;
}

void CDispatchPort::Close()
{
	for (auto i = m_clientConnections.begin(); i != m_clientConnections.end(); i++)
	{
		(*i).Close();
	}
	m_bServerIsRunning = FALSE;
	shutdown(m_serverSocket, 0);
	closesocket(m_serverSocket);
	//CloseHandle(m_hAccetpThread);
}



DWORD WINAPI CDispatchPort::AcceptThread(LPVOID pVoid)
{
	CDispatchPort* pThis = reinterpret_cast<CDispatchPort*>(pVoid);

	int addrLen = sizeof(SOCKADDR_IN);
	fd_set	allSockets, fdRead, fdException;
	struct timeval t;
	int rv;
	t.tv_sec = 1;
	t.tv_usec = 500;
	char buf[2048];


	FD_ZERO(&allSockets);
	FD_SET(pThis->m_serverSocket, &allSockets);

	while (pThis->m_bServerIsRunning){
		fdRead = allSockets;
		fdException = allSockets;

		rv = select(0, &fdRead, NULL, &fdException, &t);
		if (SOCKET_ERROR == rv)
		{
			//LOG(ERROR)<<"select error, error number:"<<WSAGetLastError();
			ExitThread(-1);
		}
		else if (0 == rv)
		{
			// timeout
			continue;
		}
		else if (rv > 0)
		{
			for (int i = 0; i<(int)fdRead.fd_count; i++)
			{
				if (FD_ISSET(fdRead.fd_array[i], &fdRead))
				{
					if (fdRead.fd_array[i] == pThis->m_serverSocket)
					{
						SOCKADDR_IN clientSocketAddr;
						memset(&clientSocketAddr, 0, sizeof(clientSocketAddr));
						int s = accept(pThis->m_serverSocket, (struct sockaddr*)&clientSocketAddr, &addrLen);
						if (s == SOCKET_ERROR)
						{
							//LOG(ERROR)<<"accept error:"<<WSAGetLastError();
							continue;
						}

						if (fdRead.fd_count >= FD_SETSIZE)
						{
							// fd_set is full
							//LOG(ERROR)<<"socket array full";
							closesocket(s);
							continue;
						}
						// set keep alive for client socket
						BOOL keepAlive = TRUE;
						int r = setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAlive, sizeof(keepAlive));
						if (r == SOCKET_ERROR)
						{
							//LOG(ERROR)<<"set keep alive error, code:"<<WSAGetLastError();
							continue;
						}
						struct tcp_keepalive inVal = { 1, 1000 * 10, 1000 * 5 };
						DWORD ret;
						WSAIoctl(s, SIO_KEEPALIVE_VALS, &inVal, sizeof(inVal), NULL, sizeof(int), &ret, NULL, NULL);

						// create fcs connection
						//LOG(INFO)<<"new fcs connection, client ip:"<<inet_ntoa(clientSocketAddr.sin_addr);
						pThis->AddConnection(s, clientSocketAddr);
						FD_SET(s, &allSockets);
					}
					else
					{
						// handle receive event
						int n = recv(fdRead.fd_array[i], buf, sizeof(buf), 0);
						if (SOCKET_ERROR == n)
						{
							//LOG(ERROR)<<"recv error,code:"<<WSAGetLastError();
							FD_CLR(fdRead.fd_array[i], &allSockets);
							pThis->CloseConnection(fdRead.fd_array[i]);
							continue;
						}
						if (0 == n)
						{
							// client close the connection
							FD_CLR(fdRead.fd_array[i], &allSockets);
							pThis->CloseConnection(fdRead.fd_array[i]);
							continue;
						}
						pThis->HandleClientMessage(fdRead.fd_array[i], buf, n);
					}
				}
			}

			// for exception sockets
			for (int i = 0; i<(int)fdException.fd_count; i++)
			{
				if (FD_ISSET(fdException.fd_array[i], &fdException))
				{
					FD_CLR(fdException.fd_array[i], &allSockets);
					pThis->CloseConnection(fdException.fd_array[i]);
				}
			}
		}
	}

	return 0;
}




void CDispatchPort::CloseConnection(SOCKET s)
{
	//LOG(INFO)<<"CloseConnection entry";
	// remove it from connection queue
	m_connectionsLocker.Lock();
	LOG(INFO) << "close connection";
	for (auto i = m_clientConnections.begin(); i != m_clientConnections.end(); i++)
	{
		if ((*i).m_socketClient == s)
		{
			(*i).Close();
			m_clientConnections.erase(i);
			break;
		}
	}
	m_connectionsLocker.UnLock();
	//LOG(INFO)<<"CloseConnection exit";
}
void CDispatchPort::AddConnection(SOCKET s, const SOCKADDR_IN& addr)
{
	//LOG(INFO)<<"AddConnection entry";
	m_connectionsLocker.Lock();
	m_clientConnections.push_back(CClientConnection(s, addr));
	m_connectionsLocker.UnLock();
	//LOG(INFO)<<"AddConnection exit";
}
void CDispatchPort::HandleClientMessage(SOCKET s, LPCSTR pMessage, DWORD dwSize)
{
	if (dwSize == 0)
	{
		return;
	}
	//LOG(INFO) << "CFcsServer::HandleFcsMessage Entry";
	for (auto i = m_clientConnections.begin(); i != m_clientConnections.end(); ++i)
	{
		if ((*i).m_socketClient == s)
		{
			(*i).HandleIncomingData(pMessage);
		}
	}
}
bool CDispatchPort::sendResultToClient(string jsonResult)
{

	for (auto i = m_clientConnections.begin(); i != m_clientConnections.end(); ++i)
	{
		
		int n = send(i->m_socketClient, jsonResult.c_str(), jsonResult.size(), 0);
		if (SOCKET_ERROR == n)
		{
			int err = WSAGetLastError();
			return FALSE;
		}
	}

	return true;
}