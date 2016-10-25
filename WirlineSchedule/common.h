#ifndef COMMON_H
#define COMMON_H


//static const BYTE GROUPCALL_TYPE = 0x4f;
//static const BYTE PRIVATE_CALL = 0x50;
//static const BYTE ALL_CALL = 0x53;

#define GROUPCALL_TYPE 0x4f//79
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
extern char CONFIG_MASTER_IP[MAX_IP_SIZE];//MASTER IP地址
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

#define MAX_RECORD_BUFFER_SIZE (100*1024)
//#define CURRENT_GROUPCALL_TYPE 0x04

#define ALL_CALL_ID 255
extern unsigned long g_targetId;//当前通话组
extern bool g_bIsHaveDefaultGroupCall;
extern bool g_bIsHavePrivateCall;
extern bool g_bIsHaveAllCall;
extern bool g_bIsHaveCurrentGroupCall;
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


#define RTTOPP_SUCCESS  0
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

extern bool g_dongle_open;//dongle 是否开启
extern BOOL g_net_connect;//网络是否已经连接

extern unsigned long g_callId;//呼出ID
extern unsigned char g_targetCallType;//呼出类型


#define WXJ_DLL FALSE

#define SEND_360MS_TIMES (2)
//#define PEER_KEEP_ALIVE_TIME (12*1000)
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
#define CALL_BACKSTAGE 0x04 //通话在后台未播放
#define CALL_PLAY 0x05 //通话开始播放
#define CALL_END 0x06 //通话播放完毕

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
