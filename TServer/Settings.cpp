/**
* @ref: http://rapidjson.org/zh-cn/md_doc_stream_8zh-cn.html#FileStreams
*/
#include "stdafx.h"
#include <shlobj.h> 
#include <Shlwapi.h>
#include "../lib/strutil/strutil.h"
#include "Settings.h"

#include <map>

#pragma comment(lib, "Shlwapi.lib")

std::auto_ptr<CSettings> CSettings::m_instance;

CSettings::CSettings()
{
	
	int a = 2;
}


CSettings::~CSettings()
{
}


int CSettings::getRoot(rapidjson::Document& d)
{
	try
	{
		HANDLE pFile;
		DWORD fileSize;
		char *buffer, *tmpBuf;
		DWORD dwBytesRead, dwBytesToRead, tmpLen;

		pFile = CreateFile(getFilePath().c_str(), GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,        //打开已存在的文件 
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (pFile == INVALID_HANDLE_VALUE)
		{
			//printf("open file error!\n");
			CloseHandle(pFile);
			return -1;
		}

		fileSize = GetFileSize(pFile, NULL);          //得到文件的大小

		buffer = (char *)malloc(fileSize +1);
		memset(buffer, 0, fileSize  +1);
		dwBytesToRead = fileSize;
		dwBytesRead = 0;
		tmpBuf = buffer;

		do{                                       //循环读文件，确保读出完整的文件    

			ReadFile(pFile, tmpBuf, 100, &dwBytesRead, NULL);

			if (dwBytesRead == 0)
				break;

			dwBytesToRead -= dwBytesRead;
			tmpBuf += dwBytesRead;

		} while (dwBytesToRead > 0);
		
		d.Parse<0>(buffer);

		free(buffer);
		CloseHandle(pFile);
	}
	catch (std::exception& e)
	{
		return -1;
	}
	catch (...)
	{
		return -1;
	}


	return 0;
}

std::string CSettings::getValue(const char* type)
{
	std::map<std::string, std::string> contents;

	rapidjson::Document d;
	if (0 == getRoot(d))
	{
		if (d.IsObject() && d.HasMember(type) )
		if (d[type].IsObject())
		{
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			d[type].Accept(writer); // Accept() traverses the DOM and generates Handler events.
			std::string jsonStr = buffer.GetString();

			return jsonStr;
		}
	}
	return "";
}


std::string replace(std::string str, const std::string & strsrc, const std::string &strdst)
{
	std::string::size_type pos = 0;//位置 
	std::string::size_type srclen = strsrc.size();//要替换的字符串大小 
	std::string::size_type dstlen = strdst.size();//目标字符串大小 
	while ((pos = str.find(strsrc, pos)) != std::string::npos)
	{
		str.replace(pos, srclen, strdst);
		pos += dstlen;
	}

	return str;
}


std::string CSettings::getResponse(char* pStatus, uint64_t callId, int errCode, const char* statusText, std::string contents)
{
	std::string jsonStr = "";
	char str[2048];

	try{
		rapidjson::Document d;

		d.SetObject();
		rapidjson::Value statusEl(rapidjson::kStringType);
		statusEl.SetString(pStatus, d.GetAllocator());

		rapidjson::Value callIdEl(rapidjson::kNumberType);
		callIdEl.SetUint64(callId);

		rapidjson::Value errCodeEl(rapidjson::kNumberType);
		errCodeEl.SetInt(errCode);

		rapidjson::Value statusTextEl(rapidjson::kStringType);
		statusTextEl.SetString(statusText, d.GetAllocator());

		rapidjson::Value contentsEl(rapidjson::kStringType);
		contentsEl.SetString("%s", d.GetAllocator());

		d.AddMember("status", statusEl, d.GetAllocator());
		d.AddMember("statusText", statusTextEl, d.GetAllocator());
		d.AddMember("callId", callIdEl, d.GetAllocator());
		d.AddMember("errCode", errCodeEl, d.GetAllocator());

		if ("" != contents)
		{
			d.AddMember("contents", contentsEl, d.GetAllocator());
		}

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		d.Accept(writer); // Accept() traverses the DOM and generates Handler events.
		jsonStr = replace(buffer.GetString(), "\"\%s\"", "%s");		
		sprintf_s(str, jsonStr.c_str(), contents.c_str());
		d.RemoveAllMembers();
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
	return std::string(str);
}

std::string CSettings::getRequest(char* pCall, char * type, uint64_t callId, std::string param)
{
	std::string jsonStr = "";
	char str[2048];

	try{
		rapidjson::Document d;

		d.SetObject();
		rapidjson::Value callEl(rapidjson::kStringType);
		callEl.SetString(pCall, d.GetAllocator());


		rapidjson::Value typeEl(rapidjson::kStringType);
		typeEl.SetString(type, d.GetAllocator());

		rapidjson::Value callIdEl(rapidjson::kNumberType);
		callIdEl.SetUint64(callId);
		
		rapidjson::Value contentsEl(rapidjson::kStringType);
		contentsEl.SetString("%s", d.GetAllocator());

		d.AddMember("call", callEl, d.GetAllocator());
		d.AddMember("type", typeEl, d.GetAllocator());
		d.AddMember("callId", callIdEl, d.GetAllocator());

		if ("" != param)
		{
			d.AddMember("param", contentsEl, d.GetAllocator());
		}

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		d.Accept(writer); // Accept() traverses the DOM and generates Handler events.
		jsonStr = replace(buffer.GetString(), "\"\%s\"", "%s");
		sprintf_s(str, jsonStr.c_str(), param.c_str());
		d.RemoveAllMembers();
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}
	return std::string(str);
}

int CSettings::setValue(const char* type,  rapidjson::Value obj)
{
	try{
		rapidjson::Document d;
		getRoot(d);

		if (d.IsNull())
		{
			d.SetObject();
		}

		if (d.HasMember(type))
		{
			d.EraseMember(rapidjson::StringRef(type));
		}

		if ((rapidjson::Value)NULL != obj)
		d.AddMember(rapidjson::StringRef(type), obj, d.GetAllocator());


		rapidjson::StringBuffer buffer;

		rapidjson::Writer<rapidjson::StringBuffer> writer1(buffer);
		d.Accept(writer1); // Accept() traverses the DOM and generates Handler events.
		std::string jsonStr = buffer.GetString();


		HANDLE pFile;
		DWORD fileSize;
		
		DWORD dwBytesWrite, dwBytesToRead, tmpLen;

		pFile = CreateFile(getFilePath().c_str(), GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,        //打开已存在的文件 
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (pFile == INVALID_HANDLE_VALUE)
		{
			//printf("open file error!\n");
			CloseHandle(pFile);
			return -1;
		}

		fileSize = GetFileSize(pFile, NULL);          //得到文件的大小
		WriteFile(pFile, jsonStr.c_str(), jsonStr.length(), &dwBytesWrite, NULL);
		
		CloseHandle(pFile);
	}
	catch (...)
	{
		return -1;
	}
	return 0;
}

std::wstring CSettings::getFilePath()
{
	TCHAR appDir[_MAX_PATH];
	memset(appDir, 0, sizeof(appDir));
	SHGetSpecialFolderPath(NULL, appDir, CSIDL_APPDATA, 0);
	std::wstring filePath = appDir;
	filePath += L"\\";
	filePath += ConmpanyName;
	
	if (!PathFileExists(filePath.c_str()))
	{
		CreateDirectory(filePath.c_str(), NULL);
	}

	filePath += L"\\";
	filePath += AppName;

	if (!PathFileExists(filePath.c_str()))
	{
		CreateDirectory(filePath.c_str(), NULL);
	}


	filePath += L"\\";
	filePath += AppVersion;

	if (!PathFileExists(filePath.c_str()))
	{
		CreateDirectory(filePath.c_str(), NULL);
	}

	filePath += L"\\";
	filePath += SettingFile;

	return filePath;
}


std::string CSettings::getFilePathA()
{
	CHAR appDir[_MAX_PATH];
	memset(appDir, 0, sizeof(appDir));
	SHGetSpecialFolderPathA(NULL, appDir, CSIDL_APPDATA, 0);
	std::string filePath = appDir;
	filePath += "\\";
	filePath += ConmpanyNameA;

	if (!PathFileExistsA(filePath.c_str()))
	{
		CreateDirectoryA(filePath.c_str(), NULL);
	}

	filePath += "\\";
	filePath += AppNameA;

	if (!PathFileExistsA(filePath.c_str()))
	{
		CreateDirectoryA(filePath.c_str(), NULL);
	}


	filePath += "\\";
	filePath += AppVersionA;

	if (!PathFileExistsA(filePath.c_str()))
	{
		CreateDirectoryA(filePath.c_str(), NULL);
	}

	filePath += "\\";
	filePath += SettingFileA;

	return filePath;
}


