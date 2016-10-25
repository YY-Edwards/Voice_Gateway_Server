#ifndef MANAGER_H
#define MANAGER_H

#include "MySQL.h"

class CSerialDongle;
class CSound;
class CTool;
class CWLNet;

extern CTool g_tool;
extern BOOL g_bPTT;      //Set or cleared by user interface.

extern CSerialDongle* g_pDongle;
extern CSound* g_pSound;
extern CWLNet* g_pNet;

extern BOOL g_dongleIsUsing;

class CManager
{
public:
	CManager(CMySQL *db);
	~CManager();
	/*��ʼ��ϵͳ*/
	int initSys();
	/*��ȡ���*/
	int initWnd(HWND current_hwnd);
	/*������־�ص�����*/
	int setLogPtr(PLogReport log_handel);
	/*����Ĭ���ļ�����*/
	int play();
	/*����ͨ��*/
	int initialCall(char* pTartgetId, char* pCallType);
	/*�ɿ�PTT*/
	int stopCall();
	/*�������*/
	void RequireDecodeEvent();
	/*�������*/
	void ReleaseDecodeEvent();
	/*��ʼ��dongle*/
	int initDongle(unsigned int serial_port);
	/*����ָ������*/
	int play(unsigned int length, char* pData);
	/*����ָ����Ƶ����*/
	int LoadVoiceData(unsigned int length, char* pData);
	/*�Ͽ������м̵�����*/
	int disConnect();
	/*����ͨ���������ļ�����*/
	int SendFile(unsigned int length, char* pData);
	//HANDLE getDecodeEvent();
	/*���õ�ǰ�����Ĳ���ģʽ*/
	int setPlayCallOfCare(char* pCallType,char* pFrom, char* pTarget);
	/*���ò���*/
	int config(char* pMasterIp
		,char* pMasterPort
		,char* pLocalPeerId
		,char* pLocalRadioId
		,char* pReccordType
		,char* pDefaultGroup
		,char* pDonglePort
		,char* pHuangTime
		,char* pMasterHeartTime
		,char* pPeerHearTime
		,char* pDefaultSlot);
private:
	PLogReport m_report;
	HWND m_hwnd;
	char m_reportMsg[512];
	int m_activePort;
	TIMECAPS m_theTimeCaps;
	HANDLE m_hWaitDecodeEvent;
	BOOL m_bDongleIsOpen;
	CMySQL *m_pDb;

	/*������־���ص�*/
	void sendLogToWindow();
	/*����ָ���ļ�����������*/
	void LoadVoiceData(LPCWSTR filePath);
};



#endif