#ifndef COMMON_H
#define COMMON_H
#include <process.h>
#include "Ambe3000.h"
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")
#include "VoiceDefs.h"

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "Tool.h"
#include <list>
#include "../lib/rpc/include/TcpServer.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "../lib//glog/logging.h"
#pragma comment(lib, "../lib//glog/lib/libglog.lib")

#include "WLSocketLog.h"

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif  // _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif  // _DEBUG


//static const BYTE GROUPCALL_TYPE = 0x4f;
//static const BYTE PRIVATE_CALL = 0x50;
//static const BYTE ALL_CALL = 0x53;


//#define TALK_TIME (20*1000)

extern WLSocketLog *g_pWLlog;
/************************************************************************/
/* MNIS
/************************************************************************/
typedef struct
{
	int Operate;
	int Target;
	int Type;
	double Cycle;
}QUERY_GPS;

typedef struct
{
	int valid;
	double lon;
	double lat;
	double speed;
}GPS;

typedef struct 
{
	int Type;
	int Target;
	int Source;
	char Contents[256];
}MNIS_MSG;

typedef struct
{
	int getType;
	//FieldValue info;
}ARS;


#define REPEATER_CONNECT 0
#define REPEATER_DISCONNECT 1

enum CLIENT_CALL_TYPE
{
	CLIENT_CALL_TYPE_All = 1,
	CLIENT_CALL_TYPE_Group = 2,
	CLIENT_CALL_TYPE_Private = 3
};

#define PRIVATE_CALL_TAGET (-1)
#define ALL_CALL_TAGET (-2)

#define StartCall 0x00
#define StopCall 0x01

#define CLIENT_TRANSFER_OK "success"
#define CLIENT_TRANSFER_FAIL "fail"
#define WL_SERVER_PORT 9002

enum WLStatus
{
	STARTING = 0,
	WAITFOR_LE_MASTER_PEER_REGISTRATION_TX,
	WAITFOR_LE_MASTER_PEER_REGISTRATION_RESPONSE,
	WAITFOR_MAP_REQUEST_TX,
	WAITFOR_LE_NOTIFICATION_MAP_BROADCAST,

	//xnl connect
	XNL_CONNECT,
	WAITFOR_XNL_DEVICE_MASTER_QUERY_TX,
	WAITFOR_XNL_MASTER_STATUS_BROADCAST,
	WAITFOR_XNL_DEVICE_AUTH_KEY_REQUEST_TX,
	WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY,
	WAITFOR_XNL_DEVICE_CONNECT_REQUEST_TX,
	WAITFOR_XNL_DEVICE_CONNECT_REPLY,
	WAITFOR_XNL_DEVICE_SYSMAP_BROADCAST,
	WAITFOR_XNL_DATA_MSG_DEVICE_INIT_1,
	WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2,
	WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2_TX,
	WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3_TX,
	WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3,
	WAITFOR_XNL_XCMP_READ_SERIAL,
	WAITFOR_XNL_XCMP_READ_SERIAL_RESULT,

	ALIVE,
	TRANSMITTING1,
	TRANSMITTING2,
	WAITINGFOR_LE_DEREGISTRATION_TXFREE,
	WAITINGFOR_LE_DEREGISTRATION_TRANSMISSION,
	BAILOUT
};

#define GROUP_CALL 0x4f//79
#define PRIVATE_CALL 0x50//80
#define ALL_CALL 0x53//83

enum _RECORD_TYPE_VALUE
{
	IPSC = 0,
	CPC,
	LCP,
};
#define MAX_IP_SIZE 16

enum _SlotNumber
{
	NULL_SLOT = 0x00,
	SLOT1,
	SLOT2,
	BOTH_SLOT1_SLOT2,
};

/*配置参数区域*/
extern bool CONFIG_SCHDULE_ISENABLE;//是否与客户交互工作
extern char CONFIG_MASTER_IP[MAX_IP_SIZE];//MASTER IP地址
extern char CONFIG_MNIS_IP[MAX_IP_SIZE];//MNIS IP地址
extern int CONFIG_MNIS_ID;//MNIS ID
extern unsigned short CONFIG_MASTER_PORT;//MASTER端口
extern unsigned long CONFIG_DEFAULT_GROUP;//默认通话组
extern unsigned long CONFIG_LOCAL_RADIO_ID;//本机RADIO ID
extern unsigned long CONFIG_LOCAL_PEER_ID;//本机PEER ID
extern _RECORD_TYPE_VALUE CONFIG_RECORD_TYPE;//当前的录音模式
extern unsigned short CONFIG_DONGLE_PORT;//dongle端口
extern long CONFIG_HUNG_TIME;//session间隔时间
extern long CONFIG_MASTER_HEART_TIME;//主中继心跳间隔
extern long CONFIG_PEER_HEART_AND_REG_TIME;//非主中继心跳间隔和注册间隔
extern _SlotNumber CONFIG_DEFAULT_SLOT;//默认信道

//////////////////////////////////////////////////////////////////////////
/*信号*/
extern HANDLE g_taskLockerEvent;
extern HANDLE g_waitHandleRemoteTask;
/*JSON相关远程命令*/
#define REMOTE_CMD_CONFIG 0x01
#define REMOTE_CMD_CALL 0x02
#define REMOTE_CMD_STOP_CALL 0x03
#define REMOTE_CMD_SET_PLAY_CALL 0x04
#define REMOTE_CMD_GET_CONN_STATUS 0x05
#define REMOTE_CMD_MNIS_QUERY_GPS 0x06
#define REMOTE_CMD_MNIS_MSG 0x07
#define REMOTE_CMD_MNIS_STATUS 0x08

#define GET_TYPE_CONN 0x01
#define GET_TYPE_ONLINE_DEVICES 0x02
/*JSON相关结构体*/

/************************************************************************/
/* 配置命令参数定义
/************************************************************************/
#define MNIS_GET_TYPE_CONNECT 0x01//过去Mnis的连接状态
#define MNIS_GET_TYPE_RADIO 0x02//获取当前关联设备的在线状态
typedef struct
{
	char ip[MAX_IP_SIZE];
	unsigned short port;
}MASTER;
typedef struct
{
	unsigned short donglePort;
}DONGLE;
typedef struct
{
	char ip[MAX_IP_SIZE];
	unsigned short port;
}MNIS;
typedef struct
{
	char Ip[MAX_IP_SIZE];
	unsigned short Port;
}SVR;
typedef struct
{
	bool IsEnable;
	int Type;
	SVR Svr;
	MNIS mnis;
	int MnisId;
	MASTER master;
	unsigned long defaultGroup;
	unsigned long localRadioId;
	unsigned long localPeerId;
	_RECORD_TYPE_VALUE recordType;
	long hangTime;
	long masterHeartTime;
	long peerHeartTime;
	_SlotNumber defaultSlot;
	DONGLE dongle;
}CONFIG;
/************************************************************************/
/* 通话命令参数定义
/************************************************************************/
typedef struct
{
	int operate;
	unsigned long source;
	unsigned long tartgetId;
	unsigned char callType;
	bool isCurrent;

}CALL_OPERATE_PARAM;
typedef struct
{
	CALL_OPERATE_PARAM operateInfo;
}CALL;
/************************************************************************/
/* 通话命令状态参数定义
/************************************************************************/
typedef struct
{
	CALL_OPERATE_PARAM operateInfo;
	int status;
}CALL_PARAM_STATUS;
/************************************************************************/
/*播放命令参数定义
/************************************************************************/
typedef struct
{
	unsigned char callType;
	int targetId;
}SET_PLAY_PARAM;
typedef struct
{
	SET_PLAY_PARAM playParam;
}SET_PLAY;
/************************************************************************/
/* 播放命令状态参数定义
/************************************************************************/
typedef struct
{
	SET_PLAY_PARAM playParam;
	int status;
}SET_PLAY_PARAM_STATUS;
/************************************************************************/
/* 获取信息参数定义
/************************************************************************/
typedef struct
{
	int getType;
	int info;
}GET_INFO_PARAM;
typedef struct
{
	GET_INFO_PARAM getInfo;
}GET_INFO;

typedef struct
{
	union
	{
		CONFIG configParam;
		CALL callParam;
		SET_PLAY setCareCallParam;
		GET_INFO getInfoParam;
		QUERY_GPS queryGpsParam;
		MNIS_MSG msgParam;
		ARS mnisStatusParam;
	}info;
}JSON_PARAM;
/*远程命令*/
typedef struct
{
	//unsigned long long callId;
	//CRemotePeer *pRemote;
	int cmd;
	JSON_PARAM param;
}REMOTE_TASK;
/*远程命令任务队列*/
extern std::list<REMOTE_TASK*> g_remoteCommandTaskQueue;
extern unsigned long long g_sn;
extern std::list<TcpClient*> g_onLineClients;
//extern TcpClient *pTempClient;
//////////////////////////////////////////////////////////////////////////

#define ALL_CALL_ID 255
extern unsigned long CONFIG_CURRENT_TAGET;//当前通话目标
//extern bool g_bIsHaveDefaultGroupCall;
//extern bool g_bIsHavePrivateCall;
//extern bool g_bIsHaveAllCall;
//extern bool g_bIsHaveCurrentGroupCall;
extern long GO_BACK_DEFAULT_GROUP_TIME;//处于非调度组的时间

#define VOICE_STATUS_START 0
#define VOICE_STATUS_CALLBACK 1
#define VOICE_STATUS_END 2

#define PATH_FILE_MAXSIZE 1024
#define DATA_TABLE_NAME_SIZE 64
#define FILE_NAME_MAXSIZE 64
typedef void(*PLogReport)(char* log_msg);
enum    ScrambleDirection  {
	IPSCTODONGLE,
	DONGLETOIPSC
};

#define MAP_MAX_SIZE 128
#define WL_REGISTRATION_ENTRY_MAX_SIZE 32

const int IPSCTODONGLETABLE[49] =
//0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12. 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48
{ 0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 41, 43, 45, 47, 1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 42, 44, 46, 48, 2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35, 38 };

const int DONGLETOIPSCTABLE[49] =
//0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12. 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48
{ 0, 18, 36, 1, 19, 37, 2, 20, 38, 3, 21, 39, 4, 22, 40, 5, 23, 41, 6, 24, 42, 7, 25, 43, 8, 26, 44, 9, 27, 45, 10, 28, 46, 11, 29, 47, 12, 30, 48, 13, 31, 14, 32, 15, 33, 16, 34, 17, 35 };


#define VOICE_DATA_PATH L"D:\\Input.bit"
#define INPUT_DATA_PATH "D:\\Input.bit"

#define INPUT_DATA_PATH_1 "D:\\Input1.pcm"

typedef struct OutData
{
	BYTE* _head;
	DWORD _length;
	BYTE _flag;
}OUTDATA;


#define WL_RETURN_OK  0
#define PLAY_FRAMES (50*2)
#define PLAY_BUFFER_TIME (28 * 20)


#define FLAG_START 0x01
#define FLAG_END 0x02

#define MASK_START 0x01
#define MASK_END 0x02

#define FLAG_DONGLE_AMBE_TO_PCM 0x01
#define FLAG_DONGLE_PCM_TO_AMBE 0x02

#define MASK_DONGLE_AMBE_TO_PCM 0x01
#define MASK_DONGLE_PCM_TO_AMBE 0x02

#define SOUND_PLAY 0
#define SOUND_RECORD 1
#define SOUND_EXIT 2

#define MASK_DECODE_PREARE 0x01//准备解码
#define MASK_DECODE_DOING 0x02//解码中
#define MASK_DECODE_PREAPRE_END 0x04//准备结束解码
#define MASK_DECODE_END 0x08//结束解码

#define FLAG_DECODE_PREARE 0x01
#define FLAG_DECODE_DOING 0x02
#define FLAG_DECODE_PREAPRE_END 0x04
#define FLAG_DECODE_END 0x08

extern bool Env_NetIsOk;//网络是否已经连接
extern bool Env_DongleIsOk;//dongle 是否开启
extern bool Env_SoundIsOk;//sound 是否正常
extern bool Env_MnisIsOk;//mnis 是否正常


extern unsigned long g_callId;//呼出ID
extern unsigned char g_targetCallType;//呼出类型


#define WXJ_DLL FALSE

#define SEND_360MS_TIMES (2)
#define REGISTRATION_PDU_ID 2

#define Wireline_Protocol_Version 0x04

#define RTROPP_OK 0

#define BURST_A 0x01
#define BURST_B 0x02
#define BURST_C 0x03
#define BURST_D 0x04
#define BURST_E 0x05
#define BURST_F 0x06
#define BURST_T 0x07
#define BURST_PH 0x08

#define SEND_TYPE_MIC 1
#define SEND_TYPE_FILE 0

//#define WL_REGISTRATION_REQUEST 0xb201

#define CALL_IDLE 0x0000 //通话已经结束
#define CALL_START 0x0001 //请求初始化通话
#define CALL_ONGOING 0x0002 //正在通话
#define CALL_HANGUP 0x0003 //通话挂起

#define OPERATE_CALL_START 0x00 //开始通话
#define OPERATE_CALL_END 0x01 //结束通话

#define STATUS_CALL_START 0x01 //通话开始
#define STATUS_CALL_END 0x02 //通话结束
#define REMOTE_CMD_SUCCESS 0x04 //操作成功
#define REMOTE_CMD_FAIL 0x08 //操作失败

#define CMD_SUCCESS 0x00
#define CMD_FAIL 0x01

#define Call_Session_Call_Hang	0x0a
#define Call_Session_End	0x0b

#define MAX_PACKET_SIZE 128

typedef struct SendVoiceStruct
{
	char* pPackageData;
	short sPackageLenth;
}SendVoicePackage;

#define STATUS_ONTIME 0x0001

#define PEER_STATUS_NULL			0x00
#define PEER_STATUS_REGIS_RESPONSE	0x01
#define PEER_STATUS_ALIVE_REQUES	0x02
#define PEER_STATUS_ALIVE_RESPONSE	0x03


#define SEND_VOICE_INTERVAL 60
#define VOICE_END_TIMEOUT 600
#define AUTHOR_RETRY_NUM 5
#define AUTHOR_WAITTIME (1000*5)

#define WAIT_LE_PEER_KEEP_ALIVE_RESPONSE_TIMER (1000)
#define PEER_STATUS_CHECK_INTERVAL (1000)

#define LEADING_CHANNEL_ID 0x00 //Refer to CPS configuration.
#define VALUE_MFID 0x00 //Standard Feature

#define REQUEST_CALL_REPEAT_FREQUENCY 3
#define REQUEST_CALL_OUT_TIMER (1000*3)

#define BURST_RTP_HEADER 0x80
#define BURST_START_RTP_MPT 0xDD
#define BURST_NORMAL_RTP_MPT 0x5D
#define BURST_END_RTP_MPT 0x5E
#define BURST_RTP_SSRC 0x00000000 //This field is not used system and should be set to 0.
#define BURST_SERVICEOPTION 0x00 //Non-emergency service Non-broadcast service

#define ALGORITHMID 0x00 //Invalid algorithm ID when CRC error is detected.
#define CALL_ATTRIBUTES 0x00//Clear Call
#define VALUE_IV 0x00000000//Invalid IV when CRC error is detected.
#define KEY_ID 0x00 //Invalid Key ID when CRC error is detected.
#define VALUE_RSSI 0x01A8
#define NUMBER_REGIS_ENTRIES 2
#define CSBK_ARGUMENTS 0x0000000000000000	//in voice call csbk auguments should set 0
#define PREAMBLE_DURATION 0x00	//Preamble Duration. For voice call set it to 0.
#define MAP_TYPE 0x01	//0:System wide map;2:master peer pragramming map


/*自定义case*/
#define WL_BURST_CHECK_TIMEOUT					0x00
#define	LE_PEER_REGISTRATION_REQUEST_LOCAL		0x0A94
#define LE_PEER_KEEP_ALIVE_REQUEST_LOCAL		0x0A98
#define	WL_REGISTRATION_REQUEST_LOCAL			0x0A01
#define	WL_VC_CHNL_CTRL_REQUEST_LOCAL			0x0A13
#define	LE_PEER_REGISTRATION_REQUEST_REMOTE		0x0E94
#define	LE_PEER_REGISTRATION_RESPONSE_REMOTE	0x0E95
#define	LE_PEER_KEEP_ALIVE_REQUEST_REMOTE		0x0E98
#define	LE_PEER_KEEP_ALIVE_RESPONSE_REMOTE		0x0E99
#define	WL_REGISTRATION_STATUS_REMOTE			0x0E02
#define	WL_REGISTRATION_GENERAL_OPS_REMOTE		0x0E03
#define	WL_CHNL_STATUS_REMOTE					0x0E11
#define	WL_CHNL_STATUS_QUERY_REMOTE				0x0E12
#define	WL_VC_CHNL_CTRL_STATUS_REMOTE			0x0E16
#define	WL_VC_CALL_SESSION_STATUS_REMOTE		0x0E20

#pragma region 协议结构体
typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char peerMode;
	unsigned long peerServices;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_90;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short peerMode;
	unsigned long peerServices;
	unsigned char leadingChannelID;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_90_LCP;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char peerMode;
	unsigned long peerServices;
	unsigned short numPeers;
	unsigned short acceptedLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_91;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short peerMode;
	unsigned long peerServices;
	unsigned char leadingChannelID;
	unsigned short numPeers;
	unsigned short acceptedLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_91_LCP;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short length;
}T_LE_PROTOCOL_92;
typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char mapType;
	unsigned short acceptedLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_92_LCP;


typedef struct
{
	unsigned long remotePeerID;
	unsigned long remoteIPAddr;
	unsigned short remotePort;
	unsigned char peerMode;
}MAP_PEER;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short mapLength;
	MAP_PEER mapPeers[MAP_MAX_SIZE];
	unsigned short length;
	unsigned short mapNums;
}T_LE_PROTOCOL_93;

typedef struct
{
	unsigned long remotePeerID;
	unsigned long remoteIPAddr;
	unsigned short remotePort;
	unsigned short peerMode;
	unsigned char leadingChannelID;
}WIDE_MAP;

typedef struct
{
	unsigned char talkgroupID;
	unsigned short configuredSiteBits;
}PROGRAMMING_MAP_INFO;

typedef struct
{
	unsigned char numConfiguredTalkgroups;
	PROGRAMMING_MAP_INFO programmingMapInfos[MAP_MAX_SIZE];
}PROGRAMMING_MAP;

typedef struct
{
	unsigned char talkgroupID;
	unsigned char configuredSiteBits[16];

}ENHANCED_PROGRAMMING_MAP_INFO;

typedef struct
{
	unsigned char numConfiguredTalkgroups;
	ENHANCED_PROGRAMMING_MAP_INFO enhancedProgrammingMapInfos[MAP_MAX_SIZE];

}ENHANCED_PROGRAMMING_MAP;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char mapType;
	unsigned short mapLength;
	union
	{
		WIDE_MAP wideMapPeers[MAP_MAX_SIZE];
		PROGRAMMING_MAP programmingMapPeers[MAP_MAX_SIZE];
		ENHANCED_PROGRAMMING_MAP enhancedProgrammingMapPeers[MAP_MAX_SIZE];
	} mapPayload;
	unsigned short acceptedLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
	unsigned short mapNums;
}T_LE_PROTOCOL_93_LCP;


typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_94;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_95;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char peerMode;
	unsigned long peerServices;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_96;


typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short peerMode;
	unsigned long peerServices;
	unsigned char leadingChannelID;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_96_LCP;


typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char peerMode;
	unsigned long peerServices;
	unsigned short acceptedLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_97;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short peerMode;
	unsigned long peerServices;
	unsigned char leadingChannelID;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_97_LCP;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char peerMode;
	unsigned long peerServices;
	unsigned short length;
}T_LE_PROTOCOL_98;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short peerMode;
	unsigned long peerServices;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_98_LCP;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char peerMode;
	unsigned long peerServices;
	unsigned short length;
}T_LE_PROTOCOL_99;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short peerMode;
	unsigned long peerServices;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_99_LCP;


typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short length;
}T_LE_PROTOCOL_9A;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_9A_LCP;

typedef struct
{
	unsigned char AddressType;
	unsigned long addressRangeStart;
	unsigned long addressRangeEnd;
	unsigned char VoiceAttributes;
	unsigned char CSBKAttributes;
}WL_REGISTRATION_ENTRY;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned char registrationSlotNumber;
	unsigned long registrationPduID;
	unsigned short registrationID;
	unsigned char wirelineStatusRegistration;
	unsigned char numberOfRegistrationEntries;
	WL_REGISTRATION_ENTRY wlRegistrationEntries[WL_REGISTRATION_ENTRY_MAX_SIZE];
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned char WirelineAuthenticationID[4];
	unsigned char WirelineAuthenticationSignature[10];
	unsigned short length;
}T_WL_PROTOCOL_01;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned long registrationPduID;
	unsigned short registrationIDSlot1;
	unsigned short registrationIDSlot2;
	unsigned char registrationStatus;
	unsigned char registrationStatusCode;
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned short length;
}T_WL_PROTOCOL_02;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned char registrationSlotNumber;
	unsigned long registrationPduID;
	unsigned char registrationOperationOpcode;
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned short length;
}T_WL_PROTOCOL_03;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned char slotNumber;
	unsigned long statusPduID;
	unsigned char conventionalchannelStatus;
	unsigned char restChannelStatus;
	unsigned char typeOfCall;
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned short length;
}T_WL_PROTOCOL_11;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned char slotNumber;
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned short length;
}T_WL_PROTOCOL_12;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned char slotNumber;
	unsigned long callID;
	unsigned char callType;
	unsigned long sourceID;
	unsigned long targetID;
	unsigned char accessCriteria;
	unsigned char callAttributes;
	unsigned char preambleDuration;
	unsigned __int64 CSBKArguments;
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned char WirelineAuthenticationID[4];
	unsigned char WirelineAuthenticationSignature[10];
	unsigned short length;
}T_WL_PROTOCOL_13;


typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned char slotNumber;
	unsigned long callID;
	unsigned char callType;
	unsigned char chnCtrlstatus;
	unsigned char DeclineReasonCode;
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned short length;
}T_WL_PROTOCOL_16;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned char slotNumber;
	unsigned long callID;
	unsigned char callType;
	unsigned long sourceID;
	unsigned long targetID;
	unsigned char callAttributes;
	unsigned char MFID;
	unsigned char serviceOption;
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned short length;
}T_WL_PROTOCOL_18;

typedef struct
{
	unsigned char header;
	unsigned char MPT;
	unsigned short SequenceNumber;
	unsigned long Timestamp;
	unsigned long SSRC;

}RTP_HEADER;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned char slotNumber;
	unsigned long callID;
	unsigned char callType;
	unsigned long sourceID;
	unsigned long targetID;
	RTP_HEADER RTPInformationField;
	unsigned char burstType;
	unsigned char MFID;
	unsigned char serviceOption;
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned short length;
}T_WL_PROTOCOL_19;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned char slotNumber;
	unsigned long callID;
	unsigned char callType;
	unsigned long sourceID;
	unsigned long targetID;
	unsigned char callSessionStatus;
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned short length;
}T_WL_PROTOCOL_20;

typedef struct
{
	char data[20];
}AMBE_VOICE_ENCODED_FRAMES;

typedef struct
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned char slotNumber;
	unsigned long callID;
	unsigned char callType;
	unsigned long sourceID;
	unsigned long targetID;
	unsigned char callAttributes;
	RTP_HEADER RTPInformationField;
	unsigned char burstType;
	unsigned char MFID;
	unsigned char serviceOption;
	unsigned char algorithmID;
	unsigned char keyID;
	unsigned long IV;
	AMBE_VOICE_ENCODED_FRAMES AMBEVoiceEncodedFrames;
	unsigned short rawRssiValue;
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned char WirelineAuthenticationID[4];
	unsigned char WirelineAuthenticationSignature[10];
	unsigned short length;
}T_WL_PROTOCOL_21;

typedef struct
{
	unsigned char Value;
	char ReasonCode[256];
	char FailureScenarios[1024];
	bool BhaveGet;
	bool NewCallRetry;
	bool HangCallRetry;
	bool RetryOfIPSC;
}DECLINE_REASON_CODE_INFO;

extern DECLINE_REASON_CODE_INFO g_callRequstDeclineReasonCodeInfo;

#pragma endregion


#endif
