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
#include <fstream>
#include "../lib/rapidjson/document.h"
#include "../lib/rapidjson/istreamwrapper.h"
#include "../lib/rapidjson/ostreamwrapper.h"
#include "../lib/rapidjson/writer.h"

#define			SettingFile				L"setting.dat"
#define			AppName					L"trbox"

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

	std::string getRadioIp();
	int setRadioIp(const char* ip);


protected:
	int getRoot(rapidjson::Document& d);
	std::wstring getFilePath();

private:
	CSettings();
	~CSettings(void);
	CSettings(const CSettings&){}
	CSettings & operator= (const CSettings &){}

	std::mutex m_writeLocker;
	friend class std::auto_ptr<CSettings>;
	static std::auto_ptr<CSettings> m_instance;
};

