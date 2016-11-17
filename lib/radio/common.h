#ifndef COMMON
#define COMMON

#include <mutex>
#include <list>
#include "../rpc/include/TcpServer.h"
#include "../rpc/include/BaseConnector.h"
#include <WinSock2.h>
#include <Windows.h>

#define               MNIS_CONNECT           0
#define               SEND_PRIVATE_MSG       9
#define               SEND_GROUP_MSG         10
#define               RECV_MSG               11
#define               GPS_IMME_COMM          12
#define               GPS_TRIGG_COMM         13
#define               GPS_IMME_CSBK          14
#define               GPS_TRIGG_CSBK         15
#define               STOP_QUERY_GPS         16
#define               RADIO_ARS              17
#define               RADIO_GPS              18
#define               RECV_GPS               20
#define               GPS_IMME_CSBK_EGPS     23
#define               GPS_TRIGG_CSBK_EGPS    24


#define PRIVATE_MSG_FLG        12
#define GROUP_MSG_FLG          225

#define GROUP                  2
#define PRIVATE                3

#define  SUCESS               0
#define  UNSUCESS              1

#define START         0
#define STOP          1

#define CONNECT_STATUS          1
#define RADIO_STATUS            2

#define RADIO_STATUS_OFFLINE   0
#define RADIO_STATUS_ONLINE    1

typedef struct tagCommand
{
	int callId;
	int ackNum;
	int timeOut;
	int timeCount;
	int command;
	int radioId;
	double cycle;
	int querymode;
	std::string radioIP;
	std::string gpsIP;
	wchar_t * text;
	TcpClient* tp;
	
}Command;
typedef  struct tagRadioStatus{
	int    id;
	int	   status = 0;
	int    gpsQueryMode = 0;
} RadioStatus;
typedef struct tagRespone
{
	int connectStatus;
	int source;
	int target;
	int msgType;
	int gpsType;
	std::string msg;
	int msgStatus;
	double lat;
	double lon;
	double speed;
	int arsStatus;
	int gpsStatus;
	int valid;
	double cycle;
	int querymode;
	int operate;
	std::map<std::string, RadioStatus> rs;
}Respone;
extern void(*myCallBackFunc)(TcpClient*, int, int, Respone);
void onData(void(*func)(TcpClient* tp, int, int, Respone), TcpClient* tp, int callId, int call, Respone data);
extern TcpClient * peer;
extern int seq;
extern std::mutex m_timeOutListLocker;
extern std::list <Command> timeOutList;
extern std::map<std::string, RadioStatus> radioStatus;
extern std::string  lastIP ;

#endif