#pragma once
#include <Windows.h>
#include <string>
#include <deque>


#define					SPACE_1G				1024 * 1024 * 1024

class CUtil
{
public:
	CUtil(void);
	~CUtil(void);

	static std::wstring CUtil::Utf8ToUtf16(LPCSTR pstrUtf8);
	static std::string Utf16ToUtf8(LPCWSTR pUtf16);
	static std::string DumpHex(PBYTE pHexBuf, DWORD dwSize);
	static std::wstring GetDataDriver();
	static BOOL IsUsbDriver(LPCTSTR pDriver);
	static std::wstring GetApplicationPath();
	static std::wstring Trim(const std::wstring& str);
	static std::wstring TrimLeft(const std::wstring& str);
	static std::wstring TrimRight(const std::wstring& str);
	static std::wstring ToLower(const std::wstring& str);
	static std::wstring ToUpper(const std::wstring& str);
	static char * strptime(const char *s, const char *format, struct tm *tm);
	static std::string strftime(const struct tm *t);
	static std::wstring GB2312ToUnicode(LPCSTR szGBString);
	/*
	Delete a folder and sub folder
	*/
	static BOOL DeleteDirectory(LPCTSTR pstrFolderPath);
	static BOOL IsDots(const TCHAR* str);
	static time_t SystemTimeToTime_t( const SYSTEMTIME& st );
	static BOOL SetApplicationAutoStart();
	static int GetAudioFiles( LPCTSTR pstrFolder, std::deque<std::wstring>& audFiles, 
		std::deque<std::wstring> audIndex );
	static int GetSubFolders( LPCTSTR pstrFolder, std::deque<std::wstring>& subFolders );


#ifdef _WIN32
	/*
	* return first local ip address which has gateway,
	* **NOTE** this function is not thread safe
	*/
	static std::string GetLocalIp();
#endif
};

