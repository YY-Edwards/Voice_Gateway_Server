#include "stdafx.h"
#include "Util.h"
#include "ConvertUTF.h"

#include <tchar.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
#include <IPHlpApi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")
#endif // _WIN32

//{{{ for strptime
const char * strp_weekdays[] = 
{ "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"};
const char * strp_monthnames[] = 
{ "january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december"};
bool strp_atoi(const char * & s, int & result, int low, int high, int offset, int len = 2)
{
	bool worked = false;
	char * end;
	std::string strDigital(s, len);
	unsigned long num = strtoul(strDigital.c_str(), NULL, 10);
	end = (char*)(s + strDigital.size());
	if (num >= (unsigned long)low && num <= (unsigned long)high)
	{
		result = (int)(num + offset);
		s = end;
		worked = true;
	}
	return worked;
}
//}}} end for strptime

CUtil::CUtil(void)
{
}


CUtil::~CUtil(void)
{
}

std::wstring CUtil::Utf8ToUtf16(LPCSTR pstrUtf8)
{
	std::wstring wstr = L"";
	const UTF8 *pUtf8 = (const UTF8*)pstrUtf8;
	UTF16 buf[1024];
	UTF16 *pwBuf = buf;
	memset(buf, 0, sizeof(buf));
	ConversionResult cr = ConvertUTF8toUTF16(&pUtf8, pUtf8 + strlen(pstrUtf8), &pwBuf, pwBuf + 1024, strictConversion);
	if (conversionOK == cr)
	{
		wstr = (LPCWSTR)buf;
	}
	return wstr;
}

std::string CUtil::Utf16ToUtf8( LPCWSTR pUtf16 )
{
	std::string str = "";

	const UTF16* pU16 = (const UTF16*) pUtf16;
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	UTF8 *pU8 = (UTF8*)buf;
	ConversionResult cr = ConvertUTF16toUTF8(&pU16, pU16 + wcslen(pUtf16), &pU8, pU8 + 1024, strictConversion);
	if (cr == conversionOK)
	{
		str = buf;
	}
	return str;
}

std::string CUtil::DumpHex( PBYTE pHexBuf, DWORD dwSize )
{
	std::string str = "";
	for (DWORD i = 0; i < dwSize; i++)
	{
		char buf[6];
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%0x02x", pHexBuf[i]);
		str += buf;
		if ((i+1)<dwSize)
		{
			str += " ";
		}
	}

	return str;
}

std::wstring CUtil::GetDataDriver()
{
	//TCHAR szBuf[100];
	//ZeroMemory(szBuf, 0 ,sizeof(szBuf));
	//GetLogicalDriveStrings(100, szBuf);

	//std::vector<std::wstring> aDrivers;
	//std::wstring driver = szBuf;

	//for (TCHAR* s = szBuf; *s; s += _tcslen(s)+1)
	//{
	//	LPCTSTR sDrivePath = s;
	//	if (GetDriveType(sDrivePath) == DRIVE_FIXED)
	//	{
	//		BOOL isRemoveableDisk = IsUsbDriver(sDrivePath);
	//		if (!isRemoveableDisk)
	//		{
	//			ULARGE_INTEGER lpFreeBytesAvailableToCaller;
	//			ULARGE_INTEGER lpTotalNumberOfBytes;
	//			ULARGE_INTEGER lpTotalNumberOfFreeBytes;

	//			GetDiskFreeSpaceEx(sDrivePath, &lpFreeBytesAvailableToCaller, 
	//				&lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes);

	//			if (lpTotalNumberOfFreeBytes.LowPart > SPACE_1G && lpTotalNumberOfFreeBytes.HighPart > 0)
	//			{
	//				aDrivers.push_back(sDrivePath);
	//			}
	//			
	//		}
	//	}
	//}

	//if (aDrivers.size() > 0)
	//{
	//	driver = aDrivers.back();
	//}

	return L"";// driver;
}

/*
@return: TRUE Hard disk
         FALSE USB hard disk
*/
//BOOL CUtil::IsUsbDriver( LPCTSTR pDriver )
//{
//	TCHAR szBuf[100];
//	ZeroMemory(szBuf, 0, sizeof(szBuf));
//	wsprintf(szBuf, _T("\\\\?\\%c:"), *pDriver);
//
//	HANDLE hDevice;         // 设备句柄
//	// 打开设备
//	hDevice = ::CreateFile(szBuf,           // 文件名
//		GENERIC_READ,                          // 软驱需要读盘
//		FILE_SHARE_READ | FILE_SHARE_WRITE,    // 共享方式
//		NULL,                                  // 默认的安全描述符
//		OPEN_EXISTING,                         // 创建方式
//		0,                                     // 不需设置文件属性
//		NULL); 
//
//	if (hDevice == INVALID_HANDLE_VALUE)
//	{
//		return TRUE;
//	}
//
//	STORAGE_PROPERTY_QUERY Query; // input param for query
//	DWORD dwOutBytes; // IOCTL output length
//	Query.PropertyId = StorageDeviceProperty;
//	Query.QueryType = PropertyStandardQuery;
//	STORAGE_DEVICE_DESCRIPTOR pDevDesc;
//	pDevDesc.Size = sizeof(STORAGE_DEVICE_DESCRIPTOR);
//	BOOL bResult = ::DeviceIoControl(hDevice, 
//		IOCTL_STORAGE_QUERY_PROPERTY,
//		&Query, 
//		sizeof(STORAGE_PROPERTY_QUERY), 
//		&pDevDesc, 
//		pDevDesc.Size, 
//		&dwOutBytes, 
//		NULL);
//
//	return pDevDesc.BusType == BusTypeUsb;
//}

std::wstring CUtil::GetApplicationPath()
{
	TCHAR szDirectory[MAX_PATH] = _T("");
	::GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory);

	return std::wstring(szDirectory);
}

std::wstring CUtil::Trim( const std::wstring& str )
{
	std::wstring t = str;
	t.erase(0, t.find_first_not_of(L" /t/n/r"));
	t.erase(t.find_last_not_of(L" /t/n/r") + 1);
	return t;
}

std::wstring CUtil::TrimLeft( const std::wstring& str )
{
	std::wstring t = str;
	t.erase(0, t.find_first_not_of(L" /t/n/r"));
	return t;
}

std::wstring CUtil::TrimRight( const std::wstring& str )
{
	std::wstring t = str;
	t.erase(t.find_last_not_of(L" /t/n/r") + 1);
	return t;
}

std::wstring CUtil::ToLower( const std::wstring& str )
{
	std::wstring t = str;
	transform(t.begin(), t.end(), t.begin(), tolower);
	return t;
}

std::wstring CUtil::ToUpper( const std::wstring& str )
{
	std::wstring t = str;
	transform(t.begin(), t.end(), t.begin(), toupper);
	return t;
}

char * CUtil::strptime( const char *s, const char *format, struct tm *tm )
{
	bool working = true;
	while (working && *format && *s)
	{
		switch (*format)
		{
		case '%':
			{
				++format;
				switch (*format)
				{
				case 'a':
				case 'A': // weekday name
					tm->tm_wday = -1;
					working = false;
					for (size_t i = 0; i < 7; ++ i)
					{
						size_t len = strlen(strp_weekdays[i]);
						if (!strnicmp(strp_weekdays[i], s, len))
						{
							tm->tm_wday = i;
							s += len;
							working = true;
							break;
						}
						else if (!strnicmp(strp_weekdays[i], s, 3))
						{
							tm->tm_wday = i;
							s += 3;
							working = true;
							break;
						}
					}
					break;
				case 'b':
				case 'B':
				case 'h': // month name
					tm->tm_mon = -1;
					working = false;
					for (size_t i = 0; i < 12; ++ i)
					{
						size_t len = strlen(strp_monthnames[i]);
						if (!strnicmp(strp_monthnames[i], s, len))
						{
							tm->tm_mon = i;
							s += len;
							working = true;
							break;
						}
						else if (!strnicmp(strp_monthnames[i], s, 3))
						{
							tm->tm_mon = i;
							s += 3;
							working = true;
							break;
						}
					}
					break;
				case 'd':
				case 'e': // day of month number
					working = strp_atoi(s, tm->tm_mday, 1, 31, 0);
					break;
				case 'D': // %m/%d/%y
					{
						const char * s_save = s;
						working = strp_atoi(s, tm->tm_mon, 1, 12, -1);
						if (working && *s == '/')
						{
							++ s;
							working = strp_atoi(s, tm->tm_mday, 1, 31, -1);
							if (working && *s == '/')
							{
								++ s;
								working = strp_atoi(s, tm->tm_year, 0, 99, 0);
								if (working && tm->tm_year < 69)
									tm->tm_year += 100;
							}
						}
						if (!working)
							s = s_save;
					}
					break;
				case 'H': // hour
					working = strp_atoi(s, tm->tm_hour, 0, 23, 0);
					break;
				case 'I': // hour 12-hour clock
					working = strp_atoi(s, tm->tm_hour, 1, 12, 0);
					break;
				case 'j': // day number of year
					working = strp_atoi(s, tm->tm_yday, 1, 366, -1);
					break;
				case 'm': // month number
					working = strp_atoi(s, tm->tm_mon, 1, 12, -1);
					break;
				case 'M': // minute
					working = strp_atoi(s, tm->tm_min, 0, 59, 0);
					break;
				case 'n': // arbitrary whitespace
				case 't':
					while (isspace((int)*s)) 
						++s;
					break;
				case 'p': // am / pm
					if (!strnicmp(s, "am", 2))
					{ // the hour will be 1 -> 12 maps to 12 am, 1 am .. 11 am, 12 noon 12 pm .. 11 pm
						if (tm->tm_hour == 12) // 12 am == 00 hours
							tm->tm_hour = 0;
					}
					else if (!strnicmp(s, "pm", 2))
					{
						if (tm->tm_hour < 12) // 12 pm == 12 hours
							tm->tm_hour += 12; // 1 pm -> 13 hours, 11 pm -> 23 hours
					}
					else
						working = false;
					break;
				case 'r': // 12 hour clock %I:%M:%S %p
					{
						const char * s_save = s;
						working = strp_atoi(s, tm->tm_hour, 1, 12, 0);
						if (working && *s == ':')
						{
							++ s;
							working = strp_atoi(s, tm->tm_min, 0, 59, 0);
							if (working && *s == ':')
							{
								++ s;
								working = strp_atoi(s, tm->tm_sec, 0, 60, 0);
								if (working && isspace((int)*s))
								{
									++ s;
									while (isspace((int)*s)) 
										++s;
									if (!strnicmp(s, "am", 2))
									{ // the hour will be 1 -> 12 maps to 12 am, 1 am .. 11 am, 12 noon 12 pm .. 11 pm
										if (tm->tm_hour == 12) // 12 am == 00 hours
											tm->tm_hour = 0;
									}
									else if (!strnicmp(s, "pm", 2))
									{
										if (tm->tm_hour < 12) // 12 pm == 12 hours
											tm->tm_hour += 12; // 1 pm -> 13 hours, 11 pm -> 23 hours
									}
									else
										working = false;
								}
							}
						}
						if (!working)
							s = s_save;
					}
					break;
				case 'R': // %H:%M
					{
						const char * s_save = s;
						working = strp_atoi(s, tm->tm_hour, 0, 23, 0);
						if (working && *s == ':')
						{
							++ s;
							working = strp_atoi(s, tm->tm_min, 0, 59, 0);
						}
						if (!working)
							s = s_save;
					}
					break;
				case 'S': // seconds
					working = strp_atoi(s, tm->tm_sec, 0, 60, 0);
					break;
				case 'T': // %H:%M:%S
					{
						const char * s_save = s;
						working = strp_atoi(s, tm->tm_hour, 0, 23, 0);
						if (working && *s == ':')
						{
							++ s;
							working = strp_atoi(s, tm->tm_min, 0, 59, 0);
							if (working && *s == ':')
							{
								++ s;
								working = strp_atoi(s, tm->tm_sec, 0, 60, 0);
							}
						}
						if (!working)
							s = s_save;
					}
					break;
				case 'w': // weekday number 0->6 sunday->saturday
					working = strp_atoi(s, tm->tm_wday, 0, 6, 0);
					break;
				case 'Y': // year
					working = strp_atoi(s, tm->tm_year, 1900, 65535, -1900, 4);
					break;
				case 'y': // 2-digit year
					working = strp_atoi(s, tm->tm_year, 0, 99, 0);
					if (working && tm->tm_year < 69)
						tm->tm_year += 100;
					break;
				case '%': // escaped
					if (*s != '%')
						working = false;
					++s;
					break;
				default:
					working = false;
				}
			}
			break;
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case '\f':
		case '\v':
			// zero or more whitespaces:
			while (isspace((int)*s))
				++ s;
			break;
		default:
			// match character
			if (*s != *format)
				working = false;
			else
				++s;
			break;
		}
		++format;
	}
	return (working?(char *)s:0);
}

std::string CUtil::strftime( const struct tm *t )
{
	char buf[50];
	ZeroMemory(buf, sizeof(buf));

	try
	{
		::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);	
	}
	catch (...)
	{
		SYSTEMTIME st;
		GetSystemTime(&st);
		sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		//::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
	}
	

	return std::string(buf);
}

std::wstring CUtil::GB2312ToUnicode(LPCSTR szGBString)

{

	UINT nCodePage = 936; //GB2312

	int nLength=MultiByteToWideChar(nCodePage,0,szGBString,-1,NULL,0);

	wchar_t* pBuffer = new wchar_t[nLength+1];

	MultiByteToWideChar(nCodePage,0,szGBString,-1,pBuffer,nLength);

	pBuffer[nLength]=0;

	std::wstring str(pBuffer);
	delete [] pBuffer;

	return str;

}
BOOL CUtil::IsDots(const TCHAR* str) {
	if(_tcscmp(str, _T(".")) && _tcscmp(str, _T(".."))) return FALSE;
	return TRUE;
}

BOOL CUtil::DeleteDirectory( LPCTSTR pstrFolderPath )
{
	HANDLE hFind;  // file handle
	WIN32_FIND_DATA FindFileData;

	TCHAR DirPath[MAX_PATH];
	TCHAR FileName[MAX_PATH];

	_tcscpy(DirPath,pstrFolderPath);
	_tcscat(DirPath, _T("\\*"));    // searching all files
	_tcscpy(FileName,pstrFolderPath);
	_tcscat(FileName, _T("\\"));

	hFind = FindFirstFile(DirPath,&FindFileData); // find the first file
	if(hFind == INVALID_HANDLE_VALUE) return FALSE;
	_tcscpy(DirPath,FileName);

	bool bSearch = true;
	while(bSearch) { // until we finds an entry
		if(FindNextFile(hFind,&FindFileData)) {
			if(IsDots(FindFileData.cFileName)) continue;
			_tcscat(FileName,FindFileData.cFileName);
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

				// we have found a directory, recurse
				if(!DeleteDirectory(FileName)) { 
					FindClose(hFind); 
					return FALSE; // directory couldn't be deleted
				}
				RemoveDirectory(FileName); // remove the empty directory
				_tcscpy(FileName,DirPath);
			}
			else {
				if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					_tchmod(FileName, _S_IWRITE); // change read-only file mode
				if(!DeleteFile(FileName)) {  // delete the file
					FindClose(hFind); 
					return FALSE; 
				}                 
				_tcscpy(FileName,DirPath);
			}
		}
		else {
			if(GetLastError() == ERROR_NO_MORE_FILES) // no more files there
				bSearch = false;
			else {
				// some error occured, close the handle and return FALSE
				FindClose(hFind); 
				return FALSE;
			}

		}

	}
	FindClose(hFind);  // closing file handle

	return RemoveDirectory(pstrFolderPath); // remove the empty directory
}


time_t CUtil::SystemTimeToTime_t( const SYSTEMTIME& st )
{
	tm temptm = {st.wSecond, 
		st.wMinute, 
		st.wHour, 
		st.wDay, 
		st.wMonth - 1, 
		st.wYear - 1900, 
		st.wDayOfWeek, 
		0, 
		0};
	return mktime(&temptm);
}

BOOL CUtil::SetApplicationAutoStart()
{
	TCHAR szPath[MAX_PATH] = {0};
	TCHAR szFileName[MAX_PATH] = {0};

	GetModuleFileName(NULL, szPath, sizeof(szPath));
	_wsplitpath(szPath, NULL, NULL, szFileName, NULL);

	HKEY	hkey;
	long r = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 
		0, KEY_ALL_ACCESS, &hkey);

	DWORD				type = REG_SZ;
	BYTE				keyValue[1024];
	LPBYTE lpa =		keyValue;
	DWORD dwAidSize =	sizeof(keyValue);
	dwAidSize = sizeof(keyValue);
	ZeroMemory(keyValue, dwAidSize);
	r = RegQueryValueEx(hkey, szFileName, 0, &type, lpa, &dwAidSize);
	if (r == ERROR_SUCCESS)
	{
		std::wstring path = (LPCTSTR)keyValue;
		if (path.compare(szPath) == 0)
		{
			RegCloseKey(hkey);
			return TRUE;
		}
	}

	// write reg key
	r = RegSetValueEx(hkey, szFileName, 0, REG_SZ, (BYTE *)szPath, wcslen(szPath) * 2);
	RegCloseKey(hkey);
	return (r == ERROR_SUCCESS);
}

int CUtil::GetSubFolders( LPCTSTR pstrFolder, std::deque<std::wstring>& subFolders )
{
	HANDLE hFind;
	WIN32_FIND_DATA ffd;

	std::wstring strPath = pstrFolder;
	if (strPath[strPath.size() - 1] != _T('\\'))
	{
		strPath += _T("\\");
	}
	std::wstring strFindPath = strPath + _T("*");

	hFind = FindFirstFile(strFindPath.c_str(), &ffd);
	if (INVALID_HANDLE_VALUE == hFind) 
	{
		return 0;
	}

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			std::wstring path = ffd.cFileName;
			if (path == _T(".") || path == _T(".."))
			{
				continue;
			}

			subFolders.push_back(ffd.cFileName);
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);

	return subFolders.size();
}


int CUtil::GetAudioFiles( LPCTSTR pstrFolder, std::deque<std::wstring>& audFiles, 
	std::deque<std::wstring> audIndex )
{
	HANDLE hFind;
	WIN32_FIND_DATA ffd;

	std::wstring strPath = pstrFolder;
	if (strPath[strPath.size() - 1] != _T('\\'))
	{
		strPath += _T("\\");
	}
	std::wstring strFindPath = strPath + _T("*.*");

	hFind = FindFirstFile(strFindPath.c_str(), &ffd);
	if (INVALID_HANDLE_VALUE == hFind) 
	{
		return 0;
	}

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		else
		{
			TCHAR tchFileName[MAX_PATH], tchExt[MAX_PATH];
			ZeroMemory(tchFileName, sizeof(tchFileName));
			ZeroMemory(tchExt, sizeof(tchExt));
			_wsplitpath(ffd.cFileName, NULL, NULL, tchFileName, tchExt);
			std::wstring strExt = tchExt;
			if (strExt == _T(".DAT"))
			{
				audFiles.push_back(ffd.cFileName);
			}
			if(strExt == _T(".INF"))
			{
				audIndex.push_back(ffd.cFileName);
			}
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);

	return audFiles.size();
}


#ifdef _WIN32
std::string CUtil::GetLocalIp()
{
	PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)malloc(sizeof(IP_ADAPTER_INFO));
	ULONG len = sizeof(IP_ADAPTER_INFO);
	DWORD dwRet = GetAdaptersInfo(pAdapterInfo, &len);
	if (dwRet == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (PIP_ADAPTER_INFO)malloc(len);
		dwRet = GetAdaptersInfo(pAdapterInfo, &len);
	}
	if (NO_ERROR == dwRet)
	{
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
		while(pAdapter)
		{
			if (inet_addr(pAdapter->GatewayList.IpAddress.String) != 0 &&
				inet_addr(pAdapter->IpAddressList.IpAddress.String) != 0)
			{
				std::string strIp = pAdapter->IpAddressList.IpAddress.String;
				free(pAdapterInfo);
				return strIp;
			}

			pAdapter = pAdapter->Next; 
		}
	}
	free(pAdapterInfo);

	return std::string("0.0.0.0");
}


#endif // _WIN32

std::string CUtil::Hex2String(PBYTE pHexBuf, DWORD dwSize)
{
	std::string str = "";
	for (DWORD i = 0; i < dwSize; i++)
	{
		char buf[6];
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%02x", pHexBuf[i]);
		str += buf;
	}

	return str;
}
