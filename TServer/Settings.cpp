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
		pFile = CreateFileA(getFilePathA().c_str(), GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,        //���Ѵ��ڵ��ļ� 
			NULL,
			NULL);

		if (pFile == INVALID_HANDLE_VALUE)
		{
			//printf("open file error!\n");
			CloseHandle(pFile);
			return -1;
		}

		fileSize = GetFileSize(pFile, NULL);          //�õ��ļ��Ĵ�С

		buffer = (char *)malloc(fileSize +1);
		memset(buffer, 0, fileSize  +1);
		dwBytesToRead = fileSize;
		dwBytesRead = 0;
		tmpBuf = buffer;

		do{                                       //ѭ�����ļ���ȷ�������������ļ�    

			ReadFile(pFile, tmpBuf, 100, &dwBytesRead, NULL);

			if (dwBytesRead == 0)
				break;

			dwBytesToRead -= dwBytesRead;
			tmpBuf += dwBytesRead;

		} while (dwBytesToRead > 0);
		
		d.Parse<0>(buffer);

		free(buffer);
		CloseHandle(pFile);

		//FILE * fl;
		//fopen_s(&fl, getFilePathA().c_str(), "r");

		//fseek(fl, 0, SEEK_END);
		//fileSize = ftell(fl);
		//fseek(fl, 0, SEEK_SET);

		//buffer = (char *)malloc(fileSize + 1);
		//memset(buffer, 0, fileSize  +1);

		//fread_s(buffer, 1, 1, fileSize, fl);
		//	
		//
		//std::string s(buffer);
		////logxxx->sendLog(s);

		//d.Parse<0>(buffer);


		//fclose(fl);
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
	//return "{\"base\":{\"Svr\":{\"Ip\":\"127.0.0.1\",\"Port\":9000},\"LogSvr\":{\"Ip\":\"127.0.0.1\",\"Port\":9003},\"IsSaveCallLog\":true,\"IsSaveMsgLog\":true,\"IsSavePositionLog\":true,\"IsSaveControlLog\":true,\"IsSaveJobLog\":true,\"IsSaveTrackerLog\":true},\"radio\":{\"IsEnable\":false,\"IsOnlyRide\":false,\"Svr\":{\"Ip\":\"127.0.0.1\",\"Port\":9001},\"Ride\":{\"Ip\":null,\"Port\":0},\"Mnis\":{\"Ip\":null,\"Port\":0},\"GpsC\":{\"Ip\":null,\"Port\":0},\"Ars\":{\"Ip\":null,\"Port\":4007},\"Message\":{\"Ip\":null,\"Port\":4005},\"Gps\":null,\"Xnl\":null},\"repeater\":{\"IsEnable\":true,\"Type\":0,\"Svr\":{\"Ip\":\"127.0.0.1\",\"Port\":9002},\"Master\":{\"Ip\":null,\"Port\":0},\"Mnis\":{\"Ip\":null,\"Port\":0},\"MnisId\":0,\"DefaultGroupId\":0,\"DefaultChannel\":1,\"MinHungTime\":0,\"MaxSiteAliveTime\":0,\"MaxPeerAliveTime\":0,\"LocalPeerId\":0,\"LocalRadioId\":0,\"Dongle\":{\"Com\":1}}}";
	std::map<std::string, std::string> contents;

	rapidjson::Document d;
	if (0 == getRoot(d))
	{
		if (d.IsObject() && d.HasMember(type) && d[type].IsObject())
		{
;
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
	std::string::size_type pos = 0;//λ�� 
	std::string::size_type srclen = strsrc.size();//Ҫ�滻���ַ�����С 
	std::string::size_type dstlen = strdst.size();//Ŀ���ַ�����С 
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
	std::string s(type);

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

		//pFile = CreateFile(getFilePath().c_str(), GENERIC_WRITE,
		//	FILE_SHARE_WRITE,
		//	NULL,
		//	OPEN_ALWAYS,        //���Ѵ��ڵ��ļ� 
		//	FILE_ATTRIBUTE_NORMAL,
		//	NULL);

		pFile = CreateFile(getFilePath().c_str(), GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);

		if (pFile == INVALID_HANDLE_VALUE)
		{
			//printf("open file error!\n");
			CloseHandle(pFile);
			return -1;
		}

		fileSize = GetFileSize(pFile, NULL);          //�õ��ļ��Ĵ�С
		SetFilePointer(pFile, NULL, NULL, FILE_BEGIN);

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


