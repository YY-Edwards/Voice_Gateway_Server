
#include "stdafx.h"
#include "Manager.h"
#include "SerialDongle.h"
#include "Sound.h"
#include "WLNet.h"
//temp delete
//#include "Net.h"

CTool g_tool;
BOOL g_bPTT;      //Set or cleared by user interface.
BOOL g_bTX;       //Set or cleared by dongle.

CSerialDongle* g_pDongle;
BOOL g_dongleIsUsing;

CManager::CManager(CMySQL *pDb)
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
	m_bDongleIsOpen = FALSE;
	g_dongleIsUsing = FALSE;
	m_pDb = pDb;
}

CManager::~CManager()
{
	m_report = NULL;
	if (m_theTimeCaps.wPeriodMin != 0)
	{
		timeEndPeriod(m_theTimeCaps.wPeriodMin);//清除最小定时器精度
	}
}

int CManager::initSys()
{
	WCHAR tmpStr[128] = { 0 };
	LPCTSTR lpctTmpStr = tmpStr;
	DWORD rlt = 0;
	BOOL netRlt = FALSE;

	//init net
	g_net_connect = g_pNet->StartNet(inet_addr(CONFIG_MASTER_IP), CONFIG_MASTER_PORT, INADDR_ANY, CONFIG_LOCAL_PEER_ID, CONFIG_LOCAL_RADIO_ID, CONFIG_RECORD_TYPE);
	if (!g_net_connect)
	{
		//m_bDongleIsOpen = FALSE;
		sprintf_s(m_reportMsg, "initDongle:open net fail");
	}
	else
	{
		//m_bDongleIsOpen = TRUE;
		sprintf_s(m_reportMsg, "initDongle:open net success");
		ReleaseDecodeEvent();
	}
	sendLogToWindow();

	initDongle(CONFIG_DONGLE_PORT);

	return 0;
}

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

int CManager::play()
{
	DWORD result = 0;
	if (m_bDongleIsOpen)
	{
		result = WaitForSingleObject(m_hWaitDecodeEvent, INFINITE);
		RequireDecodeEvent();
		if (WAIT_TIMEOUT == result)
		{
			return 1;
		}

		if (g_pDongle->changeAMBEToPCM())
		{
			LoadVoiceData(VOICE_DATA_PATH);
		}
		else
		{
			ReleaseDecodeEvent();
			return 1;
		}
	}
	return 0;
}

int CManager::play(unsigned int length, char* pData)
{
	DWORD result = 0;
	if (m_bDongleIsOpen)
	{
		result = WaitForSingleObject(m_hWaitDecodeEvent, INFINITE);
		RequireDecodeEvent();
		if (WAIT_TIMEOUT == result)
		{
			return 1;
		}

		if (g_pDongle->changeAMBEToPCM())
		{
			return LoadVoiceData(length, pData);
		}
		else
		{
			ReleaseDecodeEvent();
			return 1;
		}
	}
	return 0;
}

void CManager::LoadVoiceData(LPCWSTR filePath)
{
	//	FILE * f;

	DWORD result = 0;

	HANDLE hOpenFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (hOpenFile == INVALID_HANDLE_VALUE)
	{
		g_dongleIsUsing = FALSE;
		ReleaseDecodeEvent();
		sprintf_s(m_reportMsg, "open file fail");
		sendLogToWindow();
		return;
	}
	DWORD len = 0;
	len = GetFileSize(hOpenFile, 0);


	if (len <= 0)
	{
		g_dongleIsUsing = FALSE;
		ReleaseDecodeEvent();
		sprintf_s(m_reportMsg, "file length is zero");
		sendLogToWindow();
		CloseHandle(hOpenFile);
		return;
	}

	char *pBuffer = new char[len];
	ReadFile(hOpenFile, pBuffer, len, &len, NULL);
	CloseHandle(hOpenFile);

	int leftLen = len;

	tAMBEFrame* pAMBEFrame;

	pAMBEFrame = g_pDongle->GetFreeAMBEBuffer();
	int readLen = (leftLen >= 7) ? 7 : leftLen;
	memcpy(pAMBEFrame->fld.ChannelBits, pBuffer, readLen);
	leftLen -= readLen;
	int index = readLen;

	while (readLen == 7)
	{
		g_pDongle->deObfuscate(IPSCTODONGLE, pAMBEFrame);
		g_pDongle->MarkAMBEBufferFilled();
		pAMBEFrame = g_pDongle->GetFreeAMBEBuffer();
		if (NULL == pAMBEFrame)
		{
			g_dongleIsUsing = FALSE;
			delete[] pBuffer;
			ReleaseDecodeEvent();
			return;
		}
		readLen = (leftLen >= 7) ? 7 : leftLen;
		if (readLen <= 0)
		{
			break;
		}
		memcpy(pAMBEFrame->fld.ChannelBits, pBuffer + index, readLen);
		leftLen -= readLen;
		index += readLen;
	}


	delete[] pBuffer;
	g_pDongle->DecodeBuffers();
}

int CManager::LoadVoiceData(unsigned int length, char* pData)
{

	char *pBuffer = new char[length];
	memcpy(pBuffer, pData, length);

	int leftLen = length;

	tAMBEFrame* pAMBEFrame;

	pAMBEFrame = g_pDongle->GetFreeAMBEBuffer();
	int readLen = (leftLen >= 7) ? 7 : leftLen;
	memcpy(pAMBEFrame->fld.ChannelBits, pBuffer, readLen);
	leftLen -= readLen;
	int index = readLen;

	while (readLen == 7)
	{
		g_pDongle->deObfuscate(IPSCTODONGLE, pAMBEFrame);
		g_pDongle->MarkAMBEBufferFilled();
		pAMBEFrame = g_pDongle->GetFreeAMBEBuffer();
		if (NULL == pAMBEFrame)
		{
			g_dongleIsUsing = FALSE;
			delete[] pBuffer;
			ReleaseDecodeEvent();
			return 1;
		}
		readLen = (leftLen >= 7) ? 7 : leftLen;
		if (readLen <= 0)
		{
			break;
		}
		memcpy(pAMBEFrame->fld.ChannelBits, pBuffer + index, readLen);
		leftLen -= readLen;
		index += readLen;
	}


	delete[] pBuffer;
	g_pDongle->DecodeBuffers();
	return 0;
}

void CManager::ReleaseDecodeEvent()
{
	SetEvent(m_hWaitDecodeEvent);
}



int CManager::initialCall(char* pTartgetId, char* pCallType)
{
	unsigned long tartgetId = (unsigned long)atoll(pTartgetId);
	unsigned char callType = atoi(pCallType);
	if (m_bDongleIsOpen)
	{

		if (g_pDongle->changePCMToAMBE())
		{
			WORD callStatus = g_pNet->GetCallStatus();

			//callback
			if (callStatus == CALL_HANGUP)
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
				g_targetId = tartgetId;
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
				return 1;
			}

		}
		else
		{
			return 1;
		}
		return 0;
	}
	else
	{
		sprintf_s(m_reportMsg, "dongle is not open");
		sendLogToWindow();
		return 1;
	}
}

int CManager::stopCall()
{
	g_pNet->requestRecordEndEvent();
	g_pSound->StopRecord();
	g_pNet->waitRecordEnd();
	return 0;
}

void CManager::RequireDecodeEvent()
{
	ResetEvent(m_hWaitDecodeEvent);
}

int CManager::initDongle(unsigned int serial_port)
{

	DWORD rlt = 0;
	WCHAR tmpStr[128] = { 0 };
	LPCTSTR lpctTmpStr = tmpStr;

	// 	sprintf_s(m_reportMsg, "initDongle:serial_port:COM%u", serial_port);
	// 	sendLogToWindow();

	m_activePort = serial_port;
	swprintf_s(tmpStr, 128, L"\\\\.\\COM%d", m_activePort);

	//init dongle
	if (m_hwnd != NULL)
	{
		rlt = g_pDongle->OpenDongle(lpctTmpStr, m_hwnd, this);
		if (rlt != 0)
		{
			m_bDongleIsOpen = FALSE;
			g_dongle_open = false;
			sprintf_s(m_reportMsg, "initDongle:open dongle fail");
		}
		else
		{
			m_bDongleIsOpen = TRUE;
			g_dongle_open = true;
			sprintf_s(m_reportMsg, "initDongle:open dongle success");
			ReleaseDecodeEvent();
		}
		sendLogToWindow();
	}
	else
	{
		sprintf_s(m_reportMsg, "initDongle:m_hwnd is null");
		sendLogToWindow();
		rlt = -1;
	}

	return rlt;
}

int CManager::disConnect()
{
	g_pNet->StopNet();
	g_pDongle->CloseDongle();
	g_pSound->PleaseShutDown();
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

int CManager::setPlayCallOfCare(char* pCallType, char* pFrom, char* pTarget)
{
	return g_pNet->setPlayCallOfCare(pCallType, pFrom, pTarget);
}

int CManager::config(char* pMasterIp, char* pMasterPort, char* pLocalPeerId, char* pLocalRadioId, char* pReccordType, char* pDefaultGroup, char* pDonglePort, char* pHuangTime, char* pMasterHeartTime, char* pPeerHearTime, char* pDefaultSlot)
{
	int rlt = 0;
	strcpy_s(CONFIG_MASTER_IP, pMasterIp);
	CONFIG_MASTER_PORT = atoi(pMasterPort);
	CONFIG_LOCAL_PEER_ID = (unsigned long)atoll(pLocalPeerId);
	CONFIG_LOCAL_RADIO_ID = (unsigned long)atoll(pLocalRadioId);
	if (0 == strcmp("IPSC",pReccordType))
	{
		CONFIG_RECORD_TYPE = IPSC;
	}
	else if (0 == strcmp("CPC", pReccordType))
	{
		CONFIG_RECORD_TYPE = CPC;
	}
	else if (0 == strcmp("LCP", pReccordType))
	{
		CONFIG_RECORD_TYPE = LCP;
	}
	else
	{
		rlt = 1;
	}
	CONFIG_DEFAULT_GROUP = (unsigned long)atoll(pDefaultGroup);
	CONFIG_DONGLE_PORT = atoi(pDonglePort);
	CONFIG_HUNG_TIME = (unsigned long)atoll(pHuangTime);
	CONFIG_MASTER_HEART_TIME = (unsigned long)atoll(pMasterHeartTime);
	CONFIG_PEER_HEART_AND_REG_TIME = (unsigned long)atoll(pPeerHearTime);
	if (1 == atoi(pDefaultSlot))
	{
		CONFIG_DEFAULT_SLOT = SLOT1;
	}
	else if (2 == atoi(pDefaultSlot))
	{
		CONFIG_DEFAULT_SLOT = SLOT2;
	}
	else
	{
		rlt = 2;
	}
	return rlt;
}

