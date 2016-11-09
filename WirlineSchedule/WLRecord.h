#ifndef WLRECORD_H
#define WLRECORD_H

#include <deque>
#include "AudioLog.h"
#include "MySQL.h"
//#include "WLNet.h"


extern CTool g_tool;
//extern CWLNet* g_pNet;

class CVoiceData{
public:
	PBYTE m_pData;
	DWORD m_dwLen;
	DWORD m_dwTargetId;
	DWORD m_dwSrcId;
	DWORD m_dwCallType;
	int m_recordType;
	DWORD m_srcPeerId;
	int m_srcSlot;
	int m_srcRssi;
	int m_callStatus;
	SYSTEMTIME m_time;

public:
	CVoiceData(PBYTE p, DWORD len, DWORD srcId, DWORD tgtId, DWORD callType, int recordType, DWORD srcPeerId, int srcSlot, int srcRssi, int callStatus, SYSTEMTIME *pTime){
		m_pData = new unsigned char[len];
		m_dwLen = len;
		memcpy_s(m_pData, len, p, len);
		m_dwTargetId = tgtId;
		m_dwSrcId = srcId;
		m_dwCallType = callType;
		m_recordType = recordType;
		m_srcPeerId = srcPeerId;
		m_srcSlot = srcSlot;
		m_srcRssi = srcRssi;
		m_callStatus = callStatus;
		m_time = *pTime;
	}

	~CVoiceData(){
		if (m_pData)
		{
			delete[] m_pData;
		}
	}
};

class WLRecord
{
public:
	WLRecord(CMySQL *pDb);
	~WLRecord();
	BOOL WriteVoiceFile();
	void OnNewVoiceRecord(LPBYTE pData, DWORD dwSize, DWORD srcId, DWORD tgtId, DWORD callType, int recordType, DWORD srcPeerId, int srcSlot, int srcRssi, int callStatus, SYSTEMTIME *pTime);
	void SetLogPtr(PLogReport value);
	void stop();
private:
	HANDLE m_hVoiceDataListLocker;
	std::deque<CVoiceData*> m_voiceDataList;
	CAudioLog m_audioLog;
	HANDLE m_hFileArrived;
	wchar_t m_strAudioFilePath[PATH_FILE_MAXSIZE];
	CMySQL *m_pDb;
	PLogReport m_report;
	char m_reportMsg[512];
	bool m_bExit;
	HANDLE m_writeFileThread;
private:
	void sendLogToWindow();
	DWORD WriteVoiceFileThread();
	static unsigned __stdcall WriteVoiceFileThreadProc(LPVOID pVoid);
};


#endif