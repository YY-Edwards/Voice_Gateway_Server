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
		std::ifstream ifs(getFilePath().c_str());
		if (!ifs)
		{
			return -1;
		}
		rapidjson::IStreamWrapper isw(ifs);
		d.ParseStream(isw);
		ifs.close();
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
		if (d.HasMember(type) && d[type].IsObject())
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


		std::lock_guard<std::mutex> locker(m_writeLocker);

		std::ofstream ofs(getFilePath().c_str());
		rapidjson::OStreamWrapper osw(ofs);

		rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
		d.Accept(writer);
		ofs.close();


		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer1(buffer);
		d.Accept(writer1); // Accept() traverses the DOM and generates Handler events.
		std::string jsonStr = buffer.GetString();

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
