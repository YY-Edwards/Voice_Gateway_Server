#include "stdafx.h"
#include "SerialDongle.h"
//temp delete
//#include "Net.h"
#include "Sound.h"
#include "Manager.h"
#include "WLNet.h"

//temp delete
//CNet g_net;
CSound* g_pSound;
CWLNet* g_pNet;

CSerialDongle::CSerialDongle()
{
	m_waitNextNetDataTime = VOICE_NEXT_TIMEOUT;
	m_waitNextNetDataEvent = NULL;
	m_bCurCodingIsEnd = TRUE;
	m_bCoding = FALSE;
	m_prevAMBE = NULL;
	m_prevPCM = NULL;
	m_curAMBE = NULL;
	m_curPCM = NULL;
	//m_hParentWnd = NULL;
	m_PleaseStopSerial = TRUE;
	m_hComm = NULL;
	m_hReadSerialEvent = NULL;
	m_hRxSerialEventArray[SERIAL_TICKLE] = NULL;
	m_hRxSerialEventArray[SERIAL_RX] = NULL;
	m_hTxSerialEventArray[SERIAL_TICKLE] = NULL;
	m_hTxSerialEventArray[SERIAL_TX] = NULL;
	m_hWriteSerialEvent = NULL;
	m_hTickleTxSerialEvent = NULL;
	m_hTickleRxSerialEvent = NULL;

	m_pSerialTxThread = NULL;
	m_pSerialRxThread = NULL;

	m_pSerialTxThreadId = 0;
	m_pSerialRxThreadId = 0;

	m_SerialRxThreadRunning = FALSE;
	m_SerialTxThreadRunning = FALSE;

	ZeroMemory(&m_osWriter, sizeof(OVERLAPPED));
	ZeroMemory(&m_osReader, sizeof(OVERLAPPED));
	m_PCMBufTail = 0;
	m_AMBEBufTail = 0;

	m_bPleasePurgeAMBE = FALSE;
	m_AMBEBufHead = 0;
	m_bPleasePurgePCM = FALSE;
	m_PCMBufHead = 0;

	m_ParserState = FIND_START;
	m_RxMsgLength = 0;
	m_dwExpectedDongleRead = 0;
	//////////////////////////////////////////////////////////////////////////
	m_lpCmanager = NULL;

	m_dongleReadFrame = 0;
	m_dongleWriteFrame = 0;
	m_dongleFlag = 0x00;

	//m_bDecode = FALSE;//当前是否在解码状态
	//m_bPrepareDecode = FALSE;//当前是否在待解码状态
	//m_bPrepareEndDecode = FALSE;//当前是否在待结束解码状态
	//m_pEmptyAMBEFrame = NULL;
	m_decodeFlag = 0x00;
	//////////////////////////////////////////////////////////////////////////
}

CSerialDongle::~CSerialDongle()
{
	m_report = NULL;
	stop();
}

DWORD CSerialDongle::OpenDongle(LPCTSTR lpszDevice, CManager* lpCmanager)
{
	m_lpCmanager = lpCmanager;
	//初始化events
	m_waitNextNetDataEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD result;

	//m_hParentWnd = hParentWnd; //Save parent window in case
	//we need to send msg to ergo
	m_ParserState = FIND_START;
	m_RxMsgLength = 0;
	m_AMBEBufHead = 0;
	m_AMBEBufTail = 0;
	m_PCMBufHead = 0;
	m_PCMBufTail = 0;
	m_bPleasePurgeAMBE = FALSE;
	m_bPleasePurgePCM = FALSE;

	//初始化donggle
	stop(); //Shouldn't be anything running;
	m_PleaseStopSerial = FALSE;

	//Set up COM stuff.
	m_hReadSerialEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //Manual Reset
	if (NULL == m_hReadSerialEvent)
	{
		result = GetLastError();
		stop();
		return result;
	}
	m_hRxSerialEventArray[SERIAL_RX] = m_hReadSerialEvent;

	m_hWriteSerialEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //Manual Reset,
	if (NULL == m_hWriteSerialEvent){
		result = GetLastError();
		stop();
		return result;
	}
	m_hTxSerialEventArray[SERIAL_TX] = m_hWriteSerialEvent;

	//获取dongle的文件句柄
	m_hComm = CreateFile(
		lpszDevice,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		0
		);
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		result = GetLastError();
		stop();
		return result;
	}

	//设置dongle的参数
	result = SetupDongle();

	if (0 != result){
		//dongle配置失败
		stop();
		return result;
	}

	//清除一切对当前dongle的操作
	result = PurgeDongle(PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_TXABORT, 100);
	if (0 != result)
	{
		stop();
		return result;
	}

	//Ready serial Tx thread.
	m_hTickleTxSerialEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //Manual Reset
	if (NULL == m_hTickleTxSerialEvent){
		result = GetLastError();
		stop();
		return result;
	}
	m_hTxSerialEventArray[SERIAL_TICKLE] = m_hTickleTxSerialEvent;

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
		//创建用户界面线程TxThread失败
		stop();
		return 600;
	}

	m_SerialTxThreadRunning = TRUE;
	//Starts up below.

	//Ready serial thread.
	m_hTickleRxSerialEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //Manual Reset
	if (NULL == m_hTickleRxSerialEvent){
		result = GetLastError();
		stop();
		return result;
	}
	m_hRxSerialEventArray[SERIAL_TICKLE] = m_hTickleRxSerialEvent;

	//创建用户界面线程RxThread
	m_pSerialRxThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		SerialRxThreadProc,
		this,
		CREATE_SUSPENDED,
		&m_pSerialRxThreadId
		);
	if (NULL == m_pSerialRxThread){
		stop();
		return 601;
	}

	m_SerialRxThreadRunning = TRUE;
	//Starts up below.

	//Start serial thread.
	ResumeThread(m_pSerialTxThread);
	ResumeThread(m_pSerialRxThread);

	return 0;
}

void CSerialDongle::stop(void)
{
	sprintf_s(m_reportMsg, "↓start close dongle↓");
	sendLogToWindow();

	DWORD result;

	m_PleaseStopSerial = TRUE;


	sprintf_s(m_reportMsg, "close serial tx thread");
	sendLogToWindow();
	if (NULL != m_pSerialTxThread)
	{
		result = SetEvent(m_hTickleTxSerialEvent);
		result = WaitForSingleObject(m_pSerialTxThread, 1000);
		CloseHandle(m_pSerialTxThread);
		m_pSerialTxThread = NULL;
	}
	if (NULL != m_hTickleTxSerialEvent)
	{
		CloseHandle(m_hTickleTxSerialEvent);
		m_hTickleTxSerialEvent = NULL;
		m_hTxSerialEventArray[SERIAL_TICKLE] = NULL;
	}


	sprintf_s(m_reportMsg, "close serial rx thread");
	sendLogToWindow();
	if (NULL != m_pSerialRxThread)
	{
		result = SetEvent(m_hTickleRxSerialEvent);
		result = WaitForSingleObject(m_pSerialRxThread, 1000);
		CloseHandle(m_pSerialRxThread);
		m_pSerialRxThread = NULL;
	}
	if (NULL != m_hTickleRxSerialEvent)
	{
		CloseHandle(m_hTickleRxSerialEvent);
		m_hTickleRxSerialEvent = NULL;
		m_hRxSerialEventArray[SERIAL_TICKLE] = NULL;
	}

	sprintf_s(m_reportMsg, "purge comm");
	sendLogToWindow();
	if (NULL != m_hComm)
	{
		PurgeComm(
			m_hComm,
			PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR
			);
	}

	sprintf_s(m_reportMsg, "close comm");
	sendLogToWindow();
	if (NULL != m_hComm)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}

	if (NULL != m_hReadSerialEvent){
		CloseHandle(m_hReadSerialEvent);
		m_hReadSerialEvent = NULL;
		m_hRxSerialEventArray[SERIAL_RX] = NULL;
	}

	if (NULL != m_hWriteSerialEvent){
		CloseHandle(m_hWriteSerialEvent);
		m_hWriteSerialEvent = NULL;
		m_hTxSerialEventArray[SERIAL_TX] = NULL;
	}

	sprintf_s(m_reportMsg, "↑close dongle success↑");
	sendLogToWindow();

}

DWORD CSerialDongle::SetupDongle(void)
{
	DWORD        result;
	DCB          dcb;
	COMMTIMEOUTS cto;

	//初始化dongle通信设备的通信参数
	if (0 == SetupComm(m_hComm, INTERNALCOMBUFFSIZE, INTERNALCOMBUFFSIZE)){
		result = GetLastError();
		return result;
	}
	//指定dongle监视通信设备的事件
	if (0 == SetCommMask(m_hComm, NULL)){
		result = GetLastError();
		return result;
	}

	//读取串口设置
	if (0 == GetCommState(m_hComm, &dcb)){
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
	if (0 == SetCommState(m_hComm, &dcb)){
		result = GetLastError();
		return result;
	}

	//获取dongle设备读写时的超时参数设定
	if (0 == GetCommTimeouts(m_hComm, &cto)){
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
	if (0 == SetCommTimeouts(m_hComm, &cto)){
		result = GetLastError();
		return result;
	}
	return 0;
}

DWORD CSerialDongle::PurgeDongle(DWORD flags, DWORD fromcode)
{
	//   PURGE_RXABORT
	// | PURGE_RXCLEAR
	// | PURGE_TXABORT
	// | PURGE_TXCLEAR
	DWORD result;

	//#if USEDEBUGTIMERS
	//	if (0 == timerholdoffcounter){
	//		//performanceTimer 8 used for Dongle Purge
	//		if (MAXPERFORMANCEINDEX != performanceTimerIndex[8]){
	//			QueryPerformanceCounter(&performanceTimer[performanceTimerIndex[8]++][8]);
	//		}
	//		//ticktimer 9 used for Purge Code
	//		if (MAXTICKINDEX != tickTimerIndex[9]){
	//			tickTimer[tickTimerIndex[9]++][9] = fromcode;
	//		}
	//	}
	//#endif



	if (NULL != m_hComm)
	{
		if (FALSE == ClearCommError(m_hComm, &result, NULL)){
			result = GetLastError();
			return result;
		}
		if (FALSE == PurgeComm(m_hComm, flags))
		{
			result = GetLastError();
			return result;
		}
	}
	return 0;
}

UINT __stdcall CSerialDongle::SerialTxThreadProc(void* pArguments)
{
	CSerialDongle* pObject = (CSerialDongle*)pArguments;
	pObject->SerialTxThread();
	pObject->m_SerialTxThreadRunning = FALSE;
	return 626;
}

UINT __stdcall CSerialDongle::SerialRxThreadProc(void* pArguments)
{
	CSerialDongle* pObject = (CSerialDongle*)pArguments;
	pObject->SerialRxThread();
	pObject->m_SerialRxThreadRunning = FALSE;
	return 627;
}

void CSerialDongle::SerialTxThread()
{
	sprintf_s(m_reportMsg, "SerialTxThread Run");
	sendLogToWindow();

	DWORD result = 0;
	DWORD dwCurrentTimeout = INFINITE;
	DWORD dwWritten = 0;
	DWORD dwRead = 0;
	do
	{
		result = WaitForMultipleObjects(2, m_hTxSerialEventArray, FALSE, dwCurrentTimeout);
		//sprintf_s(m_reportMsg, "SerialTxThread:%lu", result);
		//sendLogToWindow();
		switch (result)
		{
		case WAIT_TIMEOUT:
			result = PurgeDongle(PURGE_TXABORT | PURGE_TXCLEAR, 300);
			dwCurrentTimeout = INFINITE;
			break;
		case SERIAL_TX:
			ResetEvent(m_hWriteSerialEvent);
			//是否写入成功
			if (!GetOverlappedResult(m_hComm, &m_osWriter, &dwWritten, FALSE))
			{
				result = GetLastError();
				//Try some errorrecovery.
				result = PurgeDongle(PURGE_TXABORT | PURGE_TXCLEAR, 202);
			}
			break;
		case  SERIAL_TICKLE:
			ResetEvent(m_hTickleTxSerialEvent);
			//result = PurgeDongle(PURGE_TXABORT
			//	| PURGE_TXCLEAR, 203);
			break;
		default:
			break;
		}

	} while (!m_PleaseStopSerial);

}

void CSerialDongle::SerialRxThread()
{
	sprintf_s(m_reportMsg, "SerialRxThread Run");
	sendLogToWindow();

	DWORD dwRead = 0;
	DWORD result = 0;
	DWORD dwCurrentTimeout = INFINITE;
	int AssembledCount = 0;
	DWORD dwBytesConsumed = 0;
	DWORD dwWritten = 0;
	tAMBEFrame *pPrev = NULL;
	tAMBEFrame *pCurrent = NULL;

	do
	{
		result = WaitForMultipleObjects(2, m_hRxSerialEventArray, FALSE, dwCurrentTimeout);
		//sprintf_s(m_reportMsg, "SerialRxThread:%lu", result);
		//sendLogToWindow();
		switch (result)
		{
		case WAIT_TIMEOUT:
			break;
		case SERIAL_RX:
			ResetEvent(m_hReadSerialEvent);
			if (!GetOverlappedResult(m_hComm, &m_osReader, &dwRead, FALSE))
			{
				result = GetLastError();
				//Try some errorrecovery.
				result = PurgeDongle(PURGE_RXABORT | PURGE_RXCLEAR, 401);
			}
			else
			{
				//处理数据
				AssembledCount = AssembleMsg(dwRead, &dwBytesConsumed);
				////初始化读取
				//initReadyRead();
				if (m_dongleFlag & MASK_DONGLE_AMBE_TO_PCM)
				{
					readyNextWriteAMBE();
				}
				else if (m_dongleFlag & MASK_DONGLE_PCM_TO_AMBE)
				{
					readyNextWritePCM();
				}
			}
			break;
		case  SERIAL_TICKLE:
			ResetEvent(m_hTickleRxSerialEvent);
			//if (!GetOverlappedResult(m_hComm, &m_osReader, &dwRead, FALSE))
			//{
			//	result = GetLastError();
			//}
			//else
			//{

			//}
			break;
		default:
			break;
		}
	} while (!m_PleaseStopSerial);

}

void CSerialDongle::sendLogToWindow()
{
	//SYSTEMTIME now = { 0 };
	//GetLocalTime(&now);
	//printf_s("%04u-%02u-%02u %02u:%02u:%02u %03u %s\n", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds, m_reportMsg);
	if (NULL != m_report)
	{
		m_report(m_reportMsg);
	}
}

void CSerialDongle::SetLogPtr(PLogReport value)
{
	m_report = value;
}

int CSerialDongle::AssembleMsg(DWORD numBytes, DWORD * dwBytesAssembled)
{
	long long numBytess = numBytes;
	int Index;
	unsigned __int8 ch;
	int WholeMessageCount = 0;
	DWORD bytecount = 0;

	*dwBytesAssembled = 0;
	Index = 0;
	while (0 < numBytess--)
	{
		//sprintf_s(m_reportMsg, "AssembleMsg:%lu", numBytess);
		//sendLogToWindow();
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
				ParseDVSImsg(&m_RxDVSImsg);	//call DVSI message parsing routine	
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

void CSerialDongle::ParseDVSImsg(DVSI3000struct* pMsg)
{
	UINT mType;
	//strip out just type field
	mType = pMsg->base.Type;
	int start = 0, end = 0, dif = 0;
	switch (mType)
	{
	case AMBE3000_AMBE_TYPE_BYTE:
	{
									deObfuscate(DONGLETOIPSC, (tAMBEFrame*)pMsg);
									g_pNet->NetStuffTxVoice((unsigned char*)&(pMsg->AMBEType.theAMBEFrame.fld.ChannelBits[0]));
	}
		break;
	case AMBE3000_PCM_TYPE_BYTE:
		g_pSound->BigEndianSoundOut((unsigned __int8*)&(pMsg->PCMType.thePCMFrame.fld.Samples[0]), m_AMBE_CirBuff.size());
		break;
	case AMBE3000_CCP_TYPE_BYTE:
		sprintf_s(m_reportMsg, "ParseDVSImsg,DONGLE_EVENT:%c", pMsg->base.empty[0]);
		sendLogToWindow();
		break;
	}
}

void CSerialDongle::deObfuscate(ScrambleDirection theDirection, tAMBEFrame* pAMBEFrame)
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
	case IPSCTODONGLE:
		DirectionArray = &IPSCTODONGLETABLE[0];
		break;
	case DONGLETOIPSC:
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

	if (IPSCTODONGLE == theDirection)
	{
		m_AMBELocker.lock();
		m_AMBE_CirBuff.push_back(pAMBEFrame);
		m_AMBELocker.unlock();
	}
	else if (DONGLETOIPSC == theDirection)
	{
		////test code
		if (NULL != g_pSound->m_pInputFile)
		{
			fwrite(pAMBEFrame->fld.ChannelBits, 1, 7, g_pSound->m_pInputFile);
		}
	}
}

tAMBEFrame* CSerialDongle::GetFreeAMBEBuffer(void)
{
	tAMBEFrame* pAMBEFrame;

	pAMBEFrame = new tAMBEFrame;
	pAMBEFrame->fld.Sync = AMBE3000_SYNC_BYTE;
	pAMBEFrame->fld.LengthH = AMBE3000_AMBE_LENGTH_HBYTE;
	pAMBEFrame->fld.LengthL = AMBE3000_AMBE_LENGTH_LBYTE;
	pAMBEFrame->fld.Type = AMBE3000_AMBE_TYPE_BYTE;
	pAMBEFrame->fld.ID = AMBE3000_AMBE_CHANDID_BYTE;
	pAMBEFrame->fld.Num = AMBE3000_AMBE_NUMBITS_BYTE;
	pAMBEFrame->fld.PT = AMBE3000_PARITYTYPE_BYTE;

	return pAMBEFrame;
}

void CSerialDongle::MarkAMBEBufferFilled(void)
{
	//	int NextIndex;
	DVSI3000struct* pAMBEBuffer;

	tAMBEFrame* pFrame = m_AMBE_CirBuff.back();
	pAMBEBuffer = (DVSI3000struct*)pFrame;

	pAMBEBuffer->AMBEType.theAMBEFrame.fld.PP = CheckSum(pAMBEBuffer);
}

unsigned __int8 CSerialDongle::CheckSum(DVSI3000struct* pMsg)
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

void CSerialDongle::DecodeBuffers()
{

	if (!(m_decodeFlag&MASK_DECODE_DOING) && changeAMBEToPCM())
	{
		m_decodeFlag = FLAG_DECODE_DOING;
		readyNextWriteAMBE();
		sprintf_s(m_reportMsg, "start decoding");
		sendLogToWindow();
	}
}

void CSerialDongle::SendDVSIPCMMsgtoDongle(unsigned __int8* pData, int dataLength)
{
	int i = 0;
	int length = dataLength;
	while (length)
	{
		/*存在损失最多20ms音频数据的可能*/
		if (length < 320)
		{
			break;
		}
		tPCMFrame* pPcmData = new tPCMFrame;
		pPcmData->fld.Sync = AMBE3000_SYNC_BYTE;
		pPcmData->fld.LengthH = AMBE3000_PCM_LENGTH_HBYTE;
		pPcmData->fld.LengthL = AMBE3000_PCM_LENGTH_LBYTE;
		pPcmData->fld.Type = AMBE3000_PCM_TYPE_BYTE;
		pPcmData->fld.ID = AMBE3000_PCM_SPEECHID_BYTE;
		pPcmData->fld.Num = AMBE3000_PCM_NUMSAMPLES_BYTE;
		for (i = 0; i < AMBE3000_PCM_INTSAMPLES_BYTE; i++)
		{
			pPcmData->fld.Samples[1 + (i << 1)] = *pData++; //Endian conversion.
			pPcmData->fld.Samples[(i << 1)] = *pData++;
		}
		pPcmData->fld.PT = AMBE3000_PARITYTYPE_BYTE;
		pPcmData->fld.PP = CheckSum((DVSI3000struct *)pPcmData);

		m_PCMLocker.lock();
		m_PCM_CirBuff.push_back(pPcmData);
		m_PCMLocker.unlock();
		length -= 320;
	}
	startCoding(0);
	//sprintf_s(m_reportMsg, "m_PCM_CirBuff.size:%d", m_PCM_CirBuff.size());
	//g_pWLlog->sendLog(m_reportMsg);
}

UINT8 CSerialDongle::getDongleFlag()
{
	return m_dongleFlag;
}

BOOL CSerialDongle::setDongleFlag(UINT8 value)
{
	DWORD result = 0;
	if (value & MASK_DONGLE_AMBE_TO_PCM)
	{
		m_dongleReadFrame = AMBE3000_PCM_BYTESINFRAME;
		m_dongleWriteFrame = AMBE3000_AMBE_BYTESINFRAME;
		m_dongleFlag = FLAG_DONGLE_AMBE_TO_PCM;
		//initReadyRead();
		return TRUE;
	}
	else if (value & MASK_DONGLE_PCM_TO_AMBE)
	{
		m_dongleReadFrame = AMBE3000_AMBE_BYTESINFRAME;
		m_dongleWriteFrame = AMBE3000_PCM_BYTESINFRAME;
		m_dongleFlag = FLAG_DONGLE_PCM_TO_AMBE;
		//initReadyRead();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CSerialDongle::initReadyRead()
{
	DWORD result = 0;
	DWORD dwRead = 0;
	/*初始化*/
	memset(&m_osReader, 0, sizeof(OVERLAPPED));
	ResetEvent(m_hReadSerialEvent);
	m_osReader.hEvent = m_hReadSerialEvent;

	if (!ReadFile(m_hComm, &m_DongleRxBuffer, m_dongleReadFrame, &dwRead, &m_osReader))
	{
		result = GetLastError();
		if (ERROR_IO_PENDING != result)
		{
			//Try some error recovery.
			result = PurgeDongle(PURGE_RXABORT | PURGE_RXCLEAR, 402);
		}
	}
}

void CSerialDongle::readyNextWriteAMBE()
{
	DWORD dwWritten = 0;
	DWORD result = 0;

	m_prevAMBE = m_curAMBE;
	m_curAMBE = NULL;

	//释放上一个数据
	if (m_prevAMBE != NULL)
	{
		m_AMBELocker.lock();
		m_AMBE_CirBuff.pop_front();
		m_AMBELocker.unlock();
		delete m_prevAMBE;
		m_prevAMBE = NULL;
		//m_pEmptyAMBEFrame = NULL;
	}



	if (m_AMBE_CirBuff.size() > 0)
	{
		releaseWaitNextNetDataEvent();
	}

	DWORD rlt = WaitForSingleObject(m_waitNextNetDataEvent, m_waitNextNetDataTime);
	if (rlt == WAIT_TIMEOUT && m_AMBE_CirBuff.size() <= 0)
	{
		m_decodeFlag = FLAG_DECODE_PREAPRE_END;
	}

	//当前待解码的数据数目为0且当前不需要继续解码
	if (m_decodeFlag&MASK_DECODE_PREAPRE_END)
	{
		//立即播放缓存
		g_pSound->startPlay(0);

		//initReadyRead();
		m_decodeFlag = FLAG_DECODE_END;
		g_dongleIsUsing = FALSE;
		//清除一切对当前dongle的操作
		result = PurgeDongle(PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_TXABORT, 100);

		//提醒界面
		sprintf_s(m_reportMsg, "stop decoding");
		sendLogToWindow();
		m_lpCmanager->ReleaseDecodeEvent();
		return;
	}

	//初始化读取
	initReadyRead();

	/*初始化事件和结构体*/
	memset(&m_osWriter, 0, sizeof(m_osWriter));
	ResetEvent(m_hWriteSerialEvent);
	m_osWriter.hEvent = m_hWriteSerialEvent;

	////存在待解码的数据
	//if (!(m_AMBE_CirBuff.size() <= 0))
	//{
	m_AMBELocker.lock();
	m_curAMBE = m_AMBE_CirBuff.front();
	m_AMBELocker.unlock();
	//}
	////不存在待解码的数据
	//else
	//{
	//	if (NULL == m_pEmptyAMBEFrame)
	//	{
	//		char frame[7] = { 0 };
	//		m_pEmptyAMBEFrame = g_dongle->GetFreeAMBEBuffer();
	//		if (NULL != m_pEmptyAMBEFrame)
	//		{
	//			memcpy(m_pEmptyAMBEFrame->fld.ChannelBits, frame, 7);
	//			g_dongle->deObfuscate(IPSCTODONGLE, m_pEmptyAMBEFrame);
	//			g_dongle->MarkAMBEBufferFilled();
	//		}
	//	}
	//	m_curAMBE = m_pEmptyAMBEFrame;
	//}

	//将数据写入dongle端口
	if (!WriteFile(m_hComm, &m_curAMBE->All[0], m_dongleWriteFrame, &dwWritten, &m_osWriter))
	{
		//Has not returned immediately.
		result = GetLastError();
		if (ERROR_IO_PENDING != result)
		{ //Some dreadful error occurred.
			//Try some error recovery.
			result = PurgeDongle(PURGE_TXABORT | PURGE_TXCLEAR, 208);
		}
	}
}

void CSerialDongle::readyNextWritePCM()
{
	DWORD dwWritten = 0;
	DWORD result = 0;

	m_prevPCM = m_curPCM;
	m_curPCM = NULL;
	//释放上一个数据
	if (m_prevPCM != NULL)
	{
		m_PCMLocker.lock();
		m_PCM_CirBuff.pop_front();
		m_PCMLocker.unlock();
		delete m_prevPCM;
		m_prevPCM = NULL;
	}

	//加密完毕
	if (m_PCM_CirBuff.size() <= 0)
	{
		g_dongleIsUsing = FALSE;//复位dongle的使用情况
		g_pSound->closeFile();//关闭测试文件
		//清除一切对当前dongle的操作
		result = PurgeDongle(PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_TXABORT, 100);

		/*通知此次通话结束*/
		g_pNet->releaseRecordEndEvent();
		//提醒界面
		//sprintf_s(m_reportMsg, "stop coding");
		//sendLogToWindow();
		m_bCoding = FALSE;//复位当前转换状态
		return;
	}

	//初始化读取
	initReadyRead();

	/*初始化事件和结构体*/
	memset(&m_osWriter, 0, sizeof(m_osWriter));
	ResetEvent(m_hWriteSerialEvent);
	m_osWriter.hEvent = m_hWriteSerialEvent;

	m_PCMLocker.lock();
	m_curPCM = m_PCM_CirBuff.front();
	m_PCMLocker.unlock();

	//将数据写入dongle端口
	if (!WriteFile(m_hComm, &m_curPCM->All[0], m_dongleWriteFrame, &dwWritten, &m_osWriter))
	{
		//Has not returned immediately.
		result = GetLastError();
		if (ERROR_IO_PENDING != result)
		{ //Some dreadful error occurred.
			//Try some error recovery.
			result = PurgeDongle(PURGE_TXABORT | PURGE_TXCLEAR, 208);
		}
	}
}

BOOL CSerialDongle::changeAMBEToPCM()
{
	//dongle正在使用
	if (g_dongleIsUsing)
	{
		//当前状态为AMBEToPCM
		if (MASK_DONGLE_AMBE_TO_PCM & m_dongleFlag)
		{
			return TRUE;
		}
		else
		{
			sprintf_s(m_reportMsg, "dongle is busying");
			sendLogToWindow();
			return FALSE;
		}
	}
	else
	{
		//转换状态为AMBEToPCM
		if (setDongleFlag(FLAG_DONGLE_AMBE_TO_PCM))
		{
			g_dongleIsUsing = TRUE;
			return TRUE;
		}
		else
		{
			sprintf_s(m_reportMsg, "set dongle fail");
			sendLogToWindow();
			return FALSE;
		}
	}
}

BOOL CSerialDongle::changePCMToAMBE()
{
	//dongle正在使用
	if (g_dongleIsUsing)
	{
		//当前状态为PCMToAMBE
		if (MASK_DONGLE_PCM_TO_AMBE & m_dongleFlag)
		{
			return TRUE;
		}
		else
		{
			sprintf_s(m_reportMsg, "dongle is busying");
			sendLogToWindow();
			return FALSE;
		}
	}
	else
	{
		//转换状态为PCMToAMBE
		if (setDongleFlag(FLAG_DONGLE_PCM_TO_AMBE))
		{
			g_dongleIsUsing = TRUE;
			return TRUE;
		}
		else
		{
			sprintf_s(m_reportMsg, "set dongle fail");
			sendLogToWindow();
			return FALSE;
		}
	}
}

void CSerialDongle::releaseWaitNextNetDataEvent()
{
	SetEvent(m_waitNextNetDataEvent);
}

void CSerialDongle::startCoding(int bufferSize)
{
	if (
		!m_bCoding //当前状态为未开始
		&& (m_PCM_CirBuff.size() > bufferSize)
		)
	{
		//提醒界面
		//sprintf_s(m_reportMsg, "start coding");
		//sendLogToWindow();
		m_bCoding = TRUE;
		readyNextWritePCM();
	}
}
