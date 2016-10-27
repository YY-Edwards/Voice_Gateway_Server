#include "stdafx.h"
#include "../../strutil/strutil.h"
#include "../include/TcpClientConnector.h"

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

CTcpClientConnector::CTcpClientConnector()
	: m_nConnected(NotConnect)
	, m_clientSocket(INVALID_SOCKET)
	, m_nClientRunning(ClientNotRunning)
{
}


CTcpClientConnector::~CTcpClientConnector()
{
	stop();
}

int CTcpClientConnector::start(const char* connStr)
{
	if (Connected == m_nConnected)
	{
		return -1;
	}

	m_strConnStr = connStr;

#ifdef _WIN32
	WSADATA			 wsda;					//   Structure   to   store   info
	int ret = WSAStartup(MAKEWORD(2, 2), &wsda);     //   Load   version   1.1   of   Winsock
	if (ret == SOCKET_ERROR)
	{
		//LOG(ERROR) << "WSAStartup Error";
		return FALSE;
	}
#endif
	// start net monitor thread
	m_nClientRunning = ClientRunning;
	m_recvThread = CreateThread(NULL, 0, NetThread, this, 0, NULL);

	return 0;
}

void CTcpClientConnector::stop()
{
	if (ClientRunning == m_nClientRunning)
	{
		m_nClientRunning = ClientNotRunning;

		if (INVALID_SOCKET != m_clientSocket)
		{
			WaitForSingleObject(m_recvThread, 600);
			closesocket(m_clientSocket);
			m_clientSocket = INVALID_SOCKET;
		}

		m_nConnected = NotConnect;
	}
}

int CTcpClientConnector::send(const char* pData, int dataLen)
{
	if (ClientNotRunning == m_nClientRunning)
	{
		return -1;
	}
	return ::send(m_clientSocket, (char*)pData, dataLen, 0);
}

DWORD WINAPI CTcpClientConnector::NetThread(LPVOID pVoid)
{
	CTcpClientConnector* pThis = reinterpret_cast<CTcpClientConnector*>(pVoid);
	if (pThis)
	{
		return pThis->netHandler();
	}

	return 1;
}

DWORD CTcpClientConnector::netHandler()
{
	while (ClientRunning == m_nClientRunning)
	{
		if (Connected == m_nConnected)
		{
			int addrLen = sizeof(SOCKADDR_IN);
			fd_set	fdRead, fdException;
			struct timeval t;
			int rv;
			t.tv_sec = 1;
			t.tv_usec = 500;
			char buf[2048];

			FD_ZERO(&fdRead);
			FD_ZERO(&fdException);
			FD_SET(m_clientSocket, &fdRead);
			FD_SET(m_clientSocket, &fdException);

			rv = select(0, &fdRead, NULL, &fdException, &t);
			if (SOCKET_ERROR == rv)
			{
				//LOG(ERROR)<<"select error, error number:"<<WSAGetLastError();
				return -1;
			}
			else if (0 == rv)
			{
				// timeout
				continue;
			}
			else if (rv > 0)
			{
				for (int i = 0; i < (int)fdRead.fd_count; i++)
				{
					try{
						if (FD_ISSET(fdRead.fd_array[i], &fdRead))
						{
							// handle receive event
							ZeroMemory(buf, sizeof(buf));
							int n = recv(fdRead.fd_array[i], buf, sizeof(buf), 0);
							if (SOCKET_ERROR == n || 0 == n)
							{
								//LOG(ERROR)<<"recv error,code:"<<WSAGetLastError();
								throw std::exception("socket error or remote disconnect£¡");
							}
							if (m_hReceiveData)
							{
								m_hReceiveData->onReceive(NULL, buf, n);
							}
						}

						// for exception sockets
						for (int i = 0; i < (int)fdException.fd_count; i++)
						{
							if (FD_ISSET(fdException.fd_array[i], &fdException))
							{
								throw std::exception("socket exception£¡");
							}
						}
					}
					catch (std::exception e)
					{
						closesocket(m_clientSocket);
						Sleep(2000);					// wait 2 seconds
						m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						m_nConnected = NotConnect;
					}
				}
			}
		}
		else {
			// connect or re-connect server
			Sleep(1000 * 10);
			connect(m_strConnStr.c_str());
		}
	}
	return 0;
}


/*
* Connect string
* tcp://127.0.0.1:8000
*/
int CTcpClientConnector::connect(const char* connStr)
{
	if (Connected == m_nConnected)
	{
		return 0;
	}

	int ret = 0;
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	try{
		std::vector<std::string> connVector = strutil::split(connStr, "://");
		if (0 != connVector[0].compare("tcp"))
		{
			throw std::exception("protocol not support, only support tcp");
		}
		
		std::string host = connVector[1];
		std::string port = connVector[2];
		
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		int iResult = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
		if (iResult != 0) {
			throw std::exception("get host address failed");
		}

		m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_clientSocket == SOCKET_ERROR)
		{
			throw std::exception("create client socket failed");
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
			// Connect to server.
			iResult = ::connect(m_clientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(m_clientSocket);
				m_clientSocket = INVALID_SOCKET;
				continue;
			}

			m_nConnected = Connected;
			break;
		}
	}
	catch (std::exception& e){
		ret = -1;
	}
	catch (...){
		ret = -1;
	}

	freeaddrinfo(result);
	return ret;
}

bool CTcpClientConnector::isConnected()
{
	return (Connected == m_nConnected);
}