#ifndef NS_H
#define NS_H

#include "../include/HMAC_SHA1.h"
#include "../include/MySQL.h"
#include "../include/WDK_VidPidQuery.h"
#include "../Include/Tool.h"

class NS
{
public:
	NS(const void* param, OnCall oncall, OnCallStatus onCallStatus, OnNSSystemStatusChange onSystemChange);
	~NS();

	int StartNet(StartNetParam* p);
	int InitializeDongleModel();
	int SizeDongle();
	DWORD InitSoundIn();
	DWORD InitSoundOut();
	call_thread_status_enum CallThreadStatus();
	void CallStart(make_call_param_t *p);
	void CallStop();
	le_status_enum LeStatus();
	static void setDb(CMySQL* value);
	static void setAmbeDataPath(const wchar_t* value);
	mic_status_enum MicStatus();
	void OnUpdateUsb(DWORD operateType);
	void setPlayCallType(unsigned char value);
	void setPlayTargetId(unsigned long value);
	static license_status_enum LicenseStatus();
	static void setLicenseStatus(license_status_enum value);
	static void setRepeaterNetMode(repeater_net_mode_enum value);
	static const char* GetSerialNumber();
private:
};
#endif