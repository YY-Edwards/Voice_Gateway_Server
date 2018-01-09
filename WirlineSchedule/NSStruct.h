#ifndef NSSTRUCT_H
#define NSSTRUCT_H

#include "net.h"

class NSManager;

#define Size_Ambe_Send_Data 128
#define TIMEOUT_LE (30*1000)
#define SIZE_SERIAL 10
#define INTERVAL_LOG 500
#define SIZE_COM_NAME 16
#define WL_FAIL_AMBE_DONGLE_ZERO 0x1011 //����Dongle����Ϊ0
#define WL_OK 0x0000 //�ɹ�
#define WL_FAIL 0x0001 //ʧ��
#define SIZE_RING_TX 64
#define SIZE_POOL SIZE_RING_TX
#define RX_TX_TIMEOUT 25 //�������20
#define MILSECONDS_IDLE_REAL 100 //�������ݼ��Ϊ60ms��������100ms���ж����ܽ���
#define TIMEOUT_TRY_COUNT 3
#define SLEEP_WORK_THREAD 10
#define SLEEP_TIMEOUT_THREAD 100
#define SLEEP_AMBE_THREAD 10
#define SLEEP_CHECK_AMBE_THREAD (100*3)
#define SLEEP_GETSERIAL_THREAD 1000
#define SLEEP_SOUNDOUT_THREAD 120
#define Mode_Net_P2P (unsigned short)0x0000
#define Mode_Net_WL (unsigned short)0x0100
#define Mode_Net_Mask (unsigned short)0xff00
#define Mode_Work_IPSC (unsigned short)0x0000
#define Mode_Work_CPC (unsigned short)0x0001
#define Mode_Work_LCP (unsigned short)0x0002
#define Mode_Work_Mask (unsigned short)0x00ff
#define TIMEOUT_VOICE_FRAME (600)//600msΪ�ٷ��ĵ��Ƽ�������,ʵ�����������Ҫ��������ݴ�

const int OUT_SAMPLES_PER_20mS = 160;
const DWORD  INTERNALCOMBUFFSIZE = 2048;
const DWORD  DONGLEBAUDRATE = 230400;
const __int8 DONGLEBITS = 8;
const __int8 DONGLEPARITY = NOPARITY;
const __int8 DONGLESTOP = ONESTOPBIT;

typedef void(*pOnSerialCallFunc)(unsigned char* pSerial, void* param);//��ȡ���кŵĻص�
typedef void(*pOnData)(void* pDataCallBack, unsigned long length, unsigned long index, void* param);//ת�������ݵĻص�

typedef struct _find_item_condition_t
{
	unsigned long peerId;
	unsigned char Opcode;
	unsigned char wlOpcode;
	work_item_type_enum type;
}find_item_condition_t;
typedef struct _out_data_pcm_t
{
	unsigned char _head[OUT_SAMPLES_PER_20mS * 2];
}out_data_pcm_t;
typedef struct _change_data_t
{
	int read;//dongleд����ֽ���
	int write;//dongle�������ֽ���
	void(*pOnData)(void* pDataCallBack, unsigned long length, unsigned long index, void* param);//ת�������ݵĻص�
	void* pReadyData;//��ת��������
	int index;
	void *param;
}change_data_t;
typedef struct _dongle_t
{
	wchar_t createfile[SIZE_COM_NAME];
	char strname[SIZE_COM_NAME];
	bool isusing;//��ǰ�Ƿ�����ʹ��
	HANDLE hcom;//���ھ��,��δ��,�������Ѿ���
}dongle_t;
typedef struct _test_param_t
{
	NSManager* pHandler;
	FILE* pIn;
	FILE* pOut;
}test_param_t;
typedef struct _find_peer_condition_t
{
	unsigned long peer_id;
}find_peer_condition_t;
typedef struct _find_record_condition_t
{
	unsigned long call_id;
	unsigned long src_radio;
	unsigned long target_radio;
}find_record_condition_t;

typedef enum _mic_status_enum
{
	Mic_Error,
	Mic_Idle,
	Mic_Start,
	Mic_Ready_Work,
	Mic_Work,
	Mic_Stop,
	Mic_Wait_End
}mic_status_enum;
typedef enum _record_call_status_enum
{
	VOICE_INIT,
	VOICE_START,//����������ʼ
	VOICE_BURST,//�������ݰ�,�����¼���Ϊ��״̬,����������¼��Ϊ��ʱ����
	VOICE_END_BURST,//������������,�����¼���Ϊ��״̬,����������¼δ�յ�ͨ��״̬��ʶ��
	CALL_SESSION_STATUS_HANG,//ͨ������,�����¼���Ϊ��״̬,����������¼�����call back
	CALL_SESSION_STATUS_END//ͨ������,�����¼���Ϊ��״̬,����������¼Ϊ��ǰͨ�������һ����¼
}record_call_status_enum;
enum ScrambleDirection
{
	AMBETODONGLE,
	DONGLETOAMBE
};
enum PARSERSTATE
{
	FIND_START,
	HIGH_LENGTH,
	LOW_LENGTH,
	READ_DATA
};
typedef enum _item_oprate_enum
{
	Oprate_Del,//�������ɾ��
	Oprate_Add,//��������ƽ�����һ���߳�
	Oprate_Other//��������ƽ��������߳�
}item_oprate_enum;
typedef enum _call_thread_status_enum
{
	/*
	ʱ��:��Ȩδͨ��
	ʱ��:sleep(100)
	*/
	Call_Thread_Author_No,
	/*
	ʱ��:sleep(20)
	*/
	Call_Thread_Status_Idle,
	/*
	ʱ��:�����������
	����:����ͨ������,������״̬ΪCall_Thread_Wait_Reply
	*/
	Call_Thread_Send_Request,
	/*
	ʱ��:sleep(10)
	����:��ʱ����δ�任״̬��ΪCall_Thread_Call_Fail
	����ɹ���ΪCall_Thread_Send_Burst
	����ʧ�ܿ����ط���������ط��������ö�ʱ��
	����ʧ�ܲ������ط����������ΪCall_Thread_Call_Fail
	*/
	Call_Thread_Wait_Reply,
	/*
	����:˳������������
	ʱ��:sleep(60)
	*/
	Call_Thread_Send_Burst,
	/*
	ʱ��:�յ�ֹͣͨ��
	����:���60���뽫β��ȫ������
	ʱ��:sleep(60)
	*/
	Call_Thread_Send_End,
	/*
	����:��֪���Ĵ�ͨ���Ķ���ͨ��������ΪCall_Thread_Status_Idle
	ʱ��:����ͨ�����س�ʱ
	�ظ�����ͨ������������
	����ͨ���������м�̨��֪����
	*/
	Call_Thread_Call_Fail
}call_thread_status_enum;
typedef enum _le_status_enum
{
	STARTING,
	WAITFOR_LE_MASTER_PEER_REGISTRATION_TX,
	WAITFOR_LE_MASTER_PEER_REGISTRATION_RESPONSE,
	WAITFOR_MAP_REQUEST_TX,
	WAITFOR_LE_NOTIFICATION_MAP_BROADCAST,
	ALIVE,
	TRANSMITTING1,
	TRANSMITTING2,
	WAITINGFOR_LE_DEREGISTRATION_TXFREE,
	WAITINGFOR_LE_DEREGISTRATION_TRANSMISSION,
	BAILOUT
}le_status_enum;
typedef enum _xnl_status_enum
{
	WAITLE,
	XNL_CONNECT,
	WAITFOR_XNL_DEVICE_MASTER_QUERY_TX,
	WAITFOR_XNL_MASTER_STATUS_BROADCAST,
	WAITFOR_XNL_DEVICE_AUTH_KEY_REQUEST_TX,
	WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY,
	WAITFOR_XNL_DEVICE_CONNECT_REQUEST_TX,
	WAITFOR_XNL_DEVICE_CONNECT_REPLY,
	WAITFOR_XNL_DEVICE_SYSMAP_BROADCAST,
	WAITFOR_XNL_DATA_MSG_DEVICE_INIT,
	WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2,
	WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2_TX,
	WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3_TX,
	WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3,
	WAITFOR_XNL_XCMP_READ_SERIAL,
	WAITFOR_XNL_XCMP_READ_SERIAL_RESULT,
	GET_SERIAL_SUCCESS
}xnl_status_enum;

class NSWLNet;
class CP2PNet;
typedef enum _address_type_enum
{
	IndividualCall = 0x01,
	GroupCall,
	AllIndividualCall,
	AllTalkGroupCall,
	AllWideTalkGroupCall,
	AllLocalTalkGroupCall,
}address_type_enum;

typedef enum _slot_number_enum
{
	NULL_SLOT = 0x00,
	SLOT1,
	SLOT2,
	BOTH_SLOT1_SLOT2,
}slot_number_enum;

class NSWLPeer;
typedef struct _make_call_param_t
{
	unsigned long targetID;
	unsigned char callType;
	slot_number_enum slotNumber;
	unsigned long callID;
	NSWLPeer* peer;//��ǰ����ͨ����peer,�����ٴ�����
}make_call_param_t;

typedef enum _work_mode_enum
{
	IPSC = 0,
	CPC = 1,
	LCP = 2
}work_mode_enum;

typedef struct _StartNetParam
{
	char master_ip[16];
	unsigned long matser_peer_id;
	unsigned short master_port;
	char local_ip[16];//����IP����,Ϊ�����ʾȡ��ǰ���������ַ
	unsigned long local_peer_id;
	unsigned short local_port;
	work_mode_enum work_mode;
	unsigned short master_firewall_time;//���м̶Եȷ���ǽʱ��,0��ʾ����Ĭ��,��λΪ����
	unsigned short peer_firewall_time;//���м̶Եȷ���ǽʱ��,0��ʾ����Ĭ��,��λΪ����
	unsigned short hang_time;//ͨ������ʱ��,��λΪ����
	unsigned long local_radio_id;
}StartNetParam;

typedef struct _StartSerialParam
{
	pOnSerialCallFunc pSerialCallBack;
	struct sockaddr_in *pSockaddrMaster;
	pXQTTNet pMasterXqttnet;
	StartNetParam *pNetParam;
	void* param;
}StartSerialParam;

typedef struct _wl_peer_build_param_t
{
	NSWLNet* pNet;
	unsigned long peerId;
	unsigned long addr;
	unsigned short port;
	StartNetParam* pNetParam;
}wl_peer_build_param_t;
typedef struct _p2p_peer_build_param_t
{
	CP2PNet* pNet;
	unsigned long peerId;
	unsigned long addr;
	unsigned short port;
	StartNetParam* pNetParam;
}p2p_peer_build_param_t;
typedef struct _local_ip_list_t
{
	unsigned long iplist[255];
	unsigned short count;
}local_ip_list_t;

typedef enum _wl_reg_status
{
	WL_REG_UNKNOWN = -1,
	WL_REG_SUCCESS,
	WL_REG_FAIl
}wl_reg_status;

typedef struct _decline_reason_code_info_t
{
	unsigned char Value;
	char ReasonCode[256];
	char FailureScenarios[1024];
	bool BhaveGet;
	bool NewCallRetry;
	bool HangCallRetry;
	bool RetryOfIPSC;
}decline_reason_code_info_t;

typedef struct _send_net_data_ambe_t
{
	char net_data[Size_Ambe_Send_Data];
	unsigned short net_length;
}send_net_data_ambe_t;

#endif