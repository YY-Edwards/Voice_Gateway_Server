#pragma once

/**
* 参数设置模块
* 参数文件格式：
{
	radio:{
		ip: 10.0.0.2,
	},
	wirelan:{
		ip:192.168.10.2,
	}
}
*/

#include <memory>
#include <mutex>
#include <map>
#include <fstream>
#include "../lib/rapidjson/document.h"
#include "../lib/rapidjson/istreamwrapper.h"
#include "../lib/rapidjson/ostreamwrapper.h"
#include "../lib/rapidjson/writer.h"

#define			SettingFile				L"setting.dat"
#define			ConmpanyName			L"JiHua Information"
#define			AppName					L"Trbox"
#define			AppVersion				L"3.0"

#define			SettingFileA			"setting.dat"
#define			ConmpanyNameA			"JiHua Information"
#define			AppNameA				"Trbox"
#define			AppVersionA				"3.0"

class CSettings
{
public:
	static CSettings* instance()
	{
		static std::mutex locker;

		if (NULL == m_instance.get())
		{
			std::lock_guard<std::mutex> lock(locker);

			if (NULL == m_instance.get())
			{
				m_instance.reset(new CSettings());
			}
		}

		return m_instance.get();
	}

	std::string getValue(const char* type);
	int setValue(const char* type, rapidjson::Value obj);
	std::string getResponse(char* pStatus, uint64_t callId, int errCode, const char* statusText, std::string contents);
	std::string getRequest(char* pCall, char * type, uint64_t callId, std::string contents);

protected:
	int getRoot(rapidjson::Document& d);
	std::wstring getFilePath();
	std::string getFilePathA();

private:
	CSettings();
	~CSettings(void);
	CSettings(const CSettings&){}
	CSettings & operator= (const CSettings &){}

	std::mutex m_writeLocker;
	friend class std::auto_ptr<CSettings>;
	static std::auto_ptr<CSettings> m_instance;
};

