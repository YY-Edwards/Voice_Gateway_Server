// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <map>


// TODO:  在此处引用程序需要的其他头文件

//#include <afx.h>                       //引入mfc的库
#include <deque>
#include <WinSock2.h>
#include <Windows.h>
#include<list>
#include <mutex>
typedef  struct tagRadioStatus{
	int    id;
	int	   status = 0;
	int    gpsQueryMode = 0;
} RadioStatus;
extern std::map<std::string, RadioStatus> g_radioStatus;
extern std::mutex g_radioStatusLocker;
extern long long g_sn;
#define GOOGLE_GLOG_DLL_DECL           // 使用静态glog库用这个
#define GLOG_NO_ABBREVIATED_SEVERITIES // 没这个编译会出错,传说因为和Windows.h冲突
#include "../lib/glog/logging.h"
#pragma comment(lib,"../lib/glog/lib/libglog.lib")
#pragma comment(lib,"setupapi.lib")
#pragma comment(lib,"hid.lib")

#define DEBUG_LOG  TRUE
#pragma warning(disable:4996)
#undef ERROR