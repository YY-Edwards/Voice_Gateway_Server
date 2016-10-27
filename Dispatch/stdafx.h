// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO:  在此处引用程序需要的其他头文件

//#include <afx.h>                       //引入mfc的库
#include <deque>
#include <WinSock2.h>
#include <Windows.h>
#include<list>
using namespace std;
#include <mutex>

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "../lib/rpc/include/BaseConnector.h"
using  namespace rapidjson;

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
   
#define START                   0
#define STOP                    1
#define NONE                    0
#define ALL                     1
#define GROUP                   2
#define PRIVATE                 3

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
}AllCommand;
extern list <AllCommand>allCommandList;
extern std::mutex m_allCommandListLocker;
extern CRITICAL_SECTION cs;

#include"DispatchOperate.h"
#include "../lib/rpc/include/RpcServer.h"

extern map<CRemotePeer *, DispatchOperate*>  m_dispatchOperate;

extern  int  seq;

#define GOOGLE_GLOG_DLL_DECL           // 使用静态glog库用这个
#define GLOG_NO_ABBREVIATED_SEVERITIES // 没这个编译会出错,传说因为和Windows.h冲突
#include "../lib/glog/logging.h"
#pragma comment(lib,"../lib/glog/lib/libglog.lib")


#define DEBUG_LOG  TRUE
#pragma warning(disable:4996)
#undef ERROR