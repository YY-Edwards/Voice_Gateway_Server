#pragma once
class CAudioLog
{
public:
	CAudioLog();
	~CAudioLog();

public:
	/*add code*/
	BOOL WriteAudioDataToFile(LPBYTE pByte, DWORD& dwSize, DWORD& dwOffset);
	void GetCurrentAudioFileFullPath(wchar_t* pAudioFileFullPath);
	BOOL SetAudioFilePath(const std::wstring& path);
private:
	BOOL CreateNewFileByYearMonth();

private:
	wchar_t			m_strAudioFileSaveFolderPath[PATH_FILE_MAXSIZE];
	wchar_t			m_strCurrentFilePath[PATH_FILE_MAXSIZE];
	//FILE*			m_pFile;

	HANDLE m_hOpenFile;
};

