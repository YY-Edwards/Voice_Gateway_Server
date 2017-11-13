
#include "stdafx.h"
#include <process.h>
#include "Manager.h"
#include "SerialDongle.h"
#include "Sound.h"
#include "WLNet.h"
#include "actionHandler.h"

//temp delete
//#include "Net.h"

CTool g_tool;
BOOL g_bPTT;      //Set or cleared by user interface.
BOOL g_bTX;       //Set or cleared by dongle.

CSerialDongle* g_pDongle;
BOOL g_dongleIsUsing;

CManager::CManager(CMySQL *pDb, CDataScheduling* pMnis, std::wstring& defaultAudioPath) :
m_idTaskOnTimerProc(0)
{
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
	//memset(&m_pCurrentTask, 0, sizeof(REMOTE_TASK));
	m_pCurrentTask = NULL;
	m_pMnis = pMnis;
	m_hWaitDecodeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//memset(&m_currentTask, 0, sizeof(REMOTE_TASK));
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
	stop();
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

int CManager::stopCall()
{
	sprintf_s(m_reportMsg, "handle stopCall");
	sendLogToWindow();
	if (g_pNet->canStopRecord())
	{
		g_pNet->requestRecordEndEvent();
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
	CONFIG_MNIS_ID = pConfig->mnis.ID;
	g_pNet->setAudioPath(pConfig->reapeater.AudioPath);
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
			g_pNet->setWlStatus(STARTING);
		}
	}
	CONFIG_LOCAL_RADIO_ID = pConfig->reapeater.LocalRadioId;
	if (CONFIG_RECORD_TYPE != pConfig->reapeater.recordType)
	{
		CONFIG_RECORD_TYPE = pConfig->reapeater.recordType;
		if (!bMasterChange)
		{
			g_pNet->setWlStatus(STARTING);
		}
	}
	CONFIG_DEFAULT_GROUP = pConfig->reapeater.DefaultGroupId;
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
		/*与主中继相连*/
		if (!g_pNet->StartNet(inet_addr(CONFIG_MASTER_IP), CONFIG_MASTER_PORT, INADDR_ANY, CONFIG_LOCAL_PEER_ID, CONFIG_LOCAL_RADIO_ID, CONFIG_RECORD_TYPE))
		{
			Env_NetIsOk = false;
			sprintf_s(m_reportMsg, "net initial fail");
			sendLogToWindow();
			g_pWLlog->sendLog("net initial fail");
		}
		else
		{
			Env_NetIsOk = true;
			g_pWLlog->sendLog("net initial success");
		}
		/*配置dongle*/
		WCHAR tmpStr[128] = { 0 };
		swprintf_s(tmpStr, 128, L"\\\\.\\COM%d", CONFIG_DONGLE_PORT);
		if (WL_RETURN_OK != g_pDongle->OpenDongle(tmpStr, this))
		{
			//m_bDongleIsOpen = FALSE;
			Env_DongleIsOk = false;
			sprintf_s(m_reportMsg, "open dongle fail");
			sendLogToWindow();
			g_pWLlog->sendLog("open dongle fail");
		}
		else
		{
			//m_bDongleIsOpen = TRUE;
			Env_DongleIsOk = true;
			sprintf_s(m_reportMsg, "open dongle success");
			sendLogToWindow();
			g_pWLlog->sendLog("open dongle success");
		}
		/*配置麦克风和扬声器*/
		if (WL_RETURN_OK != g_pSound->StartSound())
		{
			Env_SoundIsOk = false;
			sprintf_s(m_reportMsg, "sound initial fail");
			sendLogToWindow();
			g_pWLlog->sendLog("sound initial fail");
		}
		else
		{
			Env_SoundIsOk = true;
			sprintf_s(m_reportMsg, "sound initial success");
			sendLogToWindow();
			g_pWLlog->sendLog("sound initial success");
		}
		/*配置mnis*/
		//m_pMnis->radioConnect(CONFIG_MNIS_IP);
		m_pMnis->radioConnect(pConfig->mnis, pConfig->location, pConfig->locationindoor);

		m_bIsHaveConfig = true;
	}
	else
	{
		if (!Env_SoundIsOk)
		{
			g_pSound->stop();
			/*配置麦克风和扬声器*/
			if (WL_RETURN_OK != g_pSound->StartSound())
			{
				Env_SoundIsOk = false;
				sprintf_s(m_reportMsg, "sound initial fail");
				sendLogToWindow();
				g_pWLlog->sendLog("sound initial fail");

			}
			else
			{
				Env_SoundIsOk = true;
				sprintf_s(m_reportMsg, "sound initial success");
				sendLogToWindow();
				g_pWLlog->sendLog("sound initial success");
			}

		}
		if (bMasterChange)
		{
			/*重新与主中继相连接*/
			if (!g_pNet->StartNet(inet_addr(CONFIG_MASTER_IP), CONFIG_MASTER_PORT, INADDR_ANY, CONFIG_LOCAL_PEER_ID, CONFIG_LOCAL_RADIO_ID, CONFIG_RECORD_TYPE))
			{
				Env_NetIsOk = false;
				sprintf_s(m_reportMsg, "net initial fail");
				sendLogToWindow();
				g_pWLlog->sendLog("net initial fail");
			}
			else
			{
				Env_NetIsOk = true;
				g_pWLlog->sendLog("net initial success");
			}
		}
		if (bDongleChange)
		{
			/*重新配置dongle*/
			WCHAR tmpStr[128] = { 0 };
			swprintf_s(tmpStr, 128, L"\\\\.\\COM%d", CONFIG_DONGLE_PORT);
			if (WL_RETURN_OK != g_pDongle->OpenDongle(tmpStr, this))
			{
				//m_bDongleIsOpen = FALSE;
				Env_DongleIsOk = false;
				sprintf_s(m_reportMsg, "initDongle:open dongle fail");
				sendLogToWindow();
				g_pWLlog->sendLog("dongle initial fail");
			}
			else
			{
				//m_bDongleIsOpen = TRUE;
				Env_DongleIsOk = true;
				sprintf_s(m_reportMsg, "open dongle success");
				sendLogToWindow();
				g_pWLlog->sendLog("dongle initial success");
			}
		}
		if (bMnisChange)
		{
			/*配置mnis*/
			m_pMnis->radioConnect(pConfig->mnis, pConfig->location, pConfig->locationindoor);
		}
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
	while (m_bRemoteTaskThreadRun)
	{
		//sprintf_s(m_reportMsg, "3");
		//sendLogToWindow();
		WaitForSingleObject(g_waitHandleRemoteTask, 1000);
		while (g_remoteCommandTaskQueue.size() > 0)
		{
			//sprintf_s(m_reportMsg, "have task");
			//sendLogToWindow();
			/*处理任务*/
			get_front_task(task);
			erase_front_task();
			switch (task.cmd)
			{
			case REMOTE_CMD_CONFIG:
			{
									  sprintf_s(m_reportMsg, "Handle REMOTE_CMD_CONFIG");
									  sendLogToWindow();
									  config(&task);
			}
				break;
			case REMOTE_CMD_CALL:
			{
									//memcpy(&m_pCurrentTask, &task, sizeof(REMOTE_TASK));
									lockCurTask();
									setCurrentTask(&task);
									unLockCurTask();
									if (g_pNet->getWlStatus() == ALIVE)
									{
										initialCall(task.param.info.callParam.operateInfo.tartgetId, task.param.info.callParam.operateInfo.callType);
									}
									else
									{
										g_pNet->wlCallStatus(task.param.info.callParam.operateInfo.callType, CONFIG_LOCAL_RADIO_ID, task.param.info.callParam.operateInfo.tartgetId, STATUS_CALL_END | REMOTE_CMD_FAIL);
									}
			}
				break;
			case REMOTE_CMD_SET_PLAY_CALL:
			{
											 //setCurrentTask(&task);
											 if (setPlayCallOfCare(task.param.info.setCareCallParam.playParam.callType, task.param.info.setCareCallParam.playParam.targetId))
											 {
												 g_pNet->wlPlayStatus(CMD_FAIL, task.param.info.setCareCallParam.playParam.targetId);

											 }
											 else
											 {
												 //do nothing
												 //g_pNet->wlPlayStatus(CMD_SUCCESS, task.param.info.setCareCallParam.playParam.targetId);
											 }
			}
				break;
			case REMOTE_CMD_STOP_CALL:
			{
										 //memcpy(&m_pCurrentTask, &task, sizeof(REMOTE_TASK));
										 lockCurTask();
										 setCurrentTask(&task);
										 unLockCurTask();
										 g_pNet->wlCallStatus(task.param.info.callParam.operateInfo.callType, CONFIG_LOCAL_RADIO_ID, task.param.info.callParam.operateInfo.tartgetId, STATUS_CALL_END | REMOTE_CMD_SUCCESS);
										 if (CALL_ONGOING == g_pNet->GetCallStatus())
										 {
											 stopCall();
										 }
										 //else
										 //{
										 //g_pNet->wlCallStatus(task.param.info.callParam.operateInfo.callType, CONFIG_LOCAL_RADIO_ID, task.param.info.callParam.operateInfo.tartgetId, STATUS_CALL_END | REMOTE_CMD_SUCCESS);
										 //}
			}
				break;
			case REMOTE_CMD_GET_CONN_STATUS:
			{
											   FieldValue info(FieldValue::TInt);
											   if (g_pNet->getWlStatus() == ALIVE)
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
			case REMOTE_CMD_MNIS_QUERY_GPS:
			{
											  QUERY_GPS gps = task.param.info.queryGpsParam;
											  m_pMnis->radioGetGps(gps.Target, gps.Type, gps.Cycle, gps.SessionId, gps.Operate);
			}
				break;
			case REMOTE_CMD_MNIS_MSG:
			{
										m_pMnis->radioSendMsg(task.param.info.msgParam.Contents, task.param.info.msgParam.Target, task.param.info.msgParam.Type, task.param.info.msgParam.SessionId);
			}
				break;
			case REMOTE_CMD_MNIS_STATUS:
			{
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
													  m_pMnis->getRadioStatus(task.param.info.mnisStatusParam.getType, task.param.info.mnisStatusParam.SessionId);
										   }
											   break;
										   }
			}
				break;
				//case is no use
			case REMOTE_CMD_MNIS_LOCATION_INDOOR_CONFIG:
			{
														   m_pMnis->locationIndoorConfig(task.param.info.locationParam.internal, task.param.info.locationParam.ibconNum, task.param.info.locationParam.isEmergency);
			}
				break;
			default:
				break;
			}
		}
	}
}

void CManager::stop()
{
	if (m_hRemoteTaskThread)
	{
		m_bRemoteTaskThreadRun = false;
		SetEvent(g_waitHandleRemoteTask);
		WaitForSingleObject(m_hRemoteTaskThread, 1000);
		CloseHandle(m_hRemoteTaskThread);
	}
	freeCurrentTask();
}

REMOTE_TASK* CManager::getCurrentTask()
{
	return m_pCurrentTask;
}

void CManager::freeCurrentTask()
{
	if (m_pCurrentTask)
	{
		delete m_pCurrentTask;
		m_pCurrentTask = NULL;
	}
}

void CManager::applayCurrentTask()
{
	m_pCurrentTask = new REMOTE_TASK;
	memset(m_pCurrentTask, 0, sizeof(REMOTE_TASK));
}

void CManager::setCurrentTask(REMOTE_TASK* value)
{
	freeCurrentTask();
	applayCurrentTask();
	memcpy(m_pCurrentTask, value, sizeof(REMOTE_TASK));
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
	switch (call)
	{
	case MNIS_CONNECT:
	{
						 printf_s("MNIS_CONNECT:%d\r\n", response.connectStatus);
						 Env_MnisIsOk = true;
						 //g_pNet->wlMnisConnectStatus(response.connectStatus);
	}
		break;
	case MNIS_DIS_CONNECT:
	{
							 printf_s("MNIS_DIS_CONNECT:%d\r\n", response.connectStatus);
							 Env_MnisIsOk = false;
	}
		break;
	case SEND_PRIVATE_MSG:
		args["type"] = FieldValue(PRIVATE);
		try
		{
			args["SessionId"] = FieldValue(data.sessionId.c_str());
			args["Source"] = FieldValue(NULL);
			args["Target"] = FieldValue(data.target);
			args["Contents"] = FieldValue("");
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
			args["Contents"] = FieldValue("");
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
		FieldValue info(FieldValue::TArray);
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
	}
}

int CManager::updateOnLineRadioInfo(int radioId, int status, int gpsQueryMode)
{
	m_pMnis->updateOnLineRadioInfo(radioId, status, gpsQueryMode);
	return 0;
}

//REMOTE_TASK* CManager::getCurrentTaskR()
//{
//	return &m_currentTask;
//}

void CManager::handleTaskOnTimerProc()
{
	lockCurTask();
	/*验证当前任务是否超时*/
	REMOTE_TASK* p = getCurrentTask();
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
	unLockCurTask();
	/*验证队列任务是否超时*/
	WaitForSingleObject(g_taskLockerEvent, INFINITE);
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
	SetEvent(g_taskLockerEvent);
}

void PASCAL CManager::TaskOnTimerProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	CManager* p = (CManager*)dwUser;
	if (p)
	{
		p->handleTaskOnTimerProc();
	}
}

void CManager::lockCurTask()
{
	m_curTaskLocker.lock();
	sprintf_s(m_reportMsg, "lockCurTask");
	sendLogToWindow();
}

void CManager::unLockCurTask()
{
	m_curTaskLocker.unlock();
	sprintf_s(m_reportMsg, "unLockCurTask");
	sendLogToWindow();
}

