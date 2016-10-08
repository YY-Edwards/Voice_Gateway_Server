#ifndef WLRECORD_H
#define WLRECORD_H

#include <deque>
#include "AudioLog.h"
#include "MySQL.h"


extern CTool g_tool;

class CVoiceData{
public:
	PBYTE m_pData;
	DWORD m_dwLen;
	DWORD m_dwTargetId;
	DWORD m_dwSrcId;
	DWORD m_dwCallType;
	int m_recordType;
	int m_srcPeerId;
	int m_srcSlot;
	int m_srcRssi;

public:
	CVoiceData(PBYTE p, DWORD len, DWORD srcId, DWORD tgtId, DWORD callType, int recordType, int srcPeerId, int srcSlot, int srcRssi){
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
	void OnNewVoiceRecord(LPBYTE pData, DWORD dwSize, DWORD srcId, DWORD tgtId, DWORD callType, int recordType, int srcPeerId, int srcSlot, int srcRssi);
	void SetLogPtr(PLogReport value);
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