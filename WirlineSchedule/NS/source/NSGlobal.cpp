#include "../include/NSGlobal.h"
#include "../include/Tool.h"
#include "../include/mutex.h"

NSNetBase* g_pNSNet = NULL;
repeater_net_mode_enum g_repeater_net_mode = WL;
unsigned short g_timing_alive_time_master = (15 * 1000);
unsigned short g_timing_alive_time_peer = (15 * 1000);
unsigned short g_hang_time = (4 * 1000);
CMySQL* g_pDb = NULL;
wchar_t g_ambedata_path[260] = { 0 };
unsigned short g_network_mode = 0x0000;
CTool* g_pNSTool = NULL;
NSSound* g_pNSSound = NULL;
NSManager* g_pNSManager = NULL;
unsigned char g_playCalltype = 0x4f;
unsigned long g_playTargetId = 0;
int g_should_delete = 0;
license_status_enum g_license_status = license_status_unknown;
std::string repeaterSerial = "";
void* onCallParam = NULL;
OnCall g_event_oncall = NULL;
void* onCallStatusParam = NULL;
OnCallStatus g_event_oncallstatus = NULL;
void* onSystemStatusChangeParam = NULL;
OnNSSystemStatusChange g_event_systemstatuschange = NULL;
LOCKERTYPE lock_call = NULL;
LOCKERTYPE lock_callStatus = NULL;
LOCKERTYPE lock_SystemChange = NULL;

void NS_RegCallEvent(void* param, OnCall callback)
{
	onCallParam = param;
	g_event_oncall = callback;
	lock_call = INITLOCKER();
}

void NS_UnregCallEvent()
{
	onCallParam = NULL;
	g_event_oncall = NULL;
	DELETELOCKER(lock_call);
	lock_call = NULL;
}

void NS_RegCallStatusEvent(void* param, OnCallStatus callback)
{
	onCallStatusParam = param;
	g_event_oncallstatus = callback;
	lock_callStatus = INITLOCKER();
}

void NS_UnregCallStatusEvent()
{
	onCallStatusParam = NULL;
	g_event_oncallstatus = NULL;
	DELETELOCKER(lock_callStatus);
	lock_callStatus = NULL;
}

void NS_RegSystemStatusChangeEvent(void* param, OnNSSystemStatusChange callback)
{
	onSystemStatusChangeParam = param;
	g_event_systemstatuschange = callback;
	lock_SystemChange = INITLOCKER();
}

void NS_UnregSystemStatusChangeEvent()
{
	onSystemStatusChangeParam = NULL;
	g_event_systemstatuschange = NULL;
	DELETELOCKER(lock_SystemChange);
	lock_SystemChange = NULL;
}

void NS_SafeCallEvent(oncall_info_t* info)
{
	if (g_event_oncall)
	{
		TRYLOCK(lock_call);
		(*g_event_oncall)(onCallParam, info);
		RELEASELOCK(lock_call);
	}
}

void NS_SafeCallStatusEvent(oncallstatus_info_t* info)
{
	if (g_event_oncallstatus)
	{
		TRYLOCK(lock_callStatus);
		(*g_event_oncallstatus)(onCallStatusParam, info);
		RELEASELOCK(lock_callStatus);
	}
}

void NS_SafeSystemStatusChangeEvent(onsystemstatuschange_info_t* info)
{
	if (g_event_systemstatuschange)
	{
		TRYLOCK(lock_SystemChange);
		(*g_event_systemstatuschange)(onSystemStatusChangeParam, info);
		RELEASELOCK(lock_SystemChange);
	}
}
