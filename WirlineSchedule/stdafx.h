// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "common.h"

extern CTool* g_pTool;
extern decline_reason_code_info_t g_callRequstDeclineReasonCodeInfo;
extern std::string repeaterMode;


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
