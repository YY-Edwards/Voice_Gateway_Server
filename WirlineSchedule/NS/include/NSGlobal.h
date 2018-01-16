#ifndef NSGLOBAL_H
#define NSGLOBAL_H

class CMySQL;
class CTool;
class NSSound;
class NSManager;
class NSNetBase;

typedef struct _oncall_info_t
{
	unsigned char callType;
	unsigned long srcId;
	unsigned long tgtId;
	int status;
	bool isCurrent;
}oncall_info_t;
typedef struct _oncallstatus_info_t
{
	unsigned char callType;
	unsigned long srcId;
	unsigned long tgtId;
	int status;
}oncallstatus_info_t;
typedef struct _onsystemstatuschange_info_t
{
	int type;
	int value;
}onsystemstatuschange_info_t;

typedef enum _license_status_enum
{
	license_status_unknown,
	license_status_pass,
	license_status_nopass
}license_status_enum;
typedef enum _repeater_net_mode_enum
{
	WL,
	P2P
}repeater_net_mode_enum;

typedef void(*OnCall)(void* onCallParam, oncall_info_t* info);
typedef void(*OnCallStatus)(void* onCallStatusParam, oncallstatus_info_t* info);
typedef void(*OnNSSystemStatusChange)(void* OnSystemStatusChangeParam, onsystemstatuschange_info_t* info);

extern NSNetBase* g_pNSNet;
extern repeater_net_mode_enum g_repeater_net_mode;
extern unsigned short g_timing_alive_time_master;
extern unsigned short g_timing_alive_time_peer;
extern unsigned short g_hang_time;
extern CMySQL* g_pDb;
extern wchar_t g_ambedata_path[260];
extern unsigned short g_network_mode;//高8位表示网络模式,低8位表示工作模式.用于对应数据库的record_type
extern CTool* g_pNSTool;
extern NSSound* g_pNSSound;
extern NSManager* g_pNSManager;
extern unsigned char g_playCalltype;
extern unsigned long g_playTargetId;
extern int g_should_delete;
extern license_status_enum g_license_status;
//extern void* onCallParam;
//extern OnCall g_event_oncall;
//extern void* onCallStatusParam;
//extern OnCallStatus g_event_oncallstatus;
//extern void* onSystemStatusChangeParam;
//extern OnNSSystemStatusChange g_event_systemstatuschange;

void NS_RegCallEvent(void* param, OnCall callback);
void NS_UnregCallEvent();
void NS_SafeCallEvent(oncall_info_t* info);
void NS_RegCallStatusEvent(void* param, OnCallStatus callback);
void NS_UnregCallStatusEvent();
void NS_SafeCallStatusEvent(oncallstatus_info_t* info);
void NS_RegSystemStatusChangeEvent(void* param, OnNSSystemStatusChange callback);
void NS_UnregSystemStatusChangeEvent();
void NS_SafeSystemStatusChangeEvent(onsystemstatuschange_info_t* info);

#endif