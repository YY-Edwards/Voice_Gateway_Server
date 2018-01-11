#include "NSGlobal.h"
#include "Tool.h"

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