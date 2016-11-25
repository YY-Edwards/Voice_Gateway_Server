#include "stdafx.h"

WLSocketLog g_WLlog;

bool Env_DongleIsOk = false;
bool Env_NetIsOk = false;
bool Env_SoundIsOk = false;
bool Env_MnisIsOk = false;
unsigned long g_callId = 0;
unsigned char g_targetCallType = 0x4f;
DECLINE_REASON_CODE_INFO g_callRequstDeclineReasonCodeInfo = { 0 };
//bool g_bIsHaveDefaultGroupCall = false;
//bool g_bIsHavePrivateCall = false;
//bool g_bIsHaveAllCall = false;
//bool g_bIsHaveCurrentGroupCall=false;
unsigned long CONFIG_CURRENT_TAGET = 9;

/*配置参数区域*/
bool CONFIG_SCHDULE_ISENABLE = false;//是否与客户交互工作
char CONFIG_MASTER_IP[MAX_IP_SIZE] = "192.168.2.121";//MASTER IP地址
char CONFIG_MNIS_IP[MAX_IP_SIZE] = "192.168.11.2";//MNIS IP地址
int CONFIG_MNIS_ID = 1118;
unsigned short CONFIG_MASTER_PORT = 50000;//MASTER端口
unsigned long CONFIG_DEFAULT_GROUP = 9;//默认通话组
unsigned long CONFIG_LOCAL_RADIO_ID = 5;//本机RADIO ID
unsigned long CONFIG_LOCAL_PEER_ID = 120;//本机PEER ID
_RECORD_TYPE_VALUE CONFIG_RECORD_TYPE = CPC;//当前的录音模式
unsigned short CONFIG_DONGLE_PORT = 8;//dongle端口
long CONFIG_HUNG_TIME = 4000;//session间隔时间
long CONFIG_MASTER_HEART_TIME = 59000;//主中继心跳间隔
long CONFIG_PEER_HEART_AND_REG_TIME = 59000;//非主中继心跳间隔和注册间隔
long GO_BACK_DEFAULT_GROUP_TIME = 12 * 1000;
_SlotNumber CONFIG_DEFAULT_SLOT = SLOT1;

std::list<REMOTE_TASK*> g_remoteCommandTaskQueue;
//REMOTE_TASK *g_pNewTask = NULL;

HANDLE g_taskLockerEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
HANDLE g_waitHandleRemoteTask = CreateEvent(NULL, FALSE, FALSE, NULL);
unsigned long long g_sn = 0;
std::list<TcpClient*> g_onLineClients;
//TcpClient *pTempClient = NULL;