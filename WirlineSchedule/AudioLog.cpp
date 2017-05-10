#include "stdafx.h"
#include "AudioLog.h"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

CAudioLog::CAudioLog()
{
	ZeroMemory(m_strCurrentFilePath, sizeof(wchar_t)*PATH_FILE_MAXSIZE);
	m_hOpenFile = NULL;
}


CAudioLog::~CAudioLog()
{
	if (m_hOpenFile)
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
		SetFilePointer(m_hOpenFile, 0, NULL, FILE_END);

		//dwOffset = ftell(m_pFile);
		dwOffset = GetFileSize(m_hOpenFile, NULL);
		
		 //dwWrite = fwrite(pByte, 1, dwSize, m_pFile);
		dwWrite = WriteFile(m_hOpenFile, pByte, dwSize, &dwSize, NULL);

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
	m_hOpenFile = CreateFile(m_strCurrentFilePath, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	if (NULL == m_hOpenFile)
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

BOOL CAudioLog::SetAudioFilePath(const std::wstring& path)
{
	//ZeroMemory(m_strAudioFileSaveFolderPath, sizeof(wchar_t)*PATH_FILE_MAXSIZE);
	std::wstring temp = path;
	temp += L"\\";
	wcscpy_s(m_strAudioFileSaveFolderPath, temp.c_str());
	int createFileRlt = 0;
	if (!PathFileExists(m_strAudioFileSaveFolderPath))
	{
		//return CreateDirectory(m_strAudioFileSaveFolderPath, NULL);
		createFileRlt = _wmkdir(m_strAudioFileSaveFolderPath);
		return (0 == createFileRlt);
	}
	return TRUE;
}
