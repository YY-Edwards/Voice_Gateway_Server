
#pragma once
#include "../common.h"
#define SEND_IMM_QUERY_LENTH   10
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

#define Immediate_Location_Request                      0x05       //Immediate Location Request
#define Immediate_Location_Request_Tokens_Length        0x08       //XML协议报中包含8组数据
#define Location_RequestID_Start                        0x22       //Start of request-id element
#define Immediate_Location_Request_Time                 0x51       //Start of ret-info element, "ret-info-time" and "ret-info-accuracy" is specified as "YES"
#define Immediate_Location_RequestID_Speed              0x62       //Start of request-speed-hor element
#define Immediate_Location_Report                       0x07       //Immediate Location Report

#define Triggered_Location_Request                      0x09
#define Triggered_Location_Request_Tokens_Length        0x09
#define Start_Interval_Element_uint                     0x31       //START of interval element, indicating that a uintvar follows
#define Start_Interval_Element_ufloat                   0x32       //START of interval element, indicating that a ufloat follows
#define Start_Trigger_Element                           0x34
#define Triggered_Location_Report                       0x0D

#define CSBK_Start_Require_Data                         0x40
#define CSBK_Require_Data_Length                        0x01
#define Request_LRRP_CSBK                               0x41
#define CSBK_Triggered_Location_Request_Tokens_Length_uint                     0x0B         //interval value: uintvar
#define CSBK_Triggered_Location_Request_Tokens_Length_ufloat                   0x0C         //interval value: ufloat
#define CSBK_Location_Request_Time                                             0x52
#define CSBK_Require_Speed_Horizontal                                          0x74
#define CSBK_Require_Direction_Horizontal                                      0x69

#define Triggered_location_Start_Answer                  0x0B
#define Triggered_Location_Stop_Answer                   0x11
#define Location_Operate_Sucess                          0x38
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

class CDataScheduling;

class CRadioGps
{
public:
	CRadioGps(CDataScheduling *pMnis);
	~CRadioGps();
	bool InitGPSSocket(DWORD dwAddress/*, CRemotePeer * pRemote*/);
	bool InitGPSOverturnSocket(DWORD dwAddress);
	bool CloseGPSSocket();
	static DWORD WINAPI ReceiveDataThread(LPVOID lpParam);
	bool SendQueryGPS(DWORD dwRadioID,int queryMode,double cycle);
	bool StopQueryTriggeredGPS(DWORD dwRadioID,int queryMode);
	void RecvData();
private:
	bool m_RcvSocketOpened;
	ThreadGPS * m_ThreadGps;
	ThreadGPSOverturn *m_ThreadGpsOverturn;
	CRemotePeer* pRemotePeer;
	CDataScheduling *m_pMnis;
	
};

