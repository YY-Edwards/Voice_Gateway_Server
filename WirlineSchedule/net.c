#ifdef _WIN32
#include <stdbool.h>
#else		// _WIN32
#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#endif // Linux

#include "net.h"
#include "linklist.h"

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif // !SOCKET_ERROR

#ifndef NULL
#define NULL ((void*)0)
#endif // !NULL

/* Local variables */
int NET_MODE = WithSelect;
static pLinkList	f_pListNetClient = NULL;			// client connections when as server
static pLinkList	f_pListClientConnectons = NULL;		// client connections when as client
static int			f_bServerState = SERVER_NOT_RUN;	// server state
static bool			f_bQuitFlag = false;
#ifdef _WIN32
static HANDLE		f_hAcceptThreadId = INVALID_HANDLE_VALUE;
#else // _WIN32
static pthread_t	f_hAcceptThreadId = 0;
#endif 

/* Prototypes */
static int acceptEventWorkerWithSelect(pXQTTNet pNet);
static int acceptEventWorkerWithSelectUdp(pXQTTNet pNet);
static bool matchSocket(const void* pValue, const void* pCondition);

/*
* Match two XQTT object, with it's socket
*
* @param pValue void*, XQTT object pointer
* @param pCondition void*, socket handler pointer
* @return true matched, false not match
*/
static bool matchSocket(const void* pValue, const void* pCondition)
{
	pXQTTNet p = (pXQTTNet)pValue;

#ifdef _WIN32
	return (*((SOCKET *)pCondition) == p->_netHandler);
#else // _WIN32
	return (*((int *)pCondition) == p->_netHandler);
#endif // Linux

}

/*
* Thread for handle accept event
*/
#ifdef _WIN32
DWORD WINAPI acceptEventHandleThread(LPVOID pData)
#else // _WIN32
void* acceptEventHandleThread(void* p)
#endif 
{
	pXQTTNet pNet = (pXQTTNet)pData;
	switch (NET_MODE)
	{
	case WithSelect:
		acceptEventWorkerWithSelect(pNet);
		break;
	case WithSelectUdp:
		acceptEventWorkerWithSelectUdp(pNet);
		break;
	default:
		break;
	}

	return 0;
}

/*
* accept worker function
*
* @param pNet pXQTTNet, network object pointer for server
* @return worker exit code
*/
static int acceptEventWorkerWithSelect(pXQTTNet pNet)
{
	fd_set	fdRead, fdWrite, fdException;
	struct timeval t;
	int addrLen = sizeof(SOCKADDR_IN);
	u_int maxfd = 0;
	t.tv_sec = 0;
	t.tv_usec = 500;

	while (!f_bQuitFlag)
	{
		FD_ZERO(&fdRead);
		FD_ZERO(&fdException);
		FD_ZERO(&fdWrite);

		maxfd = pNet->_netHandler;
		FD_SET(pNet->_netHandler, &fdRead);
		FD_SET(pNet->_netHandler, &fdWrite);
		FD_SET(pNet->_netHandler, &fdException);

		pLinkList p = f_pListNetClient;
		while (NULL != p)
		{
			FD_SET(((pXQTTNet)p->data)->_netHandler, &fdRead);
			FD_SET(((pXQTTNet)p->data)->_netHandler, &fdWrite);
			FD_SET(((pXQTTNet)p->data)->_netHandler, &fdException);

			if (((pXQTTNet)p->data)->_netHandler > maxfd)
			{
				maxfd = ((pXQTTNet)p->data)->_netHandler;
			}

			p = p->pNext;
		}

#ifdef _WIN32
		int ret = select(0, &fdRead, &fdWrite, &fdException, &t);
#else
		int ret = select(maxfd + 1, &fdRead, &fdWrite, &fdException, &t);
#endif // _WIN32
		if (SOCKET_ERROR == ret)
		{
			goto err;
		}

		if (0 == ret)
		{
			continue;		// timeout
		}

		if (ret > 0)
		{
			// check read events
			for (int i = 0; i < (int)fdRead.fd_count; i++)
			{
				if (FD_ISSET(fdRead.fd_array[i], &fdRead))
				{
					if (fdRead.fd_array[i] == pNet->_netHandler)
					{
						// it's accept event
						SOCKADDR_IN clientSocketAddr;
						memset(&clientSocketAddr, 0, sizeof(clientSocketAddr));
						int s = accept(pNet->_netHandler, (struct sockaddr*)&clientSocketAddr, &addrLen);
						if (SOCKET_ERROR == s)
						{
							// error, ignore this connect request 
							continue;
						}
						pXQTTNet pNetClient = (pXQTTNet)malloc(sizeof(XQTTNet));
						memset(pNetClient, 0, sizeof(XQTTNet));
						pNetClient->_netHandler = s;
						memcpy(&pNetClient->_addr, &clientSocketAddr, sizeof(addrLen));
						SafeCall(pNet->_netOnAccept, pNet, pNetClient);

						appendData(&f_pListNetClient, pNetClient);
					}
					else {
						// data income from client event
						char recvBuf[MaxRecvBuffLen];
						memset(recvBuf, 0, MaxRecvBuffLen);
						int recvLen = recv(fdRead.fd_array[i], recvBuf, MaxRecvBuffLen, 0);

						pLinkItem pItem = findItem(f_pListNetClient, &fdRead.fd_array[i], matchSocket);
						if (NULL == pItem || NULL == pItem->data)
						{
							// no client for handle this income data
							continue;
						}
						pXQTTNet pClientNet = (pXQTTNet)pItem->data;

						if (SOCKET_ERROR == recvLen)
						{
							SafeCall(pClientNet->_netOnError, pNet, pClientNet, getNetError());
							closeAndDestoryClientNet(pClientNet);
						}
						else if (0 == recvLen) {
							// remote close connection
							SafeCall(pClientNet->_netOnDisconn, pNet, pClientNet, getNetError());
							closeAndDestoryClientNet(pClientNet);
						}
						else{
							SafeCall(pClientNet->_netOnRecv, pNet, pClientNet, recvBuf, recvLen);
						}
					}
				}
			}

			// check write event
			for (int i = 0; i < (int)fdWrite.fd_count; i++)
			{
				if (fdWrite.fd_array[i] == pNet->_netHandler)
				{
					pLinkItem pItem = findItem(f_pListNetClient, &fdRead.fd_array[i], matchSocket);
					if (NULL == pItem || NULL == pItem->data)
					{
						// no client for handle this income data
						continue;
					}
					pXQTTNet pClientNet = (pXQTTNet)pItem->data;

					SafeCall(pClientNet->_netOnSendComplete, pNet, pClientNet);
				}
			}

			// check exception event
			for (int i = 0; i < (int)fdException.fd_count; i++)
			{
				if (fdException.fd_array[i] == pNet->_netHandler)
				{
					pLinkItem pItem = findItem(f_pListNetClient, &fdRead.fd_array[i], matchSocket);
					if (NULL == pItem || NULL == pItem->data)
					{
						// no client for handle this income data
						continue;
					}
					pXQTTNet pClientNet = (pXQTTNet)pItem->data;

					SafeCall(pClientNet->_netOnError, pNet, pClientNet, getNetError());
					closeAndDestoryClientNet(pClientNet);
				}
			}
			// end event check routine
		}
	}

err:
	stopServer(pNet);

	return -1;
}

int netInit()
{
#ifdef _WIN32
	WSADATA			 wsda;							//	Structure   to   store   info
	int ret = WSAStartup(MAKEWORD(2, 2), &wsda);    //	Load   version   1.1   of   Winsock
	if (ret == SOCKET_ERROR)
	{
		return -1;
	}
#endif

	return 0;
}

void netDeinit()
{
	while (NULL != f_pListClientConnectons)
	{
		pLinkItem p = f_pListClientConnectons;
		f_pListClientConnectons = f_pListClientConnectons->pNext;

		pXQTTNet pNet = (pXQTTNet)p->data;
		if (NULL == pNet)
		{
			continue;
		}
#ifdef _WIN32
		closesocket(pNet->_netHandler);
#else	// _WIN32
		close(pNet->_netHandler);
#endif 
		SafeDelete(pNet);
		SafeDelete(p);
	}
#ifdef _WIN32
	WSACleanup();
#endif // _WIN32
}

int startAsServer(const char* pBindAddress, unsigned short nBindPort, pXQTTNet pNet)
{
	int ret = SOCKET_ERROR;

	if (NULL == pBindAddress || NULL == pNet || SERVER_RUNNING == f_bServerState)
	{
		return -1;
	}

	pNet->_netHandler = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == pNet->_netHandler)
	{
		goto err;
	}
#ifdef _WIN32
	unsigned long mode = 1;				// non block mode
	if (SOCKET_ERROR == ioctlsocket(pNet->_netHandler, FIONBIO, &mode))
	{
		goto clean;
	}
#else
	if (fcntl(pNet->_netHandler, F_SETFL, O_NONBLOCK) == -1) {
		goto clean;
	}
#endif // _WIN32

	struct sockaddr_in server_sockaddr;
	memset(&server_sockaddr, 0, sizeof(server_sockaddr));
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_addr.s_addr = inet_addr(pBindAddress);
	server_sockaddr.sin_port = htons(nBindPort);
	int server_len = sizeof(server_sockaddr);
	ret = bind(pNet->_netHandler, (struct sockaddr *) &server_sockaddr, server_len);
	if (SOCKET_ERROR == ret)
	{
		goto clean;
	}
	ret = listen(pNet->_netHandler, 5);
	if (SOCKET_ERROR == ret)
	{
		goto clean;
	}

	// start a thread for handle accept event
	NET_MODE = WithSelect;
	f_bQuitFlag = false;
#ifdef _WIN32
	f_hAcceptThreadId = CreateThread(
		NULL,						// default security attributes
		0,							// use default stack size  
		acceptEventHandleThread,    // thread function name
		pNet,						// argument to thread function 
		0,		                    // use default creation flags 
		NULL);					    // returns the thread identifier 
	if (INVALID_HANDLE_VALUE == f_hAcceptThreadId)
	{
		goto clean;
	}
#else // _WIN32
	ret = pthread_create(&f_hAcceptThreadId, NULL, acceptEventHandleThread, pNet);
	if (0 != ret)
	{
		goto clean;
	}
#endif 

	return 0;
clean:
#ifdef _WIN32
	closesocket(pNet->_netHandler);
#else
	close(pNet->_netHandler);
#endif // _WIN32

err:
	return -1;
}

int stopServer(pXQTTNet pNet)
{
	f_bQuitFlag = true;

	// close all socket when select error occurred 
	// free all network item
	while (NULL != f_pListNetClient)
	{
		pLinkList p = f_pListNetClient;
		f_pListNetClient = f_pListNetClient->pNext;

#ifdef _WIN32
		closesocket(((pXQTTNet)p->data)->_netHandler);
#else
		close(((pXQTTNet)p->data)->_netHandler);
#endif // _WIN32

		SafeDelete(p->data);
		SafeDelete(p);

	}

#ifdef _WIN32
	closesocket(pNet->_netHandler);
#else
	close(pNet->_netHandler);
#endif // _WIN32

	return 0;
}

int serverState()
{
	return f_bServerState;
}

int closeAndDestoryClientNet(pXQTTNet pClientNet)
{
	if (NULL == pClientNet)
	{
		return -1;
	}

	pLinkItem pItem = removeItem(&f_pListNetClient, pClientNet);
#ifdef _WIN32
	closesocket(pClientNet->_netHandler);
#else  // _WIN32
	close(pClientNet->_netHandler);
#endif 

	SafeDelete(pClientNet);
	SafeDelete(pItem);

	return 0;
}

int getNetError()
{
#ifdef _WIN32
	return WSAGetLastError();
#else // _WIN32
	return errno;
#endif 
}

int sendData(pXQTTNet pClient, const char* pData, int len)
{
	return send(pClient->_netHandler, pData, len, 0);
}

pXQTTNet connectServer(const char* pIp, unsigned short port)
{
	if (0 == port || NULL == pIp)
	{
		return NULL;
	}

	pXQTTNet pNet = (pXQTTNet)malloc(sizeof(XQTTNet));
	memset(pNet, 0, sizeof(XQTTNet));

	pNet->_netHandler = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == pNet->_netHandler)
	{
		goto err;
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(pIp);
	serverAddr.sin_port = htons(port);
	int addrLen = sizeof(serverAddr);
	int ret = connect(pNet->_netHandler, (struct sockaddr*)&serverAddr, addrLen);
	if (SOCKET_ERROR == ret)
	{
		goto clean;
	}

#ifdef _WIN32
	unsigned long mode = 1;				// non block mode
	if (SOCKET_ERROR == ioctlsocket(pNet->_netHandler, FIONBIO, &mode))
	{
		goto clean;
	}
#else
	if (fcntl(pNet->_netHandler, F_SETFL, O_NONBLOCK) == -1) {
		goto clean;
	}
#endif // _WIN32

	appendData(&f_pListClientConnectons, pNet);

	return pNet;

clean:
#ifdef _WIN32
	closesocket(pNet->_netHandler);
#else // _WIN32
	close(pNet->_netHandler);
#endif 
err:
	free(pNet);
	return NULL;
}

int disConnect(pXQTTNet pNet)
{
	if (NULL == pNet)
	{
		return -1;
	}

#ifdef _WIN32
	closesocket(pNet->_netHandler);
#else // _WIN32
	close(pNet->_netHandler);
#endif 

	pLinkItem pItem = removeItem(&f_pListClientConnectons, pNet);
	SafeDelete(pItem);
	SafeDelete(pNet);

	return 0;
}

pXQTTNet connectServerUdp(const char* pIp, unsigned short port, const char* pLocalIp, unsigned short localPort, struct sockaddr_in *to)
{
	if (0 == port || NULL == pIp)
	{
		return NULL;
	}

	pXQTTNet pNet = (pXQTTNet)malloc(sizeof(XQTTNet));
	memset(pNet, 0, sizeof(XQTTNet));

	memset(to, 0, sizeof(struct sockaddr_in));
	to->sin_family = AF_INET;
	to->sin_addr.s_addr = inet_addr(pIp);
	to->sin_port = htons(port);

	pNet->_netHandler = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SOCKET_ERROR == pNet->_netHandler)
	{
		goto err;
	}

	struct sockaddr_in localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	if (pLocalIp && 0 != strcmp(pLocalIp,""))
	{
		localAddr.sin_addr.s_addr = inet_addr(pLocalIp);
	}
	else
	{
		localAddr.sin_addr.s_addr = INADDR_ANY;
	}
	localAddr.sin_port = htons(localPort);
	int addrLen = sizeof(localAddr);
	int ret = bind(pNet->_netHandler, (struct sockaddr*)&localAddr, addrLen);
	if (SOCKET_ERROR == ret)
	{
		goto clean;
	}

#ifdef _WIN32
	unsigned long mode = 1;				// non block mode
	if (SOCKET_ERROR == ioctlsocket(pNet->_netHandler, FIONBIO, &mode))
	{
		goto clean;
	}
#else
	if (fcntl(pNet->_netHandler, F_SETFL, O_NONBLOCK) == -1) {
		goto clean;
	}
#endif // _WIN32

	appendData(&f_pListClientConnectons, pNet);

	// start a thread for handle accept event
	NET_MODE = WithSelectUdp;
	f_bQuitFlag = false;
#ifdef _WIN32
	f_hAcceptThreadId = CreateThread(
		NULL,						// default security attributes
		0,							// use default stack size  
		acceptEventHandleThread,    // thread function name
		pNet,						// argument to thread function 
		0,		                    // use default creation flags 
		NULL);					    // returns the thread identifier 
	if (INVALID_HANDLE_VALUE == f_hAcceptThreadId)
	{
		goto clean;
	}
#else // _WIN32
	ret = pthread_create(&f_hAcceptThreadId, NULL, acceptEventHandleThread, pNet);
	if (0 != ret)
	{
		goto clean;
	}
#endif

	return pNet;

clean:
#ifdef _WIN32
	closesocket(pNet->_netHandler);
#else // _WIN32
	close(pNet->_netHandler);
#endif 
err:
	free(pNet);
	return NULL;
}

int sendDataUdp(pXQTTNet pClient, const char* pData, int len, const struct sockaddr_in *to, int tolen)
{
	return sendto(pClient->_netHandler, pData, len, 0, (const struct sockaddr*)to, tolen);
}

static int acceptEventWorkerWithSelectUdp(pXQTTNet pNet)
{
	fd_set	fdRead, fdWrite, fdException;
	struct timeval t;
	int addrLen = sizeof(SOCKADDR_IN);
	u_int maxfd = 0;
	t.tv_sec = 0;
	t.tv_usec = 500;

	while (!f_bQuitFlag)
	{
		FD_ZERO(&fdRead);
		FD_ZERO(&fdException);
		FD_ZERO(&fdWrite);

		maxfd = pNet->_netHandler;
		FD_SET(pNet->_netHandler, &fdRead);
		FD_SET(pNet->_netHandler, &fdWrite);
		FD_SET(pNet->_netHandler, &fdException);

#ifdef _WIN32
		//int ret = select(0, &fdRead, &fdWrite, &fdException, &t);
		int ret = select(0, &fdRead, NULL, &fdException, &t);//udp模式下不需要写通知
#else
		//int ret = select(maxfd + 1, &fdRead, &fdWrite, &fdException, &t);
		int ret = select(maxfd + 1, &fdRead, NULL, &fdException, &t);//udp模式下不需要写通知
#endif // _WIN32
		if (SOCKET_ERROR == ret)
		{
			goto err;
		}

		if (0 == ret)
		{
			continue;		// timeout
		}

		if (ret > 0)
		{
			// check read events
			for (int i = 0; i < (int)fdRead.fd_count; i++)
			{
				if (FD_ISSET(fdRead.fd_array[i], &fdRead))
				{
					if (fdRead.fd_array[i] == pNet->_netHandler)
					{
						// data income from client event
						char recvBuf[MaxRecvBuffLen];
						memset(recvBuf, 0, MaxRecvBuffLen);
						int recvLen = recv(fdRead.fd_array[i], recvBuf, MaxRecvBuffLen, 0);
						pXQTTNet pClientNet = pNet;

						if (SOCKET_ERROR == recvLen)
						{
							SafeCall(pClientNet->_netOnError, pNet, pClientNet, getNetError());
							closeAndDestoryClientNet(pClientNet);
						}
						else if (0 == recvLen) {
							// remote close connection
							SafeCall(pClientNet->_netOnDisconn, pNet, pClientNet, getNetError());
							closeAndDestoryClientNet(pClientNet);
						}
						else{
							SafeCall(pClientNet->_netOnRecv, pNet, pClientNet, recvBuf, recvLen);
						}
					}
				}
			}

			// check write event
			for (int i = 0; i < (int)fdWrite.fd_count; i++)
			{
				if (fdWrite.fd_array[i] == pNet->_netHandler)
				{
					pXQTTNet pClientNet = pNet;
					SafeCall(pClientNet->_netOnSendComplete, pNet, pClientNet);
				}
			}

			// check exception event
			for (int i = 0; i < (int)fdException.fd_count; i++)
			{
				if (fdException.fd_array[i] == pNet->_netHandler)
				{
					pXQTTNet pClientNet = pNet;
					SafeCall(pClientNet->_netOnError, pNet, pClientNet, getNetError());
					closeAndDestoryClientNet(pClientNet);
				}
			}
			// end event check routine
		}
	}

err:
	stopServer(pNet);

	return -1;
}
