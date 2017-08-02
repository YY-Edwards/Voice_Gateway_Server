#include "stdafx.h"
#include "WLRecord.h"
#include <map>
#include <string>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

WLRecord::WLRecord(CMySQL *pDb, std::wstring& defaultAudioPath)
{
	m_hVoiceDataListLocker = CreateMutex(NULL, FALSE, NULL);
	m_hFileArrived = CreateEvent(NULL, FALSE, FALSE, NULL);
	//ZeroMemory(m_strAudioFilePath, PATH_FILE_MAXSIZE*sizeof(wchar_t));
	defaultAudioPath += L"\\Voice";
	m_defaultAudioPath = defaultAudioPath;
	wcscpy_s(m_strAudioFilePath, m_defaultAudioPath.c_str());
	//wcscpy_s(m_defaultAudioPath, defaultAudioPath.c_str());
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

void WLRecord::OnNewVoiceRecord(LPBYTE pData, DWORD dwSize, DWORD srcId, DWORD tgtId, DWORD callType, int recordType /*= 0*/, DWORD srcPeerId /*= 0*/, int srcSlot /*= -1*/, int srcRssi /*= -1*/, int callStatus, SYSTEMTIME *pTime)
{
	if (0 == dwSize)
	{
		return;
	}

	CVoiceData *pVoiceData = new CVoiceData(pData, dwSize, srcId, tgtId, callType, recordType, srcPeerId, srcSlot, srcRssi,callStatus,pTime);
	WaitForSingleObject(m_hVoiceDataListLocker, INFINITE);
	m_voiceDataList.push_back(pVoiceData);
	ReleaseMutex(m_hVoiceDataListLocker);
	SetEvent(m_hFileArrived);
}

BOOL WLRecord::WriteVoiceFile()
{
	//BOOL bNeedUpdate = FALSE;
	WaitForSingleObject(m_hVoiceDataListLocker, INFINITE);
	for (auto i = m_voiceDataList.begin(); i != m_voiceDataList.end(); ++i)
	{
		//sprintf_s(m_reportMsg, "WriteVoiceFile start write");
		//sendLogToWindow();
		//wcscpy_s(m_strAudioFilePath, temp.c_str());
		BOOL createStatus = m_audioLog.SetAudioFilePath(m_strAudioFilePath);
		//sprintf_s(m_reportMsg, "createStatus %d", createStatus);
		//sendLogToWindow();
		//bNeedUpdate = TRUE;
		// write file
		DWORD dwOffset = 0;
		if (!m_audioLog.WriteAudioDataToFile((*i)->m_pData, (*i)->m_dwLen, dwOffset))
		{
			sprintf_s(m_reportMsg, "Write File Failed,try set default audio path");
			sendLogToWindow();
			wcscpy_s(m_strAudioFilePath, m_defaultAudioPath.c_str());
			BOOL createStatus = m_audioLog.SetAudioFilePath(m_strAudioFilePath);
			//sprintf_s(m_reportMsg, "createStatus %d", createStatus);
			//sendLogToWindow();
			dwOffset = 0;
			BOOL bRlt = m_audioLog.WriteAudioDataToFile((*i)->m_pData, (*i)->m_dwLen, dwOffset);
			if (bRlt)
			{
				sprintf_s(m_reportMsg, "Write File Failed");
				sendLogToWindow();
				delete *i;
				m_voiceDataList.erase(i);
				ReleaseMutex(m_hVoiceDataListLocker);
				return FALSE;
			}
		}
		//sprintf_s(m_reportMsg, "WriteVoiceFile end write");
		//sendLogToWindow();
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
			sprintf_s(temp, "%u", (*i)->m_srcPeerId);
			voiceRecord["src_peer_id"] = str + temp;
			sprintf_s(temp, "%d", (*i)->m_srcSlot);
			voiceRecord["src_slot"] = str + temp;
			sprintf_s(temp, "%d", (*i)->m_srcRssi);
			voiceRecord["src_rssi"] = str + temp;
			sprintf_s(temp, "%d", (*i)->m_recordType);
			voiceRecord["record_type"] = str + temp;
			sprintf_s(temp, "%d", (*i)->m_callStatus);
			voiceRecord["call_status"] = str + temp;
			if (0 != (*i)->m_time.wYear)
			{
				sprintf_s(temp, "%04u-%02u-%02u %02u:%02u:%02u", (*i)->m_time.wYear, (*i)->m_time.wMonth, (*i)->m_time.wDay, (*i)->m_time.wHour, (*i)->m_time.wMinute, (*i)->m_time.wSecond);
				voiceRecord["time"] = str + temp;
			}

		}
		catch (...){
			sprintf_s(m_reportMsg, "WriteVoiceFile error");
			sendLogToWindow();
		}
		//sprintf_s(m_reportMsg, "WriteVoiceFile start insert");
		//sendLogToWindow();
		m_pDb->InsertVoiceRecord(voiceRecord);
		//sprintf_s(m_reportMsg, "WriteVoiceFile end insert");
		//sendLogToWindow();
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

void WLRecord::stop()
{
	if (NULL != m_writeFileThread)
	{
		m_bExit = true;
		SetEvent(m_hFileArrived);
		WaitForSingleObject(m_writeFileThread, 1000);
		CloseHandle(m_writeFileThread);
	}
}

void WLRecord::setAudioPath(const std::wstring& path)
{
	if (path.empty())
	{
		sprintf_s(m_reportMsg, "error:audio path is null,there will used default audio path");
		sendLogToWindow();
	}
	else
	{
		std::wstring temp = path;
		temp += L"\\";
		if (!PathFileExists(temp.c_str()))
		{
			std::string str = g_tool.UnicodeToANSI(temp);
			sprintf_s(m_reportMsg, "error:audio path %s not exist,there will used default audio path", str.c_str());
			sendLogToWindow();
		}
		else
		{
			temp += L"Voice";
			wcscpy_s(m_strAudioFilePath, temp.c_str());
			BOOL createStatus = m_audioLog.SetAudioFilePath(m_strAudioFilePath);
			//sprintf_s(m_reportMsg, "createStatus %d", createStatus);
			//sendLogToWindow();
		}
	}
}
