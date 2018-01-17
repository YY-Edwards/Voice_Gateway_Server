#include "stdafx.h"  
#include "../include/WDK_VidPidQuery.h"  
#include <tchar.h>  
#include <setupapi.h>  
#include <string>  
#include "iostream"  
using namespace std;

#pragma comment (lib, "Setupapi.lib")  

// �豸ʵ��ID��󳤶�  
#define DeviceInstanceIdSize 256      

// ��ȡϵͳ��VID��PID����  
INT WINAPI WDK_WhoAllVidPid(string ssin, com_use_t* pValue)
{
	int index = 0;
	HIDD_VIDPID* pVidPid = new HIDD_VIDPID[9];
	int iCapacity = 32;
	GUID* SetupClassGuid = NULL;
	GUID* InterfaceClassGuid = NULL;

	// �����豸��װ��GUID�����յ��豸��Ϣ����  
	HDEVINFO DeviceInfoSet = SetupDiCreateDeviceInfoList(SetupClassGuid, NULL);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	// �����豸��װ��GUID��ȡ�豸��Ϣ����  
	HDEVINFO hDevInfo;

	if (InterfaceClassGuid == NULL)
	{
		hDevInfo = SetupDiGetClassDevsEx(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_DEVICEINTERFACE | DIGCF_PRESENT, DeviceInfoSet, NULL, NULL);
	}
	else
	{
		hDevInfo = SetupDiGetClassDevsEx(InterfaceClassGuid, (PCWSTR)&"SCSI", NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT, DeviceInfoSet, NULL, NULL);
	}

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	// �洢�豸ʵ��ID   
	TCHAR DeviceInstanceId[DeviceInstanceIdSize];

	// �洢�豸��Ϣ����  
	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	// ��ȡ�豸��Ϣ����  
	DWORD DeviceIndex = 0;
	bool k = true;
	while (SetupDiEnumDeviceInfo(hDevInfo, DeviceIndex++, &DeviceInfoData))
	{
		// ��ȡ�豸ʵ��ID  
		if (SetupDiGetDeviceInstanceId(hDevInfo, &DeviceInfoData, DeviceInstanceId, DeviceInstanceIdSize, NULL) && k)
		{
			// ���豸ʵ��ID����ȡVID��PID  
			TCHAR* pVidIndex = _tcsstr(DeviceInstanceId, TEXT("VID_"));
			if (pVidIndex == NULL)
			{
				continue;
			}

			TCHAR* pPidIndex = _tcsstr(pVidIndex + 4, TEXT("PID_"));
			if (pPidIndex == NULL)
			{
				continue;
			}

			USHORT VendorID = (USHORT)_tcstoul(pVidIndex + 4, NULL, 16);
			USHORT ProductID = (USHORT)_tcstoul(pPidIndex + 4, NULL, 16);

			//������ں�  

			TCHAR fname[56] = { 0 };

			SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData,
				SPDRP_HARDWAREID,
				0, (PBYTE)fname,
				sizeof(fname),
				NULL);

			//�ж���ӦVID�ŵĴ���  

			//�ַ�������ʶ��FTDI  
			string str1(57, '0');
			string s1 = ssin;
			for (int i = 0; i < 56; i++)
			{
				str1[i] = (char)fname[i];
			}


			if (str1.find(s1) == string::npos) continue;



			//���fname  
			for (int i = 0; i < 56; i++)
			{
				fname[i] = 0;
			}

			//  ������ں�  
			SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData,
				SPDRP_FRIENDLYNAME,
				0, (PBYTE)fname,
				sizeof(fname),
				NULL);

			string str2(57, '0');
			string s2 = "COM";
			for (int i = 0; i < 56; i++)
			{
				str2[i] = (char)fname[i];
			}
			if (str2.find(s2) != string::npos)
			{
				int num = str2.find(s2);
				int end = str2.find(")");
				string StrCom = str2.substr(num, end - num);
				//cout << "�ҵ���COM��Ϊ" << StrCom << endl;
				strcpy_s(pValue->coms[index++], 8, StrCom.c_str());
			}
		}
	}
	pValue->num = index;
	if (pVidPid)
	{
		delete pVidPid;
		pVidPid = NULL;
	}
	return 1;
}