#ifndef SERIALDONGLE_H
#define SERIALDONGLE_H

#include <mutex>
#include <list>

const DWORD  INTERNALCOMBUFFSIZE = 2048;
const DWORD  DONGLEBAUDRATE = 230400;
const __int8 DONGLEBITS = 8;
const __int8 DONGLEPARITY = NOPARITY;
const __int8 DONGLESTOP = ONESTOPBIT;

//Serial Events (two different arrays).
const int SERIAL_TICKLE = 0;
const int SERIAL_RX = 1;
const int SERIAL_TX = 1;

//const int  MAXDONGLEPCMFRAMES = 2048; //About 41 Seconds.
//const int  MAXDONGLEPCMFRAMESMASK = MAXDONGLEPCMFRAMES - 1;
//const int  MAXDONGLEAMBEFRAMES = 2048; //About 41 Seconds.
//const int  MAXDONGLEAMBEFRAMESMASK = MAXDONGLEAMBEFRAMES - 1;

enum PARSERSTATE
{
	FIND_START,
	HIGH_LENGTH,
	LOW_LENGTH,
	READ_DATA
};

class CManager;
class CSound;
class CWLNet;

extern CWLNet* g_pNet;
extern CSound* g_pSound;


#define VOICE_NEXT_TIMEOUT (1000)

class CSerialDongle
{
public:
	CSerialDongle();
	~CSerialDongle();
	BOOL changeAMBEToPCM();
	BOOL changePCMToAMBE();
	DWORD OpenDongle(LPCTSTR lpszDevice, CManager* lpCmanager);
	void  stop(void);
	void SetLogPtr(PLogReport value);
	tAMBEFrame* GetFreeAMBEBuffer(void);
	void MarkAMBEBufferFilled(void);
	void deObfuscate(ScrambleDirection theDirection, tAMBEFrame* pAMBEFrame);
	//special function
	void DecodeBuffers();
	void SendDVSIPCMMsgtoDongle(unsigned __int8* pData, int dataLength);
	BOOL m_bCurCodingIsEnd;//本轮加密是否结束
	UINT8 getDongleFlag();
	BOOL setDongleFlag(UINT8 value);
	void initReadyRead();
	void readyNextWriteAMBE();
	void readyNextWritePCM();
	HANDLE m_hTickleTxSerialEvent;
	HANDLE m_hTickleRxSerialEvent;
	//BOOL m_bDecode;//当前是否在解码状态
	//BOOL m_bPrepareDecode;//当前是否在待解码状态
	//BOOL m_bPrepareEndDecode;//当前是否在待结束解码状态
	void releaseWaitNextNetDataEvent();
	void startCoding(int bufferSize);
private:
	DWORD m_waitNextNetDataTime;//等待解压网络数据超时处理:ms
	HANDLE m_waitNextNetDataEvent;//等待解压网络传输过来的AMBE数据
	UINT8 m_decodeFlag;//当前解压状态

	BOOL m_bCoding;//当前是否在加密
	//tAMBEFrame* m_pEmptyAMBEFrame;//用于填充的空数据帧
	tAMBEFrame* m_prevAMBE;
	tPCMFrame* m_prevPCM;
	tAMBEFrame* m_curAMBE;
	tPCMFrame* m_curPCM;
	//HWND m_hParentWnd;
	bool m_PleaseStopSerial;
	HANDLE m_hComm;
	HANDLE m_hReadSerialEvent;
	HANDLE m_hRxSerialEventArray[2];
	HANDLE m_hTxSerialEventArray[2];
	HANDLE m_hWriteSerialEvent;
	HANDLE m_pSerialTxThread;
	HANDLE m_pSerialRxThread;

	UINT m_pSerialTxThreadId;
	UINT m_pSerialRxThreadId;

	BOOL m_SerialRxThreadRunning;
	BOOL m_SerialTxThreadRunning;
	char m_reportMsg[512];
	PLogReport m_report;
	OVERLAPPED m_osReader;
	OVERLAPPED m_osWriter;
	int m_PCMBufTail;
	int m_AMBEBufTail;
	BOOL m_bPleasePurgeAMBE;
	int m_AMBEBufHead;
	BOOL m_bPleasePurgePCM;
	int m_PCMBufHead;
	//tPCMFrame m_PCM_CirBuff[MAXDONGLEPCMFRAMES];
	//tAMBEFrame m_AMBE_CirBuff[MAXDONGLEAMBEFRAMES];
	PARSERSTATE m_ParserState;
	int m_RxMsgLength;
	int m_dwExpectedDongleRead;
	unsigned __int8 m_DongleRxBuffer[INTERNALCOMBUFFSIZE];
	DVSI3000struct m_RxDVSImsg;
	int m_RxMsgIndex;

	//////////////////////////////////////////////////////////////////////////
	std::mutex m_AMBELocker;
	std::list<tAMBEFrame*> m_AMBE_CirBuff;

	std::mutex m_PCMLocker;
	std::list<tPCMFrame*> m_PCM_CirBuff;

	CManager* m_lpCmanager;

	int m_dongleReadFrame;
	int m_dongleWriteFrame;
	//////////////////////////////////////////////////////////////////////////

	DWORD SetupDongle(void);
	DWORD PurgeDongle(DWORD flags, DWORD fromcode);
	static UINT __stdcall SerialTxThreadProc(void* pArguments);
	static UINT __stdcall SerialRxThreadProc(void* pArguments);
	void SerialTxThread();
	void SerialRxThread();
	void sendLogToWindow();
	int AssembleMsg(DWORD numBytes, DWORD * dwBytesAssembled);
	void ParseDVSImsg(DVSI3000struct* pMsg);
	//Called from otherThreads.
	unsigned __int8 CheckSum(DVSI3000struct* pMsg);

	UINT8 m_dongleFlag;
};

#endif