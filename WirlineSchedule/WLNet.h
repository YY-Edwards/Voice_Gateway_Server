#ifndef WLNET_H
#define WLNET_H

#include <list>
#include "IPSCPeer.h"
#include "SerialDongle.h"
#include "Sound.h"
#include "WLRecord.h"
#include "Manager.h"
#include "WLRecordFile.h"

extern CTool g_tool;
extern CSerialDongle* g_pDongle;
extern CSound* g_pSound;
extern BOOL g_dongleIsUsing;
extern BOOL g_bPTT;

#define     SYS_SECTION   __TEXT("sys")
#define     CONNECT_RESULT  __TEXT("connect")
#define     PEER_COUNT      __TEXT("peercount")
#define     LICENSE_RESULT  __TEXT("license")
#define     PEER_SECTION    __TEXT("peer_")
#define     PEER_ADDR       __TEXT("ip")
#define     PEER_ID         __TEXT("id")
#define     PEER_PORT       __TEXT("port")

//#define CPC_PRIVATE_CALL		3
//#define CPC_GROUP_CALL		4

static const DWORD MAXEVENTS = 3;
static const DWORD TIMEOUT = WAIT_TIMEOUT;				//Timeout Event
static const DWORD FIRSTPASS = MAXEVENTS + 1;			//First pass Event
static const DWORD IDLE = MAXEVENTS + 2;				//Idle Event
static const DWORD ILLEGAL = MAXEVENTS + 3;				//Illegal Event
static const DWORD NOACTIVITYTIMEOUT = 1000;
static const DWORD RxIndex = WAIT_OBJECT_0 + 0;
static const DWORD TxIndex = WAIT_OBJECT_0 + 1;
static const DWORD TickIndex = WAIT_OBJECT_0 + 2;

static const BYTE WL_PROTOCOL = 0xb2;
static const BYTE MONITOR_CHANNEL_STATUS = 0x80;
static const BYTE NOT_MONITOR_CHANNEL_STATUS = 0x00;
static const BYTE REGISTERED_VOICE_SERVICE = 0x80;
static const BYTE WL_REGISTRATION_REQUEST = 0x01;
static const BYTE WL_REGISTRATION_STATUS = 0x02;

static const BYTE WL_REGISTRATION_GENERAL_OPS = 0x03;
static const BYTE WL_CHNL_STATUS = 0x11;
static const BYTE WL_CHNL_STATUS_QUERY = 0x12;
static const BYTE WL_VC_VOICE_START = 0x18;
static const BYTE WL_VC_VOICE_END_BURST = 0x19;
static const BYTE WL_VC_CALL_SESSION_STATUS = 0x20;
static const BYTE WL_VC_VOICE_BURST = 0x21;
static const BYTE WL_VC_PRIVACY_BURST = 0x22;
static const BYTE WL_VC_CHNL_CTRL_REQUEST = 0x13;
static const BYTE WL_VC_CHNL_CTRL_STATUS = 0x16;

static const BYTE Access_Criteria_Polite_Access = 0x01;
static const BYTE Access_Criteria_Transmit_Interrupt = 0x02;
static const BYTE Access_Criteria_Impolite_Access = 0x03;

static const BYTE Channel_Control_Request_Status_Received = 0x01;
static const BYTE Channel_Control_Request_Status_Transmitting = 0x02;
static const BYTE Channel_Control_Request_Status_Transmission_Successful = 0x03;
static const BYTE Channel_Control_Request_Status_Grant = 0x04;
static const BYTE Channel_Control_Request_Status_Declined = 0x05;
static const BYTE Channel_Control_Request_Status_Interrupting = 0x06;

const char LE_MASTER_PEER_REGISTRATION_REQUEST = (const char)0x90;
const int LE_MASTER_PEER_REGISTRATION_REQUEST_L = 14;
const int LE_MASTER_PEER_REGISTRATION_REQUEST_LCP_L = 16;
const char LE_MASTER_PEER_REGISTRATION_RESPONSE = (const char)0x91;
const int LE_MASTER_PEER_REGISTRATION_RESPONSE_L = 16;
const int LE_MASTER_PEER_REGISTRATION_RESPONSE_LCP_L = 18;
const char LE_NOTIFICATION_MAP_REQUEST = (const char)0x92;
const int LE_NOTIFICATION_MAP_REQUEST_L = 5;
const int LE_NOTIFICATION_MAP_REQUEST_LCP_L = 10;
const char LE_NOTIFICATION_MAP_BROADCAST = (const char)0x93;
const int LE_NOTIFICATION_MAP_BROADCAST_ENTRY = 11;
const char LE_PEER_REGISTRATION_REQUEST = (const char)0x94;

const char LE_PEER_REGISTRATION_RESPONSE = (const char)0x95;

const char LE_MASTER_KEEP_ALIVE_REQUEST = (const char)0x96;
const int LE_MASTER_KEEP_ALIVE_REQUEST_L = 14;
const int LE_MASTER_KEEP_ALIVE_REQUEST_LCP_L = 16;
const char LE_MASTER_KEEP_ALIVE_RESPONSE = (const char)0x97;
const char LE_PEER_KEEP_ALIVE_REQUEST = (const char)0x98;

const char LE_PEER_KEEP_ALIVE_RESPONSE = (const char)0x99;

const char LE_DEREGISTRATION_REQUEST = (const char)0x9A;
const int LE_DEREGISTRATION_REQUEST_L = 5;
const char LE_DEREGISTRATION_RESPONSE = (const char)0x9B;
const char IPSC_GRP_VOICE_CALL = (const char)0x80;
const char IPSC_PVT_VOICE_CALL = (const char)0x81;
const char IPSC_GRP_DATA_CALL = (const char)0x83;
const char IPSC_PVT_DATA_CALL = (const char)0x84;
const char LE_CALL_CONTROL_INTERFACE = (const char)0xb2;

//IPSC
/************************************************************************/
/*devspec_link_establishment_0103.pdf line 1305
/************************************************************************/
const char IPSC_PEERMODE_DEFAULT = (const char)0x6a; //IPSC only
const char IPSC_PEERSERVICES_DEFAULT_1 = (const char)0x00;//本身为Reserve位,当且仅当为0x04才能收到0x80的语音 其余的不可(0xb2正常)
const char IPSC_PEERSERVICES_DEFAULT_2 = (const char)0x20;//为第三方应用
const char IPSC_PEERSERVICES_DEFAULT_3 = (const char)0x00;
const char IPSC_PEERSERVICES_DEFAULT_4 = (const char)0x00;
const char IPSC_CURRENTLPVERSION_DEFAULT_1 = (const char)0x05;//当前版本为2.3
const char IPSC_CURRENTLPVERSION_DEFAULT_2 = (const char)0x04;
const char IPSC_OLDESTLPVERSION_DEFAULT_1 = (const char)0x05;//仅支持2.3
const char IPSC_OLDESTLPVERSION_DEFAULT_2 = (const char)0x04;

#define IPSC_CURRENTLPVERSION 0x0405
#define IPSC_OLDESTPVERSION 0x0405
#define IPSC_MODE 0x6a
#define IPSC_SERVICES 0x00002000

//CPC
const char CPC_PEERMODE_DEFAULT = (const char)0x65; //Enabled, Digital Mode, IPSC CPC Both
const char CPC_PEERSERVICES_DEFAULT_1 = (const char)0x00;
const char CPC_PEERSERVICES_DEFAULT_2 = (const char)0x25;
const char CPC_PEERSERVICES_DEFAULT_3 = (const char)0x00;
const char CPC_PEERSERVICES_DEFAULT_4 = (const char)0x00;

const char CPC_CURRENTVERSION_DEFAULT_1 = (const char)0x05;
const char CPC_CURRENTVERSION_DEFAULT_2 = (const char)0x08;
const char CPC_OLDESTVERSION_DEFAULT_1 = (const char)0x05;
const char CPC_OLDESTVERSION_DEFAULT_2 = (const char)0x08;

#define CPC_CURRENTLPVERSION 0x0805
#define CPC_OLDESTPVERSION 0x0805
#define CPC_MODE 0x65
#define CPC_SERVICES 0x00002500
//LCP
const char LCP_PEERMODE_DEFAULT_1 = (const char)0x00;
const char LCP_PEERMODE_DEFAULT_2 = (const char)0x9A;

const char LCP_PEERSERVICES_DEFAULT_1 = (const char)0x00;
const char LCP_PEERSERVICES_DEFAULT_2 = (const char)0x00;
const char LCP_PEERSERVICES_DEFAULT_3 = (const char)0x00;
const char LCP_PEERSERVICES_DEFAULT_4 = (const char)0x00;

const char LCP_CURRENTVERSION_DEFAULT_1 = (const char)0x05;
const char LCP_CURRENTVERSION_DEFAULT_2 = (const char)0x10;
const char LCP_OLDESTVERSION_DEFAULT_1 = (const char)0x05;
const char LCP_OLDESTVERSION_DEFAULT_2 = (const char)0x10;

#define LCP_CURRENTLPVERSION 0x1005
#define LCP_OLDESTPVERSION 0x1005
#define LCP_MODE 0x009A
#define LCP_SERVICES 0x00000000
//
//
//const		   char CURRENTLCP_VERSION_DEFAULT2 = 0x03;
//const		   char LCPPEERSERVICES_DEFAULT3 = 0xb9;

const int MAXPEERSSUPPORTED = 15;
const int PEERITEMLENGTH = 11;

// Call Define
const int  MAXCALLPARAMS = 28;
const unsigned __int8 DATA_TYPE_PI_HEADER = (const unsigned __int8)0x00;
const unsigned __int8 DATA_TYPE_VOICE_HEADER = (const unsigned __int8)0x01;
const unsigned __int8 DATA_TYPE_VOICE_TERMINATOR = (const unsigned __int8)0x02;
const unsigned __int8 DATA_TYPE_CSBK = (const unsigned __int8)0x03;
const unsigned __int8 DATA_TYPE_DATA_HEADER = (const unsigned __int8)0x06;
const unsigned __int8 DATA_TYPE_UNCONFIRM_DATA_CONT = (const unsigned __int8)0x07;
const unsigned __int8 DATA_TYPE_CONFIRM_DATA_CONT = (const unsigned __int8)0x08;
const unsigned __int8 DATA_TYPE_VOICE = (const unsigned __int8)0x0A;
const unsigned __int8 DATA_TYPE_SYNC_UNDETECT = (const unsigned __int8)0x13;

//////////////////////////////////////////////////////////////////////////
// Const define for XNL
//XNL Connect
const char LE_XNL = (const char)0x70;
const char LE_XNL_DEVICE_MASTER_QUERY_1 = (const char)0x00;
const char LE_XNL_DEVICE_MASTER_QUERY_2 = (const char)0x03;
const int LE_XNL_DEVICE_MASTER_QUERY_L = 19;
const char LE_XNL_MASTER_STATUS_BRDCST_1 = (const char)0x00;
const char LE_XNL_MASTER_STATUS_BRDCST_2 = (const char)0x02;
const int LE_XNL_MASTER_STATUS_BRDCST_L = 26;
const char LE_XNL_DEVICE_AUTH_KEY_REQUEST_1 = (const char)0x00;
const char LE_XNL_DEVICE_AUTH_KEY_REQUEST_2 = (const char)0x04;
const int LE_XNL_DEVICE_AUTH_KEY_REQUEST_L = 19;
const char LE_XNL_DEVICE_AUTH_KEY_REPLY_1 = (const char)0x00;
const char LE_XNL_DEVICE_AUTH_KEY_REPLY_2 = (const char)0x05;
const int LE_XNL_DEVICE_AUTH_KEY_REPLY_L = 29;
const char LE_XNL_DEVICE_CONNECT_REQUEST_1 = (const char)0x00;
const char LE_XNL_DEVICE_CONNECT_REQUEST_2 = (const char)0x06;
const int LE_XNL_DEVICE_CONNECT_REQUEST_L = 31;
const char LE_XNL_DEVICE_CONNECT_REPLY_1 = (const char)0x00;
const char LE_XNL_DEVICE_CONNECT_REPLY_2 = (const char)0x07;
const int LE_XNL_DEVICE_CONNECT_REPLY_L = 33;
const char LE_XNL_DEVICE_SYSMAP_BRDCST_1 = (const char)0x00;
const char LE_XNL_DEVICE_SYSMAP_BRDCST_2 = (const char)0x09;
const char LE_XNL_DATA_MSG_1 = (const char)0x00;
const char LE_XNL_DATA_MSG_2 = (const char)0x0B;
const char LE_XNL_DATA_MSG_ACK_1 = (const char)0x00;
const char LE_XNL_DATA_MSG_ACK_2 = (const char)0x0C;
const int LE_XNL_DATA_MSG_ACK_L = 19;
const char LE_XNL_XCMP_DEVICE_INIT_1 = (const char)0xB4;
const char LE_XNL_XCMP_DEVICE_INIT_2 = (const char)0x00;
const int LE_XNL_XCMP_DEVICE_INIT_L = 30;
const char LE_XNL_XCMP_READ_SERIAL_1 = (const char)0x00;
const char LE_XNL_XCMP_READ_SERIAL_2 = (const char)0x0E;
const int LE_XNL_XCMP_READ_SERIAL_L = 22;
//
//
//
//
//enum _WLNetLogEvent
//{
//	StartNetWork = 0,
//	OpenSocket,
//	OpenSocketFail,
//	BindSocketFail,
//	CreateEventFail,
//	StartListener,
//	SendMasterRegistration,
//	NodeOnline,
//	ConnecttedToMaster,
//	OFFline,
//};

enum _EventIndex
{
	RxEvent = 0,
	TxEvent,
	TickEvent,
};

enum _AddressType{
	IndividualCall = 0x01,
	GroupCall,
	AllIndividualCall,
	AllTalkGroupCall,
	AllWideTalkGroupCall,
	AllLocalTalkGroupCall,
};

#define LCP_PRIVATE_CALL	0x50
#define LCP_GROUP_CALL      0x4f;

#pragma pack(push, 1)
typedef struct _tagRegistrationEntry
{
	BYTE  AddressType;
	DWORD AddressRangeStart;
	DWORD AddressRangeEnd;
	BYTE  VoiceAttributes;
	BYTE  Reserved1;
	BYTE  REserved2;
}RegistrationEntry;
#pragma pack(pop)
typedef union
{
	struct
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
	}fld;
	char                 All[28];
}tCallParams;

typedef std::list<CRecordFile*> VoiceRecords;

class CWLNet
{
	friend class CIPSCPeer;

public:
	CWLNet(CMySQL *pDb,CManager *pManager);
	~CWLNet();

	int initCallParam();
	int SendFile(unsigned int length, char* pData);
	//short Build_WL_VC_VOICE_BURST(char* pAmbeData, long& lengthAmbe, bool isfirst = false);
	short Build_WL_VC_VOICE_BURST(CHAR* pPacket, T_WL_PROTOCOL_21* pData, bool bFillAmbe);
	void clearSendVoices();
	WORD GetCallStatus();
	void SetCallStatus(WORD value);
	int callBack();
	int newCall();
	void Build_T_WL_PROTOCOL_19(T_WL_PROTOCOL_19& networkData);
	void Build_T_WL_PROTOCOL_21(T_WL_PROTOCOL_21& networkData, bool bStart);
	//short Build_WL_VC_VOICE_END_BURST();
	short Build_WL_VC_VOICE_END_BURST(CHAR* pPacket, T_WL_PROTOCOL_19* pData);

	/************************************************************************/
	/* 校正缓冲数据call id
	/************************************************************************/
	void CorrectingBuffer(DWORD callId);
	//bool getIsFirstBurstA();
	//void setIsFirstBurstA(bool value);
	void requestRecordEndEvent();
	void releaseRecordEndEvent();
	void waitRecordEnd();
	void setCurrentSendVoicePeer(CIPSCPeer* value);
	void clearPeers();
	int checkDefaultGroup();
	int setPlayCallOfCare(unsigned char calltype, unsigned long srcId, unsigned long targetId);
	int thereIsCallOfCare(CRecordFile *pCallRecord);
	/*告知界面当前存在需要关注的通话正在进行以及状态*/
	int Send_CARE_CALL_STATUS(unsigned char callType, unsigned long srcId, unsigned long tgtId, int status);

protected:
	/*
	* Socket work thread
	*/
	static DWORD WINAPI NetThreadProc(LPVOID pVoid);
	DWORD NetThread();
	//void InitControlBuffer(DWORD dwSelfPeerId);


	//void Build_LE_MASTER_PEER_REGISTRATION_REQUEST();
	DWORD Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90* pData);
	DWORD Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90_LCP* pData);
	void Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91& networkData);
	void Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91_LCP& networkData);

	DWORD Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92* pData);
	DWORD Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92_LCP* pData);
	void Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93& networkData);
	void Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93_LCP& networkData);

	void Unpack_LE_PEER_REGISTRATION_REQUEST(char* pData, T_LE_PROTOCOL_94& networkData);
	void Unpack_LE_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_95& networkData);

	DWORD Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96* pData);
	DWORD Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96_LCP* pData);

	void Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98& networkData);
	void Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98_LCP& networkData);
	void Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99& networkData);
	void Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99_LCP& networkData);

	void Unpack_WL_REGISTRATION_GENERAL_OPS(char* pData, T_WL_PROTOCOL_03& networkData);
	void Unpack_WL_REGISTRATION_STATUS(char* pData, T_WL_PROTOCOL_02& networkData);
	void Unpack_WL_CHNL_STATUS(char* pData, T_WL_PROTOCOL_11& networkData);
	void Unpack_WL_CHNL_STATUS_QUERY(char* pData, T_WL_PROTOCOL_12& networkData);
	//void Unpack_WL_VC_CHNL_CTRL_REQUEST(char* pData, T_WL_PROTOCOL_13 networkData);
	void Unpack_WL_VC_CHNL_CTRL_STATUS(char* pData, T_WL_PROTOCOL_16& networkData);
	void Unpack_WL_VC_VOICE_START(char* pData, T_WL_PROTOCOL_18& networkData);
	void Unpack_WL_VC_VOICE_END_BURST(char* pData, T_WL_PROTOCOL_19& networkData);
	void Unpack_WL_VC_CALL_SESSION_STATUS(char* pData, T_WL_PROTOCOL_20& networkData);
	void Unpack_WL_VC_VOICE_BURST(char* pData, T_WL_PROTOCOL_21& networkData);


public:
	/*
	* Start Wirelan gateway network
	*
	* @param DWORD dwMasterIp: master ip address, network byte order
	* @param WORD wMasterPort: master port, host byte order
	* @param DWORD dwLocalIp: local ip used, network byte order
	* @param DWORD dwSelfPeerId: self peer id
	* @return TRUE if success, or FALSE
	*/
	BOOL StartNet(DWORD dwMasterIp, WORD wMasterPort, DWORD dwLocalIp, DWORD dwSelfPeerId, DWORD dwSelfRadioId, DWORD recType);

	/*
	* Stop wirelan gateway network
	*/
	void StopNet();

	//delete old code
	/*
	* Set event logger
	*/
	//void SetLogger(IWLNetLoger* pLogger);
	void SetLogPtr(PLogReport value);

	//Build voice pack of AMBE
	void NetTx(bool Start);

	//Returns int my peer ID in Host order.
	unsigned __int32 GetMyPeerID(void);

	//Returns int selected TG ID in Host order.
	//Simple atomic read.
	unsigned __int32 GetMyRadioID(void);

	//Returns int selected TG ID in Host order.
	//Simple atomic read.
	//unsigned __int32 GetSelectedTalkgroup(void);

	//int  GetTxSlot(void);

	void NetStuffTxVoice(unsigned char* pVoiceBytes);
	void FILL_AMBE_FRAME(char* pVoiceBytes, char* pSendVoice, int txSubCount);

	void requestNewCallEvent();
	void releaseNewCallEvent();

	void setSitePeer(CIPSCPeer* value);
private:
	DWORD m_dwChangeToCurrentTick;
	CManager* m_pManager;
	//HANDLE m_dongleIdleEvent;
	CMySQL *m_pDb;
	WLRecord *m_pEventLoger;
	//add code by chenhaidong
	//CallInfo m_currentCallInfo;
	CIPSCPeer* m_pSitePeer;
	CIPSCPeer* m_pCurrentSendVoicePeer;
	HANDLE m_endRecordEvent;
	WORD m_callStatus;
	//bool m_isRequestNewCall;
	//bool m_isFirstBurstA;
	char m_burstType;
	WORD m_SequenceNumber;
	DWORD m_Timestamp;
	char* m_pVoice;
	SendVoicePackage* m_pSendVoicePackage;
	int m_cuurentSendType;
	std::list<SendVoicePackage*> m_sendVoices;

	HANDLE m_wlInitNewCallEvent;

	unsigned char m_callSequenceNumber;

	//void FillVoiceHeaderTerminator(IPSCVoiceTemplate* pBuffer, IPSCBurstType burstType);

	//void FillBurstE(IPSCVoiceTemplate* pBuffer);

	// this function only works for burst F in the enhance privacy is off, otherwise a seperate
	// function shall be created for burst F
	//void FillBurstBCDF(IPSCVoiceTemplate* pBuffer, IPSCBurstType burstType);

	//void FillBurstA(IPSCVoiceTemplate* pBuffer);

	//void FillIPSCFormat(IPSCVoiceTemplate* pBuffer, IPSCBurstType burstType, bool lastBurst);


	CallRecord	  myCallRecord;  //to store the consistent call data for the burst generation

	//This function shall be called whenever a new call is initiated to initialize all the parameters
	//void InitialCallRecord();

	unsigned __int16 embeddedLCCheckSumGen(unsigned __int8 * inputPtr);

	//reorder_bits is for output, must allocate space before calling this function,e.g, 	unsigned __int16 reorder_bits[8];
	void FECEncoding(CallRecord * myCallRecordInput, unsigned __int16 *reorder_bits);

	unsigned __int16 hamming(unsigned __int16 inputRow);

	void Interleave(unsigned __int16 *reorder_bits, CallRecord * myCallRecordInput);

	int m_TxSubCount;    //0,1,2  20mS Frame
	IPSCBurstType m_TxBurstType;   //0,1,2,3,4,5           => A,B,C,D,E,F  [Superframe Phase]

	u_long m_masterAddress;
	WORD m_masterPort;

	void sendLogToWindow();
	char m_reportMsg[512];
	PLogReport m_report;
	/*
	* Issue a UDP read request
	*/
	int IssueReadRequest();
	/*
	* Send the packet to master
	*/
	int SendToLE(const SOCKADDR_IN* pAddr);
	/*
	* Network start routing, register to master
	*/
	void Net_RegisterLE(DWORD eventIndex);
	void Net_DeRegisterLE();
	void Net_WAITINGFOR_LE_DEREGISTRATION_TRANSMISSION(DWORD eventIndex);
	void Net_WAITFOR_LE_MASTER_PEER_REGISTRATION_TX(DWORD eventIndex);
	void Net_WAITFOR_LE_MASTER_PEER_REGISTRATION_RESPONSE(DWORD eveintIndex);
	void Net_MaintainKeepAlive();
	void Net_MaintainAlive(DWORD eventIndex);
	void Net_WAITFOR_MAP_REQUEST_TX(DWORD eventIndex);
	void Net_WAITFOR_LE_NOTIFICATION_MAP_BROADCAST(DWORD eventIndex);
	void ParseMapBroadcast(T_LE_PROTOCOL_93* p, T_LE_PROTOCOL_93_LCP* pLcp);
	//void AMBE2FrameToAMBE3000Frame(char* pFrame);

	/*
	* Build a WL registration packet
	*@param pPacket [in] buffer for save the packet data
	*@param registrationID registration id, used by the third party application to track status of multiple
	*                      commands issued to the same Repeater Peer
	*@return packet data length
	*/
	//DWORD BuildWLRegistrationRequestPacket(LPBYTE pPacket, DWORD peerId, DWORD pudId);

	/*
	* Find peer in list
	* @param peerId remote peer id
	* @return TRUE is found
	*/
	//BOOL FindPeer(u_long peerAddr);

	CIPSCPeer* GetPeer(u_long peerId);

	//void PeerTimeout();

	//void ProcessCall(DWORD dwCallType, BOOL isTimeCheckout = FALSE);

	void Process_WL_BURST_CALL(char wirelineOpCode,void  *pNetWork);

	BOOL WriteVoiceFrame(tCallParams& call, DWORD dwCallType, BOOL isCheckTimeout = FALSE);

	std::mutex m_mutextVoicRecords;//当前录音记录锁

	void requireVoiceReocrdsLock();
	VoiceRecords  m_voiceReocrds;
	void releaseVoiceReocrdsLock();

	void NetWorker_TxIfCall(void);

	void NetWorker_SendCallByWL(void);

	static void PASCAL OneMilliSecondProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);

	static void PASCAL HangTimerCallProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);

	void HangTimerCallCheck();
	void getWirelineAuthentication(char* pPacket, short &size);

private:
	int m_retryRequestCallCount;
	bool m_bIsSending;

	DWORD  m_BytesSent;

	WLStatus						m_WLStatus;
	BOOL							m_bExit;
	//DWORD							m_dwRecType;

	SOCKET							m_socket;
	WSAData							m_wsaData;
	SOCKADDR_IN						m_locaAddr;
	SOCKADDR_IN						m_masterAddr;

	WORD							m_wRegistrationId;
	DWORD							m_dwMasterKeepAliveTime;
	DWORD							m_dwMasterRegisterTimer;
	DWORD							m_dwMasterMapBroadcastTimer;

	// {{{ for receive a UDP packet async
	DWORD							m_dwByteRecevied;
	DWORD							m_dwRxFlags;
	int 							m_sizeAddress;
	SOCKADDR_IN						m_remoteAddr;
	WSAOVERLAPPED					m_RxOverlapped;     //This is used by Worker Thread Rx.
	WSABUF							m_CurrentRecvBuffer;
	char							m_RxBuffer[1024 * 10];
	DWORD                           m_RecvMap;
	//}}}

	//{{{ for send a udp packet async
	WSAOVERLAPPED					m_TxOverlapped;     //This is used by Worker Thread Tx.
	WSABUF							m_SendControlBuffer;
	DWORD							m_dwByteSent;
	DWORD							m_dwTxFlags;
	//}}}

	//{{{ Master peer information
	u_long							m_ulMasterPeerID;
	u_short							m_ucMasterMode;
	u_long							m_uMasterServices;
	//}}}

	//{{{ peer relative
	int								m_PeerCount;
	std::list<CIPSCPeer*>			m_pPeers;

	std::mutex m_readySendVoicesLock;

	//对待发送数据进行加锁
	void requireReadySendVoicesLock();
	//待发送的数据
	std::list<IPSCVoiceTemplate*> m_readySendVoices;
	//对待发送数据进行解锁
	void releaseReadySendVoicesLock();
	//当前正在组装的待发送的AMBE数据
	IPSCVoiceTemplate* m_pCurrentBuildSendvoice;

	WSABUF m_SendBurstBuffer;

	//}}}

	//DWORD m_masterPeerID;
	//unsigned __int32 m_dwMyPeerID;
	unsigned __int32 m_dwMyRadioID;
	//Host order. Note: Maybe someday this will be a list of scan groups.
	//unsigned __int32 m_dwMyRadioGroup;
	//int m_TxSlot;
	char m_ControlProto[30];
	//OpCode (placeholder)
	//PeerID[3]
	//PeerID[2]
	//PeerID[1]
	//PeerID[0]
	//Mode
	//Svc[3]
	//Svc[2]
	//Svc[1]
	//Svc[0]
	//acceptedLinkProtocolVersion[1]
	//acceptedLinkProtocolVersion[2]
	//oldestLinkProtocolVersion[1]
	//oldestLinkProtocolVersion[2]

	WSAEVENT						m_eventArray[MAXEVENTS];
	HANDLE							m_hWorkThread;

	//delete old code
	//IWLNetLoger*					m_pEventLoger;

	// For XNL
public:
	void calEnciphe();
	void Net_XNL_CONNECT(DWORD eventIndex);
	void Net_XNL_DEVICE_MASTER_QUERY_TX(DWORD eventIndex);
	void Net_WAITFOR_XNL_MASTER_STATUS_BROADCAST(DWORD eventIndex);
	void Net_WAITFOR_XNL_DEVICE_AUTH_KEY_REQUEST_TX(DWORD eventIndex);
	void Net_WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY(DWORD eventIndex);
	void Net_WAITFOR_XNL_DEVICE_CONNECT_REQUEST_TX(DWORD eventIndex);
	void Net_WAITFOR_XNL_DEVICE_CONNECT_REPLY(DWORD eventIndex);
	void Net_WAITFOR_XNL_DEVICE_SYSMAP_BROADCAST(DWORD eventIndex);
	void Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_1(DWORD eventIndex);
	void Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2_TX(DWORD eventIndex);
	void Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2(DWORD eventIndex);
	void Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3_TX(DWORD eventIndex);
	void Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3(DWORD eventIndex);
	void Net_XNL_XCMP_READ_SERIAL(DWORD eventIndex);
	void Net_WAITFOR_XNL_XCMP_READ_SERIAL_RESULT(DWORD eventIndex);
private:
	unsigned char masterAddr[2];
	unsigned char tempXNLAddr[2];
	unsigned char un_Auth[8];
	unsigned char en_Auth[8];
	unsigned char xnlAddr[2];
	unsigned char logicalAddr[2];
	unsigned char transationID[2];
	unsigned char flags;
	unsigned char m_MasterSerial[11];

	DWORD m_dwXnlMasterStatusBrdcstTimer;
	DWORD m_dwXnlDeviceAuthTimer;
	DWORD m_dwXnlConnectTimer;
	DWORD m_dwXnlDeviceSysmapBrdcstTimer;
	DWORD m_dwXnlDeviceInitTimer;
	DWORD m_dwXnlReadSerialTimer;
public:
	//delete old code
	//bool GetLic(unsigned char* Serial_Number);
private:
	DWORD m_dwRecvMasterKeepAliveTime;
	WCHAR m_strSettingFilePath[MAX_PATH];
	//CRecorderServer m_RecorderServer;
	WCHAR m_addr[32];
	WCHAR m_port[8];
	WCHAR m_peerID[128];
public:
	void WriteMapFile();
private:
	WCHAR m_NumberStr[8];

public:
	//delete old code
	//void GetLicenseFilePath(CString licenseFilePath);
private:
	//delete old code
	//CString m_strLicenseFilePath;
	CRecordFile* m_pPlayCall;
public:
	void RestartLE();
	DWORD GetIpList(void * iplist);
	bool FindLocalIP(WCHAR* strAddr);
private:
	bool isLicense;
public:
	//delete old code
	//void CloseServer();
	void setWlStatus(WLStatus value);
	WLStatus getWlStatus();
	/************************************************************************/
	/*验证此语音是否需要实时播放
	/************************************************************************/
	bool isTargetMeCall(unsigned int tagetId, unsigned char callType);
	CRecordFile* getCurrentPlayInfo();
	void setCurrentPlayInfo(CRecordFile *value);
};

#endif