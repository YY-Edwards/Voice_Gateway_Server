
#include <WinSock2.h>
#include <Windows.h>
#include "../lib/radio/common.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#pragma once
#define SEND_LENTH   4
#define RECV_LENTH   28
#define RECV_THARGET_LENTH 19

typedef struct tagThreadARS
{
	SOCKET           mySocket;
	HWND	         hwnd;
	SOCKADDR_IN      remote_addr;                 // 接收到Message所来自的IP Address
	int              ARSLength;                   // 要发送的Message的长度
	char             RcvBuffer[RECV_LENTH];   // 接收缓冲区按字节计算，实际接收的内容则是Unicode
	char             SendBuffer[SEND_LENTH];
	unsigned long    radioID;
}ThreadARS;
class CRadioARS
{
public:
	CRadioARS();
	~CRadioARS();
	bool InitARSSocket(DWORD dwAddress/*,CRemotePeer * pRemote*/);
	bool CloseARSSocket(SOCKET* s);
	static DWORD WINAPI ReceiveDataThread(LPVOID lpParam);
	bool sendArsAck(DWORD dwRadioID, int CaiNet);
	void RecvData();
private:
	bool m_RcvSocketOpened;
	ThreadARS *m_ThreadARS;
	//CRemotePeer* pRemotePeer;
};

