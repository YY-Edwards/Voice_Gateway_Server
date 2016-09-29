#include "stdafx.h"
#include "AudioLog.h"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

CAudioLog::CAudioLog()
: m_pFile(NULL)
{
	ZeroMemory(m_strCurrentFilePath, sizeof(wchar_t)*PATH_FILE_MAXSIZE);
}


CAudioLog::~CAudioLog()
{
	if (hOpenFile)
	{
		//CloseHandle(hOpenFile);

		//hOpenFile = NULL;
	}
}

/*add code*/
BOOL CAudioLog::WriteAudioDataToFile(LPBYTE pByte, DWORD& dwSize, DWORD& dwOffset)
{
	if (!CreateNewFileByYearMonth())
	{
		return FALSE;
	}
	DWORD dwWrite = 0;
	try{
		//fseek(m_pFile, 0, SEEK_END);
		SetFilePointer(hOpenFile, 0, NULL, FILE_END);

		//dwOffset = ftell(m_pFile);
		dwOffset = GetFileSize(hOpenFile, NULL);
		
		 //dwWrite = fwrite(pByte, 1, dwSize, m_pFile);
		dwWrite = WriteFile(hOpenFile, pByte, dwSize, &dwSize, NULL);

		//fflush(m_pFile);

	}
	catch(...){
		return FALSE;
	}
	
	return dwWrite ;
}

BOOL CAudioLog::CreateNewFileByYearMonth()
{
	SYSTEMTIME t = { 0 };
	GetLocalTime(&t);
	wchar_t strFileName[PATH_FILE_MAXSIZE] = { 0 };
	swprintf_s(strFileName, L"%s%04u%02u.bit", m_strAudioFileSaveFolderPath, t.wYear, t.wMonth);
	if (0 == wcscmp(m_strCurrentFilePath, strFileName))
	{
		return TRUE;
	}
	wcscpy_s(m_strCurrentFilePath, strFileName);
	hOpenFile = CreateFile(m_strCurrentFilePath, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	if (NULL == hOpenFile)
	{
		return FALSE;
	}
	return TRUE;
}

//CString& CAudioLog::GetCurrentAudioFileFullPath()
//{
//	return m_strCurrentFilePath;
//}

void CAudioLog::GetCurrentAudioFileFullPath(wchar_t* pAudioFileFullPath)
{
	wcscpy_s(pAudioFileFullPath, PATH_FILE_MAXSIZE, m_strCurrentFilePath);
}


//void CAudioLog::SetAudioFilePath(CString audioFilePath)
//{
//	m_strAudioFileSaveFolderPath = audioFilePath + _T("\\");
//
//	if (!PathFileExists(m_strAudioFileSaveFolderPath))
//	{
//		CreateDirectory(m_strAudioFileSaveFolderPath, NULL);
//	}
//}

void CAudioLog::SetAudioFilePath(wchar_t* pAudioFilePath)
{
	wcscat_s(m_strAudioFileSaveFolderPath, pAudioFilePath);
	wcscat_s(m_strAudioFileSaveFolderPath, L"\\");
	if (!PathFileExists(m_strAudioFileSaveFolderPath))
	{
		CreateDirectory(m_strAudioFileSaveFolderPath, NULL);
	}
}
