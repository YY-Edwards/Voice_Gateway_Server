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
	/*初始化系统*/
	int initSys();
	/*获取句柄*/
	int initWnd(HWND current_hwnd);
	/*设置日志回调方法*/
	int setLogPtr(PLogReport log_handel);
	/*播放默认文件语音*/
	int play();
	/*建立通话*/
	int initialCall(char* pTartgetId, char* pCallType);
	/*松开PTT*/
	int stopCall();
	/*解码加锁*/
	void RequireDecodeEvent();
	/*解码解锁*/
	void ReleaseDecodeEvent();
	/*初始化dongle*/
	int initDongle(unsigned int serial_port);
	/*播放指定数据*/
	int play(unsigned int length, char* pData);
	/*加载指定音频数据*/
	int LoadVoiceData(unsigned int length, char* pData);
	/*断开与主中继的连接*/
	int disConnect();
	/*建立通话并发送文件语音*/
	int SendFile(unsigned int length, char* pData);
	//HANDLE getDecodeEvent();
	/*设置当前语音的播放模式*/
	int setPlayCallOfCare(char* pCallType,char* pFrom, char* pTarget);
	/*配置参数*/
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

	/*推送日志到回调*/
	void sendLogToWindow();
	/*加载指定文件的语音数据*/
	void LoadVoiceData(LPCWSTR filePath);
};



#endif