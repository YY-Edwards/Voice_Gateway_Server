#ifndef MANAGER_H
#define MANAGER_H

#include "MySQL.h"
#include "../lib/radio/common.h"
#include "../lib/radio/DataScheduling.h"

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
	CManager(CMySQL *db,CDataScheduling *pMnis);
	~CManager();
// 	/*��ʼ��ϵͳ*/
// 	int initSys();
	/*��ȡ���*/
	int initWnd(HWND current_hwnd);
	/*������־�ص�����*/
	int setLogPtr(PLogReport log_handel);
// 	/*����Ĭ���ļ�����*/
// 	int play();
	/*����ͨ��*/
	int initialCall(unsigned long targetId, unsigned char callTyp);
	/*�ɿ�PTT*/
	int stopCall();
	/*�������*/
	void RequireDecodeEvent();
	/*�������*/
	void ReleaseDecodeEvent();
// 	/*��ʼ��dongle*/
// 	int initDongle(unsigned int serial_port);
// 	/*����ָ������*/
// 	int play(unsigned int length, char* pData);
// 	/*����ָ����Ƶ����*/
// 	int LoadVoiceData(unsigned int length, char* pData);
	/*�Ͽ������м̵�����*/
	int disConnect();
	/*����ͨ���������ļ�����*/
	int SendFile(unsigned int length, char* pData);
	//HANDLE getDecodeEvent();
	/*���õ�ǰ�����Ĳ���ģʽ*/
	int setPlayCallOfCare(unsigned char calltype,unsigned long targetId);
	/*���ò���*/
	int config(REMOTE_TASK* pTask);
	void startHandleRemoteTask();
	void handleRemoteTask();
	void stop();
	REMOTE_TASK* getCurrentTask();
	void freeCurrentTask();
	void applayCurrentTask();
	/*������ʱ����Ҫ���ز������������*/
	void setCurrentTask(REMOTE_TASK* value);
	/*client����*/
	static void OnConnect(CRemotePeer* pRemotePeer);
	/*client�Ͽ�*/
	static void OnDisConnect(CRemotePeer* pRemotePeer);
	/*MNIS�ص�����*/
	static void OnMnisCallBack(int callFuncId, Respone response);
	/*���������豸��Ϣ*/
	int updateOnLineRadioInfo(int radioId, int status, int gpsQueryMode = -1);
private:
	PLogReport m_report;
	HWND m_hwnd;
	char m_reportMsg[512];
	int m_activePort;
	TIMECAPS m_theTimeCaps;
	HANDLE m_hWaitDecodeEvent;
	//BOOL m_bDongleIsOpen;
	CMySQL *m_pDb;
	HANDLE m_hRemoteTaskThread;
	UINT m_remoteTaskThreadId;
	bool m_bRemoteTaskThreadRun;
	/*�Ƿ��Ѿ���ȡ������*/
	bool m_bIsHaveConfig;
	/*������־���ص�*/
	void sendLogToWindow();
// 	/*����ָ���ļ�����������*/
// 	void LoadVoiceData(LPCWSTR filePath);

	REMOTE_TASK *m_pCurrentTask;

	CDataScheduling* m_pMnis;

	static unsigned __stdcall HandleRemoteTaskProc(void * pThis);
};



#endif