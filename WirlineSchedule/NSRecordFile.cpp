#include "stdafx.h"
#include "NSRecordFile.h"
#include "NSLog.h"
#include <string>
#include <map>
#include "Tool.h"
#include "MySQL.h"
#include "NSAmbe.h"
#include "NSManager.h"
#include "NSWLPeer.h"
//#include "WLNet.h"

#define MAX_RECORD_BUFFER_SIZE (100*1024)

NSRecordFile::NSRecordFile(NSManager* pManager, NSWLPeer* peer)
:src_radio(0)
, target_radio(0)
, length(0)
, call_type('\0')
, src_peer_id(0)
, src_rssi(0)
, src_slot(0)
, call_status(VOICE_INIT)
, timeout(GetTickCount() + TIMEOUT_VOICE_FRAME)
, m_pLog(NSLog::instance())
, id(0)
, m_hOpenFile(NULL)
, m_pAmbe(NULL)
, m_peer(peer)
, m_pManager(pManager)
, buffer((char*)malloc(MAX_RECORD_BUFFER_SIZE))
{
	if (m_peer) m_peer->setRecordFile(this);
	GetLocalTime(&time);
}

NSRecordFile::~NSRecordFile()
{
#if _DEBUG
	char temp[1024] = { 0 };
	sprintf_s(temp, "Voice Record CallId %lu From %lu To %lu On Peer:%lu in Slot 0x%02x Delete", call_id, src_radio, target_radio, src_peer_id, src_slot);
	m_pLog->AddLog(Ns_Log_Info, temp);
#endif
	if (m_pAmbe)
	{
		m_pAmbe->WirteEnd();
		m_pAmbe = NULL;
	}
	if (m_peer)
	{
		m_peer->setRecordFile(NULL);
		m_peer = NULL;
	}
	free(buffer);
	buffer = NULL;
}

void NSRecordFile::WriteVoiceFrame(const char* pAmbe, int size, bool needDongle /*= true*/)
{
	timeout = GetTickCount() + TIMEOUT_VOICE_FRAME;//等待Voice_Burst或者Voice_End标识

	if (license_status_nopass == g_license_status)
	{
		m_pLog->AddLog(Ns_Log_Error, "WriteVoiceFrame fail,license_status_nopass");
		return;
	}
	if (needDongle &&
		g_playCalltype == call_type &&
		g_playTargetId == target_radio)
	{
		if (NULL == m_pAmbe)
		{
			m_pAmbe = new NSAmbe(m_pManager);
		}
		if (m_pAmbe)
		{
			m_pAmbe->Ambe2Pcm((void*)pAmbe, size);
		}
		else
		{
			m_pLog->AddLog(Ns_Log_Error, "m_pAmbe is null");
		}
	}


	if (NULL != buffer && (length + size) > MAX_RECORD_BUFFER_SIZE)
	{
		buffer = (char*)realloc(buffer, length + size);
	}
	if (buffer)
	{
		memcpy(buffer + length, pAmbe, size);
		length += size;
	}
}

bool NSRecordFile::TimeOut()
{
#if _DEBUG
	if (timeout < GetTickCount())
	{
		char temp[64] = { 0 };
		sprintf_s(temp, "TimeOut:%d", GetTickCount() - timeout);
		m_pLog->AddLog(Ns_Log_Info, temp);
	}
#endif
	return (timeout < GetTickCount());
}

SYSTEMTIME* NSRecordFile::StartTime()
{
	return &time;
}

unsigned int NSRecordFile::Length()
{
	return length;
}

//bool CRecordFile::IsThis(unsigned long src_radio, unsigned long target_radio, unsigned long call_id)
//{
//	return (src_radio == this->src_radio && target_radio == this->target_radio && call_id == this->call_id);
//}
//
//unsigned int CRecordFile::RecordId()
//{
//	return recordId;
//}
//
//void CRecordFile::setRecordId(unsigned int value)
//{
//	recordId = value;
//
//}

void NSRecordFile::setCallStatus(int value)
{

	if (value != call_status)
	{
#if _DEBUG
		m_pLog->AddLog(Ns_Log_Info, "====CallStatus From %d To %d On CallId %d====", call_status, value, call_id);
#endif
		call_status = value;
		if (VOICE_START == call_status)
		{
			if (src_radio == CONFIG_LOCAL_RADIO_ID)
			{
					oncallstatus_info_t info = { 0 };
					info.callType = call_type;
					info.srcId = src_radio;
					info.tgtId = target_radio;
					info.status = STATUS_CALL_START | REMOTE_CMD_SUCCESS;
					NS_SafeCallStatusEvent(&info);
				//g_pNet->wlCallStatus(call_type, src_radio, target_radio, STATUS_CALL_START | REMOTE_CMD_SUCCESS, SessionId);
			}
			else
			{
					oncall_info_t info = { 0 };
					info.callType = call_type;
					info.srcId = src_radio;
					info.tgtId = target_radio;
					info.status = OPERATE_CALL_START;
					info.isCurrent = (call_type == g_playCalltype && target_radio == g_playTargetId);
					NS_SafeCallEvent(&info);
				//g_pNet->wlCall(call_type, src_radio, target_radio, OPERATE_CALL_START, (call_type == g_playCalltype && target_radio == g_playTargetId));
			}
		}
		else if (VOICE_BURST == call_status)
		{
#if _DEBUG
			char temp[1024] = { 0 };
			sprintf_s(temp, "Voice Record CallId %lu From %lu To %lu On Peer:%lu in Slot 0x%02x VOICE_BURST", call_id, src_radio, target_radio, src_peer_id, src_slot);
			m_pLog->AddLog(Ns_Log_Info, temp);
#endif
		}
		else if (CALL_SESSION_STATUS_HANG == call_status)
		{
			timeout = GetTickCount() + g_hang_time;//等待Session_End标识
			if (src_radio == CONFIG_LOCAL_RADIO_ID)
			{
					oncallstatus_info_t info = { 0 };
					info.callType = call_type;
					info.srcId = src_radio;
					info.tgtId = target_radio;
					info.status = STATUS_CALL_END | REMOTE_CMD_SUCCESS;
					NS_SafeCallStatusEvent(&info);
				//g_pNet->wlCallStatus(call_type, src_radio, target_radio, STATUS_CALL_END | REMOTE_CMD_SUCCESS, SessionId);
			}
			else
			{
					oncall_info_t info = { 0 };
					info.callType = call_type;
					info.srcId = src_radio;
					info.tgtId = target_radio;
					info.status = OPERATE_CALL_END;
					info.isCurrent = (call_type == g_playCalltype && target_radio == g_playTargetId);
					NS_SafeCallEvent(&info);
				//g_pNet->wlCall(call_type, src_radio, target_radio, OPERATE_CALL_END, (call_type == g_playCalltype && target_radio == g_playTargetId));
			}
		}
		else if (VOICE_END_BURST == call_status)
		{
			timeout = GetTickCount() + TIMEOUT_VOICE_FRAME;//等待Session_Hang标识
		}
	}
}

void NSRecordFile::WriteToDb()
{
	if (license_status_nopass == g_license_status)
	{
		m_pLog->AddLog(Ns_Log_Error, "WriteToDb fail,license_status_nopass");
		return;
	}
	if (g_pDb)
	{
		std::map < std::string, std::string > voiceRecord;
		char temp[64] = { 0 };
		if (0 == id)
		{
			if (!CreateNewFileByYearMonth())
			{
				m_pLog->AddLog(Ns_Log_Error, "CreateNewFileByYearMonth fail");
			}
			else
			{
				DWORD dwWrite, offset;
				/*将音频数据写入文件*/
				SetFilePointer(m_hOpenFile, 0, NULL, FILE_END);
				offset = GetFileSize(m_hOpenFile, NULL);
				if (FALSE == WriteFile(m_hOpenFile, buffer, length, &dwWrite, NULL))
				{
					m_pLog->AddLog(Ns_Log_Error, "Record WriteFile fail");
				}
				else
				{
					/*插入此条数据记录*/
					/*src_radio*/
					sprintf_s(temp, "%lu", src_radio);
					voiceRecord["src_radio"] = temp;
					/*target_radio*/
					sprintf_s(temp, "%lu", target_radio);
					voiceRecord["target_radio"] = temp;
					/*call_type*/
					sprintf_s(temp, "%u", (unsigned short)call_type);
					voiceRecord["call_type"] = temp;
					/*length*/
					sprintf_s(temp, "%lu", length);
					voiceRecord["length"] = temp;
					/*offset*/
					sprintf_s(temp, "%lu", offset);
					voiceRecord["offset"] = temp;
					/*file_path*/
					voiceRecord["file_path"] = g_pTool->UnicodeToANSI(m_strCurrentFilePath);
					/*src_peer_id*/
					sprintf_s(temp, "%lu", src_peer_id);
					voiceRecord["src_peer_id"] = temp;
					/*src_slot*/
					sprintf_s(temp, "%d", src_slot);
					voiceRecord["src_slot"] = temp;
					/*src_rssi*/
					sprintf_s(temp, "%d", src_rssi);
					voiceRecord["src_rssi"] = temp;
					/*record_type*/
					sprintf_s(temp, "%u", g_network_mode);
					voiceRecord["record_type"] = temp;
					/*call_status*/
					sprintf_s(temp, "%d", call_status);
					voiceRecord["call_status"] = temp;
					/*time*/
					sprintf_s(temp, "%04u-%02u-%02u %02u:%02u:%02u",
						time.wYear,
						time.wMonth,
						time.wDay,
						time.wHour,
						time.wMinute,
						time.wSecond);
					voiceRecord["time"] = temp;
					if (g_pDb->InsertVoiceRecord(voiceRecord))
					{
						id = g_pDb->GetLastVoiceRowId();
					}
				}
			}
		}
		else
		{
			char strTableName[64] = { 0 };
			sprintf_s(strTableName, "voice_%4d", time.wYear);
			/*call_status*/
			sprintf_s(temp, "%d", call_status);
			voiceRecord["call_status"] = temp;

			sprintf_s(temp, "where id=%lu", id);
			g_pDb->UpdateRow(strTableName, voiceRecord, temp);
		}
	}
	else
	{
		m_pLog->AddLog(Ns_Log_Error, "An unknown database error has occurred,pointer of db is null");
	}
}

BOOL NSRecordFile::CreateNewFileByYearMonth()
{
	SYSTEMTIME t = { 0 };
	GetLocalTime(&t);
	wchar_t strFileName[PATH_FILE_MAXSIZE] = { 0 };
	swprintf_s(strFileName, L"%s\\%04u%02u.bit", g_ambedata_path, t.wYear, t.wMonth);
	if (0 == wcscmp(m_strCurrentFilePath, strFileName))
	{
		return TRUE;
	}
	wcscpy_s(m_strCurrentFilePath, strFileName);
	m_hOpenFile = CreateFile(m_strCurrentFilePath, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	if (NULL == m_hOpenFile)
	{
		return FALSE;
	}
	return TRUE;
}

int NSRecordFile::CallStatus()
{
	return call_status;
}

//bool CRecordFile::TimeOutUpdateStatusToDb()
//{
//	return (timeout_status < GetTickCount());
//}
//
//bool CRecordFile::NeedUpdateStatus()
//{
//	return bNeedUpdateStatus;
//}
