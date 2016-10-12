/**
* @ref: http://rapidjson.org/zh-cn/md_doc_stream_8zh-cn.html#FileStreams
*/
#include "stdafx.h"
#include <shlobj.h> 
#include <Shlwapi.h>
#include "../lib/strutil/strutil.h"
#include "Settings.h"

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

std::string CSettings::getRadioIp()
{
	std::string ip="";

	rapidjson::Document d;
	if (0 == getRoot(d))
	{
		if (d.HasMember("radio") && d["radio"].IsObject())
		{
			rapidjson::Value objRadio = d["radio"].GetObject();
			if (objRadio.HasMember("ip") && rapidjson::kStringType == objRadio["ip"].GetType())
			{
				ip = objRadio["ip"].GetString();
			}
		}
	}

	return ip;
}

int CSettings::setRadioIp(const char* ip)
{
	try{
		rapidjson::Document d;
		getRoot(d);

		if (d.IsNull())
		{
			d.SetObject();
		}

		if (!d.HasMember("radio"))
		{
			rapidjson::Value radioEl(rapidjson::kObjectType);
			d.AddMember("radio", radioEl, d.GetAllocator());
		}
	
		if (d["radio"].HasMember("ip"))
		{
			rapidjson::Value& ipEl = d["radio"]["ip"];
			ipEl.SetString(rapidjson::StringRef(ip));
		}
		else
		{
			rapidjson::Value ipEl(rapidjson::kStringType);
			ipEl.SetString(rapidjson::StringRef(ip));
			d["radio"].AddMember("ip", ipEl, d.GetAllocator());
		}

		std::lock_guard<std::mutex> locker(m_writeLocker);

		std::ofstream ofs(getFilePath().c_str());
		rapidjson::OStreamWrapper osw(ofs);

		rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
		d.Accept(writer);
		ofs.close();
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
	filePath += AppName;

	if (!PathFileExists(filePath.c_str()))
	{
		CreateDirectory(filePath.c_str(), NULL);
	}

	filePath += L"\\";
	filePath += SettingFile;

	return filePath;
}