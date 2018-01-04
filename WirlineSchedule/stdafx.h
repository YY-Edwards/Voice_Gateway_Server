// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "common.h"

extern CTool* g_pTool;
extern decline_reason_code_info_t g_callRequstDeclineReasonCodeInfo;
extern std::string repeaterSerial;
extern std::string repeaterMode;


// TODO:  在此处引用程序需要的其他头文件
