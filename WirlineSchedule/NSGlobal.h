#ifndef NSGLOBAL_H
#define NSGLOBAL_H

class CMySQL;
class CTool;
class NSSound;
class NSManager;
class NSNetBase;

typedef enum _repeater_net_mode_enum
{
	WL,
	P2P
}repeater_net_mode_enum;

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

#endif