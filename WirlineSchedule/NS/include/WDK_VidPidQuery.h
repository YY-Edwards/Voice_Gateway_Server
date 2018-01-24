#ifndef _FRONTEND_VID_PID_H_  
#define _FRONTEND_VID_PID_H_  

/* ----------------------------------------------------------
文件名称：WDK_VidPidQuery.h

开发环境：
Visual Studio V2008

接口函数：
WDK_WhoAllVidPid
------------------------------------------------------------ */
#pragma once  
#include <string>  
using namespace std;
#include <windows.h>
#include "NSStruct.h"


#ifndef MACRO_HIDD_VIDPID  
#define MACRO_HIDD_VIDPID  
typedef struct _HIDD_VIDPID
{
	USHORT  VendorID;
	USHORT  ProductID;
} HIDD_VIDPID;
#endif  


/*
功能：获取对应VID设备的串口号
入口参数：
[in] ssin：设备的VID号
返回值：
获取到的串口号
*/
int WINAPI WDK_WhoAllVidPid(string ssin, com_use_t* pValue);



#endif