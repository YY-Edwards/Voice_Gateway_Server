// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�

//#include <afx.h>                       //����mfc�Ŀ�
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
extern std::mutex m_allCommandListLocker;


#include"DispatchOperate.h"


extern map<SOCKET, DispatchOperate*>  m_dispatchOperate;

extern  int  seq;

#define GOOGLE_GLOG_DLL_DECL           // ʹ�þ�̬glog�������
#define GLOG_NO_ABBREVIATED_SEVERITIES // û�����������,��˵��Ϊ��Windows.h��ͻ
#include "../lib/glog/logging.h"
#pragma comment(lib,"../lib/glog/lib/libglog.lib")


#define DEBUG_LOG  TRUE
#pragma warning(disable:4996)
#undef ERROR