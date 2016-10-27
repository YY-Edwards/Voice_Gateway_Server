#include "stdafx.h"

#include <mstcpip.h>
#include "../include/TcpServer.h"

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

CTcpServer::CTcpServer()
	:m_nServerIsRunning(ServerNotRunning)
{
}


CTcpServer::~CTcpServer()
{
}

int CTcpServer::start(const char* connStr)
{
	if (ServerRunning == m_nServerIsRunning)
	{
		return -1;
	}

#ifdef _WIN32
	WSADATA			 wsda;					//   Structure   to   store   info
	int ret = WSAStartup(MAKEWORD(2, 2), &wsda);     //   Load   version   1.1   of   Winsock
	if (ret == SOCKET_ERROR)
	{
		//LOG(ERROR) << "WSAStartup Error";
		return FALSE;
	}
#endif
	m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_serverSocket == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	u_short port = std::atoi(connStr);

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

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

	m_nServerIsRunning = ServerRunning;
	m_hAccetpThread = CreateThread(NULL,0, AcceptThread, this, 0, NULL);

	return 0;
}

int CTcpServer::send(const char* pData, int dataLen)
{
	//return ::send(m_serverSocket, (char*)pData, dataLen, 0);

	if (m_mpClients.size() > 0)
	{
		for (auto client = m_mpClients.begin(); client != m_mpClients.end(); ++client)
		{
			(*client).second->sendCommand(pData, dataLen);
		}
	}

	return -1;
}

int CTcpServer::connect(const char* connStr)
{
	return 0;
}

DWORD WINAPI CTcpServer::AcceptThread(LPVOID pVoid)
{
	CTcpServer* pThis = reinterpret_cast<CTcpServer*>(pVoid);
	if (pThis)
	{
		return pThis->connectHandler();
	}

	return 1;
}

DWORD CTcpServer::connectHandler()
{
	int addrLen = sizeof(SOCKADDR_IN);
	fd_set	allSockets, fdRead, fdException;
	struct timeval t;
	int rv;
	t.tv_sec = 1;
	t.tv_usec = 500;
	char buf[2048];


	FD_ZERO(&allSockets);
	FD_SET(m_serverSocket, &allSockets);
	while (ServerRunning == m_nServerIsRunning){
		FD_ZERO(&fdRead);
		FD_ZERO(&fdException);
		fdRead = allSockets;
		fdException = allSockets;

		rv = select(0, &fdRead, NULL,&fdException, &t);
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
					if (fdRead.fd_array[i] == m_serverSocket)
					{
						SOCKADDR_IN clientSocketAddr;
						memset(&clientSocketAddr, 0, sizeof(clientSocketAddr));
						int s = accept(m_serverSocket, (struct sockaddr*)&clientSocketAddr, &addrLen);
						if (s == SOCKET_ERROR)
						{
							//LOG(ERROR)<<"accept error:"<<WSAGetLastError();
							continue;
						}

						if (fdRead.fd_count >= FD_SETSIZE)
						{
							// fd_set is full
							//LOG(ERROR) << "socket array full";
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
						WSAIoctl(s, SIO_KEEPALIVE_VALS, &inVal, sizeof(inVal), NULL,sizeof(int), &ret, NULL,NULL);
						
						addClient(s, clientSocketAddr);
						FD_SET(s, &allSockets);
					}
					else
					{
						// handle receive event
						ZeroMemory(buf, sizeof(buf));
						int n = recv(fdRead.fd_array[i], buf, sizeof(buf), 0);
						if (SOCKET_ERROR == n)
						{
							//LOG(ERROR)<<"recv error,code:"<<WSAGetLastError();
							FD_CLR(fdRead.fd_array[i], &allSockets);
							removeClient(fdRead.fd_array[i]);
							continue;
						}
						if (0 == n)
						{
							// client close the connection
							FD_CLR(fdRead.fd_array[i], &allSockets);
							removeClient(fdRead.fd_array[i]);
							continue;
						}

						if (m_hReceiveData)
						{
							m_hReceiveData->onReceive(m_mpClients[fdRead.fd_array[i]], buf, n);
						}
					}
				}
			}

			// for exception sockets
			for (int i = 0; i<(int)fdException.fd_count; i++)
			{
				if (FD_ISSET(fdException.fd_array[i], &fdException))
				{
					FD_CLR(fdException.fd_array[i], &allSockets);
					removeClient(fdException.fd_array[i]);
				}
			}
		}
	}
	return 0;
}

void CTcpServer::addClient(SOCKET s, SOCKADDR_IN addr)
{
	printf("new client connected\r\n");
	std::lock_guard<std::mutex> locker(m_clientLocker);
	try{
		if (m_mpClients.size() >= MaxClientSupport)
		{
			closesocket(s);
		} 
		else 
		{
			TcpClient* client = new TcpClient();
			client->s = s;
			memcpy(&client->addr, &addr, sizeof(SOCKADDR_IN));
			m_mpClients[s] = client;
		}
	}
	catch(...){
	}
}

void CTcpServer::removeClient(SOCKET s)
{
	printf("client closed\r\n");
	std::lock_guard<std::mutex> locker(m_clientLocker);
	try{
		for (std::map<SOCKET, TcpClient*>::iterator i = m_mpClients.begin(); i != m_mpClients.end(); i++)
		{
			if (s == i->first)
			{
				closesocket(i->second->s);
				delete (i->second);

				m_mpClients.erase(i);
				break;
			}
		}

	}
	catch(...){
	}
}

void CTcpServer::stop()
{

}

bool CTcpServer::isConnected()
{
	return (m_mpClients.size() > 0);
}