#ifndef MANAGER_H
#define MANAGER_H

#include "MySQL.h"
#include "../lib/radio/common.h"
#include "../lib/radio/DataScheduling.h"

class CSerialDongle;
class CSound;
class CTool;
class CWLNet;

extern BOOL g_bPTT;      //Set or cleared by user interface.

extern CSerialDongle* g_pDongle;
extern CSound* g_pSound;
extern CWLNet* g_pNet;

extern BOOL g_dongleIsUsing;

class CManager
{
public:
	CManager(CMySQL *db, CDataScheduling *pMnis,std::wstring& defaultAudioPath);
	~CManager();
	/*������־�ص�����*/
	int setLogPtr(PLogReport log_handel);
	/*����ͨ��*/
	int initialCall(unsigned long targetId, unsigned char callTyp);
	/*�ɿ�PTT*/
	int stopCall(bool needRequest = true);
	/*�������*/
	void RequireDecodeEvent();
	/*�������*/
	void ReleaseDecodeEvent();
	/*�Ͽ������м̵�����*/
	int disConnect();
	/*����ͨ���������ļ�����*/
	int SendFile(unsigned int length, char* pData);
	//HANDLE getDecodeEvent();
	/*���õ�ǰ�����Ĳ���ģʽ*/
	int setPlayCallOfCare(unsigned char calltype, unsigned long targetId);
	/*���ò���*/
	int config(REMOTE_TASK* pTask);
	void startHandleRemoteTask();
	void handleRemoteTask();
	//void handleVoiceLog(const VOICE_LOG& param);
	void stop();
	void getCurrentTask(REMOTE_TASK &task);
	/*������ʱ����Ҫ���ز������������*/
	void setCurrentTask(REMOTE_TASK* value);
	/*client����*/
	static void OnConnect(CRemotePeer* pRemotePeer);
	/*client�Ͽ�*/
	static void OnDisConnect(CRemotePeer* pRemotePeer);
	/*MNIS�ص�����*/
	static void OnData(int callFuncId, Respone response);
	/*���������豸��Ϣ*/
	int updateOnLineRadioInfo(int radioId, int status, int gpsQueryMode = -1);
	/*�жϵ�ǰ�Ƿ��Ѵ��ڴ�����*/
	bool isRepeat(std::string sessionId);
	/*�ж��Ƿ�͵�ǰ�����ظ�*/
	void handleIsRepeatCurTask(std::string sessionId, bool &rlt);
	/*�����Ƿ�ֹͣͨ��*/
	void handleStopCall();
	/*�����Ƿ�ֹͣͨ��*/
	void setbNeedStopCall(bool value);
	/*��ȡ��ǰ����������Ϣ*/
	void getCurrentTaskInfo(int srcId, std::string &sessionid, int &cmd);
	/*��ǰ���ڵĴ����������*/
	std::mutex m_mutexCurTask;

	int DongCount();
	void setDongleCount(int value);

	int MicphoneStatus();
	void setMicphoneStatus(int value);

	int SpeakerStatus();
	void setSpeakerStatus(int value);

	int LEStatus();
	void setLEStatus(int value);

	int WireLanStatus();
	void setWireLanStatus(int value);

	int DeviceInfoStatus();
	void setDeviceInfoStatus(int value);

	int MnisStatus();
	void setMnisStatus(int value);

	void OnUpdateUsb(DWORD type);

	static void OnUpdateUsbService(bool type);
private:
	PLogReport m_report;
	//HWND m_hwnd;
	char m_reportMsg[WL_LOG_SIZE];
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

	REMOTE_TASK m_currentTask;

	CDataScheduling* m_pMnis;
	UINT m_idTaskOnTimerProc;
	bool m_bNeedStopCall;
	int m_dongleCount;//����dongle����
	int m_micphoneStatus;//��˷��Ƿ����ӳɹ�
	int m_speakerStatus;//�������Ƿ����ӳɹ�
	int m_lEStatus;//LE�Ƿ����ӳɹ�
	int m_wireLanStatus;//wirelan�Ƿ�ע��ɹ�
	int m_deviceInfoStatus;//���к��Ƿ����
	int m_mnisStatus;

	static void OnCall(void* param, oncall_info_t* info);
	void onCall(oncall_info_t* info);
	static void OnCallStatus(void* param,oncallstatus_info_t* info);
	void onCallStatus(oncallstatus_info_t* info);
	static void OnSystemStatusChange(void* param,onsystemstatuschange_info_t* info);
	void onSystemStatusChange(onsystemstatuschange_info_t* info);
	static unsigned __stdcall HandleRemoteTaskProc(void * pThis);
	/*�жϵ�ǰ����ʱ�߳�*/
	static void PASCAL TaskOnTimerProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
	/*����ʱ����*/
	void handleTaskOnTimerProc();
	/*��֤��ǰ�����Ƿ�ʱ*/
	void handleCurTaskTimeOut();
	bool isSameSessionId(std::string sessionId, REMOTE_TASK* p);
	void initialize();
	void handleUsbAdd();
	void handleUsbDel();
};



#endif