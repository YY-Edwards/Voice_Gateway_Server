// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <map>


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�

//#include <afx.h>                       //����mfc�Ŀ�
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
#define GOOGLE_GLOG_DLL_DECL           // ʹ�þ�̬glog�������
#define GLOG_NO_ABBREVIATED_SEVERITIES // û�����������,��˵��Ϊ��Windows.h��ͻ
#include "../lib/glog/logging.h"
#pragma comment(lib,"../lib/glog/lib/libglog.lib")
#pragma comment(lib,"setupapi.lib")
#pragma comment(lib,"hid.lib")

#define DEBUG_LOG  TRUE
#pragma warning(disable:4996)
#undef ERROR