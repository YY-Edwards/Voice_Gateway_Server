#include "stdafx.h"
#include "../include/NSDongle.h"
#include "../include/NSAmbe.h"
#include <process.h>
#include "../include/NSManager.h"
#include "../include/NSLog.h"
#include "../include/NSGlobal.h"

NSDongle::NSDongle(NSManager* pManager)
: m_hReadSerialEvent(NULL)
, m_hWriteSerialEvent(NULL)
, m_ParserState(FIND_START)
, m_dongleReadFrame(AMBE3000_PCM_BYTESINFRAME)
, m_dongleWriteFrame(AMBE3000_AMBE_BYTESINFRAME)
, m_pSerialRxThread(NULL)
, m_pSerialTxThread(NULL)
, m_bRun(false)
, m_pCurHanleRing(NULL)
, m_idleTimeTickCount(0)
, m_bIdle(true)
, m_ringBuffer(createRingBuffer(SIZE_RING_TX + 1, sizeof(change_data_t)))
, m_mutexRing(INITLOCKER())
, m_sizePool(SIZE_POOL)
, m_idxIdle(0)
, m_pManager(pManager)
, m_pLog(NSLog::instance())
, m_prevTime(0)
, m_frameCount(0)
, m_totalTime(0)
, m_curAvg(0.0)
, m_sendCount(0)
{
	m_pChangeDataPcmPools = (change_data_t*)calloc(m_sizePool, sizeof(change_data_t));
	m_pChangeDataAmbePools = (change_data_t*)calloc(m_sizePool, sizeof(change_data_t));
	m_pPcmPools = (tPCMFrame*)calloc(m_sizePool, sizeof(tPCMFrame));
	m_pAmbePools = (tAMBEFrame*)calloc(m_sizePool, sizeof(tAMBEFrame));
}
NSDongle::~NSDongle()
{
	stopDongle(&m_self);
	clearCurHandleRing();
	clearRingBuffer();
	free(m_pPcmPools);
	free(m_pAmbePools);
	free(m_pChangeDataPcmPools);
	free(m_pChangeDataAmbePools);
	DELETELOCKER(m_mutexRing);
}

int NSDongle::Initialize(dongle_t* p)
{
	//m_parent = parent;
	m_self = *p;
	if (!openDongle(&m_self))
	{
		return WL_FAIL;
	}
	unsigned int m_pSerialRxThreadId, m_pSerialTxThreadId;
	/*建立写线程*/
	m_pSerialRxThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		SerialRxThreadProc,
		this,
		CREATE_SUSPENDED,
		&m_pSerialRxThreadId
		);
	if (NULL == m_pSerialRxThread)
	{
		m_pLog->AddLog(Ns_Log_Error, "%s create SerialRxThreadProc fail", m_self.strname);
		stopDongle(&m_self);
		return WL_FAIL;
	}
	/*建立读线程*/
	m_pSerialTxThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		SerialTxThreadProc,
		this,
		CREATE_SUSPENDED,
		&m_pSerialTxThreadId
		);
	if (NULL == m_pSerialTxThread)
	{
		m_pLog->AddLog(Ns_Log_Error, "%s create SerialTxThreadProc fail", m_self.strname);
		stopDongle(&m_self);
		return WL_FAIL;
	}

	/*初始化事件*/
	m_hReadSerialEvent = CreateEvent(NULL, FALSE, FALSE, NULL); //Manual Reset
	if (NULL == m_hReadSerialEvent)
	{
		m_pLog->AddLog(Ns_Log_Error, "%s create m_hReadSerialEvent fail", m_self.strname);
		stopDongle(&m_self);
		return WL_FAIL;
	}
	m_hWriteSerialEvent = CreateEvent(NULL, FALSE, FALSE, NULL); //Manual Reset,
	if (NULL == m_hWriteSerialEvent){
		m_pLog->AddLog(Ns_Log_Error, "%s create m_hWriteSerialEvent fail", m_self.strname);
		stopDongle(&m_self);
		return WL_FAIL;
	}

	/*分配内存给各自缓存*/
	int indexPool = m_sizePool;
	tAMBEFrame* pAmbe = NULL;
	tPCMFrame* pPcm = NULL;
	while (indexPool--)
	{
		pAmbe = &m_pAmbePools[indexPool];
		pAmbe->fld.Sync = AMBE3000_SYNC_BYTE;
		pAmbe->fld.LengthH = AMBE3000_AMBE_LENGTH_HBYTE;
		pAmbe->fld.LengthL = AMBE3000_AMBE_LENGTH_LBYTE;
		pAmbe->fld.Type = AMBE3000_AMBE_TYPE_BYTE;
		pAmbe->fld.ID = AMBE3000_AMBE_CHANDID_BYTE;
		pAmbe->fld.Num = AMBE3000_AMBE_NUMBITS_BYTE;
		pAmbe->fld.PT = AMBE3000_PARITYTYPE_BYTE;
		m_pChangeDataAmbePools[indexPool].pReadyData = pAmbe;

		pPcm = &m_pPcmPools[indexPool];
		pPcm->fld.Sync = AMBE3000_SYNC_BYTE;
		pPcm->fld.LengthH = AMBE3000_PCM_LENGTH_HBYTE;
		pPcm->fld.LengthL = AMBE3000_PCM_LENGTH_LBYTE;
		pPcm->fld.Type = AMBE3000_PCM_TYPE_BYTE;
		pPcm->fld.ID = AMBE3000_PCM_SPEECHID_BYTE;
		pPcm->fld.Num = AMBE3000_PCM_NUMSAMPLES_BYTE;
		m_pChangeDataPcmPools[indexPool].pReadyData = pPcm;
	}

	m_bRun = true;
	ResumeThread(m_pSerialTxThread);
	ResumeThread(m_pSerialRxThread);
	return WL_OK;
}

bool NSDongle::purgeCommDongle(dongle_t* p)
{
	if (p->hcom)
	{
		int rlt = PurgeComm(p->hcom, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
		if (0 == rlt)
		{
			m_pLog->AddLog(Ns_Log_Info, "%s PurgeComm fail:%d", m_self.strname, GetLastError());
			return false;
		}
		return true;
	}
	return false;
}

bool NSDongle::openDongle(dongle_t* p)
{
	unsigned long result = 0;
	if (p->hcom)
	{
		purgeCommDongle(p);
	}
	p->hcom = CreateFile(
		p->createfile,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		0
		);
	if (p->hcom == INVALID_HANDLE_VALUE)
	{
		m_pLog->AddLog(Ns_Log_Error, "%s CreateFile fail:%d", m_self.strname, GetLastError());
		stopDongle(p);
		return false;
	}
	//设置dongle的参数
	result = setupDongle(p);
	if (0 != result)
	{
		m_pLog->AddLog(Ns_Log_Error, "%s SetupDongle fail:%d", m_self.strname, result);
		stopDongle(p);
		return false;
	}
	//清除一切对当前dongle的操作
	result = purgeDongle(p, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_TXABORT);
	if (0 != result)
	{
		m_pLog->AddLog(Ns_Log_Error, "%s PurgeDongle fail:%d", m_self.strname, result);
		stopDongle(p);
		return false;
	}
	return true;
}

unsigned long NSDongle::setupDongle(dongle_t* p)
{
	unsigned long result;
	DCB dcb;
	COMMTIMEOUTS cto;

	//初始化dongle通信设备的通信参数
	if (0 == SetupComm(p->hcom, INTERNALCOMBUFFSIZE, INTERNALCOMBUFFSIZE))
	{
		result = GetLastError();
		return result;
	}
	//指定dongle监视通信设备的事件
	if (0 == SetCommMask(p->hcom, NULL))
	{
		result = GetLastError();
		return result;
	}

	//读取串口设置
	if (0 == GetCommState(p->hcom, &dcb)){
		result = GetLastError();
		return result;
	}

	//自定义串口部分属性
	dcb.BaudRate = DONGLEBAUDRATE;
	dcb.ByteSize = DONGLEBITS;
	dcb.Parity = DONGLEPARITY;
	dcb.fParity = FALSE;
	dcb.StopBits = DONGLESTOP;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fAbortOnError = TRUE;

	//将属性写入串口
	if (0 == SetCommState(p->hcom, &dcb)){
		result = GetLastError();
		return result;
	}

	//获取dongle设备读写时的超时参数设定
	if (0 == GetCommTimeouts(p->hcom, &cto)){
		result = GetLastError();
		return result;
	}

	//自定义dongle设备读写时的超时部分参数
	//cto.ReadIntervalTimeout = RXGAPTIMEOUT;
	cto.ReadIntervalTimeout = 0;
	cto.ReadTotalTimeoutConstant = 0; //Value of Zero==not used.
	cto.ReadTotalTimeoutMultiplier = 0; //Value of Zero==not used.
	cto.WriteTotalTimeoutConstant = 0; //Value of Zero==not used.
	cto.WriteTotalTimeoutMultiplier = 0; //Value of Zero==not used.

	//设定dongle设备读写时的超时参数
	if (0 == SetCommTimeouts(p->hcom, &cto)){
		result = GetLastError();
		return result;
	}
	return 0;
}

unsigned long NSDongle::purgeDongle(dongle_t* p, unsigned long flags)
{
	unsigned long result;
	if (NULL != p->hcom)
	{
		if (FALSE == ClearCommError(p->hcom, &result, NULL))
		{
			result = GetLastError();
			return result;
		}
		if (FALSE == PurgeComm(p->hcom, flags))
		{
			result = GetLastError();
			return result;
		}
	}
	return 0;
}

void NSDongle::stopDongle(dongle_t* p)
{
	m_bRun = false;
	purgeCommDongle(p);
	if (NULL != m_pSerialTxThread)
	{
		SetEvent(m_hReadSerialEvent);
		WaitForSingleObject(m_pSerialTxThread, 1000);
		CloseHandle(m_pSerialTxThread);
		m_pSerialTxThread = NULL;
	}
	if (NULL != m_pSerialTxThread)
	{
		SetEvent(m_hWriteSerialEvent);
		WaitForSingleObject(m_pSerialTxThread, 1000);
		CloseHandle(m_pSerialTxThread);
		m_pSerialTxThread = NULL;
	}
	if (NULL != m_hReadSerialEvent)
	{
		CloseHandle(m_hReadSerialEvent);
		m_hReadSerialEvent = NULL;
	}
	if (NULL != m_hWriteSerialEvent)
	{
		CloseHandle(m_hWriteSerialEvent);
		m_hWriteSerialEvent = NULL;
	}
	if (p->hcom)
	{
		CloseHandle(p->hcom);
		p->hcom = NULL;
	}
}

unsigned int __stdcall NSDongle::SerialTxThreadProc(void* pArguments)
{
	NSDongle* p = (NSDongle*)pArguments;
	if (p)
	{
		p->SerialTxThread();
	}
	return 0;
}

unsigned int __stdcall NSDongle::SerialRxThreadProc(void* pArguments)
{
	NSDongle* p = (NSDongle*)pArguments;
	if (p)
	{
		p->SerialRxThread();
	}
	return 0;
}

void NSDongle::SerialTxThread()
{
	unsigned long result = 0;
	unsigned long dwWritten = 0;
	m_pLog->AddLog(Ns_Log_Info, "%s SerialTxThread Start", m_self.strname);
	while (true)
	{
		unsigned long rlt = WaitForSingleObject(m_hWriteSerialEvent, RX_TX_TIMEOUT);
		if (!m_bRun)
		{
			m_pLog->AddLog(Ns_Log_Info, "%s SerialTxThread will exit", m_self.strname);
			break;
		}
		if (rlt == WAIT_TIMEOUT)
		{
			continue;
		}
		initRead();
	}
	m_pLog->AddLog(Ns_Log_Info, "%s SerialTxThread End", m_self.strname);
}

void NSDongle::SerialRxThread()
{
	m_pLog->AddLog(Ns_Log_Info, "%s SerialRxThread Start", m_self.strname);
	unsigned long dwBytesConsumed = 0;
	unsigned long result = 0;
	unsigned long dwRead = 0;
	while (true)
	{
		unsigned long rlt = WaitForSingleObject(m_hReadSerialEvent, RX_TX_TIMEOUT);
		if (!m_bRun)
		{
			m_pLog->AddLog(Ns_Log_Info, "%s SerialRxThread will exit", m_self.strname);
			break;
		}
		if (rlt == WAIT_TIMEOUT)
		{
			handleEndWork();
			continue;
		}
		if (!GetOverlappedResult(m_self.hcom, &m_osReader, &dwRead, FALSE))
		{
			m_pLog->AddLog(Ns_Log_Info, "%s GetOverlappedResult error", m_self.strname);
			result = GetLastError();
			result = purgeDongle(&m_self, PURGE_RXABORT | PURGE_RXCLEAR);
		}
		else
		{
			/*处理数据*/
			assembleMsg(dwRead, &dwBytesConsumed);
		}
		/*结束标识*/
		if (0 == sizeRing())
		{
			setUsing(false);
			continue;
		}
		initWrite();
	}
	m_pLog->AddLog(Ns_Log_Info, "%s SerialRxThread End", m_self.strname);
}

tAMBEFrame* NSDongle::getFreeAmbeBuffer(void)
{
	tAMBEFrame* pAMBEFrame = new tAMBEFrame;
	pAMBEFrame->fld.Sync = AMBE3000_SYNC_BYTE;
	pAMBEFrame->fld.LengthH = AMBE3000_AMBE_LENGTH_HBYTE;
	pAMBEFrame->fld.LengthL = AMBE3000_AMBE_LENGTH_LBYTE;
	pAMBEFrame->fld.Type = AMBE3000_AMBE_TYPE_BYTE;
	pAMBEFrame->fld.ID = AMBE3000_AMBE_CHANDID_BYTE;
	pAMBEFrame->fld.Num = AMBE3000_AMBE_NUMBITS_BYTE;
	pAMBEFrame->fld.PT = AMBE3000_PARITYTYPE_BYTE;
	return pAMBEFrame;
}

void NSDongle::WriteAmbe(void* src, int dataLen, pOnData fun, void* param)
{
	if (NULL == src || 0 >= dataLen)
	{
		m_pLog->AddLog(Ns_Log_Error, "%s src is NULL", m_self.strname);
		return;
	}
	char *pBuffer = (char*)src;
	int count = 0;
	int leftLen = dataLen;
	int index = 0;
	int readLen = 0;
	while (leftLen > 0 && 1 == m_bRun)
	{
		/*申请内存*/
		change_data_t* item = getFreeAmbeChangeDataBuffer(fun, param);
		if (NULL == item)
		{
			m_pLog->AddLog(Ns_Log_Error, "%s getFreeAmbeChangeDataBuffer fail", m_self.strname);
			m_idxIdle--;
			return;
		}
		tAMBEFrame* pAMBEFrame = (tAMBEFrame*)item->pReadyData;
		item->index = ++count;

		/*数据构建*/
		int readLen = (leftLen >= 7) ? 7 : leftLen;
		memcpy(pAMBEFrame->fld.ChannelBits, pBuffer + index, readLen);
		leftLen -= readLen;
		index += readLen;
		deObfuscate(AMBETODONGLE, pAMBEFrame);
		markAmbeBufferFilled(pAMBEFrame);

		/*写入缓冲*/
		//int size =sizeRing();
		//if (size == SIZE_RING_TX)
		//{
		//	m_pLog->AddLog("%s ring buffer will full,then sleep...", m_self.strname);
		//	Sleep(RX_TX_TIMEOUT);
		//}
		//if (!addRingItem(item))
		//{
		//	m_pLog->AddLog("%s ring buffer push fail", m_self.strname);
		//	m_idxIdle--;
		//	break;
		//}
		if (sizeRing() < SIZE_RING_TX)
		{
			if (!addRingItem(item))
			{
				char temp[1024] = { 0 };
				StatusInfo(temp);
				m_pLog->AddLog(Ns_Log_Error, "ambe ring buffer push fail,%s", temp);
				m_idxIdle--;
				break;
			}
		}
		else
		{
			char temp[1024] = { 0 };
			StatusInfo(temp);
			m_pLog->AddLog(Ns_Log_Error, "ambe ring buffer full,%s", temp);
			m_idxIdle--;
			break;
		}
	}
}

void NSDongle::deObfuscate(ScrambleDirection theDirection, tAMBEFrame* &pAMBEFrame)
{
	char DebugBitsComingIn[49];
	char BitsGoingOut[49];
	int  i, j;
	int  InColumn, OutColumn;
	const int * DirectionArray;
	char theByte;

	//Code to break apart and shuffle the bits.
	switch (theDirection)
	{
	case AMBETODONGLE:
		DirectionArray = &IPSCTODONGLETABLE[0];
		break;
	case DONGLETOAMBE:
		DirectionArray = &DONGLETOIPSCTABLE[0];
		break;
	default:
		//Shouldn't happen.
		return;
	}

	for (i = 0; i < 6; i++)
	{
		//Will treat last bit as special case.
		InColumn = i * 8 + 7; //7, 15, 23, 31, 39,47
		theByte = pAMBEFrame->fld.ChannelBits[i];
		for (j = 0; j < 8; j++)
		{
			OutColumn = *(DirectionArray + InColumn);

			DebugBitsComingIn[InColumn] = theByte & 0x01;
			BitsGoingOut[OutColumn] = theByte & 0x01;
			InColumn--;
			theByte = theByte >> 1;
		}
	}
	InColumn = 48;  //Special case of last bit.
	OutColumn = *(DirectionArray + 48);
	theByte = ((pAMBEFrame->fld.ChannelBits[6]) >> 7) & 0x01;
	DebugBitsComingIn[48] = theByte;
	BitsGoingOut[OutColumn] = theByte;

	//Code to re-assemble the shuffled bits.
	OutColumn = 0;
	//More efficient to increment than calculate(??)
	for (i = 0; i < 6; i++)
	{
		//Will treat last bit as special case.
		theByte = 0;
		for (j = 0; j < 8; j++){
			theByte = (theByte << 1) + BitsGoingOut[OutColumn++];
		}
		pAMBEFrame->fld.ChannelBits[i] = theByte;
	}
	theByte = BitsGoingOut[48] << 7;  //Handle special case.
	pAMBEFrame->fld.ChannelBits[6] = theByte;
}

void NSDongle::markAmbeBufferFilled(tAMBEFrame* &pFrame)
{
	DVSI3000struct* pAMBEBuffer;
	pAMBEBuffer = (DVSI3000struct*)pFrame;
	pAMBEBuffer->AMBEType.theAMBEFrame.fld.PP = CheckSum(pAMBEBuffer);
}

unsigned __int8 NSDongle::CheckSum(DVSI3000struct* pMsg)
{
	int i, length;
	unsigned __int8 sum;
	length = (pMsg->base.LengthH) << 8;
	length += pMsg->base.LengthL;
	sum = 0;
	for (i = 1; i < length + 3; i++)
	{
		sum ^= pMsg->All[i];
	}
	return sum;
}

int NSDongle::assembleMsg(unsigned long numBytes, unsigned long * dwBytesAssembled)
{
	int Index;
	unsigned __int8 ch;
	int WholeMessageCount = 0;
	DWORD bytecount = 0;
	DVSI3000struct m_RxDVSImsg;

	*dwBytesAssembled = 0;
	Index = 0;
	while (0 < numBytes--)
	{
		bytecount++;
		ch = m_DongleRxBuffer[Index++];
		switch (m_ParserState)	//Simple state machine to get generic DVSI msg
		{
		case FIND_START:		//search for start byte
			if (AMBE3000_SYNC_BYTE == ch)
			{
				m_RxDVSImsg.base.Sync = AMBE3000_SYNC_BYTE;
				m_ParserState = HIGH_LENGTH;	//get high byte of length
			}
			break;
		case HIGH_LENGTH:	//here for high byte of length
			m_RxDVSImsg.base.LengthH = ch;
			m_RxMsgLength = ch << 8;
			m_ParserState = LOW_LENGTH;	//get low byte of length
			break;
		case LOW_LENGTH:	//here for low byte of length
			m_RxDVSImsg.base.LengthL = ch;
			m_RxMsgLength += ch;
			if (0x144 < m_RxMsgLength)
			{
				m_RxMsgLength = 0x144;
			}
			m_RxMsgLength += 1; //length remaining.
			m_RxMsgIndex = 3;
			m_ParserState = READ_DATA;	//get rest of data
			break;
		case READ_DATA:	//try to read the rest of the message or to end of buffer
			m_RxDVSImsg.All[m_RxMsgIndex++] = ch;
			if ((--m_RxMsgLength) == 0)
			{
				m_RxMsgLength = ((m_RxDVSImsg.base.LengthH) << 8) + (m_RxDVSImsg.base.LengthL) + 4; //Total length.
				m_ParserState = FIND_START;	//go back to first stage
				parseDVSImsg(&m_RxDVSImsg);	//call DVSI message parsing routine	
				*dwBytesAssembled += bytecount;
				bytecount = 0;
				WholeMessageCount++;
			}
			break;
		default:
			m_ParserState = FIND_START;	//reset parser to first stage
			break;
		} //end switch statement
	}
	return WholeMessageCount;
}

void NSDongle::parseDVSImsg(DVSI3000struct* pMsg)
{
	UINT mType;
	//strip out just type field
	mType = pMsg->base.Type;
	int start = 0, end = 0, dif = 0;
	switch (mType)
	{
	case AMBE3000_AMBE_TYPE_BYTE:
	{
									tAMBEFrame* p = (tAMBEFrame*)pMsg;
									deObfuscate(DONGLETOAMBE, p);
									handleOutAmbe((unsigned char*)&(pMsg->AMBEType.theAMBEFrame.fld.ChannelBits[0]));
	}
		break;
	case AMBE3000_PCM_TYPE_BYTE:
	{
								   handleOutPcm((unsigned __int8*)&(pMsg->PCMType.thePCMFrame.fld.Samples[0]));
	}
		break;
	case AMBE3000_CCP_TYPE_BYTE:
	{
								   m_pLog->AddLog(Ns_Log_Info, "%s ParseDVSImsg,DONGLE_EVENT:%c", pMsg->base.empty[0], m_self.strname);

	}
		break;
	}
}

void NSDongle::initRead()
{
	//m_pLog->AddLog("%s initRead", Name());
#if _DEBUG
	unsigned long cur = GetTickCount();
	int dealTime = cur - m_prevTime;
	if (dealTime > MILSECONDS_IDLE_REAL)
	{
		m_frameCount = 0;
		m_totalTime = 0;
		m_curAvg = 0;
		m_sendCount = 0;
	}
	else
	{
		m_frameCount++;
		m_totalTime += dealTime;
		m_curAvg = (double)m_totalTime / (double)m_frameCount;
	}
	m_prevTime = cur;
	//m_pLog->AddLog("%s cur frame time:%dms,avg:%.4fms", Name(), dealTime, rlt);
#endif
	alive();
	unsigned long dwRead = 0;
	unsigned long result = 0;
	/*初始化读*/
	memset(&m_osReader, 0, sizeof(OVERLAPPED));
	m_osReader.hEvent = m_hReadSerialEvent;

	if (!ReadFile(m_self.hcom, &m_DongleRxBuffer, m_dongleReadFrame, &dwRead, &m_osReader))
	{
		result = GetLastError();
		if (ERROR_IO_PENDING != result)
		{
			/*发生意外错误,清空当前*/
			m_pLog->AddLog(Ns_Log_Info, "%s Dongle ReadFile fail:%d", m_self.strname, result);
			result = purgeDongle(&m_self, PURGE_RXABORT | PURGE_RXCLEAR);
		}
	}
}

void NSDongle::initWrite()
{
	//m_pLog->AddLog("%s initWrite",Name());
	alive();
	unsigned long dwWritten = 0;
	unsigned long result = 0;
	const void* pdata = NULL;

	//LOG_INFO("pop");
	//获取待处理的AMPE帧数据
	clearCurHandleRing();
	m_pCurHanleRing = new change_data_t;
	popRingItem(m_pCurHanleRing);
	m_dongleReadFrame = m_pCurHanleRing->read;
	m_dongleWriteFrame = m_pCurHanleRing->write;
	if (AMBE3000_AMBE_BYTESINFRAME == m_dongleWriteFrame)
	{
		tAMBEFrame* pAmbe = (tAMBEFrame*)m_pCurHanleRing->pReadyData;
		pdata = &pAmbe->All[0];
	}
	else
	{
		tPCMFrame* pPcm = (tPCMFrame*)m_pCurHanleRing->pReadyData;
		pdata = &pPcm->All[0];
	}

	memset(&m_osWriter, 0, sizeof(OVERLAPPED));
	m_osWriter.hEvent = m_hWriteSerialEvent;
	if (!WriteFile(m_self.hcom, pdata, m_dongleWriteFrame, &dwWritten, &m_osWriter))
	{
		result = GetLastError();
		if (ERROR_IO_PENDING != result)
		{
			/*发生意外错误,清空当前*/
			m_pLog->AddLog(Ns_Log_Info, "%s Dongle WriteFile fail:%d", m_self.strname, result);
			result = purgeDongle(&m_self, PURGE_TXABORT | PURGE_TXCLEAR);
		}
	}
}

void NSDongle::handleOutPcm(unsigned __int8* pSamples)
{
	m_sendCount++;
	int i = 0;
	//定义新的数据指针
	out_data_pcm_t* pData = &m_outPcmData;
	//大小端转换
	for (i = 0; i < OUT_SAMPLES_PER_20mS; i++)
	{
		pData->_head[1 + (i << 1)] = *pSamples++;
		pData->_head[+(i << 1)] = *pSamples++;
	}
	if (m_pCurHanleRing)
	{
		if (m_pCurHanleRing->pOnData)
		{
			//处理PCM数据
			(*(m_pCurHanleRing->pOnData))(pData->_head, sizeof(pData->_head), m_pCurHanleRing->index, m_pCurHanleRing->param);
		}
		else
		{
			m_pLog->AddLog(Ns_Log_Error, "m_pCurHanleRing->pOnData is null");
		}
	}
	else
	{
		m_pLog->AddLog(Ns_Log_Error, "m_pCurHanleRing is null");
	}
}

bool NSDongle::IsIdle()
{
	return m_bIdle;
}

void NSDongle::clearRingBuffer()
{
	TRYLOCK(m_mutexRing);
	freeRingBuffer(m_ringBuffer);
	m_ringBuffer = NULL;
	RELEASELOCK(m_mutexRing);
}

change_data_t* NSDongle::getFreeAmbeChangeDataBuffer(pOnData fun, void* param)
{
	m_idxIdle++;
	m_idxIdle = m_idxIdle%m_sizePool;
	change_data_t* p = &m_pChangeDataAmbePools[m_idxIdle];
	p->read = AMBE3000_PCM_BYTESINFRAME;
	p->write = AMBE3000_AMBE_BYTESINFRAME;
	p->pOnData = fun;
	p->param = param;
	return p;
}

void NSDongle::clearCurHandleRing()
{
	if (m_pCurHanleRing)
	{
		m_pCurHanleRing->param = NULL;
		m_pCurHanleRing->pOnData = NULL;
		delete m_pCurHanleRing;
		m_pCurHanleRing = NULL;
	}
}

void NSDongle::setUsing(bool value)
{
	m_self.isusing = value;
}

void NSDongle::handleInitWrite()
{
	if (!m_self.isusing && 0 < sizeRing())
	{
		setUsing(true);
		initWrite();
	}
}

void NSDongle::handleEndWork()
{
	if (!m_bIdle)
	{
		if (m_idleTimeTickCount < GetTickCount())
		{
			if (!m_self.isusing)
			{
				m_bIdle = true;
				/*回收此Dongle*/
				m_pManager->AddIdleDonglesItem(this);
			}
			else
			{
				if (g_pNSManager)
				{
					int size_dongle = g_pNSManager->SizeDongle() - 1;
					size_dongle = (size_dongle < 0) ? 0 : size_dongle;
					g_pNSManager->setSizeDongle(size_dongle);
					onsystemstatuschange_info_t info = { 0 };
					info.type = System_DongleCount;
					info.value = size_dongle;
					NS_SafeSystemStatusChangeEvent(&info);
				}
				m_pLog->AddLog(Ns_Log_Error, "%s is bad,maybe you need Re-plug dongle device.if invalid,please replace dongle device", Name());
			}
			if (m_pCurHanleRing)
			{
				(*(m_pCurHanleRing->pOnData))(NULL, 0, 0, m_pCurHanleRing->param);
				clearCurHandleRing();
			}
		}
	}
}

bool NSDongle::addRingItem(change_data_t* p)
{
	TRYLOCK(m_mutexRing);
	bool rlt = (-1 != push(m_ringBuffer, p));
	RELEASELOCK(m_mutexRing);
	if (rlt)
	{
		handleInitWrite();
	}
	return rlt;
}

void NSDongle::WritePcm(void* src, int dataLen, pOnData fun, void* param)
{
	change_data_t tmpItem = { 0 };
	change_data_t* pTmpItem = &tmpItem;
	while (g_should_delete > 0)
	{
		popRingItem(pTmpItem);
		g_should_delete--;
	}
	pTmpItem = NULL;

	if (NULL == src || 0 >= dataLen)
	{
		m_pLog->AddLog(Ns_Log_Info, "%s src is NULL", m_self.strname);
		return;
	}
	//m_pLog->AddLog("%s pcm to ambe start", m_self.strname);
	unsigned __int8* pData = (unsigned __int8*)src;
	int i = 0;
	int length = dataLen;
	int count = 0;
	while (length && 1 == m_bRun)
	{
		/*存在损失最多20ms音频数据的可能*/
		if (length < 320)
		{
			break;
		}
		change_data_t* item = getFreePcmChangeDataBuffer(fun, param);
		if (NULL == item)
		{
			m_pLog->AddLog(Ns_Log_Error, "%s getFreePcmChangeDataBuffer fail", m_self.strname);
			m_idxIdle--;
			return;
		}
		tPCMFrame* pPcmData = (tPCMFrame*)item->pReadyData;
		item->index = ++count;
		for (i = 0; i < AMBE3000_PCM_INTSAMPLES_BYTE; i++)
		{
			pPcmData->fld.Samples[1 + (i << 1)] = *pData++; //Endian conversion.
			pPcmData->fld.Samples[(i << 1)] = *pData++;
		}
		pPcmData->fld.PT = AMBE3000_PARITYTYPE_BYTE;
		pPcmData->fld.PP = CheckSum((DVSI3000struct *)pPcmData);
		length -= 320;

		/*写入缓冲*/
		//int size = sizeRing();
		//if (size == SIZE_RING_TX)
		//{
		//	m_pLog->AddLog("%s pcm ring buffer will full,then sleep...,status:%s", m_self.strname, IsIdle() ? "IDLE" : "WORK");
		//	Sleep(RX_TX_TIMEOUT);
		//}
		////LOG_INFO("push");
		//if (!addRingItem(item))
		//{
		//	m_pLog->AddLog("%s ring buffer push fail", m_self.strname);
		//	m_idxIdle--;
		//	break;
		//}
		if (sizeRing() < SIZE_RING_TX)
		{
			if (!addRingItem(item))
			{
				char temp[1024] = { 0 };
				StatusInfo(temp);
				m_pLog->AddLog(Ns_Log_Error, "pcm ring buffer push fail,%s", temp);
				m_idxIdle--;
				break;
			}
		}
		else
		{
			char temp[1024] = { 0 };
			StatusInfo(temp);
			m_pLog->AddLog(Ns_Log_Error, "pcm ring buffer full,%s", temp);
			m_idxIdle--;
			break;
		}
	}
}

change_data_t* NSDongle::getFreePcmChangeDataBuffer(pOnData fun, void* param)
{
	m_idxIdle++;
	m_idxIdle = m_idxIdle%m_sizePool;
	change_data_t* p = &m_pChangeDataPcmPools[m_idxIdle];
	p->read = AMBE3000_AMBE_BYTESINFRAME;
	p->write = AMBE3000_PCM_BYTESINFRAME;
	p->pOnData = fun;
	p->param = param;
	return p;
}

void NSDongle::handleOutAmbe(unsigned __int8* pSamples)
{
	m_sendCount++;
	if (m_pCurHanleRing)
	{
		if (m_pCurHanleRing->pOnData)
		{
			//处理AMBE数据
			(*(m_pCurHanleRing->pOnData))(pSamples, 7, m_pCurHanleRing->index, m_pCurHanleRing->param);
		}
		else
		{
			m_pLog->AddLog(Ns_Log_Error, "m_pCurHanleRing->pOnData is null");
		}
	}
	else
	{
		m_pLog->AddLog(Ns_Log_Error, "m_pCurHanleRing is null");
	}
}

int NSDongle::sizeRing()
{
	TRYLOCK(m_mutexRing);
	int size = getRingBufferSize(m_ringBuffer);
	RELEASELOCK(m_mutexRing);
	return size;
}

void NSDongle::popRingItem(change_data_t* &item)
{
	TRYLOCK(m_mutexRing);
	pop(m_ringBuffer, item);
	RELEASELOCK(m_mutexRing);
}

char* NSDongle::Name()
{
	return m_self.strname;
}

void NSDongle::StatusInfo(char* msg)
{
	if (msg)
	{
		sprintf(msg, "name:%s,work status:%s, avg : %.4fms, waste time : %lums, frame count : %lu,send count:%lu,ring size:%d"
			, Name()
			, IsIdle() ? "IDLE" : "WORK"
			, m_curAvg, m_totalTime
			, m_frameCount
			, m_sendCount
			, sizeRing());
	}
}
void NSDongle::ReadyUse()
{
	m_pLog->AddLog(Ns_Log_Info, "%s reset start", Name());
	m_idxIdle = 0;
	change_data_t item = { 0 };
	change_data_t* pItem = &item;
	while (sizeRing() > 0)
	{
		popRingItem(pItem);
	}
	pItem = NULL;
	m_pLog->AddLog(Ns_Log_Info, "%s reset end", Name());
}

void NSDongle::alive()
{
	m_bIdle = false;
	m_idleTimeTickCount = GetTickCount() + MILSECONDS_IDLE_REAL;
}
