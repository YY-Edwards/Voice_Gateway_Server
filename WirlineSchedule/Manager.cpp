
#include "stdafx.h"
#include <process.h>
#include "Manager.h"
#include "SerialDongle.h"
#include "Sound.h"
#include "WLNet.h"
#include "actionHandler.h"
//#include "NS/include/WDK_VidPidQuery.h"
//#include "NS/include/NSManager.h"
//#include "NS/include/NSWLNet.h"
//#include "NS/include/NSLog.h"
//#include "NS/include/NSSound.h"
#include "NS/include/NS.h"
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

//temp delete
//#include "Net.h"

BOOL g_bPTT;      //Set or cleared by user interface.
BOOL g_bTX;       //Set or cleared by dongle.

CSerialDongle* g_pDongle;
BOOL g_dongleIsUsing;

CManager::CManager(CMySQL *pDb, CDataScheduling* pMnis, std::wstring& defaultAudioPath)
:m_idTaskOnTimerProc(0)
, m_bNeedStopCall(false)
, m_dongleCount(0)
, m_micphoneStatus(WL_SYSTEM_DISCONNECT)
, m_speakerStatus(WL_SYSTEM_DISCONNECT)
, m_lEStatus(WL_SYSTEM_DISCONNECT)
, m_wireLanStatus(WL_REGISTER_FAL)
, m_deviceInfoStatus(WL_SERIL_FAL)
, m_mnisStatus(WL_SYSTEM_DISCONNECT)
{
	//g_pNSTool = new CTool();
	//g_pNSManager = new NSManager();
	//g_pNSSound = new NSSound();
	//g_pNSNet = new NSWLNet(g_pNSManager);
	/*注册事件*/
	//NS_RegCallEvent(this, &OnCall);
	//NS_RegCallStatusEvent(this, &OnCallStatus);
	//NS_RegSystemStatusChangeEvent(this, &OnSystemStatusChange);
	m_pModelNs = new NS(this, &OnCall, &OnCallStatus, &OnSystemStatusChange);

	g_pNet = new CWLNet(pDb, this, defaultAudioPath);
	g_pDongle = new CSerialDongle();
	g_pSound = new CSound();
	g_bPTT = FALSE;
	g_bTX = FALSE;

	m_report = NULL;
	//m_hwnd = NULL;
	m_activePort = 0;
	m_hWaitDecodeEvent = NULL;

	memset(&m_theTimeCaps, 0, sizeof(TIMECAPS));
	if (!FAILED(timeGetDevCaps(&m_theTimeCaps, sizeof(TIMECAPS))))
	{
		timeBeginPeriod(m_theTimeCaps.wPeriodMin);//建立最小定时器精度
	}
	//m_bDongleIsOpen = FALSE;
	g_dongleIsUsing = FALSE;
	m_pDb = pDb;
	m_hRemoteTaskThread = NULL;
	m_remoteTaskThreadId = 0;
	m_bRemoteTaskThreadRun = false;
	m_bIsHaveConfig = false;
	memset(&m_currentTask, 0, sizeof(REMOTE_TASK));
	//m_pCurrentTask = NULL;
	m_pMnis = pMnis;
	m_hWaitDecodeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	/*获取当前Dongle的数量*/
	initialize();
}

CManager::~CManager()
{
	m_report = NULL;
	if (m_theTimeCaps.wPeriodMin != 0)
	{
		timeEndPeriod(m_theTimeCaps.wPeriodMin);//清除最小定时器精度
	}
	if (m_idTaskOnTimerProc != 0)
	{
		timeKillEvent(m_idTaskOnTimerProc);
		m_idTaskOnTimerProc = 0;
	}
	if (m_pModelNs)
	{
		delete m_pModelNs;
		m_pModelNs = NULL;
	}
	stop();
	///*注销事件*/
	//NS_UnregCallEvent();
	//NS_UnregCallStatusEvent();
	//NS_UnregSystemStatusChangeEvent();

	//if (g_pNSSound)
	//{
	//	delete g_pNSSound;
	//	g_pNSSound = NULL;
	//}
	//if (g_pNSManager)
	//{
	//	delete g_pNSManager;
	//	g_pNSManager = NULL;
	//}
	//if (g_pNSNet)
	//{
	//	if (WL == g_repeater_net_mode)
	//	{
	//		delete (NSWLNet*)g_pNSNet;
	//		g_pNSNet = NULL;
	//	}
	//}
	//if (g_pNSTool)
	//{
	//	delete g_pNSTool;
	//	g_pNSTool = NULL;
	//}
}

// int CManager::initSys()
// {
// 	WCHAR tmpStr[128] = { 0 };
// 	LPCTSTR lpctTmpStr = tmpStr;
// 	DWORD rlt = 0;
// 	BOOL netRlt = FALSE;
// 
// 	//init net
// 	g_net_connect = g_pNet->StartNet(inet_addr(CONFIG_MASTER_IP), CONFIG_MASTER_PORT, INADDR_ANY, CONFIG_LOCAL_PEER_ID, CONFIG_LOCAL_RADIO_ID, CONFIG_RECORD_TYPE);
// 	if (!g_net_connect)
// 	{
// 		//m_bDongleIsOpen = FALSE;
// 		sprintf_s(m_reportMsg, "initDongle:open net fail");
// 	}
// 	else
// 	{
// 		//m_bDongleIsOpen = TRUE;
// 		sprintf_s(m_reportMsg, "initDongle:open net success");
// 		ReleaseDecodeEvent();
// 	}
// 	sendLogToWindow();
// 
// 	initDongle(CONFIG_DONGLE_PORT);
// 
// 	return 0;
// }

//int CManager::initWnd(HWND current_hwnd)
//{
//	// 	sprintf_s(m_reportMsg, "initDongle:current_hwnd:0x%x", current_hwnd);
//	// 	sendLogToWindow();
//	
//	m_hwnd = current_hwnd;
//	return 0;
//}

int CManager::setLogPtr(PLogReport log_handel)
{

	m_report = log_handel;

	g_pDongle->SetLogPtr(log_handel);
	g_pSound->SetLogPtr(log_handel);
	g_pNet->SetLogPtr(log_handel);
	return 0;
}

void CManager::sendLogToWindow()
{
	//SYSTEMTIME now = { 0 };
	//GetLocalTime(&now);
	//printf_s("%04u-%02u-%02u %02u:%02u:%02u %03u %s\n", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds, m_reportMsg);
	if (NULL != m_report)
	{
		m_report(m_reportMsg);
	}
}

// int CManager::play()
// {
// 	DWORD result = 0;
// 	if (m_bDongleIsOpen)
// 	{
// 		result = WaitForSingleObject(m_hWaitDecodeEvent, INFINITE);
// 		RequireDecodeEvent();
// 		if (WAIT_TIMEOUT == result)
// 		{
// 			return 1;
// 		}
// 
// 		if (g_pDongle->changeAMBEToPCM())
// 		{
// 			LoadVoiceData(VOICE_DATA_PATH);
// 		}
// 		else
// 		{
// 			ReleaseDecodeEvent();
// 			return 1;
// 		}
// 	}
// 	return 0;
// }

// int CManager::play(unsigned int length, char* pData)
// {
// 	DWORD result = 0;
// 	if (m_bDongleIsOpen)
// 	{
// 		result = WaitForSingleObject(m_hWaitDecodeEvent, INFINITE);
// 		RequireDecodeEvent();
// 		if (WAIT_TIMEOUT == result)
// 		{
// 			return 1;
// 		}
// 
// 		if (g_pDongle->changeAMBEToPCM())
// 		{
// 			return LoadVoiceData(length, pData);
// 		}
// 		else
// 		{
// 			ReleaseDecodeEvent();
// 			return 1;
// 		}
// 	}
// 	return 0;
// }

// void CManager::LoadVoiceData(LPCWSTR filePath)
// {
// 	//	FILE * f;
// 
// 	DWORD result = 0;
// 
// 	HANDLE hOpenFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
// 	if (hOpenFile == INVALID_HANDLE_VALUE)
// 	{
// 		g_dongleIsUsing = FALSE;
// 		ReleaseDecodeEvent();
// 		sprintf_s(m_reportMsg, "open file fail");
// 		sendLogToWindow();
// 		return;
// 	}
// 	DWORD len = 0;
// 	len = GetFileSize(hOpenFile, 0);
// 
// 
// 	if (len <= 0)
// 	{
// 		g_dongleIsUsing = FALSE;
// 		ReleaseDecodeEvent();
// 		sprintf_s(m_reportMsg, "file length is zero");
// 		sendLogToWindow();
// 		CloseHandle(hOpenFile);
// 		return;
// 	}
// 
// 	char *pBuffer = new char[len];
// 	ReadFile(hOpenFile, pBuffer, len, &len, NULL);
// 	CloseHandle(hOpenFile);
// 
// 	int leftLen = len;
// 
// 	tAMBEFrame* pAMBEFrame;
// 
// 	pAMBEFrame = g_pDongle->GetFreeAMBEBuffer();
// 	int readLen = (leftLen >= 7) ? 7 : leftLen;
// 	memcpy(pAMBEFrame->fld.ChannelBits, pBuffer, readLen);
// 	leftLen -= readLen;
// 	int index = readLen;
// 
// 	while (readLen == 7)
// 	{
// 		//sprintf_s(m_reportMsg, "1");
// 		//sendLogToWindow();
// 		g_pDongle->deObfuscate(IPSCTODONGLE, pAMBEFrame);
// 		g_pDongle->MarkAMBEBufferFilled();
// 		pAMBEFrame = g_pDongle->GetFreeAMBEBuffer();
// 		if (NULL == pAMBEFrame)
// 		{
// 			g_dongleIsUsing = FALSE;
// 			delete[] pBuffer;
// 			ReleaseDecodeEvent();
// 			return;
// 		}
// 		readLen = (leftLen >= 7) ? 7 : leftLen;
// 		if (readLen <= 0)
// 		{
// 			break;
// 		}
// 		memcpy(pAMBEFrame->fld.ChannelBits, pBuffer + index, readLen);
// 		leftLen -= readLen;
// 		index += readLen;
// 	}
// 
// 
// 	delete[] pBuffer;
// 	g_pDongle->DecodeBuffers();
// }

// int CManager::LoadVoiceData(unsigned int length, char* pData)
// {
// 
// 	char *pBuffer = new char[length];
// 	memcpy(pBuffer, pData, length);
// 
// 	int leftLen = length;
// 
// 	tAMBEFrame* pAMBEFrame;
// 
// 	pAMBEFrame = g_pDongle->GetFreeAMBEBuffer();
// 	int readLen = (leftLen >= 7) ? 7 : leftLen;
// 	memcpy(pAMBEFrame->fld.ChannelBits, pBuffer, readLen);
// 	leftLen -= readLen;
// 	int index = readLen;
// 
// 	while (readLen == 7)
// 	{
// 		//sprintf_s(m_reportMsg, "2");
// 		//sendLogToWindow();
// 		g_pDongle->deObfuscate(IPSCTODONGLE, pAMBEFrame);
// 		g_pDongle->MarkAMBEBufferFilled();
// 		pAMBEFrame = g_pDongle->GetFreeAMBEBuffer();
// 		if (NULL == pAMBEFrame)
// 		{
// 			g_dongleIsUsing = FALSE;
// 			delete[] pBuffer;
// 			ReleaseDecodeEvent();
// 			return 1;
// 		}
// 		readLen = (leftLen >= 7) ? 7 : leftLen;
// 		if (readLen <= 0)
// 		{
// 			break;
// 		}
// 		memcpy(pAMBEFrame->fld.ChannelBits, pBuffer + index, readLen);
// 		leftLen -= readLen;
// 		index += readLen;
// 	}
// 
// 
// 	delete[] pBuffer;
// 	g_pDongle->DecodeBuffers();
// 	return 0;
// }

void CManager::ReleaseDecodeEvent()
{
	SetEvent(m_hWaitDecodeEvent);
}



int CManager::initialCall(unsigned long targetId, unsigned char callTyp)
{
	unsigned long tartgetId = targetId;
	unsigned char callType = callTyp;
	if (Env_DongleIsOk &&
		Env_SoundIsOk)
	{

		if (g_pDongle->changePCMToAMBE())
		{
			WORD callStatus = g_pNet->GetCallStatus();

			//callback
			if (callStatus == CALL_HANGUP
				&& targetId == CONFIG_CURRENT_TAGET)
			{
				g_bPTT = TRUE;
				sprintf_s(m_reportMsg, "call back");
				sendLogToWindow();
				return g_pNet->callBack();
			}
			//new call
			else if (callStatus == CALL_IDLE)
			{
				//CONFIG_DEFAULT_GROUP = tartgetId;
				CONFIG_CURRENT_TAGET = tartgetId;
				g_targetCallType = callType;
				g_bPTT = TRUE;
				sprintf_s(m_reportMsg, "new call");
				sendLogToWindow();
				return g_pNet->newCall();
			}
			//other call is running
			else
			{
				sprintf_s(m_reportMsg, "Other Call Is Running");
				sendLogToWindow();
				g_pNet->wlCallStatus(callTyp, CONFIG_LOCAL_RADIO_ID, targetId, STATUS_CALL_END | REMOTE_CMD_FAIL);
				return 1;
			}

		}
		else
		{
			g_pNet->wlCallStatus(callTyp, CONFIG_LOCAL_RADIO_ID, targetId, STATUS_CALL_END | REMOTE_CMD_FAIL);
			return 1;
		}
		return 0;
	}
	else
	{
		g_pNet->wlCallStatus(callTyp, CONFIG_LOCAL_RADIO_ID, targetId, STATUS_CALL_END | REMOTE_CMD_FAIL);
		sprintf_s(m_reportMsg, "dongle is not open");
		sendLogToWindow();
		return 1;
	}
}

int CManager::stopCall(bool needRequest)
{
	sprintf_s(m_reportMsg, "handle stopCall");
	sendLogToWindow();
	if (g_pNet->canStopRecord())
	{
		if (needRequest) g_pNet->requestRecordEndEvent();
		g_pSound->setbRecord(FALSE);
		g_pNet->waitRecordEnd();
	}
	sprintf_s(m_reportMsg, "handle success");
	sendLogToWindow();
	return 0;
}

void CManager::RequireDecodeEvent()
{
	ResetEvent(m_hWaitDecodeEvent);
}

// int CManager::initDongle(unsigned int serial_port)
// {
// 
// 	DWORD rlt = 0;
// 	WCHAR tmpStr[128] = { 0 };
// 	LPCTSTR lpctTmpStr = tmpStr;
// 
// 	// 	sprintf_s(m_reportMsg, "initDongle:serial_port:COM%u", serial_port);
// 	// 	sendLogToWindow();
// 
// 	m_activePort = serial_port;
// 	swprintf_s(tmpStr, 128, L"\\\\.\\COM%d", m_activePort);
// 
// 	//init dongle
// 	if (m_hwnd != NULL)
// 	{
// 		rlt = g_pDongle->OpenDongle(lpctTmpStr, m_hwnd, this);
// 		if (rlt != 0)
// 		{
// 			m_bDongleIsOpen = FALSE;
// 			g_dongle_open = false;
// 			sprintf_s(m_reportMsg, "initDongle:open dongle fail");
// 		}
// 		else
// 		{
// 			m_bDongleIsOpen = TRUE;
// 			g_dongle_open = true;
// 			sprintf_s(m_reportMsg, "initDongle:open dongle success");
// 			ReleaseDecodeEvent();
// 		}
// 		sendLogToWindow();
// 	}
// 	else
// 	{
// 		sprintf_s(m_reportMsg, "initDongle:m_hwnd is null");
// 		sendLogToWindow();
// 		rlt = -1;
// 	}
// 
// 	return rlt;
// }

int CManager::disConnect()
{
	g_pNet->stop();
	g_pDongle->stop();
	g_pSound->stop();
	return 0;
}

int CManager::SendFile(unsigned int length, char* pData)
{
	return g_pNet->SendFile(length, pData);
}

// HANDLE CManager::getDecodeEvent()
// {
// 	return m_hWaitDecodeEvent;
// }

int CManager::setPlayCallOfCare(unsigned char calltype, unsigned long targetId)
{
	//g_playCalltype = calltype;
	m_pModelNs->setPlayCallType(calltype);
	if (PRIVATE_CALL == calltype)
	{
		//g_playTargetId = CONFIG_LOCAL_RADIO_ID;
		m_pModelNs->setPlayTargetId(CONFIG_LOCAL_RADIO_ID);
	}
	else if (ALL_CALL == calltype)
	{
		//g_playTargetId = ALL_CALL_ID;
		m_pModelNs->setPlayTargetId(ALL_CALL_ID);
	}
	else
	{
		//g_playTargetId = targetId;
		m_pModelNs->setPlayTargetId(targetId);
	}
	return g_pNet->setPlayCallOfCare(calltype, targetId);
}

int CManager::config(REMOTE_TASK* pTask)
{
	CONFIG *pConfig = &(pTask->param.info.configParam);
	int rlt = 0;
	bool bMasterChange = false;
	bool bDongleChange = false;
	bool bMnisChange = false;
	CONFIG_SCHDULE_ISENABLE = pConfig->reapeater.IsEnable;
	if (!wlScheduleIsEnable())
	{
		sprintf_s(m_reportMsg, "wlScheduleIsEnable is false");
		sendLogToWindow();
		return 0;
	}
	CONFIG_MNIS_ID = pConfig->mnis.ID;
	//g_pNet->setAudioPath(pConfig->reapeater.AudioPath);
	if (0 != strcmp(CONFIG_MASTER_IP, pConfig->reapeater.Master.ip))
	{
		strcpy_s(CONFIG_MASTER_IP, pConfig->reapeater.Master.ip);
		bMasterChange = true;
	}
	if (0 != strcmp(CONFIG_MNIS_IP, pConfig->mnis.Host))
	{
		strcpy_s(CONFIG_MNIS_IP, pConfig->mnis.Host);
		bMnisChange = true;
	}
	if (CONFIG_MASTER_PORT != pConfig->reapeater.Master.port)
	{
		CONFIG_MASTER_PORT = pConfig->reapeater.Master.port;
		bMasterChange = true;
	}
	if (CONFIG_LOCAL_PEER_ID != pConfig->reapeater.LocalPeerId)
	{
		CONFIG_LOCAL_PEER_ID = pConfig->reapeater.LocalPeerId;
		if (!bMasterChange)
		{
			//g_pNet->setWlStatus(STARTING);
		}
	}
	CONFIG_LOCAL_RADIO_ID = pConfig->reapeater.LocalRadioId;
	if (CONFIG_RECORD_TYPE != pConfig->reapeater.recordType)
	{
		CONFIG_RECORD_TYPE = pConfig->reapeater.recordType;
		if (!bMasterChange)
		{
			//g_pNet->setWlStatus(STARTING);
		}
	}
	CONFIG_DEFAULT_GROUP = pConfig->reapeater.DefaultGroupId;
	//g_playTargetId = CONFIG_DEFAULT_GROUP;
	m_pModelNs->setPlayCallType(GROUP_CALL);
	m_pModelNs->setPlayTargetId(CONFIG_DEFAULT_GROUP);
	if (CONFIG_DONGLE_PORT != pConfig->reapeater.Dongle.donglePort)
	{
		CONFIG_DONGLE_PORT = pConfig->reapeater.Dongle.donglePort;
		bDongleChange = true;
	}
	CONFIG_HUNG_TIME = pConfig->reapeater.MinHungTime;
	CONFIG_MASTER_HEART_TIME = pConfig->reapeater.MaxSiteAliveTime;
	CONFIG_PEER_HEART_AND_REG_TIME = pConfig->reapeater.MaxPeerAliveTime;
	CONFIG_DEFAULT_SLOT = pConfig->reapeater.DefaultChannel;

	if (!m_bIsHaveConfig)
	{
		//////////////////////////////////////////////////////////////////////////
		//程序启动后第一次获取配置
		//////////////////////////////////////////////////////////////////////////
		CONFIG_CURRENT_TAGET = CONFIG_DEFAULT_GROUP;
		StartNetParam param = { 0 };
		param.hang_time = (unsigned short)CONFIG_HUNG_TIME;
		param.local_peer_id = CONFIG_LOCAL_PEER_ID;
		param.local_port = 40000;
		param.local_radio_id = CONFIG_LOCAL_RADIO_ID;
		param.master_firewall_time = (unsigned short)CONFIG_MASTER_HEART_TIME;
		strcpy(param.master_ip, CONFIG_MASTER_IP);
		param.master_port = CONFIG_MASTER_PORT;
		param.peer_firewall_time = (unsigned short)CONFIG_PEER_HEART_AND_REG_TIME;
		param.work_mode = CONFIG_RECORD_TYPE;
		/*handle audio path*/
		HandleAudioPath(pConfig->reapeater.AudioPath);

		/*初始化网络*/
		//rlt = g_pNSNet->StartNet(&param);
		rlt = m_pModelNs->StartNet(&param);
		/*初始化Dongle*/
		//g_pNSManager->Initialize();
		m_pModelNs->InitializeDongleModel();
		//setDongleCount(g_pNSManager->SizeDongle());
		setDongleCount(m_pModelNs->SizeDongle());
		Env_DongleIsOk = (0 != DongCount());
		/*初始化声卡*/
		//if (0 != g_pNSSound->InitSoundIn())
		if (0 != m_pModelNs->InitSoundIn())
		{
			setMicphoneStatus(WL_SYSTEM_DISCONNECT);
		}
		else
		{
			setMicphoneStatus(WL_SYSTEM_CONNECT);
		}
		//if (0 != g_pNSSound->InitSoundOut())
		if (0 != m_pModelNs->InitSoundOut())
		{
			setSpeakerStatus(WL_SYSTEM_DISCONNECT);
		}
		else
		{
			setSpeakerStatus(WL_SYSTEM_CONNECT);
		}
		///*与主中继相连*/
		//if (!g_pNet->StartNet(inet_addr(CONFIG_MASTER_IP), CONFIG_MASTER_PORT, INADDR_ANY, CONFIG_LOCAL_PEER_ID, CONFIG_LOCAL_RADIO_ID, CONFIG_RECORD_TYPE))
		//{
		//	Env_NetIsOk = false;
		//	sprintf_s(m_reportMsg, "net initial fail");
		//	sendLogToWindow();
		//	g_pWLlog->sendLog("net initial fail");
		//}
		//else
		//{
		//	Env_NetIsOk = true;
		//	g_pWLlog->sendLog("net initial success");
		//}
		///*配置dongle*/
		//WCHAR tmpStr[128] = { 0 };
		//swprintf_s(tmpStr, 128, L"\\\\.\\COM%d", CONFIG_DONGLE_PORT);
		//if (WL_RETURN_OK != g_pDongle->OpenDongle(tmpStr, this))
		//{
		//	//m_bDongleIsOpen = FALSE;
		//	setDongleCount(0);
		//	Env_DongleIsOk = false;
		//	sprintf_s(m_reportMsg, "open dongle fail");
		//	sendLogToWindow();
		//	g_pWLlog->sendLog("open dongle fail");
		//	handleUsbAdd();
		//}
		//else
		//{
		//	//m_bDongleIsOpen = TRUE;
		//	setDongleCount(1);
		//	Env_DongleIsOk = true;
		//	sprintf_s(m_reportMsg, "open dongle success");
		//	sendLogToWindow();
		//	g_pWLlog->sendLog("open dongle success");
		//}
		///*配置麦克风和扬声器*/
		//if (WL_RETURN_OK != g_pSound->StartSound())
		//{
		//	Env_SoundIsOk = false;
		//	setMicphoneStatus(WL_SYSTEM_DISCONNECT);
		//	setSpeakerStatus(WL_SYSTEM_DISCONNECT);
		//	sprintf_s(m_reportMsg, "sound initial fail");
		//	sendLogToWindow();
		//	g_pWLlog->sendLog("sound initial fail");
		//}
		//else
		//{
		//	Env_SoundIsOk = true;
		//	setMicphoneStatus(WL_SYSTEM_CONNECT);
		//	setSpeakerStatus(WL_SYSTEM_CONNECT);
		//	sprintf_s(m_reportMsg, "sound initial success");
		//	sendLogToWindow();
		//	g_pWLlog->sendLog("sound initial success");
		//}
		/*配置mnis*/
		//m_pMnis->radioConnect(CONFIG_MNIS_IP);
		m_pMnis->radioConnect(pConfig->mnis, pConfig->location, pConfig->locationindoor);
		m_bIsHaveConfig = true;
	}
	else
	{
		NSLog::instance()->AddLog(Ns_Log_Error, "recive config,but this is not first load,please restart service");

		//if (!Env_SoundIsOk)
		//{
		//	g_pSound->stop();
		//	/*配置麦克风和扬声器*/
		//	if (WL_RETURN_OK != g_pSound->StartSound())
		//	{
		//		Env_SoundIsOk = false;
		//		sprintf_s(m_reportMsg, "sound initial fail");
		//		sendLogToWindow();
		//		g_pWLlog->sendLog("sound initial fail");

		//	}
		//	else
		//	{
		//		Env_SoundIsOk = true;
		//		sprintf_s(m_reportMsg, "sound initial success");
		//		sendLogToWindow();
		//		g_pWLlog->sendLog("sound initial success");
		//	}
		//}
		//if (bMasterChange)
		//{
		//	/*重新与主中继相连接*/
		//	if (!g_pNet->StartNet(inet_addr(CONFIG_MASTER_IP), CONFIG_MASTER_PORT, INADDR_ANY, CONFIG_LOCAL_PEER_ID, CONFIG_LOCAL_RADIO_ID, CONFIG_RECORD_TYPE))
		//	{
		//		Env_NetIsOk = false;
		//		sprintf_s(m_reportMsg, "net initial fail");
		//		sendLogToWindow();
		//		g_pWLlog->sendLog("net initial fail");
		//	}
		//	else
		//	{
		//		Env_NetIsOk = true;
		//		g_pWLlog->sendLog("net initial success");
		//	}
		//}
		//if (bDongleChange)
		//{
		//	/*重新配置dongle*/
		//	WCHAR tmpStr[128] = { 0 };
		//	swprintf_s(tmpStr, 128, L"\\\\.\\COM%d", CONFIG_DONGLE_PORT);
		//	if (WL_RETURN_OK != g_pDongle->OpenDongle(tmpStr, this))
		//	{
		//		//m_bDongleIsOpen = FALSE;
		//		setDongleCount(0);
		//		Env_DongleIsOk = false;
		//		sprintf_s(m_reportMsg, "initDongle:open dongle fail");
		//		sendLogToWindow();
		//		g_pWLlog->sendLog("dongle initial fail");
		//		handleUsbAdd();
		//	}
		//	else
		//	{
		//		//m_bDongleIsOpen = TRUE;
		//		setDongleCount(1);
		//		Env_DongleIsOk = true;
		//		sprintf_s(m_reportMsg, "open dongle success");
		//		sendLogToWindow();
		//		g_pWLlog->sendLog("dongle initial success");
		//	}
		//}
		//if (bMnisChange)
		//{
		//	/*配置mnis*/
		//	m_pMnis->radioConnect(pConfig->mnis, pConfig->location, pConfig->locationindoor);
		//}
	}
	return rlt;
}

void CManager::startHandleRemoteTask()
{
	if (m_bRemoteTaskThreadRun)
	{
		return;
	}
	m_hRemoteTaskThread = (HANDLE)_beginthreadex(NULL,         // security
		0,            // stack size
		HandleRemoteTaskProc,
		this,           // arg list
		CREATE_SUSPENDED,  // so we can later call ResumeThread()
		&m_remoteTaskThreadId);
	m_bRemoteTaskThreadRun = true;
	ResumeThread(m_hRemoteTaskThread);

}

unsigned __stdcall CManager::HandleRemoteTaskProc(void * pThis)
{
	CManager* p = (CManager*)pThis;
	if (p)
	{
		p->handleRemoteTask();
	}
	return 0;
}

void CManager::handleRemoteTask()
{
	REMOTE_TASK task = { 0 };
	//NSWLNet* pNSNet = NULL;
	while (m_bRemoteTaskThreadRun)
	{
		while (size_task() > 0)
		{
			/*处理任务*/
			get_front_task(task);
			erase_front_task();
			switch (task.cmd)
			{
			case REMOTE_CMD_CONFIG:
			{
									  NSLog::instance()->AddLog(Ns_Log_Info, "Handle REMOTE_CMD_CONFIG");
									  config(&task);
			}
				break;
			case REMOTE_CMD_CALL:
			{
									if (license_status_nopass == NS::LicenseStatus())
									{
										NSLog::instance()->AddLog(Ns_Log_Error, "REMOTE_CMD_CALL fail,license_status_nopass");
										g_pNet->wlCallStatus(task.param.info.callParam.operateInfo.callType, CONFIG_LOCAL_RADIO_ID, task.param.info.callParam.operateInfo.tartgetId, STATUS_CALL_END | REMOTE_CMD_FAIL);
										continue;
									}
									NSLog::instance()->AddLog(Ns_Log_Info, "Handle REMOTE_CMD_CALL");
									//mic_status_enum micStatus = g_pNSSound->MicStatus();
									mic_status_enum micStatus = m_pModelNs->MicStatus();
									setCurrentTask(&task);
									CALL_OPERATE_PARAM cmdInfo = task.param.info.callParam.operateInfo;
									//pNSNet = (NSWLNet*)g_pNSNet;
									//call_thread_status_enum callStatus = pNSNet->CallThreadStatus();
									call_thread_status_enum callStatus = m_pModelNs->CallThreadStatus();
									if (Call_Thread_Status_Idle == callStatus && Mic_Idle == micStatus)
									{
										//pNSNet->CurCallCmd = cmdInfo;
										make_call_param_t param = { 0 };
										param.callType = cmdInfo.callType;
										param.targetID = cmdInfo.tartgetId;
										//pNSNet->CallStart(&param);
										m_pModelNs->CallStart(&param);
									}
									else
									{
										NSLog::instance()->AddLog(Ns_Log_Info, "CallThreadStatus is not idle or MicStatus is not idle");
										g_pNet->wlCallStatus(task.param.info.callParam.operateInfo.callType, CONFIG_LOCAL_RADIO_ID, task.param.info.callParam.operateInfo.tartgetId, STATUS_CALL_END | REMOTE_CMD_FAIL);
									}
			}
				break;
			case REMOTE_CMD_SET_PLAY_CALL:
			{
											 if (license_status_nopass == NS::LicenseStatus())
											 {
												 NSLog::instance()->AddLog(Ns_Log_Error, "REMOTE_CMD_SET_PLAY_CALL fail,license_status_nopass");
												 g_pNet->wlPlayStatus(CMD_FAIL, task.param.info.setCareCallParam.playParam.targetId);
												 continue;
											 }
											 NSLog::instance()->AddLog(Ns_Log_Info, "Handle REMOTE_CMD_SET_PLAY_CALL");
											 setCurrentTask(&task);
											 setPlayCallOfCare(task.param.info.setCareCallParam.playParam.callType, task.param.info.setCareCallParam.playParam.targetId);
											 g_pNet->wlPlayStatus(CMD_SUCCESS, task.param.info.setCareCallParam.playParam.targetId);
			}
				break;
			case REMOTE_CMD_STOP_CALL:
			{
										 if (license_status_nopass == NS::LicenseStatus())
										 {
											 NSLog::instance()->AddLog(Ns_Log_Error, "REMOTE_CMD_STOP_CALL fail,license_status_nopass");
											 g_pNet->wlCallStatus(task.param.info.callParam.operateInfo.callType, task.param.info.callParam.operateInfo.source, task.param.info.callParam.operateInfo.tartgetId, STATUS_CALL_END | REMOTE_CMD_SUCCESS, task.param.info.callParam.operateInfo.SessionId);
											 continue;
										 }
										 NSLog::instance()->AddLog(Ns_Log_Info, "Handle REMOTE_CMD_STOP_CALL");
										 setCurrentTask(&task);
										 //pNSNet = (NSWLNet*)g_pNSNet;
										 //pNSNet->CurCallCmd = task.param.info.callParam.operateInfo;
										 //pNSNet->CallStop();
										 m_pModelNs->CallStop();

			}
				break;
			case REMOTE_CMD_GET_CONN_STATUS:
			{
											   NSLog::instance()->AddLog(Ns_Log_Info, "Handle REMOTE_CMD_GET_CONN_STATUS");
											   //pNSNet = (NSWLNet*)g_pNSNet;
											   FieldValue info(FieldValue::TInt);
											   //if (pNSNet->LeStatus() == ALIVE)
											   if (m_pModelNs->LeStatus() == ALIVE)
											   {
												   info.setInt(REPEATER_CONNECT);
											   }
											   else
											   {
												   info.setInt(REPEATER_DISCONNECT);
											   }
											   g_pNet->wlInfo(GET_TYPE_CONN, info, task.param.info.getInfoParam.getInfo.SessionId);
			}
				break;
			case REMOTE_CMD_SESSION_STATUS:
			{
											  NSLog::instance()->AddLog(Ns_Log_Info, "Handle REMOTE_CMD_SESSION_STATUS");
											  FieldValue value(FieldValue::TArray);
											  GET_INFO_PARAM info = task.param.info.getInfoParam.getInfo;
											  /*获取当前已经处理的会话任务状态*/
											  g_pNet->getSessionStatusList(value);
											  /*清空当前已经处理的会话任务状态*/
											  g_pNet->clearSessionStatusList();
											  g_pNet->wlInfo(GET_TYPE_SESSION_STATUS, value, info.SessionId);

			}
				break;
			case REMOTE_CMD_SYSTEM_STATUS:
			{
											 NSLog::instance()->AddLog(Ns_Log_Info, "Handle REMOTE_CMD_SYSTEM_STATUS");
											 GET_INFO_PARAM info = task.param.info.getInfoParam.getInfo;
											 FieldValue value(FieldValue::TObject);
											 value.setKeyVal("MnisStatus", FieldValue(MnisStatus()));//mnis状态-Tserver侧修改
											 value.setKeyVal("DongleCount", FieldValue(DongCount()));//dongle数量
											 value.setKeyVal("MicphoneStatus", FieldValue(MicphoneStatus()));//麦克风状态
											 value.setKeyVal("SpeakerStatus", FieldValue(SpeakerStatus()));//扬声器
											 value.setKeyVal("LEStatus", FieldValue(LEStatus()));//LE注册状态
											 value.setKeyVal("WireLanStatus", FieldValue(WireLanStatus()));//wirelan注册状态
											 value.setKeyVal("DeviceInfoStatus", FieldValue(DeviceInfoStatus()));//设备序列号
											 g_pNet->wlInfo(GET_TYPE_SYSTEM_STATUS, value, info.SessionId);

			}
				break;
			case REMOTE_CMD_MNIS_QUERY_GPS:
			{
											  if (license_status_nopass == NS::LicenseStatus())
											  {
												  NSLog::instance()->AddLog(Ns_Log_Error, "REMOTE_CMD_MNIS_QUERY_GPS fail,license_status_nopass");
												  continue;
											  }
											  NSLog::instance()->AddLog(Ns_Log_Info, "Handle REMOTE_CMD_MNIS_QUERY_GPS");
											  QUERY_GPS gps = task.param.info.queryGpsParam;
											  switch (gps.Operate)
											  {
											  case GPS_Immediate:
											  case GPS_start_Triggered:
											  {
																		  m_pMnis->radioGetGps(gps.Target, gps.Type, gps.Cycle, gps.SessionId, gps.Operate);//0.1
											  }
												  break;
											  case GPS_stop_Triggered:
											  {
																		 m_pMnis->radioStopGps(gps.Target, gps.Type, gps.SessionId);//2
											  }
												  break;
											  default:
												  break;
											  }
			}
				break;
			case REMOTE_CMD_MNIS_MSG:
			{
										if (license_status_nopass == NS::LicenseStatus())
										{
											NSLog::instance()->AddLog(Ns_Log_Error, "REMOTE_CMD_MNIS_MSG fail,license_status_nopass");
											continue;
										}
										NSLog::instance()->AddLog(Ns_Log_Info, "Handle REMOTE_CMD_MNIS_MSG");
										m_pMnis->radioSendMsg(task.param.info.msgParam.Contents, task.param.info.msgParam.Target, task.param.info.msgParam.Type, task.param.info.msgParam.SessionId);
			}
				break;
			case REMOTE_CMD_MNIS_STATUS:
			{
										   NSLog::instance()->AddLog(Ns_Log_Info, "Handle REMOTE_CMD_MNIS_STATUS");
										   switch (task.param.info.mnisStatusParam.getType)
										   {
										   case MNIS_GET_TYPE_CONNECT:
										   {
																		 FieldValue info(FieldValue::TInt);
																		 if (Env_MnisIsOk)
																		 {
																			 info.setInt(0);
																		 }
																		 else
																		 {
																			 info.setInt(1);
																		 }
																		 g_pNet->wlMnisStatus(MNIS_GET_TYPE_CONNECT, info, task.param.info.mnisStatusParam.SessionId);
										   }
											   break;
										   default:
										   {
													  if (license_status_nopass == NS::LicenseStatus())
													  {
														  NSLog::instance()->AddLog(Ns_Log_Error, "getRadioStatus fail,license_status_nopass");
														  continue;
													  }
													  NSLog::instance()->AddLog(Ns_Log_Info, "getRadioStatus call start");
													  m_pMnis->getRadioStatus(task.param.info.mnisStatusParam.getType, task.param.info.mnisStatusParam.SessionId);
													  NSLog::instance()->AddLog(Ns_Log_Info, "getRadioStatus call end");
										   }
											   break;
										   }
			}
				break;
				//case is no use
			case REMOTE_CMD_MNIS_LOCATION_INDOOR_CONFIG:
			{
														   if (license_status_nopass == NS::LicenseStatus())
														   {
															   NSLog::instance()->AddLog(Ns_Log_Error, "REMOTE_CMD_MNIS_LOCATION_INDOOR_CONFIG fail,license_status_nopass");
															   continue;
														   }
														   NSLog::instance()->AddLog(Ns_Log_Info, "Handle REMOTE_CMD_MNIS_LOCATION_INDOOR_CONFIG");
														   m_pMnis->locationIndoorConfig(task.param.info.locationParam.internal, task.param.info.locationParam.ibconNum, task.param.info.locationParam.isEmergency);
			}
				break;
			default:
				NSLog::instance()->AddLog(Ns_Log_Error, "Handle Unknown %d", task.cmd);
				break;
			}
		}
		WaitForSingleObject(g_waitHandleRemoteTask, INFINITE);
	}
}

void CManager::stop()
{
	m_bRemoteTaskThreadRun = false;
	if (m_hRemoteTaskThread)
	{
		SetEvent(g_waitHandleRemoteTask);
		WaitForSingleObject(m_hRemoteTaskThread, 1000);
		CloseHandle(m_hRemoteTaskThread);
		m_hRemoteTaskThread = NULL;
	}
}

void CManager::getCurrentTask(REMOTE_TASK &task)
{
	NSLog::instance()->AddLog(Ns_Log_Info, "Lock m_mutexCurTask");
	std::lock_guard <std::mutex> locker(m_mutexCurTask);
	task = m_currentTask;
	NSLog::instance()->AddLog(Ns_Log_Info, "UnLock m_mutexCurTask");
}

void CManager::setCurrentTask(REMOTE_TASK* value)
{
	NSLog::instance()->AddLog(Ns_Log_Info, "Lock m_mutexCurTask");
	std::lock_guard <std::mutex> locker(m_mutexCurTask);
	m_currentTask = *value;
	NSLog::instance()->AddLog(Ns_Log_Info, "UnLock m_mutexCurTask");
}

void CManager::OnConnect(CRemotePeer* pRemotePeer)
{
	if (pRemotePeer)
	{
		addCRemotePeer((TcpClient*)pRemotePeer);
		g_pNet->wlGetConfig();

		//发送serial to tserver
		g_pNet->wlSendSerial();
	}
}

void CManager::OnDisConnect(CRemotePeer* pRemotePeer)
{
	if (pRemotePeer)
	{
		removeCRemotePeer((TcpClient*)pRemotePeer);
	}
}

void CManager::OnData(int callFuncId, Respone response)
{
	int call = callFuncId;
	Respone data = response;
	ArgumentType args;
	FieldValue Gps(FieldValue::TObject);
	FieldValue Indoor(FieldValue::TObject);
	std::map<std::string, RadioStatus>::iterator it;
	//std::list<BconMajMinTimeReport>::iterator mBcon;
	FieldValue bcons(FieldValue::TArray);
	FieldValue info(FieldValue::TArray);
	switch (call)
	{
	case GPS_TRIGG_COMM_INDOOR:
	case GPS_TRIGG_CSBK_INDOOR:
	case GPS_IMME_COMM_INDOOR:
	case GPS_IMME_CSBK_INDOOR:
	{
								 FieldValue element(FieldValue::TObject);
								 element.setKeyVal("txpower", FieldValue(data.bcon.TXPower));
								 element.setKeyVal("rssi", FieldValue(data.bcon.RSSI));
								 element.setKeyVal("timestamp", FieldValue(data.bcon.TimeStamp));
								 element.setKeyVal("major", FieldValue(data.bcon.Major));
								 element.setKeyVal("minor", FieldValue(data.bcon.Minor));
								 args["SessionId"] = FieldValue(data.sessionId.c_str());
								 args["Target"] = FieldValue(data.target);
								 args["Type"] = 1;   //1:becons 
								 args["Cycle"] = FieldValue(data.cycle);
								 args["Operate"] = FieldValue(data.operate);
								 args["Status"] = FieldValue(data.gpsStatus);
								 args["Report"] = element;
								 g_pNet->send2Client("locationStatus", args);
	}
		break;
	case CONNECT_STATUS:
		switch (response.connectStatus)
		{
		case MNIS_CONNECT:
		{
							 printf_s("MNIS_CONNECT:%d\r\n", response.connectStatus);
							 Env_MnisIsOk = true;
							 if (g_manager) g_manager->setMnisStatus(WL_SYSTEM_CONNECT);
							 //g_pNet->wlMnisConnectStatus(response.connectStatus);
		}
			break;
		default:
		{
				   printf_s("MNIS_DIS_CONNECT:%d\r\n", response.connectStatus);
				   Env_MnisIsOk = false;
				   if (g_manager) g_manager->setMnisStatus(WL_SYSTEM_DISCONNECT);
		}
			break;
		}
	case SEND_PRIVATE_MSG:
		args["type"] = FieldValue(PRIVATE);
		try
		{
			args["SessionId"] = FieldValue(data.sessionId.c_str());
			args["Source"] = FieldValue(NULL);
			args["Target"] = FieldValue(data.target);
			args["Contents"] = FieldValue(data.msg.c_str());
			args["Status"] = FieldValue(data.msgStatus);
			args["Type"] = FieldValue(data.msgType);
			g_pNet->send2Client("messageStatus", args);
		}
		catch (std::exception e)
		{

		}
		break;
	case SEND_GROUP_MSG:
		args["type"] = FieldValue(GROUP);
		try
		{
			args["SessionId"] = FieldValue(data.sessionId.c_str());
			args["Source"] = FieldValue(NULL);
			args["Target"] = FieldValue(data.target);
			args["Contents"] = FieldValue(data.msg.c_str());
			args["Status"] = FieldValue(data.msgStatus);
			args["Type"] = FieldValue(data.msgType);
			g_pNet->send2Client("messageStatus", args);
		}
		catch (std::exception e)
		{

		}
		break;
	case RECV_MSG:
		args["Target"] = FieldValue(data.target);
		args["Source"] = FieldValue(data.source);
		args["Contents"] = FieldValue(data.msg.c_str());
		args["Type"] = FieldValue(PRIVATE);
		g_pNet->send2Client("message", args);
		break;
	case  GPS_IMME_COMM:
	case GPS_TRIGG_COMM:
	case GPS_IMME_CSBK:
	case GPS_TRIGG_CSBK:
	case GPS_IMME_CSBK_EGPS:
	case GPS_TRIGG_CSBK_EGPS:
	case STOP_QUERY_GPS:
	{
						   FieldValue element(FieldValue::TObject);
						   element.setKeyVal("Lat", FieldValue(data.lat));
						   element.setKeyVal("Lon", FieldValue(data.lon));
						   element.setKeyVal("Alt", FieldValue(data.altitude));
						   element.setKeyVal("Speed", FieldValue(data.speed));
						   element.setKeyVal("Valid", FieldValue(data.valid));
						   args["SessionId"] = FieldValue(data.sessionId.c_str());
						   args["Target"] = FieldValue(data.target);
						   args["Type"] = 0;   //0:gps 
						   args["Cycle"] = FieldValue(data.cycle);
						   args["Operate"] = FieldValue(data.operate);
						   args["Status"] = FieldValue(data.gpsStatus);
						   args["Report"] = element;
						   g_pNet->send2Client("locationStatus", args);
	}
		break;
	case RECV_GPS:
	{
					 FieldValue element(FieldValue::TObject);
					 element.setKeyVal("Lat", FieldValue(data.lat));
					 element.setKeyVal("Lon", FieldValue(data.lon));
					 element.setKeyVal("Alt", FieldValue(data.altitude));
					 element.setKeyVal("Speed", FieldValue(data.speed));
					 element.setKeyVal("Valid", FieldValue(data.valid));
					 args["Source"] = FieldValue(data.source);
					 args["Report"] = element;
					 g_pNet->send2Client("sendGps", args);
	}
		break;
	case RECV_LOCATION_INDOOR:
	{
								 FieldValue element(FieldValue::TObject);
								 FieldValue uuid(FieldValue::TArray);
								 for (int i = 0; i < 16; i++)
								 {
									 FieldValue temp(FieldValue::TInt);
									 temp.setInt(data.bcon.uuid[i]);
									 uuid.push(temp);
								 }
								 element.setKeyVal("uuid", FieldValue(uuid));
								 element.setKeyVal("txpower", FieldValue(data.bcon.TXPower));
								 element.setKeyVal("rssi", FieldValue(data.bcon.RSSI));
								 element.setKeyVal("timestamp", FieldValue(data.bcon.TimeStamp));
								 element.setKeyVal("major", FieldValue(data.bcon.Major));
								 element.setKeyVal("minor", FieldValue(data.bcon.Minor));

								 args["Report"] = element;
								 args["source"] = data.source;
								 g_pNet->send2Client("sendBeacons", args);
	}
		break;
	case RADIO_ARS:
		args["Target"] = FieldValue(data.source);
		if (data.arsStatus == SUCESS)
		{
			args["IsOnline"] = FieldValue("True");
		}
		else
		{
			args["IsOnline"] = FieldValue("False");
		}
		g_pNet->send2Client("sendArs", args);
		break;
	case RADIO_STATUS:
		args["getType"] = RADIO_STATUS;
		for (it = data.rs.begin(); it != data.rs.end(); it++)
		{
			FieldValue element(FieldValue::TObject);
			element.setKeyVal("radioId", FieldValue(it->second.id));
			bool isGps = false;
			if (it->second.gpsQueryMode > 0)
			{
				if (it->second.gpsQueryMode != 25)
				{
					isGps = true;
				}
			}
			bool isLocationIndoor = false;
			if (it->second.gpsQueryMode > 0)
			{
				if (it->second.gpsQueryMode == 25 || it->second.gpsQueryMode == 26)
				{
					isLocationIndoor = true;
				}
			}
			bool isArs = false;
			if (it->second.status != 0)
			{
				isArs = true;
			}
			element.setKeyVal("IsInLocationIndoor", FieldValue(isLocationIndoor));
			element.setKeyVal("IsInGps", FieldValue(isGps));
			element.setKeyVal("IsOnline", FieldValue(isArs));
			info.push(element);
		}
		args["info"] = info;
		args["SessionId"] = FieldValue((data.sessionId).c_str());
		g_pNet->send2Client("wlInfo", args);
		break;
	default:
		break;
	}
}

int CManager::updateOnLineRadioInfo(int radioId, int status, int gpsQueryMode)
{
	m_pMnis->updateOnLineRadioInfo(radioId, status, gpsQueryMode);
	return 0;
}

void CManager::handleTaskOnTimerProc()
{
	/*验证队列任务是否超时*/
	handleCurTaskTimeOut();
	//WaitForSingleObject(g_taskLockerEvent, INFINITE);
	TRYLOCK(g_mutexRemoteCommandTaskQueue);
	std::list<REMOTE_TASK*>::iterator it = g_remoteCommandTaskQueue.begin();
	while (it != g_remoteCommandTaskQueue.end())
	{
		REMOTE_TASK* p = *it;
		if (p->timeOutTickCout != 0 && p->flag == FLAG_NHANDLE)
		{
			if (p->timeOutTickCout > GetTickCount())
			{
				p->flag = FLAG_HANDLED;
				switch (p->cmd)
				{
				case StartCall:
				case StopCall:
				{
								 g_pNet->wlCallStatus(p, STATUS_CALL_END | REMOTE_CMD_FAIL);
								 g_remoteCommandTaskQueue.erase(it);
								 delete p;
								 p = NULL;
				}
					break;
				default:
					break;
				}
			}
		}
		it++;
	}
	RELEASELOCK(g_mutexRemoteCommandTaskQueue);
	//SetEvent(g_taskLockerEvent);
}

void PASCAL CManager::TaskOnTimerProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	CManager* p = (CManager*)dwUser;
	if (p)
	{
		p->handleTaskOnTimerProc();
	}
}

//void CManager::lockCurTask()
//{
//	m_curTaskLocker.lock();
//	sprintf_s(m_reportMsg, "lockCurTask");
//	sendLogToWindow();
//}

//void CManager::unLockCurTask()
//{
//	m_curTaskLocker.unlock();
//	sprintf_s(m_reportMsg, "unLockCurTask");
//	sendLogToWindow();
//}

bool CManager::isRepeat(std::string sessionId)
{
	bool rlt = false;
	//lockCurTask();
	handleIsRepeatCurTask(sessionId, rlt);
	//unLockCurTask();
	if (rlt) return rlt;
	//WaitForSingleObject(g_taskLockerEvent, INFINITE);
	TRYLOCK(g_mutexRemoteCommandTaskQueue);
	REMOTE_TASK* p = NULL;
	std::list<REMOTE_TASK*>::iterator it = g_remoteCommandTaskQueue.begin();
	while (it != g_remoteCommandTaskQueue.end())
	{
		p = *it;
		rlt = isSameSessionId(sessionId, p);
		if (rlt) break;
		it++;
	}
	RELEASELOCK(g_mutexRemoteCommandTaskQueue);
	//SetEvent(g_taskLockerEvent);
	return rlt;
}

bool CManager::isSameSessionId(std::string sessionId, REMOTE_TASK* p)
{
	bool rlt = false;
	int cmd = p->cmd;
	switch (cmd)
	{
	case REMOTE_CMD_CONFIG:
	case REMOTE_CMD_SET_PLAY_CALL:
	case REMOTE_CMD_MNIS_LOCATION_INDOOR_CONFIG:
		break;
	case REMOTE_CMD_STOP_CALL:
	case REMOTE_CMD_CALL:
	{
							CALL_OPERATE_PARAM info = p->param.info.callParam.operateInfo;
							sprintf_s(m_reportMsg, "%s and %s is %d", sessionId.c_str(), info.SessionId, rlt = (0 == strcmp(sessionId.c_str(), info.SessionId)));
							//sendLogToWindow();
							return rlt;
	}
		break;
	case REMOTE_CMD_GET_CONN_STATUS:
	case REMOTE_CMD_SESSION_STATUS:
	case REMOTE_CMD_SYSTEM_STATUS:
	{
									 GET_INFO_PARAM info = p->param.info.getInfoParam.getInfo;
									 sprintf_s(m_reportMsg, "%s and %s is %d", sessionId.c_str(), info.SessionId, rlt = (0 == strcmp(sessionId.c_str(), info.SessionId)));
									 //sendLogToWindow();
									 return rlt;
	}
		break;
	case REMOTE_CMD_MNIS_QUERY_GPS:
	{
									  QUERY_GPS info = p->param.info.queryGpsParam;
									  sprintf_s(m_reportMsg, "%s and %s is %d", sessionId.c_str(), info.SessionId, rlt = (0 == strcmp(sessionId.c_str(), info.SessionId)));
									  //sendLogToWindow();
									  return rlt;
	}
		break;
	case REMOTE_CMD_MNIS_MSG:
	{
								MNIS_MSG info = p->param.info.msgParam;
								sprintf_s(m_reportMsg, "%s and %s is %d", sessionId.c_str(), info.SessionId, rlt = (0 == strcmp(sessionId.c_str(), info.SessionId)));
								//sendLogToWindow();
								return rlt;
	}
		break;
	case REMOTE_CMD_MNIS_STATUS:
	{
								   ARS info = p->param.info.mnisStatusParam;
								   sprintf_s(m_reportMsg, "%s and %s is %d", sessionId.c_str(), info.SessionId, rlt = (0 == strcmp(sessionId.c_str(), info.SessionId)));
								   //sendLogToWindow();
								   return rlt;

	}
		break;
	default:
		break;
	}
	return rlt;
}

void CManager::handleStopCall()
{
	if (m_bNeedStopCall)
	{
		setbNeedStopCall(false);
		stopCall(false);
	}
}

void CManager::setbNeedStopCall(bool value)
{
	if (value != m_bNeedStopCall)
	{
		sprintf_s(m_reportMsg, "=============NEED_STOP:%d->%d=============", m_bNeedStopCall, value);
		sendLogToWindow();
	}
	m_bNeedStopCall = value;

}

void CManager::getCurrentTaskInfo(int srcId, std::string &sessionid, int &cmd)
{
	NSLog::instance()->AddLog(Ns_Log_Info, "Lock m_mutexCurTask");
	std::lock_guard<std::mutex> locker(m_mutexCurTask);
	if (m_currentTask.cmd != 0 && sessionid == "" && srcId == CONFIG_LOCAL_RADIO_ID)
	{
		sessionid = m_currentTask.param.info.callParam.operateInfo.SessionId;
		cmd = m_currentTask.cmd;
	}
	NSLog::instance()->AddLog(Ns_Log_Info, "UnLock m_mutexCurTask");
}

void CManager::handleCurTaskTimeOut()
{
	REMOTE_TASK task = { 0 };
	getCurrentTask(task);
	REMOTE_TASK* p = &task;
	if (p)
	{
		if (p->timeOutTickCout != 0 && p->flag == FLAG_NHANDLE)
		{
			if (p->timeOutTickCout > GetTickCount())
			{
				p->flag = FLAG_HANDLED;
				switch (p->cmd)
				{
				case StartCall:
				case StopCall:
					g_pNet->wlCallStatus(p, STATUS_CALL_END | REMOTE_CMD_FAIL);
					break;
				default:
					break;
				}
			}
		}
	}
	setCurrentTask(p);
}

void CManager::handleIsRepeatCurTask(std::string sessionId, bool &rlt)
{
	REMOTE_TASK task = { 0 };
	getCurrentTask(task);
	REMOTE_TASK* p = &task;
	if (p)
	{
		rlt = isSameSessionId(sessionId, p);
	}
}

void CManager::initialize()
{
	///*获取dongle数量*/
	//com_use_t result = { 0 };
	//WDK_WhoAllVidPid(VID_PID, &result);
	////memcpy(&m_curDongleInfo, &result, sizeof(com_use_t));
	//setDongleCount(result.num);
}

int CManager::DongCount()
{
	return m_dongleCount;
}

void CManager::setDongleCount(int value)
{
	if (m_dongleCount != value)
	{
		m_dongleCount = value;
		FieldValue info(FieldValue::TObject);
		info.setKeyVal("DongleCount", FieldValue(value));//dongle数量
		if (g_pNet) g_pNet->wlInfo(GET_TYPE_SYSTEM_STATUS, info, "");
	}
}

int CManager::MicphoneStatus()
{
	return m_micphoneStatus;
}

void CManager::setMicphoneStatus(int value)
{
	if (m_micphoneStatus != value)
	{
		m_micphoneStatus = value;
		FieldValue info(FieldValue::TObject);
		info.setKeyVal("MicphoneStatus", FieldValue(value));
		if (g_pNet) g_pNet->wlInfo(GET_TYPE_SYSTEM_STATUS, info, "");
	}
}

int CManager::SpeakerStatus()
{
	return m_speakerStatus;
}

void CManager::setSpeakerStatus(int value)
{
	if (m_speakerStatus != value)
	{
		m_speakerStatus = value;
		FieldValue info(FieldValue::TObject);
		info.setKeyVal("SpeakerStatus", FieldValue(value));
		if (g_pNet) g_pNet->wlInfo(GET_TYPE_SYSTEM_STATUS, info, "");
	}
}

int CManager::LEStatus()
{
	return m_lEStatus;
}

void CManager::setLEStatus(int value)
{
	if (m_lEStatus != value)
	{
		m_lEStatus = value;
		FieldValue info(FieldValue::TObject);
		info.setKeyVal("LEStatus", FieldValue(value));
		if (g_pNet) g_pNet->wlInfo(GET_TYPE_SYSTEM_STATUS, info, "");
	}
}

int CManager::WireLanStatus()
{
	return m_wireLanStatus;
}

void CManager::setWireLanStatus(int value)
{
	if (m_wireLanStatus != value)
	{
		m_wireLanStatus = value;
		FieldValue info(FieldValue::TObject);
		info.setKeyVal("WireLanStatus", FieldValue(value));
		if (g_pNet) g_pNet->wlInfo(GET_TYPE_SYSTEM_STATUS, info, "");
	}
}

int CManager::DeviceInfoStatus()
{
	return m_deviceInfoStatus;
}

void CManager::setDeviceInfoStatus(int value)
{
	if (m_deviceInfoStatus != value)
	{
		m_deviceInfoStatus = value;
		FieldValue info(FieldValue::TObject);
		info.setKeyVal("DeviceInfoStatus", FieldValue(value));
		if (g_pNet) g_pNet->wlInfo(GET_TYPE_SYSTEM_STATUS, info, "");
	}
}

void CManager::OnUpdateUsb(DWORD type)
{
	//g_pNSManager->OnUpdateUsb(type);
	//setDongleCount(g_pNSManager->SizeDongle());
	m_pModelNs->OnUpdateUsb(type);
	setDongleCount(m_pModelNs->SizeDongle());
	//sprintf_s(m_reportMsg, "OnUpdateUsb");
	//sendLogToWindow();
	////LOG_INFO("OnUpdateUsb");
	//switch (type)
	//{
	//case USB_ADD:
	//{
	//				//LOG_INFO("USB_ADD");
	//				sprintf_s(m_reportMsg, "USB_ADD");
	//				sendLogToWindow();
	//				handleUsbAdd();
	//}
	//	break;
	//case USB_DEL:
	//{
	//				//LOG_INFO("USB_DEL");
	//				sprintf_s(m_reportMsg, "USB_DEL");
	//				sendLogToWindow();
	//				handleUsbDel();
	//}
	//	break;
	//default:
	//	break;
	//}
}

void CManager::OnUpdateUsbService(bool type)
{
	if (g_manager)
	{
		g_manager->OnUpdateUsb(type ? DBT_DEVICEARRIVAL : DBT_DEVICEREMOVECOMPLETE);
	}
}

void CManager::handleUsbAdd()
{
	com_use_t result = { 0 };
	WDK_WhoAllVidPid(VID_PID, &result);
	//memcpy(&m_curDongleInfo, &result, sizeof(com_use_t));
	if (0 == DongCount())
	{
		for (int i = 0; i < result.num; i++)
		{
			/*重新配置dongle*/
			WCHAR tmpStr[128] = { 0 };
			swprintf_s(tmpStr, 128, L"\\\\.\\%s", g_pTool->ANSIToUnicode(result.coms[i]).c_str());
			if (WL_RETURN_OK != g_pDongle->OpenDongle(tmpStr, this))
			{
				//m_bDongleIsOpen = FALSE;
				setDongleCount(0);
				Env_DongleIsOk = false;
				sprintf_s(m_reportMsg, "initDongle:open dongle fail");
				sendLogToWindow();
				g_pWLlog->sendLog("dongle initial fail");
				continue;
			}
			else
			{
				//m_bDongleIsOpen = TRUE;
				sscanf_s(result.coms[i], "COM%d", &CONFIG_DONGLE_PORT);
				setDongleCount(1);
				Env_DongleIsOk = true;
				sprintf_s(m_reportMsg, "open dongle success");
				sendLogToWindow();
				g_pWLlog->sendLog("dongle initial success");
				break;
			}
		}
	}
}

void CManager::handleUsbDel()
{
	com_use_t result = { 0 };
	WDK_WhoAllVidPid(VID_PID, &result);
	//memcpy(&m_curDongleInfo, &result, sizeof(com_use_t));
	if (0 == DongCount())
	{
		for (int i = 0; i < result.num; i++)
		{
			/*重新配置dongle*/
			WCHAR tmpStr[128] = { 0 };
			swprintf_s(tmpStr, 128, L"\\\\.\\%s", g_pTool->ANSIToUnicode(result.coms[i]).c_str());
			if (WL_RETURN_OK != g_pDongle->OpenDongle(tmpStr, this))
			{
				//m_bDongleIsOpen = FALSE;
				setDongleCount(0);
				Env_DongleIsOk = false;
				sprintf_s(m_reportMsg, "initDongle:open dongle fail");
				sendLogToWindow();
				g_pWLlog->sendLog("dongle initial fail");
				continue;
			}
			else
			{
				//m_bDongleIsOpen = TRUE;
				sscanf_s(result.coms[i], "COM%d", &CONFIG_DONGLE_PORT);
				setDongleCount(1);
				Env_DongleIsOk = true;
				sprintf_s(m_reportMsg, "open dongle success");
				sendLogToWindow();
				g_pWLlog->sendLog("dongle initial success");
				break;
			}
		}
	}
	else
	{
		bool ishave = false;
		char temp[64] = { 0 };
		sprintf_s(temp, "COM%d", CONFIG_DONGLE_PORT);
		for (int i = 0; i < result.num; i++)
		{
			if (0 == strcmp(temp, result.coms[i]))
			{
				ishave = true;
				break;
			}
		}
		if (!ishave)
		{
			for (int i = 0; i < result.num; i++)
			{
				/*重新配置dongle*/
				WCHAR tmpStr[128] = { 0 };
				swprintf_s(tmpStr, 128, L"\\\\.\\%s", g_pTool->ANSIToUnicode(result.coms[i]).c_str());
				if (WL_RETURN_OK != g_pDongle->OpenDongle(tmpStr, this))
				{
					//m_bDongleIsOpen = FALSE;
					setDongleCount(0);
					Env_DongleIsOk = false;
					sprintf_s(m_reportMsg, "initDongle:open dongle fail");
					sendLogToWindow();
					g_pWLlog->sendLog("dongle initial fail");
					continue;
				}
				else
				{
					sscanf_s(result.coms[i], "COM%d", &CONFIG_DONGLE_PORT);
					setDongleCount(1);
					Env_DongleIsOk = true;
					sprintf_s(m_reportMsg, "open dongle success");
					sendLogToWindow();
					g_pWLlog->sendLog("dongle initial success");
					break;
				}
			}
		}
	}
}

int CManager::MnisStatus()
{
	return m_mnisStatus;
}

void CManager::setMnisStatus(int value)
{
	if (m_mnisStatus != value)
	{
		m_mnisStatus = value;
		FieldValue info(FieldValue::TObject);
		info.setKeyVal("MnisStatus", FieldValue(value));
		if (g_pNet) g_pNet->wlInfo(GET_TYPE_SYSTEM_STATUS, info, "");
	}
}

void CManager::OnCall(void* param, oncall_info_t* info)
{
	CManager* p = (CManager*)param;
	if (p)
	{
		p->onCall(info);
	}
}

void CManager::onCall(oncall_info_t* info)
{
	NSLog::instance()->AddLog(Ns_Log_Info, "OnCall callType:0x%02x,srcId:%lu,tgtId:%lu,status:%d,isCurrent:%s", info->callType, info->srcId, info->tgtId, info->status, info->isCurrent ? "TRUE" : "FALSE");
	g_pNet->wlCall(info->callType, info->srcId, info->tgtId, info->status, info->isCurrent);
}

void CManager::OnCallStatus(void* param, oncallstatus_info_t* info)
{
	CManager* p = (CManager*)param;
	if (p)
	{
		p->onCallStatus(info);
	}
}

void CManager::onCallStatus(oncallstatus_info_t* info)
{
	NSLog::instance()->AddLog(Ns_Log_Info, "onCallStatus callType:0x%02x,srcId:%lu,tgtId:%lu,status:%d", info->callType, info->srcId, info->tgtId, info->status);
	REMOTE_TASK task = { 0 };
	getCurrentTask(task);
	CALL_OPERATE_PARAM CurCallCmd = task.param.info.callParam.operateInfo;
	g_pNet->wlCallStatus(CurCallCmd.callType, CONFIG_LOCAL_RADIO_ID, CurCallCmd.tartgetId, info->status, CurCallCmd.SessionId);
}

void CManager::OnSystemStatusChange(void* param, onsystemstatuschange_info_t* info)
{
	CManager* p = (CManager*)param;
	if (p)
	{
		p->onSystemStatusChange(info);
	}
}

void CManager::onSystemStatusChange(onsystemstatuschange_info_t* info)
{
	NSLog::instance()->AddLog(Ns_Log_Info, "onSystemStatusChange type:%d,value:%d", info->type, info->value);
	switch (info->type)
	{
	case System_WorkMode:
		break;
	case System_ServerStatus:
		break;
	case System_DeviceStatus:
		break;
	case System_MnisStatus:
		break;
	case System_DatabaseStatus:
		break;
	case System_DongleCount:
	{
							   setDongleCount(m_pModelNs->SizeDongle());
	}
		break;
	case System_MicphoneStatus:
		break;
	case System_SpeakerStatus:
		break;
	case System_LEStatus:
	{
							setLEStatus(info->value);
	}
		break;
	case System_WireLanStatus:
	{
								 setWireLanStatus(info->value);
	}
		break;
	case System_DeviceInfoStatus:
	{
									setDeviceInfoStatus(info->value);
	}
		break;
	case System_RepeaterStatus:
	{
								  FieldValue value(FieldValue::TInt);
								  value.setInt(info->value);
								  g_pNet->wlInfo(GET_TYPE_CONN, value, "");
	}
		break;
	case System_SendSerial:
	{
							  g_pNet->wlSendSerial();
	}
		break;
	case System_SendTimeOutCallEnd:
	{
									  REMOTE_TASK task = { 0 };
									  getCurrentTask(task);
									  CALL_OPERATE_PARAM CurCallCmd = task.param.info.callParam.operateInfo;
									  g_pNet->wlRequestCallEnd(CurCallCmd);
	}
		break;
	default:
		break;
	}
}

void CManager::setDb(CMySQL* value)
{
	m_pModelNs->setDb(value);
	m_pDb = value;
}

void CManager::HandleAudioPath(const char* strpath)
{
	std::wstring path = g_pNSTool->ANSIToUnicode(strpath);
	if (path.empty())
	{
		NSLog::instance()->AddLog(Ns_Log_Error, "error:audio path is null,there will used default audio path");
	}
	else
	{
		path += L"\\";
		if (!PathFileExists(path.c_str()))
		{
			std::string str = g_pTool->UnicodeToANSI(path);
			NSLog::instance()->AddLog(Ns_Log_Error, "error:audio path %s not exist,there will used default audio path", str.c_str());
		}
		else
		{
			path += L"Voice\\";
			int createFileRlt = 0;
			if (!PathFileExists(path.c_str()))
			{
				createFileRlt = _wmkdir(path.c_str());
			}
			if (0 == createFileRlt)
			{
				NS::setAmbeDataPath(path.c_str());
			}
			else
			{
				std::string str = g_pTool->UnicodeToANSI(path);
				NSLog::instance()->AddLog(Ns_Log_Error, "error:create file %s fail,there will used default audio path", str.c_str());
			}
		}
	}
}

//void CManager::setAmbeDataPath(const wchar_t* value)
//{
//	m_pModelNs->setAmbeDataPath(value);
//}
