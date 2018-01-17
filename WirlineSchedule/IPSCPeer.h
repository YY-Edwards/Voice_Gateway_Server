#ifndef IPSCPEER_H
#define IPSCPEER_H
//#include "NS/include/HMAC_SHA1.h"
#include "NS/include/NS.h"

//enum SlotConfiguration
//{
//	None = 0,
//	Slot1 = 1,
//	Slot2 = 2,
//	Both = 3
//};

//const DWORD FIREWALLOPENTIMER_DEFAULT = 6000;
//const SlotConfiguration SLOTCONFIGURATION_DEFAULT = None;

// enum _PEER_STATUS
// {
// 	WAIT_REMOTE_REQUEST = 0,	// wait for remote send LE_PEER_REGISTRATION_REQUEST to local
// 	REMOTE_TO_LOCAL_CONNECTED,  // already sent LE_PEER_REGISTRATION_RESPONSE to remote
// 	WAIT_SEND_REGISTRATION_REQUEST_TO_REMOTE,
// 	WAIT_REMOTE_RESPONSE,       // wait for LE_PEER_REGISTRATION_RESPONSE from remote
// 	LOCAL_TO_REMOTE_CONNECTED,  // received LE_PEER_REGISTRATION_RESPONSE from remote
// };

// enum _IPSC_PEER_STATUS
// {
// 	WAIT_LE_PEER_REGISTRATION_RESPONSE = 0,
// 	WAIT_LE_PEER_KEEPALIVE_REQUEST,
// 	WAIT_LE_PEER_KEEPALIVE_RESPONSE
// };

class CWLNet;

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
	/*设置PEER ID*/
	void SetPeerID(u_long PeerNetID);
	//void SetPeerMode(char PeerMode);
	//void SetPeerServices(u_short PeerServices);
	//PSOCKADDR GetpPeerAddress(void);
	/*获取peer的地址以sockaddr_in方式*/
	sockaddr_in GetPeerAddressOfSockaddrin(void);
	//u_long GetAddress();
	/*获取PEER ID*/
	u_long GetPeerID();
	/*处理数据包*/
	BOOL HandlePacket(DWORD handleCode, void* pParameter, u_long masterIp, u_short masterPort, BOOL isNeedResponse = TRUE, BOOL isCallBack = FALSE);
	//void HandlePeerTimeout();
	/*设置日志回调*/
	void setLogPtr(PLogReport log_handel);
	/*打印PEER的概要信息*/
	void printInfo();
	/*获取当前PEER正在使用SLOT 0表示未使用*/
	slot_number_enum getUseSlot();
	/*设置当前PEER使用的SLOT*/
	void setUseSlot(unsigned char value);
	/*关闭定时器*/
	void destroy();
	/*开启定时器并进行LE注册*/
	void peerInit();
	bool getbFirstWlRegistration();
	bool getbFirstInit();
	bool m_bIsTemp;
protected:
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
	/*发送数据到PEER*/
	int SendToPeer(const SOCKADDR_IN* pAddr);

private:
	slot_number_enum m_useSlot;
	bool m_Remote3rdParty;
	PLogReport m_report;
	char m_reportMsg[WL_LOG_SIZE];
	u_long m_ulPeerID; //Stored in network byte order.
	//char m_ucPeerMode;
	//u_short m_uPeerServices;
	//DWORD m_FirewallOpenTimerReset;
	//SlotConfiguration m_SlotConfiguration;
	struct sockaddr_in m_PeerAddr;   //This is the address peers.
	//{{{ Peer Status, include Local->Remote and Remote->Local status
	//int m_LocalToRemoteState;
	//int m_RemoteToLocalState;
	//}}}
	CWLNet* m_pWLNet;
	WSAOVERLAPPED m_TxOverlapped;     //This is used by Worker Thread Tx.
	WSABUF m_SendControlBuffer;
	DWORD m_dwByteSent;
	DWORD m_dwTxFlags;
	char m_controlBuffer[1000];
	/* used by the third party application to track status of multiple  */
	/* commands issued to the same Repeater Peer						*/
	WORD m_wRegistrationId;
	//DWORD m_dwPeerKeepAliverTimer;
	//DWORD m_dwPeerState;
	//DWORD m_dwRecvPeerKeepAliverTimer;

	/*将日志发送给回调*/
	void sendLogToWindow();
public:
	//u_long GetPort();
	/*获取当前是否为第三方*/
	bool getRemote3rdParty();
	/*设置当前是否为第三方*/
	void setRemote3rdParty(bool value);
	/*比较指定的peer和当前的peer是否为同一个peer*/
	bool isSame(CIPSCPeer *pPeer);
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
	/*调用指定算法生成对应数据*/
	void getWirelineAuthentication(char* pPacket, DWORD &size);
	//void Timerwait_LE_PEER_REGISTRATION_RESPONSE_Proc();
	//void Timerwait_LE_PEER_KEEP_ALIVE_REQUEST_Proc();
	//void Timerwait_LE_PEER_KEEP_ALIVE_RESPONSE_Proc();
	/*PEER状态检测执行线程*/
	void PeerStatusCheckProc();
	//static void PASCAL Timerwait_LE_PEER_REGISTRATION_RESPONSE(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
	//static void PASCAL Timerwait_LE_PEER_KEEP_ALIVE_REQUEST(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
	//static void PASCAL Timerwait_LE_PEER_KEEP_ALIVE_RESPONSE(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
	/*PEER状态检测线程*/
	static void PASCAL PeerStatusCheck(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
	/*获取请求通话结果相关信息*/
	void getCallRequestRltInfo(decline_reason_code_info_t &declineReasonCodeInfo);

	bool m_bIsMaster;
	bool m_bFirstInit;
	bool m_bFirstWlRegistration;
};

#endif