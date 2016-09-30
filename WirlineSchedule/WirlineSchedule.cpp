// WirlineSchedule.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "Manager.h"
#include "MySQL.h"


//#ifndef DLLEXPORT
//#define DLLEXPORT extern "C" _declspec(dllexport)
//#endif
//static CManager manager;
///*����ָ����AMBE����*/
//DLLEXPORT bool Play(unsigned int length, char* pData)
//{
//
//	if (length == 0
//		|| NULL == pData)
//	{
//		return false;
//	}
//
//	int rlt = manager.play(length, pData);
//	if (0 != rlt)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//DLLEXPORT int SetLogPtr(PLogReport logHandel)
//{
//	return manager.setLogPtr(logHandel);
//}
//
///*�ر�*/
//DLLEXPORT bool DisConnect()
//{
//	int rlt = manager.disConnect();
//	if (0 != rlt)
//	{
//		return false;
//	}
//	return true;
//}
//
//#if WXJ_DLL
///************************************************************************/
///* dll for wxj
///************************************************************************/
///*���Ӳ���ʼ��������*/
//DLLEXPORT bool Connect(HWND current_hwnd, unsigned int serial_port)
//{
//	int rlt = manager.initWnd(current_hwnd);
//	if (rlt != 0)
//	{
//		return false;
//	}
//	rlt = manager.initDongle(serial_port);
//	if (rlt != 0)
//	{
//		return false;
//	}
//	return true;
//}
//#else
//DLLEXPORT int InitSys(char* master_ip, //���м�IP
//	unsigned int master_port, //���м�UDP�˿�
//	unsigned int local_id, //Consle ID
//	unsigned int local_radio_id,// Consle RadioId
//	//unsigned int master_id, //���м�ID
//	unsigned int record_type, //¼��ģʽ
//	unsigned int local_slot, //Consle �ŵ�
//	unsigned int local_group, //Consle ��
//	unsigned int serial_port //dongle �˿�
//	)
//{
//	return manager.initSys(master_ip, //���м�IP
//		master_port, //���м�UDP�˿�
//		local_id, //Consle ID
//		local_radio_id,// Consle RadioId
//		//master_id, //���м�ID
//		record_type, //¼��ģʽ
//		local_slot, //Consle �ŵ�
//		local_group, //Consle ��
//		serial_port //dongle �˿�
//		);
//}
//
//DLLEXPORT int  InitWnd(HWND current_hwnd)
//{
//	return manager.initWnd(current_hwnd);
//}
//
//DLLEXPORT int StablePlay()
//{
//	return manager.play();
//}
//
//DLLEXPORT int Record(unsigned long tartgetId, unsigned char callType)
//{
//	return manager.initialCall(tartgetId, callType);
//}
//
//DLLEXPORT int StopRecord()
//{
//	return manager.stopRecord();
//}
//
//DLLEXPORT int SendFile(unsigned int length, char* pData)
//{
//	return manager.SendFile(length, pData);
//}
//#endif


#define MASTER_IP			"192.168.2.121"
#define MASTER_PORT			50000
#define LOCAL_PEER_ID		120
#define LOCAL_RADIO_ID		5
#define RECORD_TYPE			CPC
#define DEFAULT_SLOT		SLOT1
#define DEFAULT_GROUP		9
#define DONGLE_PORT			8
#define DEFAULT_CALLTYPE	0x4f

#define DB_HOST				"127.0.0.1"
#define DB_PORT				3306
#define DB_USER				"root"
#define DB_PWD				""
#define DB_NAME				"WirelineMotoVoice"

HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.
	HWND hwndFound;         // This is what is returned to the caller.
	wchar_t pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
	// WindowTitle.
	wchar_t pszOldWindowTitle[MY_BUFSIZE]; // Contains original
	// WindowTitle.

	// Fetch current window title.

	GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

	// Format a "unique" NewWindowTitle.

	wsprintf(pszNewWindowTitle, L"%d/%d",
		GetTickCount(),
		GetCurrentProcessId());

	// Change current window title.

	SetConsoleTitle(pszNewWindowTitle);

	// Ensure window title has been updated.

	Sleep(40);

	// Look for NewWindowTitle.

	hwndFound = FindWindow(NULL, pszNewWindowTitle);

	// Restore original window title.

	SetConsoleTitle(pszOldWindowTitle);

	return(hwndFound);
}


void handleLog(char *pLog)
{
	SYSTEMTIME now = { 0 };
	GetLocalTime(&now);
	printf_s("%04u-%02u-%02u %02u:%02u:%02u %03u %s\n", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds, pLog);
}

int main()
{
	/*�ֲ���������*/
	int returnValue;
	CMySQL m_db;
	CManager *m_pManager = new CManager(&m_db);
	int cmd = 0;
	bool run = true;
	PLogReport m_report = handleLog;
	/*��ǰ�����ȡ*/
	HWND m_hwnd = GetConsoleHwnd();
	/*��ʼ����־�ص�*/
	m_pManager->setLogPtr(m_report);
	m_db.SetLogPtr(m_report);
	/*��ʼ�����ݿ�*/
	BOOL ret = m_db.Open(DB_HOST, DB_PORT, DB_USER, DB_PWD, DB_NAME);
	if (!ret)
	{
		handleLog("open data server fail");
		char temp = 0x00;
		scanf_s("%c", &temp, 1);
		return 0;
	}
	/*��ʼ�����硢Dongle��Sound*/
	returnValue = m_pManager->initWnd(m_hwnd);
	if (returnValue != 0)
	{
		handleLog("Initialization system fail");
		char temp = 0x00;
		scanf_s("%c", &temp, 1);
		return 0;
	}
	else
	{
		handleLog("Initialization system success");

	}
	returnValue = m_pManager->initSys(MASTER_IP, MASTER_PORT, LOCAL_PEER_ID, LOCAL_RADIO_ID, RECORD_TYPE, DEFAULT_SLOT, DEFAULT_GROUP, DONGLE_PORT);
	if (returnValue != 0)
	{
		handleLog("Initialization system fail");
		char temp = 0x00;
		scanf_s("%c", &temp, 1);
		return 0;
	}
	else
	{
		handleLog("Initialization system success");
	}
	/*����˵�ѡ��*/
	while (run)
	{
		printf_s("/************/\n/* 1.����PTT\n/* 2.�ɿ�PTT\n/* 3.¼���ط�\n/* 0.�˳�\n/************/\n\r");
		scanf_s("%d", &cmd, 1);
		switch (cmd)
		{
		case 0x01:
		{
					 m_pManager->initialCall(DEFAULT_GROUP, DEFAULT_CALLTYPE);
		}
			break;
		case 0x02:
		{
					 m_pManager->stopRecord();
		}
			break;
		case 0x03:
		{
					 /*����ָ������*/
					 m_pManager->play();
		}
			break;
		default:
		{
				   run = false;
		}
			break;
		}
	}
	return 0;
}