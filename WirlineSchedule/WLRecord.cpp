#include "stdafx.h"
#include "WLRecord.h"
#include <map>
#include <string>

WLRecord::WLRecord(CMySQL *pDb)
{
	m_hVoiceDataListLocker = CreateMutex(NULL, FALSE, NULL);
	m_hFileArrived = CreateEvent(NULL, TRUE, FALSE, NULL);
	ZeroMemory(m_strAudioFilePath, PATH_FILE_MAXSIZE*sizeof(wchar_t));
	wcscpy_s(m_strAudioFilePath, L"C:\\WirlineSchedule");
	m_pDb = pDb;
	m_bExit = false;
	m_writeFileThread =(HANDLE)_beginthreadex(NULL,         // security
		0,            // stack size
		WriteVoiceFileThreadProc,
		this,           // arg list
		CREATE_SUSPENDED,  // so we can later call ResumeThread()
		NULL);
	ResumeThread(m_writeFileThread);
}

WLRecord::~WLRecord()
{

}

void WLRecord::OnNewVoiceRecord(LPBYTE pData, DWORD dwSize, DWORD srcId, DWORD tgtId, DWORD callType, int recordType /*= 0*/, int srcPeerId /*= 0*/, int srcSlot /*= -1*/, int srcRssi /*= -1*/)
{
	if (0 == dwSize)
	{
		return;
	}
	CVoiceData *pVoiceData = new CVoiceData(pData, dwSize, srcId, tgtId, callType, recordType, srcPeerId, srcSlot, srcRssi);
	WaitForSingleObject(m_hVoiceDataListLocker, INFINITE);
	m_voiceDataList.push_back(pVoiceData);
	ReleaseMutex(m_hVoiceDataListLocker);
	SetEvent(m_hFileArrived);
}

BOOL WLRecord::WriteVoiceFile()
{
	BOOL bNeedUpdate = FALSE;
	WaitForSingleObject(m_hVoiceDataListLocker, INFINITE);
	for (auto i = m_voiceDataList.begin(); i != m_voiceDataList.end(); ++i)
	{
		m_audioLog.SetAudioFilePath(m_strAudioFilePath);
		bNeedUpdate = TRUE;
		// write file
		DWORD dwOffset = 0;
		if (!m_audioLog.WriteAudioDataToFile((*i)->m_pData, (*i)->m_dwLen, dwOffset))
		{
			//AfxMessageBox(_T("Write File Failed"));
			delete *i;
			m_voiceDataList.erase(i);
			return FALSE;
		}

		// write to database
		wchar_t strAudioPath[PATH_FILE_MAXSIZE] = { 0 };
		m_audioLog.GetCurrentAudioFileFullPath(strAudioPath);
		std::map < std::string, std::string > voiceRecord;
		std::string str = "";
		char temp[64] = { 0 };
		try{
			sprintf_s(temp, "%u", (*i)->m_dwSrcId);
			voiceRecord["src_radio"] = str + temp;
			sprintf_s(temp, "%u", (*i)->m_dwTargetId);
			voiceRecord["target_radio"] = str + temp;
			sprintf_s(temp, "%u", (*i)->m_dwCallType);
			voiceRecord["call_type"] = str + temp;
			sprintf_s(temp, "%u", (*i)->m_dwLen);
			voiceRecord["length"] = str + temp;
			sprintf_s(temp, "%u", dwOffset);
			voiceRecord["offset"] = str + temp;
			voiceRecord["file_path"] = g_tool.UnicodeToANSI(strAudioPath);
			sprintf_s(temp, "%d", (*i)->m_srcPeerId);
			voiceRecord["src_peer_id"] = str + temp;
			sprintf_s(temp, "%d", (*i)->m_srcSlot);
			voiceRecord["src_slot"] = str + temp;
			sprintf_s(temp, "%d", (*i)->m_srcRssi);
			voiceRecord["src_rssi"] = str + temp;
			sprintf_s(temp, "%d", (*i)->m_recordType);
			voiceRecord["record_type"] = str + temp;
		}
		catch (...){
			sprintf_s(m_reportMsg, "WriteVoiceFile error");
			sendLogToWindow();
		}

		m_pDb->InsertVoiceRecord(voiceRecord);

		delete *i;
		i = m_voiceDataList.erase(i);
		if (i == m_voiceDataList.end())
		{
			break;
		}
	}
	ReleaseMutex(m_hVoiceDataListLocker);

	//if (bNeedUpdate)
	//{
	//	PostMessage(WM_NEWRECORD);
	//}
	return TRUE;
}

void WLRecord::SetLogPtr(PLogReport value)
{
	m_report = value;
}

void WLRecord::sendLogToWindow()
{
	if (m_report)
	{
		m_report(m_reportMsg);
	}
}

DWORD WLRecord::WriteVoiceFileThread()
{
	while (!m_bExit)
	{
		DWORD ret = WaitForSingleObject(m_hFileArrived, 1000);
		// check list
		WriteVoiceFile();
	}
	return 0;
}

unsigned __stdcall WLRecord::WriteVoiceFileThreadProc(LPVOID pVoid)
{
	WLRecord *pThis = reinterpret_cast<WLRecord*>(pVoid);
	if (pThis)
	{
		pThis->WriteVoiceFileThread();
	}

	return 0;
}
