#include "stdafx.h"

bool g_dongle_open = false;
BOOL g_net_connect = false;
unsigned long g_callId = 0;
unsigned char g_targetCallType = 0x4f;
DECLINE_REASON_CODE_INFO g_callRequstDeclineReasonCodeInfo = { 0 };
bool g_bIsHaveDefaultGroupCall = false;
bool g_bIsHavePrivateCall = false;
bool g_bIsHaveAllCall = false;
bool g_bIsHaveCurrentGroupCall=false;
unsigned long g_targetId = 9;

/*���ò�������*/
char CONFIG_MASTER_IP[MAX_IP_SIZE] = "192.168.2.121";//MASTER IP��ַ
unsigned short CONFIG_MASTER_PORT = 50000;//MASTER�˿�
unsigned long CONFIG_DEFAULT_GROUP = 9;//Ĭ��ͨ����
unsigned long CONFIG_LOCAL_RADIO_ID = 5;//����RADIO ID
unsigned long CONFIG_LOCAL_PEER_ID = 120;//����PEER ID
_RECORD_TYPE_VALUE CONFIG_RECORD_TYPE = CPC;//��ǰ��¼��ģʽ
unsigned short CONFIG_DONGLE_PORT = 8;//dongle�˿�
long CONFIG_HUNG_TIME = 4000;//session���ʱ��
long CONFIG_MASTER_HEART_TIME = 60000;//���м��������
long CONFIG_PEER_HEART_AND_REG_TIME = 60000;//�����м����������ע����
long GO_BACK_DEFAULT_GROUP_TIME = 12 * 1000;
_SlotNumber CONFIG_DEFAULT_SLOT = SLOT1;

std::list<REMOTE_TASK*> g_remoteCommandTaskQueue;
REMOTE_TASK *g_pNewTask = NULL;

HANDLE g_taskLockerEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
unsigned long long g_sn = 0;
std::list<TcpClient*> g_onLineClients;
TcpClient *g_pTempClient = NULL;