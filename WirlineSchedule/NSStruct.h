#ifndef NSSTRUCT_H
#define NSSTRUCT_H

#include "net.h"

class NSManager;

#define Size_Ambe_Send_Data 128
#define TIMEOUT_LE (30*1000)
#define SIZE_SERIAL 10
#define INTERVAL_LOG 500
#define SIZE_COM_NAME 16
#define WL_FAIL_AMBE_DONGLE_ZERO 0x1011 //可用Dongle数量为0
#define WL_OK 0x0000 //成功
#define WL_FAIL 0x0001 //失败
#define SIZE_RING_TX 64
#define SIZE_POOL SIZE_RING_TX
#define RX_TX_TIMEOUT 25 //必须大于20
#define MILSECONDS_IDLE_REAL 100 //网络数据间隔为60ms这里设置100ms来判定解密结束
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
#define TIMEOUT_VOICE_FRAME (600)//600ms为官方文档推荐的数据,实际情况可能需要比这个数据大

const int OUT_SAMPLES_PER_20mS = 160;
const DWORD  INTERNALCOMBUFFSIZE = 2048;
const DWORD  DONGLEBAUDRATE = 230400;
const __int8 DONGLEBITS = 8;
const __int8 DONGLEPARITY = NOPARITY;
const __int8 DONGLESTOP = ONESTOPBIT;

typedef void(*pOnSerialCallFunc)(unsigned char* pSerial, void* param);//获取序列号的回调
typedef void(*pOnData)(void* pDataCallBack, unsigned long length, unsigned long index, void* param);//转换后数据的回调

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
	int read;//dongle写入的字节数
	int write;//dongle读出的字节数
	void(*pOnData)(void* pDataCallBack, unsigned long length, unsigned long index, void* param);//转换后数据的回调
	void* pReadyData;//待转换的数据
	int index;
	void *param;
}change_data_t;
typedef struct _dongle_t
{
	wchar_t createfile[SIZE_COM_NAME];
	char strname[SIZE_COM_NAME];
	bool isusing;//当前是否正在使用
	HANDLE hcom;//串口句柄,空未打开,不空则已经打开
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
	VOICE_START,//语音正常开始
	VOICE_BURST,//语音数据包,如果记录最后为此状态,则代表此条记录因为超时结束
	VOICE_END_BURST,//语音正常结束,如果记录最后为此状态,则代表此条记录未收到通话状态标识符
	CALL_SESSION_STATUS_HANG,//通话挂起,如果记录最后为此状态,则代表此条记录后存在call back
	CALL_SESSION_STATUS_END//通话结束,如果记录最后为此状态,则代表此条记录为当前通话的最后一条记录
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
	Oprate_Del,//处理完后删除
	Oprate_Add,//处理完后移交给下一级线程
	Oprate_Other//处理完后移交给其他线程
}item_oprate_enum;
typedef enum _call_thread_status_enum
{
	/*
	时机:授权未通过
	时延:sleep(100)
	*/
	Call_Thread_Author_No,
	/*
	时延:sleep(20)
	*/
	Call_Thread_Status_Idle,
	/*
	时机:发起呼叫请求
	处理:发送通话请求,并设置状态为Call_Thread_Wait_Reply
	*/
	Call_Thread_Send_Request,
	/*
	时延:sleep(10)
	处理:超时若还未变换状态置为Call_Thread_Call_Fail
	请求成功置为Call_Thread_Send_Burst
	请求失败可以重发的情况下重发请求并重置定时器
	请求失败不可以重发的情况下置为Call_Thread_Call_Fail
	*/
	Call_Thread_Wait_Reply,
	/*
	处理:顺序发送语音数据
	时延:sleep(60)
	*/
	Call_Thread_Send_Burst,
	/*
	时机:收到停止通话
	处理:间隔60毫秒将尾包全部发送
	时延:sleep(60)
	*/
	Call_Thread_Send_End,
	/*
	处理:告知关心此通话的对象通话结束置为Call_Thread_Status_Idle
	时机:请求通话返回超时
	重复发送通话请求次数完成
	发起通话过程中中继台告知结束
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
	NSWLPeer* peer;//当前请求通话的peer,方便再次请求
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
	char local_ip[16];//本机IP都中,为空则表示取当前电脑任意地址
	unsigned long local_peer_id;
	unsigned short local_port;
	work_mode_enum work_mode;
	unsigned short master_firewall_time;//主中继对等防火墙时间,0表示采用默认,单位为毫秒
	unsigned short peer_firewall_time;//从中继对等防火墙时间,0表示采用默认,单位为毫秒
	unsigned short hang_time;//通话挂起时间,单位为毫秒
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