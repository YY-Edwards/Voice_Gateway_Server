
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

CManager::CManager(CMySQL *pDb,CDataScheduling* pMnis)
{
	g_pNet = new CWLNet(pDb, this);
	g_pDongle = new CSerialDongle();
	g_pSound = new CSound();
	g_bPTT = FALSE;
	g_bTX = FALSE;

	m_report = NULL;
	m_hwnd = NULL;
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
}

CManager::~CManager()
{
	m_report = NULL;
	if (m_theTimeCaps.wPeriodMin != 0)
	{
		timeEndPeriod(m_theTimeCaps.wPeriodMin);//清除最小定时器精度
	}
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

int CManager::initWnd(HWND current_hwnd)
{
	// 	sprintf_s(m_reportMsg, "initDongle:current_hwnd:0x%x", current_hwnd);
	// 	sendLogToWindow();
	m_hWaitDecodeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hwnd = current_hwnd;
	return 0;
}

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
	if (g_pNet->canStopRecord())
	{
		g_pNet->requestRecordEndEvent();
		g_pSound->setbRecord(FALSE);
		g_pNet->waitRecordEnd();
	}
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

int CManager::setPlayCallOfCare(unsigned char calltype,unsigned long targetId)
{
	return g_pNet->setPlayCallOfCare( calltype,targetId);
}

int CManager::config(REMOTE_TASK* pTask)
{
	CONFIG *pConfig = &(pTask->param.info.configParam);
	int rlt = 0;
	bool bMasterChange = false;
	bool bDongleChange = false;
	bool bMnisChange = false;
	if (0 != strcmp(CONFIG_MASTER_IP, pConfig->master.ip))
	{
		strcpy_s(CONFIG_MASTER_IP, pConfig->master.ip);
		bMasterChange = true;
	}
	if (0 != strcmp(CONFIG_MNIS_IP, pConfig->mnis.ip))
	{
		strcpy_s(CONFIG_MNIS_IP, pConfig->mnis.ip);
		bMnisChange = true;
	}
	if (CONFIG_MASTER_PORT != pConfig->master.port)
	{
		CONFIG_MASTER_PORT = pConfig->master.port;
		bMasterChange = true;
	}
	if (CONFIG_LOCAL_PEER_ID != pConfig->localPeerId)
	{
		CONFIG_LOCAL_PEER_ID = pConfig->localPeerId;
		if (!bMasterChange)
		{
			g_pNet->setWlStatus(STARTING);
		}
	}
	CONFIG_LOCAL_RADIO_ID = pConfig->localRadioId;
	if (CONFIG_RECORD_TYPE != pConfig->recordType)
	{
		CONFIG_RECORD_TYPE = pConfig->recordType;
		if (!bMasterChange)
		{
			g_pNet->setWlStatus(STARTING);
		}
	}
	CONFIG_DEFAULT_GROUP = pConfig->defaultGroup;
	if (CONFIG_DONGLE_PORT != pConfig->dongle.donglePort)
	{
		CONFIG_DONGLE_PORT = pConfig->dongle.donglePort;
		bDongleChange = true;
	}
	CONFIG_HUNG_TIME = pConfig->hangTime;
	CONFIG_MASTER_HEART_TIME = pConfig->masterHeartTime;
	CONFIG_PEER_HEART_AND_REG_TIME = pConfig->peerHeartTime;
	CONFIG_DEFAULT_SLOT = pConfig->defaultSlot;

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
			sprintf_s(m_reportMsg,"net initial fail");
			sendLogToWindow();
		}
		else
		{
			Env_NetIsOk = true;
		}
		/*配置dongle*/
		WCHAR tmpStr[128] = { 0 };
		swprintf_s(tmpStr, 128, L"\\\\.\\COM%d", CONFIG_DONGLE_PORT);
		if (WL_RETURN_OK != g_pDongle->OpenDongle(tmpStr, m_hwnd, this))
		{
			//m_bDongleIsOpen = FALSE;
			Env_DongleIsOk = false;
			sprintf_s(m_reportMsg, "open dongle fail");
			sendLogToWindow();
		}
		else
		{
			//m_bDongleIsOpen = TRUE;
			Env_DongleIsOk = true;
			sprintf_s(m_reportMsg, "open dongle success");
			sendLogToWindow();
		}
		/*配置麦克风和扬声器*/
		if (WL_RETURN_OK != g_pSound->StartSound(m_hwnd, 0, 0))
		{
			Env_SoundIsOk = false;
			sprintf_s(m_reportMsg, "sound initial fail");
			sendLogToWindow();
		}
		else
		{
			Env_SoundIsOk = true;
			sprintf_s(m_reportMsg, "sound initial success");
			sendLogToWindow();
		}
		/*配置mnis*/
		m_pMnis->radioConnect((TcpClient*)pTask->pRemote, CONFIG_MNIS_IP, pTask->callId);

		m_bIsHaveConfig = true;
	}
	else
	{
		if (!Env_SoundIsOk)
		{
			g_pSound->stop();
			/*配置麦克风和扬声器*/
			if (WL_RETURN_OK != g_pSound->StartSound(m_hwnd, 0, 0))
			{
				Env_SoundIsOk = false;
				sprintf_s(m_reportMsg, "sound initial fail");
				sendLogToWindow();
			}
			else
			{
				Env_SoundIsOk = true;
				sprintf_s(m_reportMsg, "sound initial success");
				sendLogToWindow();
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
			}
			else
			{
				Env_NetIsOk = true;
			}
		}
		if (bDongleChange)
		{
			/*重新配置dongle*/
			WCHAR tmpStr[128] = { 0 };
			swprintf_s(tmpStr, 128, L"\\\\.\\COM%d", CONFIG_DONGLE_PORT);
			if (WL_RETURN_OK != g_pDongle->OpenDongle(tmpStr, m_hwnd, this))
			{
				//m_bDongleIsOpen = FALSE;
				Env_DongleIsOk = false;
				sprintf_s(m_reportMsg, "initDongle:open dongle fail");
				sendLogToWindow();
			}
			else
			{
				//m_bDongleIsOpen = TRUE;
				Env_DongleIsOk = true;
				sprintf_s(m_reportMsg, "open dongle success");
				sendLogToWindow();
			}
		}
		if (bMnisChange)
		{
			/*配置mnis*/
			m_pMnis->radioConnect((TcpClient*)pTask->pRemote, CONFIG_MNIS_IP, pTask->callId);
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
		if (g_remoteCommandTaskQueue.size() > 0)
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
									  sprintf_s(m_reportMsg,"Handle REMOTE_CMD_CONFIG");
									  sendLogToWindow();
									  config(&task);
			}
				break;
			case REMOTE_CMD_CALL:
			{
									//memcpy(&m_pCurrentTask, &task, sizeof(REMOTE_TASK));
									setCurrentTask(&task);
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
										 setCurrentTask(&task);
										 if (CALL_ONGOING == g_pNet->GetCallStatus())
										 {
											 stopCall();
										 }
										 else
										 {
											 g_pNet->wlCallStatus(task.param.info.callParam.operateInfo.callType, CONFIG_LOCAL_RADIO_ID, task.param.info.callParam.operateInfo.tartgetId, STATUS_CALL_END | REMOTE_CMD_SUCCESS);
										 }
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
											   g_pNet->wlInfo(GET_TYPE_CONN, info);
			}
				break;
			case REMOTE_CMD_MNIS_QUERY_GPS:
			{
											  m_pMnis->radioGetGps((TcpClient*)task.pRemote, task.param.info.queryGpsParam.Target, task.param.info.queryGpsParam.Type, task.param.info.queryGpsParam.Cycle, task.callId);
			}
				break;
			case REMOTE_CMD_MNIS_MSG:
			{
										m_pMnis->radioSendMsg((TcpClient*)task.pRemote, task.param.info.msgParam.Contents, task.param.info.msgParam.Source, task.callId, task.param.info.msgParam.Type);
			}
				break;
			default:
				break;
			}
		}
		else
		{
			//sprintf_s(m_reportMsg, "0 task");
			//sendLogToWindow();
			/*休眠*/
			Sleep(20);
			continue;
		}
	}
}

void CManager::stop()
{
	if (m_hRemoteTaskThread)
	{
		m_bRemoteTaskThreadRun = false;
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
	}
}

void CManager::OnDisConnect(CRemotePeer* pRemotePeer)
{
	if (pRemotePeer)
	{
		removeCRemotePeer((TcpClient*)pRemotePeer);
	}
}

void CManager::OnMnisCallBack(TcpClient *m_pTcpClient, int callId, int callFuncId, Respone response)
{
	switch (callFuncId)
	{
	case MNIS_CONNECT:
	{
						 printf_s("MNIS_CONNECT:%d\r\n", response.connectStatus);
						 g_pNet->wlMnisConnectStatus(response.connectStatus);
	}
		break;
	case SEND_PRIVATE_MSG:
	{

							 printf_s("SEND_PRIVATE_MSG:%d\r\n", response.msgStatus);
							 g_pNet->wlMnisMessageStatus(response.msgType, response.target, response.source, response.msg, response.msgStatus);
	}
		break;
	case SEND_GROUP_MSG:
	{
						   printf_s("SEND_GROUP_MSG:%d\r\n", response.msgStatus);
						   g_pNet->wlMnisMessageStatus(response.msgType, response.target, response.source, response.msg, response.msgStatus);
	}
		break;
	case RECV_MSG:
	{
					 printf_s("RECV_MSG:%s\r\n", response.msg.c_str());
					 g_pNet->wlMnisMessageStatus(response.msgType, response.target, response.source, response.msg, response.msgStatus);
					 //g_pNet->wlMnisMessage(PRIVATE_MSG_FLG,10750,1118,"逗比");
					 /************************************************************************/
					 /* temp code
					 /************************************************************************/
					 //REMOTE_TASK *pTask = new REMOTE_TASK;
					 //memset(pTask, 0, sizeof(REMOTE_TASK));
					 //pTask->cmd = REMOTE_CMD_MNIS_MSG;
					 ////task.pRemote = (CRemotePeer*)g_onLineClients.front();
					 //swprintf_s(pTask->param.info.msgParam.Contents, L"逗比");
					 //pTask->param.info.msgParam.Source = 1118;
					 //pTask->param.info.msgParam.Target = 10750;
					 //pTask->param.info.msgParam.Type = PRIVATE_MSG_FLG;
					 //push_back_task(pTask);

	}
		break;
	case GPS_IMME_COMM:
	{
						  printf_s("GPS_IMME_COMM:%d\r\n", response.gpsStatus);
						  g_pNet->wlMnisSendGpsStatus(response.operate, response.target, response.gpsType, response.cycle, response.gpsStatus);
	}
		break;
	case GPS_TRIGG_COMM:
	{
						   printf_s("GPS_TRIGG_COMM:%d\r\n", response.gpsStatus);
						   g_pNet->wlMnisSendGpsStatus(response.operate, response.target, response.gpsType, response.cycle, response.gpsStatus);
	}
		break;
	case GPS_IMME_CSBK:
	{
						  printf_s("GPS_IMME_CSBK:%d\r\n", response.gpsStatus);
						  g_pNet->wlMnisSendGpsStatus(response.operate, response.target, response.gpsType, response.cycle, response.gpsStatus);
	}
		break;
	case GPS_TRIGG_CSBK:
	{
						   printf_s("GPS_TRIGG_CSBK:%d\r\n", response.gpsStatus);
						   g_pNet->wlMnisSendGpsStatus(response.operate, response.target, response.gpsType, response.cycle, response.gpsStatus);
	}
		break;
	case STOP_QUERY_GPS:
	{
						   printf_s("STOP_QUERY_GPS:%d\r\n", response.gpsStatus);
						   g_pNet->wlMnisSendGpsStatus(response.operate, response.target, response.gpsType, response.cycle, response.gpsStatus);
	}
		break;
	case RADIO_ARS:
	{
					  printf_s("RADIO_ARS:%d\r\n", response.arsStatus);
	}
		break;
		// do nothing
	case RADIO_GPS:
	{
					  printf_s("RADIO_GPS:%d\r\n", response.gpsStatus);
	}
		break;
	case RECV_GPS:
	{
					 printf_s("RECV_GPS:%d\r\n", response.gpsStatus);
					 GPS gps = { 0 };
					 gps.lat = response.lat;
					 gps.lon = response.lon;
					 gps.speed = response.speed;
					 gps.valid = response.valid;
					 g_pNet->wlMnisSendGps(response.source, gps);
	}
		break;
	case GPS_IMME_CSBK_EGPS:
	{
							   printf_s("GPS_IMME_CSBK_EGPS:%d\r\n", response.gpsStatus);
							   g_pNet->wlMnisSendGpsStatus(response.operate, response.target, response.gpsType, response.cycle, response.gpsStatus);
	}
		break;
	case GPS_TRIGG_CSBK_EGPS:
	{
								printf_s("GPS_TRIGG_CSBK_EGPS:%d\r\n", response.gpsStatus);
								g_pNet->wlMnisSendGpsStatus(response.operate, response.target, response.gpsType, response.cycle, response.gpsStatus);
	}
		break;
	default:
		break;
	}
}

