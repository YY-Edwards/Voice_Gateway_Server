#ifndef COMMON
#define COMMON

#include <mutex>
#include <list>
#include "../rpc/include/TcpServer.h"
#include "../rpc/include/BaseConnector.h"
#include <WinSock2.h>
#include <Windows.h>
#include "allCommon.h"


#define               MNIS_CONNECT           0              
#define				  CONNECT_STATUS         1                   //status
#define				  RADIO_STATUS           2                   //status
#define               SEND_PRIVATE_MSG       9                   //messageStatus
#define               SEND_GROUP_MSG         10					 //messageStatus
#define               RECV_MSG               11					 //message
#define               GPS_IMME_COMM          12                  //sendGpsStatus
#define               GPS_TRIGG_COMM         13                  //sendGpsStatus
#define               GPS_IMME_CSBK          14                  //sendGpsStatus
#define               GPS_TRIGG_CSBK         15                  //sendGpsStatus
#define               STOP_QUERY_GPS         16					 //sendGpsStatus
#define               RADIO_ARS              17					 //sendArs
#define               RADIO_GPS              18
#define               RECV_GPS               20					 //sendGps
#define               RECV_LOCATION_INDOOR   21
#define               GPS_IMME_CSBK_EGPS     23					 //sendGpsStatus
#define               GPS_TRIGG_CSBK_EGPS    24					 //sendGpsStatus
#define               GPS_TRIGG_COMM_INDOOR  25
#define               GPS_TRIGG_CSBK_INDOOR  26
#define               GPS_TRIGG_CSBK_EGPS_INDOOR 27
#define               MNIS_DIS_CONNECT       30



#define PRIVATE_MSG_FLG        12
#define GROUP_MSG_FLG          225
#define GROUP                  2
#define PRIVATE                3

#define  SUCESS               0
#define  UNSUCESS              1

#define START         0
#define STOP          1



#define RADIO_STATUS_OFFLINE   0
#define RADIO_STATUS_ONLINE    1
struct BconMajMinTimeReport
{
	unsigned short Major;
	unsigned short Minor;
	unsigned short TimeStamp;
};
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
	std::string text;
}Command;

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
	double altitude;
	std::map<std::string, RadioStatus> rs;
	std::list< BconMajMinTimeReport> becon;
}Respone;
extern void(*myCallBackFunc)(  int, Respone);
void onData(void(*func)(int, Respone),  int call, Respone data);
extern std::mutex m_timeOutListLocker;
extern std::list <Command> timeOutList;
extern std::mutex m_workListLocker;
#endif