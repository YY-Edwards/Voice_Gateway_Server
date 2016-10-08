#ifndef MANAGER_H
#define MANAGER_H

#include "MySQL.h"

class CSerialDongle;
class CSound;
class CTool;
class CWLNet;

extern CTool g_tool;
extern BOOL g_bPTT;      //Set or cleared by user interface.
//extern BOOL g_bTX;       //Set or cleared by dongle.

extern CSerialDongle* g_pDongle;
extern CSound* g_pSound;
//temp delete
extern CWLNet* g_pNet;

extern BOOL g_dongleIsUsing;

class CManager
{
public:
	CManager(CMySQL *db);
	~CManager();
	int initSys(char* master_ip, unsigned int master_port, unsigned int local_id, unsigned int local_radio_id, unsigned int record_type, unsigned int local_slot, unsigned int local_group, unsigned int serial_port);
	int initWnd(HWND current_hwnd);
	int setLogPtr(PLogReport log_handel);

	int play(); 
	int initialCall(unsigned long tartgetId, unsigned char callType);
	int stopRecord();

	void RequireDecodeEvent();
	void ReleaseDecodeEvent();

	int initDongle(unsigned int serial_port);

	int play(unsigned int length, char* pData);
	int LoadVoiceData(unsigned int length, char* pData);
	int disConnect();
	int SendFile(unsigned int length, char* pData);
private:
	PLogReport m_report;
	HWND m_hwnd;
	char m_reportMsg[512];
	int m_activePort;
	TIMECAPS m_theTimeCaps;

	void sendLogToWindow();
	void LoadVoiceData(LPCWSTR filePath);

	HANDLE m_hWaitDecodeEvent;
	BOOL m_bDongleIsOpen;

	CMySQL *m_pDb;
};



#endif