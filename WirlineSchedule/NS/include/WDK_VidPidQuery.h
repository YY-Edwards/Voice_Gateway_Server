#ifndef _FRONTEND_VID_PID_H_  
#define _FRONTEND_VID_PID_H_  

/* ----------------------------------------------------------
�ļ����ƣ�WDK_VidPidQuery.h

����������
Visual Studio V2008

�ӿں�����
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
���ܣ���ȡ��ӦVID�豸�Ĵ��ں�
��ڲ�����
[in] ssin���豸��VID��
����ֵ��
��ȡ���Ĵ��ں�
*/
int WINAPI WDK_WhoAllVidPid(string ssin, com_use_t* pValue);



#endif