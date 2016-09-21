#include "stdafx.h"
#include "../../strutil/strutil.h"
#include "../include/TcpClientConnector.h"

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

CTcpClientConnector::CTcpClientConnector()
	: m_nConnected(NotConnect)
	, m_clientSocket(INVALID_SOCKET)
{
}


CTcpClientConnector::~CTcpClientConnector()
{
}

int CTcpClientConnector::start()
{
	if (Connected == m_nConnected)
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
	m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_clientSocket == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	return 0;
}

void CTcpClientConnector::stop()
{
	if (Connected)
	{
		if (INVALID_SOCKET != m_clientSocket)
		{
			closesocket(m_clientSocket);
			m_clientSocket = INVALID_SOCKET;
			WSACleanup();
		}

		m_nConnected = NotConnect;
	}
}

int CTcpClientConnector::send(unsigned char* pData, int dataLen)
{
	return 0;
}

/*
* Connect string
* tcp://127.0.0.1:8000
*/
int CTcpClientConnector::connect(char* connStr)
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
	catch (std::exception e){
		ret = -1;
	}
	catch (...){
		ret = -1;
	}

	freeaddrinfo(result);
	return ret;
}

