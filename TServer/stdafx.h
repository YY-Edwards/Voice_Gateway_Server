// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <WinSvc.h>

// TODO: reference additional headers your program requires here
#define GOOGLE_GLOG_DLL_DECL           // ʹ�þ�̬glog�������
#define GLOG_NO_ABBREVIATED_SEVERITIES // û�����������,��˵��Ϊ��Windows.h��ͻ
#include "../lib/glog/logging.h"
#pragma comment(lib,"../lib/glog/lib/libglog.lib")