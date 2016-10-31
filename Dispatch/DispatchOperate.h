#include "XNLConnection.h"
#include "TextMsg.h"
#include "RadioARS.h"
#include "RadioGps.h"
#include <map>
#include <mutex>
#include<list>
#include "../lib/AES/Aes.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#pragma once
extern  bool udpIsConnect;



#define CMD_NULL               0
#define RADIO_CONNECT          1
#define PRIVATE_CALL           2
#define GROUP_CALL             3
#define ALL_CALL               4
#define REMOTE_CLOSE           5
#define REMOTE_OPEN            6
#define CHECK_RADIO_ONLINE     7
#define REMOTE_MONITOR         8
#define SEND_PRIVATE_MSG       9
#define SEND_GROUP_MSG         10
#define RECV_MSG               11
#define GPS_IMME_COMM          12
#define GPS_TRIGG_COMM         13
#define GPS_IMME_CSBK          14
#define GPS_TRIGG_CSBK         15
#define STOP_QUERY_GPS         16
#define RADIO_ARS              17
#define RADIO_GPS              18
#define STOP_CALL              19
#define GPS_RECV               20
#define REMOTE_REPLY           21
#define EMERGENCY              22
#define GPS_IMME_CSBK_EGPS     23
#define GPS_TRIGG_CSBK_EGPS    24
#define TCP_CONNECT            25
#define RADIO_UDP_CONNECT      26
#define MNIS_UDP_CONNECT       27
#define LICENSE                28
#define CALL_START             29
#define CALL_END               30

#define RADIO_STATUS_OFFLINE   0
#define RADIO_STATUS_ONLINE    1

typedef  struct tagradioStatus{
	int status;
	int    gpsQueryMode;
} status;
extern map<string, status> radioStatus;
typedef struct tagAllCommand
{
	int callId;
	int ackNum;
	int timeOut;
	int timeCount;
	int command;
	int radioId;
	int cycle;
	int querymode;
	string radioIP;
	string mnisIP;
	string gpsIP;
	wchar_t * text;
	CRemotePeer* pRemote;
	SOCKET s;
}AllCommand;
extern list <AllCommand>allCommandList;
extern  int  seq;
extern std::mutex m_allCommandListLocker;
class DispatchOperate
{
public:
	DispatchOperate();
	~DispatchOperate();
	
	static DWORD WINAPI TCPConnectionThread(LPVOID lpParam);
	static DWORD WINAPI TimeOutThread(LPVOID lpParam);
	static DWORD WINAPI WorkThread(LPVOID lpParam);

	void AddAllCommand(CRemotePeer* pRemote,SOCKET s, int command, string radioIP, string mnisIP, string gpsIP, int id,wchar_t* text, int cycle, int querymode, int callId);
private:
	void WorkThreadFunc();
	void TimeOut();
	CXNLConnection  *pXnlConnection;
	CTextMsg        pTextMsg;
	CRadioARS       pRadioARS;
	CRadioGps       pRadioGPS;
	int             m_queryMode;
	DWORD            dwIP;
	DWORD            dwip;
	bool             textConnectResult;
	bool             ARSConnectResult;
	bool            GPSConnectResult;
	std::map <int, int> gpsDic;
	CRemotePeer* pRemotePeer;
	int callID;



	int Connect(CRemotePeer* pRemote, const char* ip, const char  * pIP, int callId);
	int call(CRemotePeer* pRemote, int id, int callId);
	int groupCall(CRemotePeer* pRemote, int id, int callId);
	int allCall(CRemotePeer* pRemote, int callId);
	int stopCall(CRemotePeer* pRemote, int callId);
	int remotePowerOn(CRemotePeer* pRemote, int id, int callId);
	int remotePowerOff(CRemotePeer* pRemote, int id, int callId);
	int radioCheck(CRemotePeer* pRemote, int id, int callId);
	int wiretap(CRemotePeer* pRemote, int id, int callId);
	int sendSms(CRemotePeer* pRemote, int id, wchar_t* message, int callId);
	int sendGroupSms(CRemotePeer* pRemote, int id, wchar_t* message, int callId);
	int getGps(CRemotePeer* pRemote, int id, int queryMode, int cycle, int callId);              //queryMode  12: 常规查询   13： 常规周期查询  14：csbk查询   15： csbk 周期查询
	int cancelPollGps(CRemotePeer* pRemote, int id, int callId);
	int RadioConnect();
	void TcpConnect();
	int getOverturnGps(CRemotePeer* pRemote, const char* ip, int callId);
	int getLic(const char* licPath);
	int tcpConnect(const char *ip);
	int radioUdpConnect(const char *ip);
	int mnisUdpConnect(const char* ip);


	std::mutex   m_addCommandLocker;
	list <AllCommand>  commandList;
};

