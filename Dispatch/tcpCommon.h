#ifndef TCPCOMMON
#define TCPCOMMON

#include <mutex>
#include <list>

#include "../lib/rpc/include/TcpServer.h"
#include "../lib/rpc/include/BaseConnector.h"
#include <WinSock2.h>
#include <Windows.h>
#include "../lib/radio/allCommon.h"
#define RADIO_CONNECT          1
#define PRIVATE_CALL           2
#define GROUP_CALL             3
#define ALL_CALL               4
#define REMOTE_CLOSE           5
#define REMOTE_OPEN            6
#define CHECK_RADIO_ONLINE     7
#define REMOTE_MONITOR         8
#define RADIO_ARS              17					 //sendArs
#define STOP_CALL              19
#define REMOTE_REPLY           21
#define EMERGENCY              22
#define TCP_CONNECT            25
#define RADIO_UDP_CONNECT      26
#define MNIS_UDP_CONNECT       27
#define LICENSE                28
#define CALL_START             29
#define CALL_END               30
#define RADIO_SERIAL           31

//#define RADIO_STATUS_OFFLINE   0
//#define RADIO_STATUS_ONLINE    1

#define DATA_SUCESS_DISPATCH_SUCESS      0
#define DATA_FAILED_DISPATCH_SUCESS      1
#define DATA_SUCESS_DISPATCH_FAILED      2
#define DATA_FAILED_DISPATCH_FAILED      3


#define CALL_FAILED              0
//#define CALL_START               1
//#define CALL_END                 2

#define REMOTE_SUCESS             0
#define REMOTE_FAILED             1
#define REMOTE_CONNECT_FAILED     2

#define START                   0
#define STOP                    1
#define NONE                    0
#define ALL                     1
#define GROUP                   2
#define PRIVATE                 3


#define RADIO_STATUS_OFFLINE   0
#define RADIO_STATUS_ONLINE    1

#define  RADIOCHECK    0
#define  MONITOR  1
#define  OFF      2
#define  ON       3

#define  SUCESS               0
#define  UNSUCESS              1
typedef struct tagTcpRespone
{
	int id;
	int controlType;
	int callType;
	int result;
	int arsStatus;
	std::string radioSerial;
}TcpRespone;
extern void(*myTcpCallBackFunc)(int, TcpRespone);
void onTcpData(void(*func)( int, TcpRespone), int call, TcpRespone data);
extern std::string m_radioIP;
extern std::string m_mnisIP;
typedef struct tagTcpCommand
{
	int callId;
	int timeOut;
	int timeCount;
	int command;
	int radioId;
	std::string radioIP;
	//SOCKET s;
}TcpCommand;
extern std::list <TcpCommand> tcpCommandTimeOutList;
extern  int  num;
extern std::mutex m_allCommandListLocker;

#endif