#ifndef MANAGER_H
#define MANAGER_H

#include "NS/include/MySQL.h"
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
	/*设置日志回调方法*/
	int setLogPtr(PLogReport log_handel);
	/*建立通话*/
	int initialCall(unsigned long targetId, unsigned char callTyp);
	/*松开PTT*/
	int stopCall(bool needRequest = true);
	/*解码加锁*/
	void RequireDecodeEvent();
	/*解码解锁*/
	void ReleaseDecodeEvent();
	/*断开与主中继的连接*/
	int disConnect();
	/*建立通话并发送文件语音*/
	int SendFile(unsigned int length, char* pData);
	//HANDLE getDecodeEvent();
	/*设置当前语音的播放模式*/
	int setPlayCallOfCare(unsigned char calltype, unsigned long targetId);
	/*配置参数*/
	int config(REMOTE_TASK* pTask);
	void startHandleRemoteTask();
	void handleRemoteTask();
	//void handleVoiceLog(const VOICE_LOG& param);
	void stop();
	void getCurrentTask(REMOTE_TASK &task);
	/*设置延时的需要返回操作结果的任务*/
	void setCurrentTask(REMOTE_TASK* value);
	/*client接入*/
	static void OnConnect(CRemotePeer* pRemotePeer);
	/*client断开*/
	static void OnDisConnect(CRemotePeer* pRemotePeer);
	/*MNIS回调方法*/
	static void OnData(int callFuncId, Respone response);
	/*更新在线设备信息*/
	int updateOnLineRadioInfo(int radioId, int status, int gpsQueryMode = -1);
	/*判断当前是否已存在此任务*/
	bool isRepeat(std::string sessionId);
	/*判断是否和当前任务重复*/
	void handleIsRepeatCurTask(std::string sessionId, bool &rlt);
	/*处理是否停止通话*/
	void handleStopCall();
	/*设置是否停止通话*/
	void setbNeedStopCall(bool value);
	/*获取当前任务的相关信息*/
	void getCurrentTaskInfo(int srcId, std::string &sessionid, int &cmd);
	/*当前正在的处理的任务锁*/
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
	/*是否已经获取过配置*/
	bool m_bIsHaveConfig;
	/*推送日志到回调*/
	void sendLogToWindow();

	REMOTE_TASK m_currentTask;

	CDataScheduling* m_pMnis;
	UINT m_idTaskOnTimerProc;
	bool m_bNeedStopCall;
	int m_dongleCount;//当期dongle数量
	int m_micphoneStatus;//麦克风是否连接成功
	int m_speakerStatus;//扬声器是否连接成功
	int m_lEStatus;//LE是否连接成功
	int m_wireLanStatus;//wirelan是否注册成功
	int m_deviceInfoStatus;//序列号是否更新
	int m_mnisStatus;

	static void OnCall(void* param, oncall_info_t* info);
	void onCall(oncall_info_t* info);
	static void OnCallStatus(void* param,oncallstatus_info_t* info);
	void onCallStatus(oncallstatus_info_t* info);
	static void OnSystemStatusChange(void* param,onsystemstatuschange_info_t* info);
	void onSystemStatusChange(onsystemstatuschange_info_t* info);
	static unsigned __stdcall HandleRemoteTaskProc(void * pThis);
	/*判断当前任务超时线程*/
	static void PASCAL TaskOnTimerProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
	/*处理超时任务*/
	void handleTaskOnTimerProc();
	/*验证当前任务是否超时*/
	void handleCurTaskTimeOut();
	bool isSameSessionId(std::string sessionId, REMOTE_TASK* p);
	void initialize();
	void handleUsbAdd();
	void handleUsbDel();
};



#endif