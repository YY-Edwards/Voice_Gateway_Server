#ifndef NETSTRUCT_H
#define NETSTRUCT_H

#define Map_Peer_Size 123 //by 《devspec_link_establishment_0103.pdf》 line 1254
#define WL_REGISTRATION_ENTRY_MAX_SIZE 32
#define SIZE_SEND_BUFF 1024
#define Master_Peer_Programming_Size 15 //by 《devspec_link_establishment_0103.pdf》 line 968
#define Enhanced_Master_Peer_Programming_Map_Size 20

#pragma region Moto协议结构体
typedef struct _T_LE_PROTOCOL_90
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char peerMode;
	unsigned long peerServices;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_90;

typedef struct _T_LE_PROTOCOL_90_LCP
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

typedef struct _T_LE_PROTOCOL_91
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

typedef struct _T_LE_PROTOCOL_91_LCP
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

typedef struct _T_LE_PROTOCOL_92
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short length;
}T_LE_PROTOCOL_92;
typedef struct _T_LE_PROTOCOL_92_LCP
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char mapType;
	unsigned short acceptedLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_92_LCP;


typedef struct _map_peer_info_t
{
	unsigned long remotePeerID;
	unsigned long remoteIPAddr;
	unsigned short remotePort;
	unsigned char peerMode;
}map_peer_info_t;

typedef struct _T_LE_PROTOCOL_93
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short mapLength;
	map_peer_info_t mapPeers[Map_Peer_Size];
	unsigned short length;
	unsigned short mapNums;
}T_LE_PROTOCOL_93;

typedef struct _System_Wide_Map_t
{
	unsigned long remotePeerID;
	unsigned long remoteIPAddr;
	unsigned short remotePort;
	unsigned short peerMode;
	unsigned char leadingChannelID;
}System_Wide_Map_t;

typedef struct _Master_Peer_Programming_Map_Info_t
{
	unsigned char talkgroupID;
	unsigned short configuredSiteBits;
}Master_Peer_Programming_Map_Info_t;

typedef struct _Master_Peer_Programming_Map_t
{
	unsigned char numConfiguredTalkgroups;
	Master_Peer_Programming_Map_Info_t programmingMapInfos[Master_Peer_Programming_Size];
}Master_Peer_Programming_Map_t;

typedef struct _Enhanced_Master_Peer_Programming_Map_Info_t
{
	unsigned char talkgroupID;
	unsigned char configuredSiteBits[16];

}Enhanced_Master_Peer_Programming_Map_Info_t;

typedef struct _Enhanced_Master_Peer_Programming_Map_t
{
	unsigned char numConfiguredTalkgroups;
	Enhanced_Master_Peer_Programming_Map_Info_t enhancedProgrammingMapInfos[Enhanced_Master_Peer_Programming_Map_Size];

}Enhanced_Master_Peer_Programming_Map_t;

typedef union _Map_Payload_t
{
	System_Wide_Map_t wideMapPeers[Map_Peer_Size];
	Master_Peer_Programming_Map_t programmingMapPeers;
	Enhanced_Master_Peer_Programming_Map_t enhancedProgrammingMapPeers;
} Map_Payload_t;

typedef struct _T_LE_PROTOCOL_93_LCP
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char mapType;
	unsigned short mapLength;
	Map_Payload_t mapPayload;
	unsigned short acceptedLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
	unsigned short mapNums;
}T_LE_PROTOCOL_93_LCP;


typedef struct _T_LE_PROTOCOL_94
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_94;

typedef struct _T_LE_PROTOCOL_95
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_95;

typedef struct _T_LE_PROTOCOL_96
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char peerMode;
	unsigned long peerServices;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_96;


typedef struct _T_LE_PROTOCOL_96_LCP
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


typedef struct _T_LE_PROTOCOL_97
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char peerMode;
	unsigned long peerServices;
	unsigned short acceptedLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_97;

typedef struct _T_LE_PROTOCOL_97_LCP
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

typedef struct _T_LE_PROTOCOL_98
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char peerMode;
	unsigned long peerServices;
	unsigned short length;
}T_LE_PROTOCOL_98;

typedef struct _T_LE_PROTOCOL_98_LCP
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short peerMode;
	unsigned long peerServices;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_98_LCP;

typedef struct _T_LE_PROTOCOL_99
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char peerMode;
	unsigned long peerServices;
	unsigned short length;
}T_LE_PROTOCOL_99;

typedef struct _T_LE_PROTOCOL_99_LCP
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short peerMode;
	unsigned long peerServices;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_99_LCP;


typedef struct _T_LE_PROTOCOL_9A
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short length;
}T_LE_PROTOCOL_9A;

typedef struct _T_LE_PROTOCOL_9A_LCP
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned short currentLinkProtocolVersion;
	unsigned short oldestLinkProtocolVersion;
	unsigned short length;
}T_LE_PROTOCOL_9A_LCP;

typedef struct _WL_REGISTRATION_ENTRY
{
	unsigned char AddressType;
	unsigned long addressRangeStart;
	unsigned long addressRangeEnd;
	unsigned char VoiceAttributes;
	unsigned char CSBKAttributes;
}WL_REGISTRATION_ENTRY;

typedef struct _T_WL_PROTOCOL_01
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

typedef struct _T_WL_PROTOCOL_02
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

typedef struct _T_WL_PROTOCOL_03
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

typedef struct _T_WL_PROTOCOL_11
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

typedef struct _T_WL_PROTOCOL_12
{
	unsigned char Opcode;
	unsigned long peerID;
	unsigned char wirelineOpcode;
	unsigned char slotNumber;
	unsigned char currentLinkProtocolVersion;
	unsigned char oldestLinkProtocolVersion;
	unsigned short length;
}T_WL_PROTOCOL_12;

typedef struct _T_WL_PROTOCOL_13
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


typedef struct _T_WL_PROTOCOL_16
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

typedef struct _T_WL_PROTOCOL_18
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

typedef struct _RTP_HEADER
{
	unsigned char header;
	unsigned char MPT;
	unsigned short SequenceNumber;
	unsigned long Timestamp;
	unsigned long SSRC;

}RTP_HEADER;

typedef struct _T_WL_PROTOCOL_19
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

typedef struct _T_WL_PROTOCOL_20
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

typedef struct _AMBE_VOICE_ENCODED_FRAMES
{
	char data[20];

}AMBE_VOICE_ENCODED_FRAMES;

typedef struct _send_ambe_voice_encoded_frames_t
{
	AMBE_VOICE_ENCODED_FRAMES ambe;
	bool start;
}send_ambe_voice_encoded_frames_t;

typedef struct _T_WL_PROTOCOL_21
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

typedef struct _T_P2P_CALL_HEADER   /*从moto2.0中获得*/
{
	char                        CallOpcode;                               // 0
	unsigned __int8             CallSequenceNumber;                       // 1
	unsigned __int8             CallPriority;                             // 2
	unsigned __int8             CallControlInformation;                   // 3
	unsigned __int8             RepeaterBurstDataType;                    // 4
	unsigned __int8             ESNLIEHB;                                 // 5
	unsigned __int16            RTPSequenceNumber;     //Host Order       // 6
	unsigned __int32            CallOriginatingPeerID; //Host Order       // 8
	unsigned __int32            CallFloorControlTag;   //Host Order       //12
	unsigned __int32            CallSrcID;             //Host Order 0x3210//16
	unsigned __int32            CallTgtID;             //Host Order 0x3210//20
	unsigned __int32            RTPTimeStamp;          //Host order 0x4321//24
}P2P_CALL_HEADER;

typedef struct _T_XNL_DEVICE_MASTER_STATUS_BRDCST
{
	unsigned short Opcode;
	unsigned char  ProtocolId;
	unsigned char  XnlFlags;
	unsigned short DesAddress;
	unsigned short SourceAddress;
	unsigned short TrascationId;
	unsigned short PayloadLength;
	unsigned short MinorXNLVersionNumber;
	unsigned short MajorXNVersioNumber;
	unsigned short MasterLogicalIdentifier;
	unsigned short DataMessageSent;
}T_XNL_DEVICE_MASTER_STATUS_BRDCST;
typedef struct _T_XNL_DEVICE_MASTER_QUERY
{
	unsigned short Opcode;
	unsigned char  ProtocolId;
	unsigned char  XnlFlags;
	unsigned short DesAddress;
	unsigned short SourceAddress;
	unsigned short TrascationId;
	unsigned short PayloadLength;
}T_XNL_DEVICE_MASTER_QUERY;

typedef struct _T_XNL_DEVICE_AUTH_KEY_REQUEST
{
	unsigned short Opcode;
	unsigned char  ProtocolId;
	unsigned char  XnlFlags;
	unsigned short DesAddress;
	unsigned short SourceAddress;
	unsigned short TrascationId;
	unsigned short PayloadLength;
}T_XNL_DEVICE_AUTH_KEY_REQUEST;

typedef struct _T_XNL_DEVICE_AUTH_KEY_REPLY
{
	unsigned short Opcode;
	unsigned char  ProtocolId;
	unsigned char  XnlFlags;
	unsigned short DesAddress;
	unsigned short SourceAddress;
	unsigned short TrascationId;
	unsigned short PayloadLength;
	unsigned short TemporaryXnlAddress;
	unsigned char UnencryptedAuthenticationValue[8];
}T_XNL_DEVICE_AUTH_KEY_REPLY;

typedef struct _T_XNL_DEVICE_CONN_REQUEST
{
	unsigned short Opcode;
	unsigned char  ProtocolId;
	unsigned char  XnlFlags;
	unsigned short DesAddress;
	unsigned short SourceAddress;
	unsigned short TrascationId;
	unsigned short PayloadLength;
	unsigned short PreferredXnlAddress;
	unsigned char  DeviceType;
	unsigned char  AuthenticationIndex;
	unsigned char  EncryptedAuthenticationValue[8];
}T_XNL_DEVICE_CONN_REQUEST;

typedef struct _T_XNL_DEVICE_CONN_REPLY
{
	unsigned short Opcode;
	unsigned char  ProtocolId;
	unsigned char  XnlFlags;
	unsigned short DesAddress;
	unsigned short SourceAddress;
	unsigned short TrascationId;
	unsigned short PayloadLength;
	unsigned char  ResultCode;
	unsigned char  TrascationIdBase;
	unsigned short XnlAddress;
	unsigned short LogicalAddress;
	unsigned char  EncryptedAuthenticationValue[8];
}T_XNL_DEVICE_CONN_REPLY;

typedef struct _T_XNL_DEVICE_SYSMAP_REQUEST
{
	unsigned short Opcode;
	unsigned char  ProtocolId;
	unsigned char  XnlFlags;
	unsigned short DesAddress;
	unsigned short SourceAddress;
	unsigned short TrascationId;
	unsigned short PayloadLength;
}T_XNL_DEVICE_SYSMAP_REQUEST;

typedef struct _T_XNL_DEVICE_SYSMAP_BRDCST
{
	unsigned short Opcode;
	unsigned char  ProtocolId;
	unsigned char  XnlFlags;
	unsigned short DesAddress;
	unsigned short SourceAddress;
	unsigned short TrascationId;
	unsigned short PayloadLength;
	unsigned short SizeOfMap;
	unsigned short LogicalIdentifier_1;
	unsigned short XnlAddress_1;
	unsigned char AuthenticationIndex_1;
}T_XNL_DEVICE_SYSMAP_BRDCST;

typedef struct _T_XCMP_RADIO_STATUS_REQUEST
{
	unsigned short Opcode;
	unsigned char  Condition;
}XCMP_RADIO_STATUS_REQUEST;

typedef struct _T_XCMP_RADIO_STATUS_REPLY
{
	unsigned short Opcode;
	unsigned char Result;
	unsigned char Condition;
	unsigned char ProductSerialNumber[10];
}XCMP_RADIO_STATUS_REPLY;
typedef union _T_RADIO_STATUS
{
	XCMP_RADIO_STATUS_REQUEST RadioStatusRequest;
	XCMP_RADIO_STATUS_REPLY   RadioStatusReply;
}RADIO_STATUS;
typedef struct _T_DEVICE_INIT
{
	unsigned short Opcode;
	unsigned long  Version;
	unsigned char  DeviceInitType[3];
}DEVICE_INIT;
typedef union _T_XCMP_XNL_PAYLOAD
{
	//DEVICE_INIT   DeviceInit;
	RADIO_STATUS  RadioStatus;
}XCMP_XNL_PAYLOAD;
typedef struct _T_XNL_DATA_MSG
{
	unsigned short     Opcode;
	unsigned char      ProtocolId;
	unsigned char      XnlFlags;
	unsigned short     DesAddress;
	unsigned short     SourceAddress;
	unsigned short     TrascationId;
	unsigned short     PayloadLength;
	XCMP_XNL_PAYLOAD    Payload;
}T_XNL_DATA_MSG;

typedef struct _T_XNL_DATA_MSG_ACK
{
	unsigned short Opcode;
	unsigned char  ProtocolId;
	unsigned char  XnlFlags;
	unsigned short DesAddress;
	unsigned short SourceAddress;
	unsigned short TrascationId;
	unsigned short PayloadLength;
}T_XNL_DATA_MSG_ACK;
#pragma endregion

typedef union _moto_protocol_le_t
{
	T_LE_PROTOCOL_90 PROTOCOL_90;
	T_LE_PROTOCOL_91 PROTOCOL_91;
	T_LE_PROTOCOL_92 PROTOCOL_92;
	T_LE_PROTOCOL_93 PROTOCOL_93;
	T_LE_PROTOCOL_94 PROTOCOL_94;
	T_LE_PROTOCOL_95 PROTOCOL_95;
	T_LE_PROTOCOL_96 PROTOCOL_96;
	T_LE_PROTOCOL_97 PROTOCOL_97;
	T_LE_PROTOCOL_98 PROTOCOL_98;
	T_LE_PROTOCOL_99 PROTOCOL_99;
	T_LE_PROTOCOL_9A PROTOCOL_9A;
}moto_protocol_le_t;

typedef union _moto_protocol_le_lcp_t
{
	T_LE_PROTOCOL_90_LCP PROTOCOL_90_LCP;
	T_LE_PROTOCOL_91_LCP PROTOCOL_91_LCP;
	T_LE_PROTOCOL_92_LCP PROTOCOL_92_LCP;
	T_LE_PROTOCOL_93_LCP PROTOCOL_93_LCP;
	T_LE_PROTOCOL_94 PROTOCOL_94_LCP;
	T_LE_PROTOCOL_95 PROTOCOL_95_LCP;
	T_LE_PROTOCOL_96_LCP PROTOCOL_96_LCP;
	T_LE_PROTOCOL_97_LCP PROTOCOL_97_LCP;
	T_LE_PROTOCOL_98_LCP PROTOCOL_98_LCP;
	T_LE_PROTOCOL_99_LCP PROTOCOL_99_LCP;
	T_LE_PROTOCOL_9A_LCP PROTOCOL_9A_LCP;
}moto_protocol_le_lcp_t;

typedef union _moto_protocol_wl_t
{
	T_WL_PROTOCOL_01 PROTOCOL_01;
	T_WL_PROTOCOL_02 PROTOCOL_02;
	T_WL_PROTOCOL_03 PROTOCOL_03;
	T_WL_PROTOCOL_11 PROTOCOL_11;
	T_WL_PROTOCOL_12 PROTOCOL_12;
	T_WL_PROTOCOL_13 PROTOCOL_13;
	T_WL_PROTOCOL_16 PROTOCOL_16;
	T_WL_PROTOCOL_18 PROTOCOL_18;
	T_WL_PROTOCOL_19 PROTOCOL_19;
	T_WL_PROTOCOL_20 PROTOCOL_20;
	T_WL_PROTOCOL_21 PROTOCOL_21;
}moto_protocol_wl_t;

typedef  union _T_XNL
{
	T_XNL_DEVICE_MASTER_QUERY  DEVICE_MASTER_QUERY;
	T_XNL_DEVICE_MASTER_STATUS_BRDCST  DEVICE_MASTER_STATUS_BRDCST;
	T_XNL_DEVICE_AUTH_KEY_REQUEST DEVICE_AUTH_KEY_REQUEST;
	T_XNL_DEVICE_AUTH_KEY_REPLY   DEVICE_AUTH_KEY_REPLY;
	T_XNL_DEVICE_CONN_REQUEST  DEVICE_CONN_REQUEST;
	T_XNL_DEVICE_CONN_REPLY    DEVICE_CONN_REPLY;
	T_XNL_DEVICE_SYSMAP_REQUEST DEVICE_SYSMAP_REQUEST;
	T_XNL_DEVICE_SYSMAP_BRDCST DEVICE_SYSMAP_BRDCST;
	T_XNL_DATA_MSG             XNL_DATA_MSG;
	T_XNL_DATA_MSG_ACK        XNL_DATA_MSG_ACK;
}T_XNL;
typedef struct _moto_protocol_xcmp_xnl_t
{
	unsigned char Opcode;
	unsigned long PeerId;
	unsigned short lenth;
	T_XNL          Payload;
}moto_protocol_xcmp_xnl_t;
typedef struct _T_P2P_CALL
{
	P2P_CALL_HEADER    CallHeader;
	AMBE_VOICE_ENCODED_FRAMES AmbeVoiceEncodedFrames;
	unsigned char CallType;
}T_P2P_CALL;
typedef union _moto_protocol_p2p_t
{
	T_P2P_CALL P2PCall;
	char All[28];
}moto_protocol_p2p_t;

typedef union _moto_protocol_t
{
	moto_protocol_le_t le;
	moto_protocol_le_lcp_t le_lcp;
	moto_protocol_wl_t wl;
	moto_protocol_xcmp_xnl_t xcmp;
	moto_protocol_p2p_t p2p;
}moto_protocol_t;

typedef struct _send_data_t
{
	moto_protocol_t protocol;
	char net_data[SIZE_SEND_BUFF];
	unsigned int net_lenth;
	//pXQTTNet dst;
	unsigned long timeout_send;//重发时间 0表示无意义
	unsigned short timeout_try;//重发次数 0表示不再发送并做相应的处理,当为0xffff时表示一直重发
	unsigned long timing;//定时发送时间 0表示无意义
	void* send_to;//udp 发送的目标
	void* pFrom;//从哪个Peer发出的
}send_data_t;

typedef struct _recive_data_t
{
	moto_protocol_t protocol;
}recive_data_t;

typedef union _work_item_data_t
{
	recive_data_t recive_data;
	send_data_t send_data;
}work_item_data_t;

typedef enum _work_item_type_enum
{
	Recive,
	Send,
	ReciveOrSend
}work_item_type_enum;

typedef struct _work_item_t
{
	work_item_type_enum type;
	work_item_data_t data;
}work_item_t;
#endif