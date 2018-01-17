#ifndef _XQTT_NET_H
#define _XQTT_NET_H

#include "global.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")
#endif // _WIN32

#define			SERVER_RUNNING		1
#define			SERVER_NOT_RUN		0

#define			MaxClient			200				// max amount of client 
#define			MaxRecvBuffLen		16384			// 1024 * 16

typedef enum _net_mode{
	WithSelect = 1,
	WithEpoll,
	WithSelectUdp
};

extern int NET_MODE;

#ifdef __cplusplus
extern "C" {
#endif
	struct _xqtt_net;

	/*
	* Network Object
	*/
	typedef		int(*NetSend)(struct _xqtt_net* pNet, char* pData, int len);
	typedef		int(*NetConn)(struct _xqtt_net* pNet, const char* pAddr, unsigned short port);
	typedef		void(*OnAccept)(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient);
	typedef		void(*OnRecv)(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len);
	typedef		void(*OnConn)(struct _xqtt_net* pNet);
	typedef		void(*OnDisconn)(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	typedef		void(*OnSendComplete)(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient);
	typedef		void(*OnError)(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);

	typedef struct _xqtt_net
	{
	#ifdef _WIN32
		SOCKET		_netHandler;
	#else
		int			_netHandler;
	#endif // _WIN32
		struct sockaddr_in _addr;
		void*		data;
		void*		param;

		/* net operation handlers */
		NetSend		_netSend;
		NetConn		_netConn;
		OnAccept	_netOnAccept;
		OnRecv		_netOnRecv;
		OnConn		_netOnConn;
		OnDisconn	_netOnDisconn;
		OnSendComplete	_netOnSendComplete;
		OnError		_netOnError;

	}XQTTNet, *pXQTTNet;

	/*
	* Initialize network, for Windows, call WSAStartup
	*/
	int netInit();

	/*
	* DeInitialize network
	*/
	void netDeinit();

	/*
	* Start the net as server mode
	*/
	int startAsServer(const char* pBindAddress, unsigned short nBindPort, pXQTTNet pNet);

	/*
	* Stop the server
	*/
	int stopServer(pXQTTNet pNet);

	/*
	* Test if the server is running
	*/
	int serverState();

	/*
	* Close and destroy client net as Server Role
	*/
	int closeAndDestoryClientNet(pXQTTNet pClient);

	/*
	* Get last net error
	*/
	int getNetError();

	/*
	* Send data to remote point
	*/
	int sendData(pXQTTNet pClient, const char* pData, int len);

	/*
	* Send data to remote point by udp
	*/
	int sendDataUdp(pXQTTNet pClient, const char* pData, int len,const struct sockaddr_in *to, int tolen);

	/*
	* Connect to server
	*/
	pXQTTNet connectServer(const char* pIp, unsigned short port);

	/*
	* Disconnect client connection, as Client role
	*
	* @param pNet pXQTTNet, pointer of XQTT
	* @return 0: success, -1: false
	*/
	int disConnect(pXQTTNet pNet);

	/*
	* Connect to server by udp
	*/
	pXQTTNet connectServerUdp(const char* pIp, unsigned short port, const char* pLocalIp, unsigned short localPort, struct sockaddr_in *to);

#ifdef __cplusplus
}
#endif
#endif // _XQTT_NET_H
