#ifndef MOTODEFINE_H
#define MOTODEFINE_H
const char LE_MASTER_PEER_REGISTRATION_REQUEST = (const char)0x90;
const char LE_MASTER_PEER_REGISTRATION_RESPONSE = (const char)0x91;
const char LE_NOTIFICATION_MAP_REQUEST = (const char)0x92;
const char LE_NOTIFICATION_MAP_BROADCAST = (const char)0x93;
const char LE_PEER_REGISTRATION_REQUEST = (const char)0x94;
const char LE_PEER_REGISTRATION_RESPONSE = (const char)0x95;
const char LE_MASTER_KEEP_ALIVE_REQUEST = (const char)0x96;
const char LE_MASTER_KEEP_ALIVE_RESPONSE = (const char)0x97;
const char LE_PEER_KEEP_ALIVE_REQUEST = (const char)0x98;
const char LE_PEER_KEEP_ALIVE_RESPONSE = (const char)0x99;
const char LE_DEREGISTRATION_REQUEST = (const char)0x9A;
const int  LE_DEREGISTRATION_REQUEST_L = 5;
const char LE_DEREGISTRATION_RESPONSE = (const char)0x9B;
/*p2p call*/
const char P2P_GRP_VOICE_CALL = (const char)0x80;
const char P2P_PVT_VOICE_CALL = (const char)0x81;
const char P2P_GRP_DATA_CALL = (const char)0x83;
const char P2P_PVT_DATA_CALL = (const char)0x84;

const unsigned __int8 DATA_TYPE_VOICE_HEADER = (const unsigned __int8)0x01;    // the Voice Header is used to initiate a voice call
const unsigned __int8 DATA_TYPE_VOICE_TERMINATOR = (const unsigned __int8)0x02; //the Voice Terminator is used to terminate a voice call
const unsigned __int8 DATA_TYPE_VOICE = (const unsigned __int8)0x0A;  //voice data


/*LE_XCMP_XNL*/
const char  LE_XNL = (const char)0x70;
const short LE_XNL_MASTER_STATUS_BRDCST = 0x0002;
const short LE_XNL_DEVICE_MASTER_QUERY = 0x0003;
const short LE_XNL_DEVICE_AUTH_KEY_REQUEST = 0x0004;
const short LE_XNL_DEVICE_AUTH_KEY_REPLY = 0x0005;
const short LE_XNL_DEVICE_CONN_REQUEST = 0x0006;
const short LE_XNL_DEVICE_CONN_REPLY = 0x0007;
const short LE_XNL_DEVICE_SYSMAP_REQUEST = 0x0008;
const short LE_XNL_DEVICE_SYSMAP_BRDCST = 0x0009;
const short LE_XNL_DATA_MSG = 0x000b;
const short LE_XNL_DATA_MSG_ACK = 0x000c;
//const short LE_XNL_XCMP_DEVICE_INIT = 0xb400;
//const long  LE_XNL_XCMP_VERSION = 0x00000002;
//const char  LE_XNL_XCMP_DEVICE_INIT_TYPE = 0x00;
const unsigned short LE_XNL_XCMP_RADIO_STATUS = 0x000e;
const unsigned short LE_XNL_XCMP_RADIO_STATUS_REPLY = 0x800e;
const unsigned short LE_XNL_XCMP_RADIO_STATUS_BRDCST = 0xb00e;
const unsigned char  LE_XNL_XCMP_RADIO_STATUS_CONDITION_SERIAL = 0x0b;

const char  LE_XNL_PROTOCOL_ID = 0x00;
const char  LE_XNL_FLAGS = 0x00;
const char  LE_XNL_DEVICE_TYPE = 0x0a;
const char  LE_XNL_AUTHENTICATION_INDEX = 0x01;
const char  LE_XNL_XCMP_CONNECT_SUCCESS = 0x01;
const char  LE_XNL_XCMP_READ_SERIAL_SUCCESS = (const char)0x00;

#define P2P_IPSC_CURRENTLPVERSION 0x0405
#define P2P_IPSC_OLDESTPVERSION 0x0405
#define P2P_IPSC_MODE 0x6a
#define P2P_IPSC_SERVICES 0x000020a0//0x00002000

#define P2P_CPC_CURRENTLPVERSION 0x0805
#define P2P_CPC_OLDESTPVERSION 0x0805
#define P2P_CPC_MODE 0x65
#define P2P_CPC_SERVICES 0x00002fa4      //0x00002500 

#define IPSC_CURRENTLPVERSION 0x0405
#define IPSC_OLDESTPVERSION 0x0405
#define IPSC_MODE 0x6a
#define IPSC_SERVICES 0x000020a0//0x00002000

#define CPC_CURRENTLPVERSION 0x0805
#define CPC_OLDESTPVERSION 0x0805
#define CPC_MODE 0x65
#define CPC_SERVICES 0x000020a0      //0x00002500 

#define LCP_CURRENTLPVERSION 0x1005
#define LCP_OLDESTPVERSION 0x1005
#define LCP_MODE 0x019A
#define LCP_SERVICES 0x00002060  //0x00000000

#define Wireline_Protocol_Version 0x04
#define REGISTRATION_PDU_ID 2
#define REGISTRATION_ID 2
#define NUMBER_REGIS_ENTRIES 2

static const  char WL_PROTOCOL = (const char)0xb2;
static const  char MONITOR_CHANNEL_STATUS = (const char)0x80;
static const  char NOT_MONITOR_CHANNEL_STATUS = 0x00;
static const  char REGISTERED_VOICE_SERVICE = (const char)0x80;

static const  char WL_REGISTRATION_REQUEST = 0x01;
static const  char WL_REGISTRATION_STATUS = 0x02;
static const  char WL_REGISTRATION_GENERAL_OPS = 0x03;
static const  char WL_CHNL_STATUS = 0x11;
static const  char WL_CHNL_STATUS_QUERY = 0x12;
static const  char WL_VC_CHNL_CTRL_REQUEST = 0x13;
static const  char WL_VC_CHNL_CTRL_STATUS = 0x16;
static const  char WL_VC_VOICE_START = 0x18;
static const  char WL_VC_VOICE_END_BURST = 0x19;
static const  char WL_VC_CALL_SESSION_STATUS = 0x20;
static const  char WL_VC_VOICE_BURST = 0x21;
static const  char WL_VC_PRIVACY_BURST = 0x22;

#define Call_Session_Call_Hang	0x0a
#define Call_Session_End	0x0b

#define LEADING_CHANNEL_ID 0x00 //Refer to CPS configuration.
/*0:System wide map;2:master peer pragramming map;4:Voter Map;7:Map Continuation Indicator《adk_link_establishment_spec_0101.pdf》-line1174,实际情况于此不符*/
#define System_Wide_Map_Type 0x01
#define Master_Peer_Programming_Map_Type 0x04
#define Voter_Map_Type 0x10
#define Map_Continuation_Indicator_Type 0x80

static const unsigned char Channel_Control_Request_Status_Received = 0x01;
static const unsigned char Channel_Control_Request_Status_Transmitting = 0x02;
static const unsigned char Channel_Control_Request_Status_Transmission_Successful = 0x03;
static const unsigned char Channel_Control_Request_Status_Grant = 0x04;
static const unsigned char Channel_Control_Request_Status_Declined = 0x05;
static const unsigned char Channel_Control_Request_Status_Interrupting = 0x06;

#define GROUP_CALL 0x4f//79
#define PRIVATE_CALL 0x50//80
#define ALL_CALL 0x53//83
#define CALL_ATTRIBUTES 0x00//Clear Call
static const unsigned char Access_Criteria_Polite_Access = 0x01;
#define CSBK_ARGUMENTS 0x0000000000000000	//in voice call csbk auguments should set 0
#define PREAMBLE_DURATION 0x00	//Preamble Duration. For voice call set it to 0.
#define ALL_CALL_ID 255

#define BURST_A 0x01
#define BURST_B 0x02
#define BURST_C 0x03
#define BURST_D 0x04
#define BURST_E 0x05
#define BURST_F 0x06
#define BURST_T 0x07
#define BURST_PH 0x08

#define ALGORITHMID 0x00 //Invalid algorithm ID when CRC error is detected.
#define VALUE_IV 0x00000000//Invalid IV when CRC error is detected.
#define KEY_ID 0x00 //Invalid Key ID when CRC error is detected.
#define VALUE_MFID 0x00 //Standard Feature
#define VALUE_RSSI 0x01A8
#define BURST_RTP_HEADER 0x80
#define BURST_START_RTP_MPT 0xDD
#define BURST_NORMAL_RTP_MPT 0x5D
#define BURST_END_RTP_MPT 0x5E
#define BURST_RTP_SSRC 0x00000000 //This field is not used system and should be set to 0.
#define BURST_SERVICEOPTION 0x00 //Non-emergency service Non-broadcast service
#endif