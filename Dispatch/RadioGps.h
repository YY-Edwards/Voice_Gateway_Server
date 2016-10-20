
//#include <WinSock2.h>
//#include <Windows.h>
#include "../lib/rpc/include/BaseConnector.h"
#pragma once
#define SEND_IMM_QUERY_LENTH   12
#define SEND_TRG_QUERY_LENTH   11
#define SEND_IMM_CSBK_QUERY_LENTH   10
#define SEND_TRG_CSBK_QUERY_LENTH   14
#define SEND_IMM_CSBK_EGPD_QUERY_LENTH   13
#define SEND_TRG_CSBK_EGPD_QUERY_LENTH   16
#define SEND_STOP_LENTH   8
#define RECV_IMME_LENTH   28
#define RECV_TRG_LENTH   17
#define RECV_CSBK_LENTH   19
#define RECV_CSBK_EGPS_LENTH   32
#define MAX_RECV_LENGTH     512
typedef struct tagThreadGPS
{
	SOCKET           mySocket;
	HWND	         hwnd;
	SOCKADDR_IN      remote_addr;                 // 接收到Message所来自的IP Address
	int              gpsLength;                   // 要发送的Message的长度
	char             RcvBuffer[MAX_RECV_LENGTH];       // 接收缓冲区按字节计算，实际接收的内容则是Unicode
	char             SendBuffer[SEND_TRG_CSBK_QUERY_LENTH];
	unsigned long    radioID;
}ThreadGPS;
typedef struct tagThreadGPSOverturn
{
	SOCKET           mySocket;
	HWND	         hwnd;
	SOCKADDR_IN      remote_addr;                 // 接收到Message所来自的IP Address
	int              gpsLength;                   // 要发送的Message的长度
	char             RcvBuffer[MAX_RECV_LENGTH];       // 接收缓冲区按字节计算，实际接收的内容则是Unicode
	char             SendBuffer[SEND_TRG_CSBK_QUERY_LENTH];
	unsigned long    radioID;
}ThreadGPSOverturn;

class CRadioGps
{
public:
	CRadioGps();
	~CRadioGps();
	bool InitGPSSocket(DWORD dwAddress, CRemotePeer * pRemote);
	bool InitGPSOverturnSocket(DWORD dwAddress);
	bool CloseGPSSocket(SOCKET* s);
	static DWORD WINAPI ReceiveDataThread(LPVOID lpParam);
	bool SendQueryGPS(DWORD dwRadioID,int queryMode,int cycle);
	bool StopQueryTriggeredGPS(DWORD dwRadioID,int queryMode);
	void RecvData();
private:
	bool m_RcvSocketOpened;
	ThreadGPS * m_ThreadGps;
	ThreadGPSOverturn *m_ThreadGpsOverturn;
	CRemotePeer* pRemotePeer;
	
};

