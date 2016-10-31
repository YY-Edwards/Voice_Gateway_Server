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
typedef  struct radioStatus{
	string ststus;
	int    gpsQueryMode;
} status;



#include"DispatchOperate.h"

extern std::map<SOCKET, DispatchOperate*>  m_dispatchOperate;

extern  int  seq;

#define GOOGLE_GLOG_DLL_DECL           // 使用静态glog库用这个
#define GLOG_NO_ABBREVIATED_SEVERITIES // 没这个编译会出错,传说因为和Windows.h冲突
#include "../lib/glog/logging.h"
#pragma comment(lib,"../lib/glog/lib/libglog.lib")


#define DEBUG_LOG  TRUE
#pragma warning(disable:4996)
#undef ERROR