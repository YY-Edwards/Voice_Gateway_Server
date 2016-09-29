#ifndef IPSCPEER_H
#define IPSCPEER_H


#include "HMAC_SHA1.h"

enum SlotConfiguration
{
	None = 0,
	Slot1 = 1,
	Slot2 = 2,
	Both = 3
};

const DWORD FIREWALLOPENTIMER_DEFAULT = 6000;
const SlotConfiguration SLOTCONFIGURATION_DEFAULT = None;

enum _PEER_STATUS
{
	WAIT_REMOTE_REQUEST = 0,	// wait for remote send LE_PEER_REGISTRATION_REQUEST to local
	REMOTE_TO_LOCAL_CONNECTED,  // already sent LE_PEER_REGISTRATION_RESPONSE to remote
	WAIT_SEND_REGISTRATION_REQUEST_TO_REMOTE,
	WAIT_REMOTE_RESPONSE,       // wait for LE_PEER_REGISTRATION_RESPONSE from remote
	LOCAL_TO_REMOTE_CONNECTED,  // received LE_PEER_REGISTRATION_RESPONSE from remote
};

enum _IPSC_PEER_STATUS
{
	WAIT_LE_PEER_REGISTRATION_RESPONSE = 0,
	WAIT_LE_PEER_KEEPALIVE_REQUEST,
	WAIT_LE_PEER_KEEPALIVE_RESPONSE
};

class CWLNet;

extern CTool g_tool;

class CIPSCPeer
{
public:
	CIPSCPeer(CWLNet* pLELayer, WCHAR* IP_Address, WCHAR* Port);
	//add code by chenhaidong
	CIPSCPeer(CWLNet* pLELayer, u_long IP_Address, u_short Port);
	CIPSCPeer(CWLNet* pLELayer, sockaddr_in* thesockaddr);
	virtual ~CIPSCPeer();
	//void SetSlotConfiguration(SlotConfiguration theConfiguration);
	//SlotConfiguration GetSlotConfiguration(void);
	//void SetFirewallOpenTimer(int theResetTime);
	//int  GetFirewallOpenTimer();
	void SetPeerID(u_long PeerNetID);
	//void SetPeerMode(char PeerMode);
	//void SetPeerServices(u_short PeerServices);
	//PSOCKADDR GetpPeerAddress(void);

	//获取peer的地址以sockaddr_in方式
	sockaddr_in GetPeerAddressOfSockaddrin(void);

	u_long GetAddress();
	u_long GetPeerID();

	BOOL HandlePacket(DWORD handleCode, void* pParameter, u_long masterIp, u_short masterPort, BOOL isNeedResponse = TRUE, BOOL isCallBack = FALSE);

	//void HandlePeerTimeout();
	void setLogPtr(PLogReport log_handel);

	void printInfo();

	_SlotNumber getUseSlot();

	void destroy();

protected:
	void Init(BOOL isMasterPeer = FALSE);

	//int Build_LE_PEER_REGISTRATION_REQUEST();
	DWORD Build_LE_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_94* pData);

	//int Build_LE_PEER_REGISTRATION_RESPONSE();

	DWORD Build_LE_PEER_REGISTRATION_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_95* pData);

	//int Build_LE_PEER_KEEP_ALIVE_RESPONSE();
	DWORD Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99* pData);
	DWORD Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99_LCP* pData);

	//int Build_LE_PEER_KEEP_ALIVE_REQUEST();
	DWORD Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98* pData);
	DWORD Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98_LCP* pData);

	//int Build_WL_REGISTRATION_STATUS(DWORD pudID, WORD SLOT1RID, WORD SLOT2RID);

	/*
	* Build a WL registration packet
	*@param pPacket [in] buffer for save the packet data
	*@param registrationID registration id, used by the third party application to track status of multiple
	*                      commands issued to the same Repeater Peer
	*@return packet data length
	*/
	//DWORD BuildWLRegistrationRequestPacket(CHAR* pPacket, DWORD peerId, DWORD pudId);


	//DWORD Build_WL_REGISTRATION_REQUEST(CHAR* pPacket, DWORD peerId, DWORD pudId, char slot);

	DWORD Build_WL_REGISTRATION_REQUEST(CHAR* pPacket, T_WL_PROTOCOL_01* pData);

	//DWORD Build_WL_VC_CHNL_CTRL_REQUEST(CHAR* pPacket, DWORD peerId, DWORD pudId, char slot);

	DWORD Build_WL_VC_CHNL_CTRL_REQUEST(CHAR* pPacket, T_WL_PROTOCOL_13* pData);

	/*
	* Send the packet to peer
	*/
	int SendToPeer(const SOCKADDR_IN* pAddr);

private:
	_SlotNumber m_useSlot;
	bool m_Remote3rdParty;
	void sendLogToWindow();
	PLogReport m_report;
	char m_reportMsg[512];
	u_long	m_ulPeerID; //Stored in network byte order.
	char    m_ucPeerMode;
	u_short m_uPeerServices;
	DWORD		m_FirewallOpenTimerReset;
	SlotConfiguration m_SlotConfiguration;
	struct sockaddr_in m_PeerAddr;   //This is the address peers.

	//{{{ Peer Status, include Local->Remote and Remote->Local status
	int		m_LocalToRemoteState;
	int		m_RemoteToLocalState;
	//}}}

	CWLNet*							m_pWLNet;

	WSAOVERLAPPED					m_TxOverlapped;     //This is used by Worker Thread Tx.
	WSABUF							m_SendControlBuffer;
	DWORD							m_dwByteSent;
	DWORD							m_dwTxFlags;
	char							m_controlBuffer[1000];

	/* used by the third party application to track status of multiple  */
	/* commands issued to the same Repeater Peer						*/
	WORD							m_wRegistrationId;

	DWORD							m_dwPeerKeepAliverTimer;
	DWORD							m_dwPeerState;
	DWORD m_dwRecvPeerKeepAliverTimer;
public:
	//u_long GetPort();
	bool getRemote3rdParty();
	void setRemote3rdParty(bool value);
private:
	bool m_bKillTimer;
	DWORD m_startTickCount;
	DWORD m_peerStatus;
	UINT m_timerIdPeerStatusCheck;
	//WORD m_statusWaitPeerRegistrationResponse;
	//WORD m_statusWaitPeerAliveRequest;
	//WORD m_statusWaitPeerAliveResponse;
	//DWORD m_timerIdWaitPeerRegistrationResponse;//0x95定时器
	//DWORD m_timerIdWaitPeerAliveRequest;//0x98定时器
	//DWORD m_timerIdWaitPeerAliveResponse;//0x99定时器
	void getWirelineAuthentication(char* pPacket, DWORD &size);
	//void Timerwait_LE_PEER_REGISTRATION_RESPONSE_Proc();
	//void Timerwait_LE_PEER_KEEP_ALIVE_REQUEST_Proc();
	//void Timerwait_LE_PEER_KEEP_ALIVE_RESPONSE_Proc();
	void PeerStatusCheckProc();

	//static void PASCAL Timerwait_LE_PEER_REGISTRATION_RESPONSE(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
	//static void PASCAL Timerwait_LE_PEER_KEEP_ALIVE_REQUEST(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
	//static void PASCAL Timerwait_LE_PEER_KEEP_ALIVE_RESPONSE(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
	static void PASCAL PeerStatusCheck(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
	/************************************************************************/
	/* 获取请求通话结果相关信息
	/************************************************************************/
	void getCallRequestRltInfo(DECLINE_REASON_CODE_INFO &declineReasonCodeInfo);
};

#endif