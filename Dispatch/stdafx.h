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
#include"DispatchOperate.h"
extern std::map<SOCKET, DispatchOperate*>  m_dispatchOperate;


#define GOOGLE_GLOG_DLL_DECL           // ʹ�þ�̬glog�������
#define GLOG_NO_ABBREVIATED_SEVERITIES // û�����������,��˵��Ϊ��Windows.h��ͻ
#include "../lib/glog/logging.h"
#pragma comment(lib,"../lib/glog/lib/libglog.lib")


#define DEBUG_LOG  TRUE
#pragma warning(disable:4996)
#undef ERROR