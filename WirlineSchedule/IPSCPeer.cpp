#include "stdafx.h"
#include "IPSCPeer.h"
#include "WLNet.h"

#define AUTHENTIC_ID_SIZE	4
#define VENDER_KEY_SIZE		20

//static const unsigned char AuthenticId[AUTHENTIC_ID_SIZE] = { 0x01, 0x02, 0x00, 0x67 };
//static const unsigned char VenderKey[VENDER_KEY_SIZE] = { 0xaf, 0x3f, 0x46, 0xa2, 0x5e, 0xc9, 0xf9, 0x6c, 0x52, 0xe4, 0x87, 0x6f, 0x84, 0x18, 0x6d, 0xd7, 0x09, 0x33, 0x44, 0x28 };

static const unsigned char AuthenticId[AUTHENTIC_ID_SIZE] = { 0x01, 0x02, 0x00, 0x0d };
static const unsigned char VenderKey[VENDER_KEY_SIZE] = { 0x6b, 0xe5, 0xff, 0x95, 0x6a, 0xb5, 0xe8, 0x82, 0xa8, 0x6f, 0x29, 0x5f, 0x9d, 0x9d, 0x5e, 0xcf, 0xe6, 0x57, 0x61, 0x5a };


CIPSCPeer::CIPSCPeer(CWLNet* pLELayer, WCHAR* IP_Address, WCHAR* Port)
: m_pWLNet(pLELayer)
//, m_dwPeerState(WAIT_LE_PEER_REGISTRATION_RESPONSE)
, m_wRegistrationId(2)
//, m_dwRecvPeerKeepAliverTimer(0)
, m_Remote3rdParty(false)
, m_useSlot(NULL_SLOT)
{
	m_SendControlBuffer.buf = m_controlBuffer;
	//memset(&m_peerSlots, 0, sizeof(PEER_SLOTS));
	m_ulPeerID = 0; //Unknown at this point.
	//m_ucPeerMode = 0;
	//m_uPeerServices = 0;

	memset(&m_PeerAddr, 0, sizeof(m_PeerAddr));
	m_PeerAddr.sin_addr.s_addr = inet_addr(g_tool.UnicodeToANSI(IP_Address).c_str());
	m_PeerAddr.sin_port = htons(((u_short)(atoi(g_tool.UnicodeToANSI(Port).c_str()))));
	m_PeerAddr.sin_family = AF_INET;

	//m_FirewallOpenTimerReset = FIREWALLOPENTIMER_DEFAULT;
	//m_SlotConfiguration = SLOTCONFIGURATION_DEFAULT;
	//m_timerIdWaitPeerRegistrationResponse = 0;
	//m_timerIdWaitPeerAliveRequest = 0;
	//m_timerIdWaitPeerAliveResponse = 0;
	//m_statusWaitPeerRegistrationResponse = 0;
	//m_statusWaitPeerAliveRequest = 0;
	//m_statusWaitPeerAliveResponse = 0;
	m_peerStatus = PEER_STATUS_NULL;
	m_startTickCount = 0;
	m_timerIdPeerStatusCheck = 0;
	m_bKillTimer = false;
	m_bIsMaster = false;
	m_bFirstInit = true;
	m_bFirstWlRegistration = true;
	m_bIsTemp = false;
	//peerInit();
}

CIPSCPeer::CIPSCPeer(CWLNet* pLELayer, sockaddr_in* thesockaddr)
: m_pWLNet(pLELayer)
, m_Remote3rdParty(false)
, m_useSlot(NULL_SLOT)
{
	m_SendControlBuffer.buf = m_controlBuffer;
	//	memset(&m_peerSlots, 0, sizeof(PEER_SLOTS));
	m_ulPeerID = 0; //Unknown at this point.
	//m_ucPeerMode = 0;
	//m_uPeerServices = 0;

	memset(&m_PeerAddr, 0, sizeof(m_PeerAddr));
	m_PeerAddr.sin_addr = thesockaddr->sin_addr;
	m_PeerAddr.sin_port = thesockaddr->sin_port;
	m_PeerAddr.sin_family = AF_INET;

	//m_FirewallOpenTimerReset = FIREWALLOPENTIMER_DEFAULT;
	//m_SlotConfiguration = SLOTCONFIGURATION_DEFAULT;
	//m_timerIdWaitPeerRegistrationResponse = 0;
	//m_timerIdWaitPeerAliveRequest = 0;
	//m_timerIdWaitPeerAliveResponse = 0;
	//m_statusWaitPeerRegistrationResponse = 0;
	//m_statusWaitPeerAliveRequest = 0;
	//m_statusWaitPeerAliveResponse = 0;
	m_peerStatus = PEER_STATUS_NULL;
	m_startTickCount = 0;
	m_timerIdPeerStatusCheck = 0;
	m_bKillTimer = false;
	m_bIsMaster = false;
	m_bFirstInit = true;
	m_bFirstWlRegistration = true;
	m_bIsTemp = false;
	//peerInit();
}

//add code by chenhaidong
CIPSCPeer::CIPSCPeer(CWLNet* pLELayer, u_long IP_Address, u_short Port)
: m_pWLNet(pLELayer)
//, m_dwPeerState(WAIT_LE_PEER_REGISTRATION_RESPONSE)
, m_wRegistrationId(2)
//, m_dwRecvPeerKeepAliverTimer(0)
, m_Remote3rdParty(false)
, m_useSlot(NULL_SLOT)
{
	m_SendControlBuffer.buf = m_controlBuffer;
	//	memset(&m_peerSlots, 0, sizeof(PEER_SLOTS));
	m_ulPeerID = 0; //Unknown at this point.
	//m_ucPeerMode = 0;
	//m_uPeerServices = 0;

	memset(&m_PeerAddr, 0, sizeof(m_PeerAddr));
	m_PeerAddr.sin_addr.s_addr = IP_Address;
	m_PeerAddr.sin_port = htons(Port);
	m_PeerAddr.sin_family = AF_INET;

	//m_FirewallOpenTimerReset = FIREWALLOPENTIMER_DEFAULT;
	//m_SlotConfiguration = SLOTCONFIGURATION_DEFAULT;
	//m_timerIdWaitPeerRegistrationResponse = 0;
	//m_timerIdWaitPeerAliveRequest = 0;
	//m_timerIdWaitPeerAliveResponse = 0;
	//m_statusWaitPeerRegistrationResponse = 0;
	//m_statusWaitPeerAliveRequest = 0;
	//m_statusWaitPeerAliveResponse = 0;
	m_peerStatus = PEER_STATUS_NULL;
	m_startTickCount = 0;
	m_bKillTimer = false;
	m_bIsMaster = true;
	m_bFirstInit = true;
	m_bFirstWlRegistration = true;
	m_bIsTemp = false;
	//peerInit(TRUE);
}

CIPSCPeer::~CIPSCPeer()
{

}

// void CIPSCPeer::SetSlotConfiguration(SlotConfiguration theConfiguration)
// {
// 
// }
// 
// SlotConfiguration CIPSCPeer::GetSlotConfiguration(void)
// {
// 	return None;
// }
// 
// void CIPSCPeer::SetFirewallOpenTimer(int theResetTime)
// {
// 	m_FirewallOpenTimerReset = theResetTime;
// }
// 
// int CIPSCPeer::GetFirewallOpenTimer()
// {
// 	return m_FirewallOpenTimerReset;
// }

void CIPSCPeer::SetPeerID(u_long PeerNetID)
{
	m_ulPeerID = PeerNetID;
}

// void CIPSCPeer::SetPeerMode(char PeerMode)
// {
// 	m_ucPeerMode = PeerMode;
// }
// 
// void CIPSCPeer::SetPeerServices(u_short PeerServices)
// {
// 	m_uPeerServices = PeerServices;
// }
// 
// PSOCKADDR CIPSCPeer::GetpPeerAddress(void)
// {
// 	return (PSOCKADDR)&m_PeerAddr;
// }

// u_long CIPSCPeer::GetAddress()
// {
// 	return ntohl(m_PeerAddr.sin_addr.s_addr);
// }

u_long CIPSCPeer::GetPeerID()
{
	return m_ulPeerID;
}

BOOL CIPSCPeer::HandlePacket(DWORD handleCode, void* pParameter, u_long masterIp, u_short masterPort, BOOL isNeedResponse, BOOL isCallBack)
{
	DWORD temp_addr = masterIp;
	WORD temp_port = htons(masterPort);
	char* strAddr = inet_ntoa(m_PeerAddr.sin_addr);
	WORD wPort = htons(m_PeerAddr.sin_port);
	switch (handleCode)
	{
	case LE_PEER_REGISTRATION_REQUEST_REMOTE:
	{
												T_LE_PROTOCOL_95 networkData = { 0 };
												DWORD recordType = CONFIG_RECORD_TYPE;
												if (IPSC == recordType)
												{
													networkData.currentLinkProtocolVersion = IPSC_CURRENTLPVERSION;
													networkData.oldestLinkProtocolVersion = IPSC_OLDESTPVERSION;
												}
												else if (CPC == recordType)
												{
													networkData.currentLinkProtocolVersion = CPC_CURRENTLPVERSION;
													networkData.oldestLinkProtocolVersion = CPC_OLDESTPVERSION;
												}
												else
												{
													networkData.currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
													networkData.oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
												}
												networkData.Opcode = LE_PEER_REGISTRATION_RESPONSE;
												networkData.peerID = CONFIG_LOCAL_PEER_ID;

												m_SendControlBuffer.len = Build_LE_PEER_REGISTRATION_RESPONSE(m_SendControlBuffer.buf, &networkData);
												SendToPeer(&m_PeerAddr);
	}
		break;
	case LE_PEER_REGISTRATION_REQUEST_LOCAL:
	{
											   DWORD recordType = CONFIG_RECORD_TYPE;
											   T_LE_PROTOCOL_94 networkData = { 0 };
											   if (IPSC == recordType)
											   {
												   networkData.currentLinkProtocolVersion = IPSC_CURRENTLPVERSION;
												   networkData.oldestLinkProtocolVersion = IPSC_OLDESTPVERSION;
											   }
											   else if (CPC == recordType)
											   {
												   networkData.currentLinkProtocolVersion = CPC_CURRENTLPVERSION;
												   networkData.oldestLinkProtocolVersion = CPC_OLDESTPVERSION;
											   }
											   else
											   {
												   networkData.currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
												   networkData.oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
											   }
											   networkData.Opcode = LE_PEER_REGISTRATION_REQUEST;
											   networkData.peerID = CONFIG_LOCAL_PEER_ID;
											   m_SendControlBuffer.len = Build_LE_PEER_REGISTRATION_REQUEST(m_SendControlBuffer.buf, &networkData);
											   m_peerStatus = PEER_STATUS_REGIS_RESPONSE;
											   m_startTickCount = GetTickCount();
											   SendToPeer(&m_PeerAddr);
											   //sprintf_s(m_reportMsg, "PEER %lu build 0x94 packet,TickCount:%lu,this:%lu", m_ulPeerID, m_startTickCount, (DWORD)this);
											   //sendLogToWindow();
	}
		break;
	case LE_PEER_REGISTRATION_RESPONSE_REMOTE:
	{
												 m_startTickCount = GetTickCount();
												 m_peerStatus = PEER_STATUS_ALIVE_REQUES;
												 //sprintf_s(m_reportMsg, "PEER %lu recive 0x95 packet,TickCount:%lu,this:%lu", m_ulPeerID, m_startTickCount, DWORD(this));
												 //sendLogToWindow();
	}
		break;
	case LE_PEER_KEEP_ALIVE_REQUEST_REMOTE:
	{
											  ////////////////////////////////////////////////////////////////////////////
											  ///*更新->等待请求心跳包(S)-<定时器,使准时*/
											  ////////////////////////////////////////////////////////////////////////////
											  //m_statusWaitPeerAliveRequest = STATUS_ONTIME;
											  m_startTickCount = GetTickCount();
											  DWORD recordType = CONFIG_RECORD_TYPE;
											  T_LE_PROTOCOL_98* p = NULL;
											  T_LE_PROTOCOL_98_LCP* pLcp = NULL;
											  if (LCP == recordType)
											  {
												  pLcp = (T_LE_PROTOCOL_98_LCP*)pParameter;
												  /*当前peer是否为第三方应用*/
												  if (pLcp->peerMode & 0x0080)
												  {
													  setRemote3rdParty(true);
												  }
												  else
												  {
													  setRemote3rdParty(false);
												  }
											  }
											  else
											  {
												  p = (T_LE_PROTOCOL_98*)pParameter;
												  /*当前peer是否为第三方应用*/

												  if (p->peerServices & 0x00002000)
												  {
													  setRemote3rdParty(true);
												  }
												  else
												  {
													  setRemote3rdParty(false);
												  }
											  }
											  /************************************************************************/
											  /* 改动理由: devspec_link_establishment_0103.pdf line 370
											  /************************************************************************/
											  /*回复心跳包*/
											  if (LCP == recordType)
											  {
												  T_LE_PROTOCOL_99_LCP networkData = { 0 };
												  networkData.currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
												  networkData.oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
												  networkData.Opcode = LE_PEER_KEEP_ALIVE_RESPONSE;
												  networkData.peerID = CONFIG_LOCAL_PEER_ID;
												  networkData.peerMode = LCP_MODE;
												  networkData.peerServices = LCP_SERVICES;
												  m_SendControlBuffer.len = Build_LE_PEER_KEEP_ALIVE_RESPONSE(m_SendControlBuffer.buf, &networkData);
												  SendToPeer(&m_PeerAddr);
											  }
											  else
											  {
												  T_LE_PROTOCOL_99 networkData = { 0 };
												  networkData.Opcode = LE_PEER_KEEP_ALIVE_RESPONSE;
												  ;
												  networkData.peerID = CONFIG_LOCAL_PEER_ID;
												  if (CPC == recordType)
												  {
													  networkData.peerMode = CPC_MODE;
													  networkData.peerServices = CPC_SERVICES;
												  }
												  else
												  {
													  networkData.peerMode = IPSC_MODE;
													  networkData.peerServices = IPSC_SERVICES;
												  }
												  m_SendControlBuffer.len = Build_LE_PEER_KEEP_ALIVE_RESPONSE(m_SendControlBuffer.buf, &networkData);
												  SendToPeer(&m_PeerAddr);
											  }
											  /*发起心跳包*/
											  HandlePacket(LE_PEER_KEEP_ALIVE_REQUEST_LOCAL, NULL, 0, 0);
	}
		break;
	case LE_PEER_KEEP_ALIVE_REQUEST_LOCAL:
	{
											 DWORD recordType = CONFIG_RECORD_TYPE;
											 /*发起心跳包*/
											 if (LCP == recordType)
											 {
												 T_LE_PROTOCOL_98_LCP networkData = { 0 };
												 networkData.currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
												 networkData.oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
												 networkData.Opcode = LE_PEER_KEEP_ALIVE_REQUEST;
												 networkData.peerID = CONFIG_LOCAL_PEER_ID;
												 networkData.peerMode = LCP_MODE;
												 networkData.peerServices = LCP_SERVICES;
												 m_SendControlBuffer.len = Build_LE_PEER_KEEP_ALIVE_REQUEST(m_SendControlBuffer.buf, &networkData);
												 m_peerStatus = PEER_STATUS_ALIVE_RESPONSE;
												 m_startTickCount = GetTickCount();
												 SendToPeer(&m_PeerAddr);
											 }
											 else
											 {
												 T_LE_PROTOCOL_98 networkData = { 0 };
												 networkData.Opcode = LE_PEER_KEEP_ALIVE_REQUEST;
												 networkData.peerID = CONFIG_LOCAL_PEER_ID;
												 if (CPC == recordType)
												 {
													 networkData.peerMode = CPC_MODE;
													 networkData.peerServices = CPC_SERVICES;
												 }
												 else
												 {
													 networkData.peerMode = IPSC_MODE;
													 networkData.peerServices = IPSC_SERVICES;
												 }

												 m_SendControlBuffer.len = Build_LE_PEER_KEEP_ALIVE_REQUEST(m_SendControlBuffer.buf, &networkData);
												 m_peerStatus = PEER_STATUS_ALIVE_RESPONSE;
												 m_startTickCount = GetTickCount();
												 SendToPeer(&m_PeerAddr);
											 }
	}
		break;
	case LE_PEER_KEEP_ALIVE_RESPONSE_REMOTE:
	{
											   m_startTickCount = GetTickCount();
											   m_peerStatus = PEER_STATUS_ALIVE_REQUES;

	}
		break;
	case WL_REGISTRATION_REQUEST_LOCAL:
	{

										  m_bFirstWlRegistration = false;
										  T_WL_PROTOCOL_01 networkData = { 0 };
										  networkData.currentLinkProtocolVersion = Wireline_Protocol_Version;
										  networkData.numberOfRegistrationEntries = NUMBER_REGIS_ENTRIES;
										  networkData.oldestLinkProtocolVersion = Wireline_Protocol_Version;
										  networkData.Opcode = WL_PROTOCOL;
										  networkData.peerID = CONFIG_LOCAL_PEER_ID;
										  networkData.registrationID = m_wRegistrationId;
										  networkData.registrationPduID = REGISTRATION_PDU_ID;
										  networkData.registrationSlotNumber = BOTH_SLOT1_SLOT2;
										  networkData.wirelineOpcode = WL_REGISTRATION_REQUEST;
										  networkData.wirelineStatusRegistration = REGISTERED_VOICE_SERVICE;
										  networkData.wlRegistrationEntries[0].VoiceAttributes = REGISTERED_VOICE_SERVICE;
										  networkData.wlRegistrationEntries[0].AddressType = AllIndividualCall;
										  networkData.wlRegistrationEntries[1].VoiceAttributes = REGISTERED_VOICE_SERVICE;
										  networkData.wlRegistrationEntries[1].AddressType = AllTalkGroupCall;
										  //改动理由:devspec_nai_voice_csbk_0102.pdf line 998
										  if (IPSC == CONFIG_RECORD_TYPE
											  && !getRemote3rdParty())
										  {
											  networkData.registrationSlotNumber = SLOT1;
											  m_SendControlBuffer.len = Build_WL_REGISTRATION_REQUEST(m_SendControlBuffer.buf, &networkData);
											  SendToPeer(&m_PeerAddr);

											  networkData.registrationSlotNumber = SLOT2;
											  m_SendControlBuffer.len = Build_WL_REGISTRATION_REQUEST(m_SendControlBuffer.buf, &networkData);
											  SendToPeer(&m_PeerAddr);
										  }
										  else if ((CPC == CONFIG_RECORD_TYPE)
											  && !getRemote3rdParty())
										  {
											  if (0 != m_ulPeerID)
											  {
												  networkData.registrationSlotNumber = BOTH_SLOT1_SLOT2;
												  m_SendControlBuffer.len = Build_WL_REGISTRATION_REQUEST(m_SendControlBuffer.buf, &networkData);
												  SendToPeer(&m_PeerAddr);
											  }
											  else
											  {
												  m_pWLNet->setSitePeer(this);
											  }
										  }
										  else if (LCP == CONFIG_RECORD_TYPE
											  && !getRemote3rdParty())
										  {
											  if (0x00FFFFFF & m_ulPeerID)
											  {
												  networkData.registrationSlotNumber = BOTH_SLOT1_SLOT2;
												  m_SendControlBuffer.len = Build_WL_REGISTRATION_REQUEST(m_SendControlBuffer.buf, &networkData);
												  SendToPeer(&m_PeerAddr);
											  }
											  else
											  {
												  m_pWLNet->setSitePeer(this);
											  }
											  /*判定是否为第三方peer*/
											  if (0xFF000000 & m_ulPeerID)
											  {
												  setRemote3rdParty(false);
											  }
											  else
											  {
												  setRemote3rdParty(true);
											  }
										  }

	}
		break;
	case WL_VC_CHNL_CTRL_REQUEST_LOCAL:
	{
										  DWORD recordType = CONFIG_RECORD_TYPE;
										  /*获取关键信息*/
										  T_WL_PROTOCOL_13 networkData = { 0 };
										  networkData.accessCriteria = Access_Criteria_Polite_Access;
										  networkData.callAttributes = CALL_ATTRIBUTES;//clear call
										  g_callId++;
										  networkData.callID = g_callId;
										  networkData.callType = g_targetCallType;
										  networkData.CSBKArguments = CSBK_ARGUMENTS;
										  networkData.currentLinkProtocolVersion = Wireline_Protocol_Version;
										  networkData.oldestLinkProtocolVersion = Wireline_Protocol_Version;
										  networkData.Opcode = WL_PROTOCOL;
										  networkData.peerID = CONFIG_LOCAL_PEER_ID;
										  networkData.preambleDuration = PREAMBLE_DURATION;
										  networkData.sourceID = CONFIG_LOCAL_RADIO_ID;
										  networkData.targetID = CONFIG_CURRENT_TAGET;
										  networkData.wirelineOpcode = WL_VC_CHNL_CTRL_REQUEST;
										  if (isCallBack)
										  {
											  networkData.slotNumber = m_useSlot;
										  }
										  else
										  {
											  if (IPSC == recordType)
											  {
												  networkData.slotNumber = *((SlotNumber_e*)(pParameter));
											  }
											  else
											  {
												  networkData.slotNumber = NULL_SLOT;
											  }
										  }

										  m_SendControlBuffer.len = Build_WL_VC_CHNL_CTRL_REQUEST(m_SendControlBuffer.buf, &networkData);
										  SendToPeer(&m_PeerAddr);
	}
		break;
	case WL_VC_CHNL_CTRL_STATUS_REMOTE:
	{
										  T_WL_PROTOCOL_16* p = (T_WL_PROTOCOL_16*)pParameter;
										  g_callRequstDeclineReasonCodeInfo.Value = p->DeclineReasonCode;
										  getCallRequestRltInfo(g_callRequstDeclineReasonCodeInfo);
										  switch (p->chnCtrlstatus)
										  {
										  case Channel_Control_Request_Status_Grant:
										  {
																					   m_useSlot = (SlotNumber_e)p->slotNumber;
																					   m_pWLNet->setCurrentSendVoicePeer(this);
																					   m_pWLNet->CorrectingBuffer(p->callID);
																					   m_pWLNet->releaseNewCallEvent();
										  }
											  break;
										  case Channel_Control_Request_Status_Declined:
										  {
																						  WORD callStatus = m_pWLNet->GetCallStatus();
																						  if (callStatus == CALL_START)
																						  {
																							  m_pWLNet->releaseNewCallEvent();
																						  }
																						  else if (callStatus != CALL_IDLE)
																						  {
																							  sprintf_s(m_reportMsg, "call end:%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
																							  sendLogToWindow();
																							  /*当前是否可以停止录音*/
																							  //g_manager->setbNeedStopCall(true);
																							  if (g_pNet->canStopRecord())
																							  {
																								  g_pNet->requestRecordEndEvent();
																								  g_pSound->setbRecord(FALSE);
																								  g_pNet->waitRecordEnd();
																							  }
																						  }
										  }
											  break;
										  case Channel_Control_Request_Status_Interrupting:
										  {
										  }
											  break;
										  case Channel_Control_Request_Status_Received:
										  {
										  }
											  break;
										  case Channel_Control_Request_Status_Transmission_Successful:
										  {
										  }
											  break;
										  case Channel_Control_Request_Status_Transmitting:
										  {
										  }
											  break;
										  default:
											  break;
										  }
	}
		break;
	default:
		break;
	}
	return TRUE;
}

//void CIPSCPeer::HandlePeerTimeout()
//{
//	DWORD current = GetTickCount();
//	long intercept = current - m_dwPeerKeepAliverTimer;
//	if (intercept > PEER_KEEP_ALIVE_TIME)
//	{
//
//		//等待peer注册回复超时
//		if (WAIT_LE_PEER_REGISTRATION_RESPONSE == m_dwPeerState)
//		{
//			if (m_dwRecvPeerKeepAliverTimer == m_dwPeerKeepAliverTimer
//				&& m_dwRecvPeerKeepAliverTimer != 0)
//			{
//				//当前已经成功注册
//				m_dwPeerState = WAIT_LE_PEER_KEEPALIVE_REQUEST;
//			}
//			else
//			{
//
//				//注册失败，对象peer不可达
//				sprintf_s(m_reportMsg, "PEER ID:%lu未回复,再次LE注册", m_ulPeerID);
//				sendLogToWindow();
//
//				//重新请求注册
//				DWORD recordType = m_pWLNet->m_dwRecType;
//				T_LE_PROTOCOL_94 networkData = { 0 };
//				if (IPSC == recordType)
//				{
//					networkData.currentLinkProtocolVersion = IPSC_CURRENTLPVERSION;
//					networkData.oldestLinkProtocolVersion = IPSC_OLDESTPVERSION;
//				}
//				else if (CPC == recordType)
//				{
//					networkData.currentLinkProtocolVersion = CPC_CURRENTLPVERSION;
//					networkData.oldestLinkProtocolVersion = CPC_OLDESTPVERSION;
//				}
//				else
//				{
//					networkData.currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
//					networkData.oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
//				}
//				networkData.Opcode = LE_PEER_REGISTRATION_REQUEST;
//				networkData.peerID = g_localPeerId;
//				m_SendControlBuffer.len = Build_LE_PEER_REGISTRATION_REQUEST(m_SendControlBuffer.buf, &networkData);
//				SendToPeer(&m_PeerAddr);
//
//			}
//			//初始化接收时间
//			sendLogToWindow();
//		}
//
//		//向PEER发送心跳包时间到
//		else if (WAIT_LE_PEER_KEEPALIVE_REQUEST == m_dwPeerState)
//		{
//			//上一轮心跳包正常
//			if (m_dwRecvPeerKeepAliverTimer == m_dwPeerKeepAliverTimer)
//			{
//				//sprintf_s(m_reportMsg, "PEER:%lu心跳包正常", m_ulPeerID);
//				//sendLogToWindow();
//				//组建正常心跳包
//				//Build_LE_PEER_KEEP_ALIVE_REQUEST();
//				DWORD recordType = m_pWLNet->m_dwRecType;
//				if (LCP == recordType)
//				{
//					T_LE_PROTOCOL_98_LCP networkData = { 0 };
//					networkData.currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
//					networkData.oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
//					networkData.Opcode = LE_PEER_KEEP_ALIVE_REQUEST;
//					networkData.peerID = g_localPeerId;
//					networkData.peerMode = LCP_MODE;
//					networkData.peerServices = LCP_SERVICES;
//					m_SendControlBuffer.len = Build_LE_PEER_KEEP_ALIVE_REQUEST(m_SendControlBuffer.buf, &networkData);
//					//SendToPeer(&m_PeerAddr);
//				}
//				else
//				{
//					T_LE_PROTOCOL_98 networkData = { 0 };
//					networkData.Opcode = LE_PEER_KEEP_ALIVE_REQUEST;
//					networkData.peerID = g_localPeerId;
//					if (CPC == recordType)
//					{
//						networkData.peerMode = CPC_MODE;
//						networkData.peerServices = CPC_SERVICES;
//					}
//					else
//					{
//						networkData.peerMode = IPSC_MODE;
//						networkData.peerServices = IPSC_SERVICES;
//					}
//
//					m_SendControlBuffer.len = Build_LE_PEER_KEEP_ALIVE_REQUEST(m_SendControlBuffer.buf, &networkData);
//					//SendToPeer(&m_PeerAddr);
//				}
//			}
//			//心跳异常
//			else
//			{
//				sprintf_s(m_reportMsg, "PEER:%lu心跳包异常", m_ulPeerID);
//				sendLogToWindow();
//
//				//重新请求注册
//				DWORD recordType = m_pWLNet->m_dwRecType;
//				T_LE_PROTOCOL_94 networkData = { 0 };
//				if (IPSC == recordType)
//				{
//					networkData.currentLinkProtocolVersion = IPSC_CURRENTLPVERSION;
//					networkData.oldestLinkProtocolVersion = IPSC_OLDESTPVERSION;
//				}
//				else if (CPC == recordType)
//				{
//					networkData.currentLinkProtocolVersion = CPC_CURRENTLPVERSION;
//					networkData.oldestLinkProtocolVersion = CPC_OLDESTPVERSION;
//				}
//				else
//				{
//					networkData.currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
//					networkData.oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
//				}
//				networkData.Opcode = LE_PEER_REGISTRATION_REQUEST;
//				networkData.peerID = g_localPeerId;
//				m_SendControlBuffer.len = Build_LE_PEER_REGISTRATION_REQUEST(m_SendControlBuffer.buf, &networkData);
//				//修改当前peer状态
//				m_dwPeerState = WAIT_LE_PEER_REGISTRATION_RESPONSE;
//			}
//			//初始化接受时间
//			m_dwRecvPeerKeepAliverTimer = 0;
//			SendToPeer(&m_PeerAddr);
//		}
//		//更新时间
//		m_dwPeerKeepAliverTimer = GetTickCount();
//	}
//}

void CIPSCPeer::peerInit()
{

	if (!m_bIsMaster)
	{
		m_timerIdPeerStatusCheck = timeSetEvent(PEER_STATUS_CHECK_INTERVAL, 1, PeerStatusCheck, (DWORD)this, TIME_PERIODIC);
		HandlePacket(LE_PEER_REGISTRATION_REQUEST_LOCAL, NULL, 0, 0);
		m_bFirstInit = false;
	}
}

// int CIPSCPeer::Build_LE_PEER_REGISTRATION_REQUEST()
// {
// 	DWORD temp = g_localPeerId;
// 	m_controlBuffer[4] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[3] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[2] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[1] = (char)(temp & 0x000000FF);
// 
// 	if (m_pWLNet->m_dwRecType == IPSC)
// 	{
// 		m_controlBuffer[5] = 0x04;			// current version IPSC and R2.3
// 		m_controlBuffer[6] = 0x04;
// 
// 		m_controlBuffer[7] = 0x04;			// old version IPSC and Reserved 
// 		m_controlBuffer[8] = 0x00;
// 	}
// 	else if (CPC == m_pWLNet->m_dwRecType)
// 	{
// 		m_controlBuffer[5] = CPC_CURRENTVERSION_DEFAULT_2;			// current version CPC and R2.3
// 		m_controlBuffer[6] = CPC_CURRENTVERSION_DEFAULT_1;
// 
// 		m_controlBuffer[7] = CPC_OLDESTVERSION_DEFAULT_2;			// old version CPC and Reserved 
// 		m_controlBuffer[8] = CPC_OLDESTVERSION_DEFAULT_1;
// 	}
// 	else
// 	{
// 		// LCP
// 		m_controlBuffer[5] = LCP_CURRENTVERSION_DEFAULT_2;
// 		m_controlBuffer[6] = LCP_CURRENTVERSION_DEFAULT_1;
// 
// 		m_controlBuffer[7] = LCP_OLDESTVERSION_DEFAULT_2;
// 		m_controlBuffer[8] = LCP_OLDESTVERSION_DEFAULT_1;
// 	}
// 
// 	m_controlBuffer[0] = LE_PEER_REGISTRATION_REQUEST;
// 
// 	m_SendControlBuffer.len = 9;
// 
// 	return 9;
// }

DWORD CIPSCPeer::Build_LE_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_94* pData)
{
	DWORD size = 0;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	/*currentLinkProtocolVersion*/
	*((WORD*)(&pPacket[5])) = htons(pData->currentLinkProtocolVersion);
	/*oldestLinkProtocolVersion*/
	*((WORD*)(&pPacket[7])) = htons(pData->oldestLinkProtocolVersion);
	size = 9;
	return size;
}

// int CIPSCPeer::Build_LE_PEER_REGISTRATION_RESPONSE()
// {
// 	DWORD temp = g_localPeerId;
// 	m_controlBuffer[4] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[3] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[2] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[1] = (char)(temp & 0x000000FF);
// 
// 	if (m_pWLNet->m_dwRecType == IPSC)
// 	{
// 		m_controlBuffer[5] = IPSC_CURRENTLPVERSION_DEFAULT_2;			// current version LCP and R2.3
// 		m_controlBuffer[6] = IPSC_CURRENTLPVERSION_DEFAULT_1;
// 
// 		m_controlBuffer[7] = IPSC_OLDESTLPVERSION_DEFAULT_2;			// old version LCP and Reserved 
// 		m_controlBuffer[8] = IPSC_OLDESTLPVERSION_DEFAULT_1;
// 	}
// 	else if (CPC == m_pWLNet->m_dwRecType)
// 	{
// 		m_controlBuffer[5] = CPC_CURRENTVERSION_DEFAULT_2;			// current version LCP and R2.3
// 		m_controlBuffer[6] = CPC_CURRENTVERSION_DEFAULT_1;
// 
// 		m_controlBuffer[7] = CPC_OLDESTVERSION_DEFAULT_2;			// old version LCP and Reserved 
// 		m_controlBuffer[8] = CPC_OLDESTVERSION_DEFAULT_1;
// 	}
// 	else
// 	{
// 		// LCP
// 		m_controlBuffer[5] = LCP_CURRENTVERSION_DEFAULT_2;			// current version LCP and R2.3
// 		m_controlBuffer[6] = LCP_CURRENTVERSION_DEFAULT_1;
// 
// 		m_controlBuffer[7] = LCP_OLDESTVERSION_DEFAULT_2;			// old version LCP and Reserved 
// 		m_controlBuffer[8] = LCP_OLDESTVERSION_DEFAULT_1;
// 	}
// 
// 	m_controlBuffer[0] = LE_PEER_REGISTRATION_RESPONSE;
// 
// 	m_SendControlBuffer.len = 9;
// 
// 	return 9;
// }

DWORD CIPSCPeer::Build_LE_PEER_REGISTRATION_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_95* pData)
{
	DWORD size = 0;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	/*currentLinkProtocolVersion*/
	*((WORD*)(&pPacket[5])) = htons(pData->currentLinkProtocolVersion);
	/*oldestLinkProtocolVersion*/
	*((WORD*)(&pPacket[7])) = htons(pData->oldestLinkProtocolVersion);
	size = 9;
	return size;

}

// int CIPSCPeer::Build_LE_PEER_KEEP_ALIVE_RESPONSE()
// {
// 	m_controlBuffer[0] = LE_PEER_KEEP_ALIVE_RESPONSE;
// 	DWORD temp = g_localPeerId;
// 	m_controlBuffer[4] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[3] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[2] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[1] = (char)(temp & 0x000000FF);
// 
// 	int length = 0;
// 
// 	if (LCP == m_pWLNet->m_dwRecType)
// 	{
// 		m_controlBuffer[5] = LCP_PEERMODE_DEFAULT_1;
// 		m_controlBuffer[6] = LCP_PEERMODE_DEFAULT_2;
// 
// 		m_controlBuffer[7] = LCP_PEERSERVICES_DEFAULT_1;
// 		m_controlBuffer[8] = LCP_PEERSERVICES_DEFAULT_2;
// 		m_controlBuffer[9] = LCP_PEERSERVICES_DEFAULT_3;
// 		m_controlBuffer[10] = LCP_PEERSERVICES_DEFAULT_4;
// 
// 		m_controlBuffer[11] = LCP_CURRENTVERSION_DEFAULT_2;
// 		m_controlBuffer[12] = LCP_CURRENTVERSION_DEFAULT_1;
// 
// 		m_controlBuffer[13] = LCP_OLDESTVERSION_DEFAULT_2;
// 		m_controlBuffer[14] = LCP_OLDESTVERSION_DEFAULT_1;
// 
// 		m_SendControlBuffer.len = 15;
// 		length = 15;
// 	}
// 	else  if (IPSC == m_pWLNet->m_dwRecType)
// 	{
// 		// @see <<adk_link_establishment_spec_0101.pdf>> page 95
// 		m_controlBuffer[5] = 0x6a;		// Peer Mode. Enabled(7:6) Current Signaling Mode(5:4) Digital. Slot1, and Slot2 LCP
// 
// 		m_controlBuffer[6] = 0x00;
// 		m_controlBuffer[7] = 0x00;
// 		m_controlBuffer[8] = 0x20;
// 		m_controlBuffer[9] = (unsigned char)0xE4;
// 
// 		m_SendControlBuffer.len = 10;
// 		length = 10;
// 	}
// 	else if (CPC == m_pWLNet->m_dwRecType)
// 	{
// 
// 		m_controlBuffer[5] = CPC_PEERMODE_DEFAULT;		// Peer Mode. Enabled(7:6) Current Signaling Mode(5:4) Digital. Slot1, and Slot2 LCP
// 
// 		m_controlBuffer[6] = CPC_PEERSERVICES_DEFAULT_4;
// 		m_controlBuffer[7] = CPC_PEERSERVICES_DEFAULT_3;
// 		m_controlBuffer[8] = CPC_PEERSERVICES_DEFAULT_2;
// 		m_controlBuffer[9] = CPC_PEERSERVICES_DEFAULT_1;
// 
// 		m_SendControlBuffer.len = 10;
// 		length = 10;
// 	}
// 
// 	return length;
// }

DWORD CIPSCPeer::Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99* pData)
{
	DWORD size = 0;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	/*peerMode*/
	pPacket[5] = pData->peerMode;
	/*peerServices*/
	*((DWORD*)(&pPacket[6])) = htonl(pData->peerServices);
	size = 10;
	return size;
}

DWORD CIPSCPeer::Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99_LCP* pData)
{
	DWORD size = 0;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	/*peerMode*/
	*((WORD*)(&pPacket[5])) = htons(pData->peerMode);
	/*peerServices*/
	*((DWORD*)(&pPacket[7])) = htonl(pData->peerServices);
	/*currentLinkProtocolVersion*/
	*((WORD*)(&pPacket[11])) = htons(pData->currentLinkProtocolVersion);
	/*oldestLinkProtocolVersion*/
	*((WORD*)(&pPacket[13])) = htons(pData->oldestLinkProtocolVersion);
	size = 15;
	return size;
}

//int CIPSCPeer::Build_LE_PEER_KEEP_ALIVE_REQUEST()
//{
//	m_controlBuffer[0] = LE_PEER_KEEP_ALIVE_REQUEST;
//
//	DWORD temp = g_localPeerId;
//	m_controlBuffer[4] = (char)(temp & 0x000000FF);
//	temp = temp >> 8;
//	m_controlBuffer[3] = (char)(temp & 0x000000FF);
//	temp = temp >> 8;
//	m_controlBuffer[2] = (char)(temp & 0x000000FF);
//	temp = temp >> 8;
//	m_controlBuffer[1] = (char)(temp & 0x000000FF);
//
//	int length = 0;
//
//	if (LCP == m_pWLNet->m_dwRecType)
//	{
//		m_controlBuffer[5] = LCP_PEERMODE_DEFAULT_1;
//		m_controlBuffer[6] = LCP_PEERMODE_DEFAULT_2;
//
//		m_controlBuffer[7] = LCP_PEERSERVICES_DEFAULT_1;
//		m_controlBuffer[8] = LCP_PEERSERVICES_DEFAULT_2;
//		m_controlBuffer[9] = LCP_PEERSERVICES_DEFAULT_3;
//		m_controlBuffer[10] = LCP_PEERSERVICES_DEFAULT_4;
//
//		m_controlBuffer[11] = LCP_CURRENTVERSION_DEFAULT_2;
//		m_controlBuffer[12] = LCP_CURRENTVERSION_DEFAULT_1;
//
//		m_controlBuffer[13] = LCP_OLDESTVERSION_DEFAULT_2;
//		m_controlBuffer[14] = LCP_OLDESTVERSION_DEFAULT_1;
//
//		m_SendControlBuffer.len = 15;
//		length = 15;
//	}
//	else  if (IPSC == m_pWLNet->m_dwRecType)
//	{
//		// @see <<adk_link_establishment_spec_0101.pdf>> page 95
//		m_controlBuffer[5] = IPSC_PEERMODE_DEFAULT;		// Peer Mode. Enabled(7:6) Current Signaling Mode(5:4) Digital. Slot1, and Slot2 LCP
//
//		m_controlBuffer[6] = IPSC_PEERSERVICES_DEFAULT_4;
//		m_controlBuffer[7] = IPSC_PEERSERVICES_DEFAULT_3;
//		m_controlBuffer[8] = IPSC_PEERSERVICES_DEFAULT_2;
//		m_controlBuffer[9] = IPSC_PEERSERVICES_DEFAULT_1;
//
//		m_SendControlBuffer.len = 10;
//		length = 10;
//	}
//	else if (CPC == m_pWLNet->m_dwRecType)
//	{
//		m_controlBuffer[5] = CPC_PEERMODE_DEFAULT;		// Peer Mode. Enabled(7:6) Current Signaling Mode(5:4) Digital. Slot1, and Slot2 LCP
//
//		m_controlBuffer[6] = CPC_PEERSERVICES_DEFAULT_4;
//		m_controlBuffer[7] = CPC_PEERSERVICES_DEFAULT_3;
//		m_controlBuffer[8] = CPC_PEERSERVICES_DEFAULT_2;
//		m_controlBuffer[9] = CPC_PEERSERVICES_DEFAULT_1;
//
//		m_SendControlBuffer.len = 10;
//		length = 10;
//	}
//
//
//	return length;
//}

DWORD CIPSCPeer::Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98* pData)
{
	DWORD size = 0;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	/*peerMode*/
	pPacket[5] = pData->peerMode;
	/*peerServices*/
	*((DWORD*)(&pPacket[6])) = htonl(pData->peerServices);
	size = 10;
	return size;
}

DWORD CIPSCPeer::Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98_LCP* pData)
{
	DWORD size = 0;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	/*peerMode*/
	*((WORD*)(&pPacket[5])) = htons(pData->peerMode);
	/*peerServices*/
	*((DWORD*)(&pPacket[7])) = htonl(pData->peerServices);
	/*currentLinkProtocolVersion*/
	*((WORD*)(&pPacket[11])) = htons(pData->currentLinkProtocolVersion);
	/*oldestLinkProtocolVersion*/
	*((WORD*)(&pPacket[13])) = htons(pData->oldestLinkProtocolVersion);
	size = 15;
	return size;
}

// int CIPSCPeer::Build_WL_REGISTRATION_STATUS(DWORD pudID, WORD SLOT1RID, WORD SLOT2RID)
// {
// 	DWORD size = 0;
// 
// 	DWORD temp = g_localPeerId;
// 	m_controlBuffer[4] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[3] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[2] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_controlBuffer[1] = (char)(temp & 0x000000FF);
// 
// 	m_controlBuffer[0] = WL_PROTOCOL;								// class
// 	//*((DWORD*)&pPacket[1]) = htonl(peerId & 0x00ffffff);			// peer id
// 	m_controlBuffer[5] = WL_REGISTRATION_STATUS;							// wireline opcode
// 	memcpy(&m_controlBuffer[6], &pudID, sizeof(DWORD));
// 	memcpy(&m_controlBuffer[10], &SLOT1RID, sizeof(WORD));
// 	memcpy(&m_controlBuffer[12], &SLOT2RID, sizeof(WORD));
// 	m_controlBuffer[14] = 0x00;								//	registrationStatus, successful
// 	m_controlBuffer[15] = 0x00;								//	registrationStatusCode, RESERVED
// 	m_controlBuffer[16] = 0x13;								//	current wl version
// 	m_controlBuffer[17] = (unsigned char)0xb5;								//	old wl version
// 	size = 18;
// 
// 	return size;
// }

// DWORD CIPSCPeer::BuildWLRegistrationRequestPacket(CHAR* pPacket, DWORD peerId, DWORD pudId)
// {
// #pragma region OLD_CODE
// 	DWORD size = 0;
// 
// 	DWORD temp = peerId;
// 	pPacket[4] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	pPacket[3] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	pPacket[2] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	pPacket[1] = (char)(temp & 0x000000FF);
// 
// 	pPacket[0] = WL_PROTOCOL;								// class
// 	//*((DWORD*)&pPacket[1]) = htonl(peerId & 0x00ffffff);			// peer id
// 	pPacket[5] = WL_REGISTRATION_REQUEST;							// wireline opcode
// 	pPacket[6] = BOTH_SLOT1_SLOT2;									// registration slot number
// 	*((DWORD*)&pPacket[7]) = htonl(pudId);							// registration pud id
// 	*((WORD*)&pPacket[11]) = htons(m_wRegistrationId);				// registration id
// 	pPacket[13] = REGISTERED_VOICE_SERVICE;						// Wireline Channel Status Flag
// 	pPacket[14] = 2;												// 3 registration entry
// 	size = 15;
// 
// 	//RegistrationEntry allIndivdualCall;
// 	//memset(&allIndivdualCall, 0, sizeof(RegistrationEntry));
// 	//allIndivdualCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
// 	//allIndivdualCall.AddressType = AllIndividualCall;
// 	//memcpy(&pPacket[size], &allIndivdualCall, sizeof(RegistrationEntry));
// 	//size += sizeof(RegistrationEntry);
// 
// 	//RegistrationEntry allTalkGroupCall;
// 	//memset(&allTalkGroupCall, 0, sizeof(RegistrationEntry));
// 	//allTalkGroupCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
// 	//allTalkGroupCall.AddressType = AllTalkGroupCall;
// 	//memcpy(&pPacket[size], &allTalkGroupCall, sizeof(RegistrationEntry));
// 	//size += sizeof(allTalkGroupCall);
// 
// 	RegistrationEntry privateCall;
// 	memset(&privateCall, 0, sizeof(RegistrationEntry));
// 	privateCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
// 	privateCall.AddressType = IndividualCall;
// 	privateCall.AddressRangeStart = htonl(1);
// 	privateCall.AddressRangeEnd = htonl(100000);
// 	memcpy(&pPacket[size], &privateCall, sizeof(RegistrationEntry));
// 	size += sizeof(privateCall);
// 
// 	RegistrationEntry allLocalTalkGroupCall;
// 	memset(&allLocalTalkGroupCall, 0, sizeof(allLocalTalkGroupCall));
// 	allLocalTalkGroupCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
// 	allLocalTalkGroupCall.AddressType = GroupCall;
// 	allLocalTalkGroupCall.AddressRangeStart = htonl(1);
// 	allLocalTalkGroupCall.AddressRangeEnd = htonl(1000);
// 	memcpy(&pPacket[size], &allLocalTalkGroupCall, sizeof(RegistrationEntry));
// 	size += sizeof(allLocalTalkGroupCall);
// 
// 
// 	pPacket[size++] = 0x04;
// 	pPacket[size++] = 0x04;
// 
// 	CHMAC_SHA1 sha1;
// 	unsigned char digest[20];
// 	unsigned char kw[40];
// 	memset(kw, 0, sizeof(kw));
// 	memcpy_s(kw, sizeof(kw), VenderKey, sizeof(VenderKey));
// 	sha1.HMAC_SHA1((unsigned char*)pPacket, size, kw, sizeof(kw), digest);
// 
// 	memcpy_s(pPacket + size, 10, AuthenticId, AUTHENTIC_ID_SIZE);
// 	size += 4;
// 	memcpy_s(pPacket + size, 20, digest, 10);
// 	size += 10;
// 	return size;
// #pragma endregion
// 
// }

int CIPSCPeer::SendToPeer(const SOCKADDR_IN* pAddr)
{
	int rc;

	m_dwByteSent = 0;
	rc = WSASendTo(m_pWLNet->m_socket,
		&m_SendControlBuffer, 1,
		&m_dwByteSent, 0,
		(const struct sockaddr *)pAddr,
		sizeof(SOCKADDR_IN), &m_pWLNet->m_TxOverlapped,
		NULL);
	if (0 != rc){
		rc = WSAGetLastError();
	}
	return rc;
}

// u_long CIPSCPeer::GetPort()
// {
// 	return m_PeerAddr.sin_port;
// }

void CIPSCPeer::setLogPtr(PLogReport log_handel)
{
	m_report = log_handel;
}

void CIPSCPeer::sendLogToWindow()
{
	//SYSTEMTIME now = { 0 };
	//GetLocalTime(&now);
	//printf_s("%04u-%02u-%02u %02u:%02u:%02u %03u %s\n", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds, m_reportMsg);
	if (NULL != m_report)
	{
		m_report(m_reportMsg);
	}
}

void CIPSCPeer::printInfo()
{
	//char* strAddr = inet_ntoa(m_PeerAddr.sin_addr);
	//sprintf_s(m_reportMsg, "peer info->%s:%u,peer id:%lu", strAddr, htons(m_PeerAddr.sin_port), m_ulPeerID);
	//sendLogToWindow();
	//TRACE(_T("peer info->%s:%u\r\n"), (CString)strAddr, htons(m_PeerAddr.sin_port));
}

sockaddr_in CIPSCPeer::GetPeerAddressOfSockaddrin(void)
{
	return m_PeerAddr;
}

// DWORD CIPSCPeer::Build_WL_REGISTRATION_REQUEST(CHAR* pPacket, DWORD peerId, DWORD pudId, char slot)
// {
// 	DWORD size = 0;
// 
// 	/*Opcode*/
// 	pPacket[0] = WL_PROTOCOL;
// 
// 	/*peerID*/
// 	DWORD temp = peerId;
// 	pPacket[4] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	pPacket[3] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	pPacket[2] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	pPacket[1] = (char)(temp & 0x000000FF);
// 
// 	/*wirelineOpcode*/
// 	pPacket[5] = WL_REGISTRATION_REQUEST;
// 
// 	/*registrationSlotNumber*/
// 	pPacket[6] = slot;
// 
// 	/*registrationPduID*/
// 	*((DWORD*)&pPacket[7]) = htonl(pudId);
// 
// 	/*registrationID*/
// 	*((WORD*)&pPacket[11]) = htons(m_wRegistrationId);
// 
// 	/*wirelineStatusRegistration*/
// 	pPacket[13] = REGISTERED_VOICE_SERVICE;
// 
// 	/*numberOfRegistrationEntries*/
// 	pPacket[14] = 2;
// 
// 	size = 15;
// 
// 	/*记录所有的个呼*/
// 	RegistrationEntry privateCall;
// 	memset(&privateCall, 0, sizeof(RegistrationEntry));
// 	privateCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
// 	privateCall.AddressType = AllIndividualCall;
// 	//privateCall.AddressRangeStart = htonl(1);
// 	//privateCall.AddressRangeEnd = htonl(100000);
// 	memcpy(&pPacket[size], &privateCall, sizeof(RegistrationEntry));
// 	size += sizeof(privateCall);
// 
// 	/*记录所有的组呼*/
// 	RegistrationEntry allLocalTalkGroupCall;
// 	memset(&allLocalTalkGroupCall, 0, sizeof(allLocalTalkGroupCall));
// 	allLocalTalkGroupCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
// 	allLocalTalkGroupCall.AddressType = AllTalkGroupCall;
// 	//allLocalTalkGroupCall.AddressRangeStart = htonl(1);
// 	//allLocalTalkGroupCall.AddressRangeEnd = htonl(1000);
// 	memcpy(&pPacket[size], &allLocalTalkGroupCall, sizeof(RegistrationEntry));
// 	size += sizeof(allLocalTalkGroupCall);
// 
// 
// 	pPacket[size++] = Wireline_Protocol_Version;
// 	pPacket[size++] = Wireline_Protocol_Version;
// 
// 
// 	getWirelineAuthentication(pPacket, size);
// 
// 	//CHMAC_SHA1 sha1;
// 	//unsigned char digest[20];
// 	//unsigned char kw[40];
// 	//memset(kw, 0, sizeof(kw));
// 	//memcpy_s(kw, sizeof(kw), VenderKey, sizeof(VenderKey));
// 	//sha1.HMAC_SHA1((unsigned char*)pPacket, size, kw, sizeof(kw), digest);
// 
// 	//memcpy_s(pPacket + size, 10, AuthenticId, AUTHENTIC_ID_SIZE);
// 	//size += 4;
// 	//memcpy_s(pPacket + size, 20, digest, 10);
// 	//size += 10;
// 
// 	return size;
// }

DWORD CIPSCPeer::Build_WL_REGISTRATION_REQUEST(CHAR* pPacket, T_WL_PROTOCOL_01* pData)
{
	DWORD size = 0;

	/*Opcode*/
	pPacket[0] = pData->Opcode;

	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);

	/*wirelineOpcode*/
	pPacket[5] = pData->wirelineOpcode;

	/*registrationSlotNumber*/
	pPacket[6] = pData->registrationSlotNumber;

	/*registrationPduID*/
	*((DWORD*)&pPacket[7]) = htonl(pData->registrationPduID);

	/*registrationID*/
	*((WORD*)&pPacket[11]) = htons(pData->registrationID);

	/*wirelineStatusRegistration*/
	pPacket[13] = pData->wirelineStatusRegistration;

	/*numberOfRegistrationEntries*/
	pPacket[14] = pData->numberOfRegistrationEntries;

	size = 15;


	for (int i = 0; i < pData->numberOfRegistrationEntries; i++)
	{
		int index = 12 * i + 15;
		pPacket[index] = pData->wlRegistrationEntries[i].AddressType;
		*((DWORD*)&pPacket[index + 1]) = htonl(pData->wlRegistrationEntries[i].addressRangeStart);
		*((DWORD*)&pPacket[index + 5]) = htonl(pData->wlRegistrationEntries[i].addressRangeEnd);
		pPacket[index + 9] = pData->wlRegistrationEntries[i].VoiceAttributes;
		pPacket[index + 10] = pData->wlRegistrationEntries[i].CSBKAttributes;
		size += 12;
	}

	///*记录所有的个呼*/
	//RegistrationEntry privateCall;
	//memset(&privateCall, 0, sizeof(RegistrationEntry));
	//privateCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
	//privateCall.AddressType = AllIndividualCall;
	////privateCall.AddressRangeStart = htonl(1);
	////privateCall.AddressRangeEnd = htonl(100000);
	//memcpy(&pPacket[size], &privateCall, sizeof(RegistrationEntry));
	//size += sizeof(privateCall);

	///*记录所有的组呼*/
	//RegistrationEntry allLocalTalkGroupCall;
	//memset(&allLocalTalkGroupCall, 0, sizeof(allLocalTalkGroupCall));
	//allLocalTalkGroupCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
	//allLocalTalkGroupCall.AddressType = AllTalkGroupCall;
	////allLocalTalkGroupCall.AddressRangeStart = htonl(1);
	////allLocalTalkGroupCall.AddressRangeEnd = htonl(1000);
	//memcpy(&pPacket[size], &allLocalTalkGroupCall, sizeof(RegistrationEntry));
	//size += sizeof(allLocalTalkGroupCall);

	/*Current / Accepted Wireline Protocol Version*/
	pPacket[size++] = pData->currentLinkProtocolVersion;
	/*Oldest Wireline Protocol Version*/
	pPacket[size++] = pData->oldestLinkProtocolVersion;

	//CHMAC_SHA1 sha1;
	//unsigned char digest[20];
	//unsigned char kw[40];
	//memset(kw, 0, sizeof(kw));
	//memcpy_s(kw, sizeof(kw), VenderKey, sizeof(VenderKey));
	//sha1.HMAC_SHA1((unsigned char*)pPacket, size, kw, sizeof(kw), digest);

	getWirelineAuthentication(pPacket, size);

	return size;
}

// DWORD CIPSCPeer::Build_WL_VC_CHNL_CTRL_REQUEST(CHAR* pPacket, DWORD peerId, DWORD pudId, char slot)
// {
// 	DWORD size = 0;
// 
// 	/*Opcode*/
// 	pPacket[0] = WL_PROTOCOL;
// 
// 	/*peerID*/
// 	DWORD temp = peerId;
// 	pPacket[4] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	pPacket[3] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	pPacket[2] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	pPacket[1] = (char)(temp & 0x000000FF);
// 
// 	/*wirelineOpcode*/
// 	pPacket[5] = WL_VC_CHNL_CTRL_REQUEST;
// 
// 	/*registrationSlotNumber*/
// 	pPacket[6] = slot;
// 
// 	/*callID*/
// 	g_callId++;
// 	*((DWORD*)&pPacket[7]) = htonl(g_callId);
// 	//*((DWORD*)&pPacket[7]) = htonl(1);
// 
// 	/*callType*/
// 	pPacket[11] = Group_Voice_Call;
// 
// 	/*source ID*/
// 	*((DWORD*)&pPacket[12]) = htonl(g_localRadioId);
// 
// 	/*target ID*/
// 	*((DWORD*)&pPacket[16]) = htonl(g_localGroup);
// 
// 	/*accessCriteria*/
// 	pPacket[20] = Access_Criteria_Polite_Access;
// 
// 	/*callAttributes*/
// 	pPacket[21] = 0x00;//clear call
// 
// 	/*RESERVED*/
// 	pPacket[22] = 0x00;
// 
// 	/*preambleDuration*/
// 	pPacket[23] = 0x00;//Preamble Duration. For voice call set it to 0.
// 
// 	/*RESERVED*/
// 	memset(pPacket + 24, 0, 2);
// 	/*CSBK Arguments*/
// 	memset(pPacket + 26, 0, 8);//CSBK Parameters: Not needed for voice call
// 	size = 34;
// 
// 	/*Current Accepted Wireline Protocol Version*/
// 	pPacket[size++] = Wireline_Protocol_Version;
// 	/*Oldest Wireline Protocol Version*/
// 	pPacket[size++] = Wireline_Protocol_Version;
// 
// 	//CHMAC_SHA1 sha1;
// 	//unsigned char digest[20];
// 	//unsigned char kw[40];
// 	//memset(kw, 0, sizeof(kw));
// 	//memcpy_s(kw, sizeof(kw), VenderKey, sizeof(VenderKey));
// 	//sha1.HMAC_SHA1((unsigned char*)pPacket, size, kw, sizeof(kw), digest);
// 
// 	//memcpy_s(pPacket + size, 10, AuthenticId, AUTHENTIC_ID_SIZE);
// 	//size += 4;
// 	//memcpy_s(pPacket + size, 20, digest, 10);
// 	//size += 10;
// 	getWirelineAuthentication(pPacket, size);
// 
// 	return size;
// }

DWORD CIPSCPeer::Build_WL_VC_CHNL_CTRL_REQUEST(CHAR* pPacket, T_WL_PROTOCOL_13* pData)
{
	DWORD size = 0;

	/*Opcode*/
	pPacket[0] = pData->Opcode;

	/*peerID*/
	*((DWORD*)&pPacket[1]) = htonl(pData->peerID);

	/*wirelineOpcode*/
	pPacket[5] = pData->wirelineOpcode;

	/*registrationSlotNumber*/
	pPacket[6] = pData->slotNumber;

	/*callID*/
	*((DWORD*)&pPacket[7]) = htonl(pData->callID);

	/*callType*/
	pPacket[11] = pData->callType;

	/*source ID*/
	*((DWORD*)&pPacket[12]) = htonl(pData->sourceID);

	/*target ID*/
	*((DWORD*)&pPacket[16]) = htonl(pData->targetID);

	/*accessCriteria*/
	pPacket[20] = pData->accessCriteria;

	/*callAttributes*/
	pPacket[21] = pData->callAttributes;//clear call

	/*RESERVED*/
	pPacket[22] = 0x00;

	/*preambleDuration*/
	pPacket[23] = pData->preambleDuration;//Preamble Duration. For voice call set it to 0.

	/*RESERVED*/
	*((WORD*)&pPacket[24]) = 0x0000;
	/*CSBK Arguments*/
	*((unsigned long long*)&pPacket[26]) = pData->CSBKArguments;
	size = 34;

	/*Current Accepted Wireline Protocol Version*/
	pPacket[size++] = pData->currentLinkProtocolVersion;
	/*Oldest Wireline Protocol Version*/
	pPacket[size++] = pData->oldestLinkProtocolVersion;

	//CHMAC_SHA1 sha1;
	//unsigned char digest[20];
	//unsigned char kw[40];
	//memset(kw, 0, sizeof(kw));
	//memcpy_s(kw, sizeof(kw), VenderKey, sizeof(VenderKey));
	//sha1.HMAC_SHA1((unsigned char*)pPacket, size, kw, sizeof(kw), digest);

	//memcpy_s(pPacket + size, 10, AuthenticId, AUTHENTIC_ID_SIZE);
	//size += 4;
	//memcpy_s(pPacket + size, 20, digest, 10);
	//size += 10;
	getWirelineAuthentication(pPacket, size);

	return size;
}

bool CIPSCPeer::getRemote3rdParty()
{
	return m_Remote3rdParty;
}

void CIPSCPeer::setRemote3rdParty(bool value)
{
	m_Remote3rdParty = value;
}

SlotNumber_e CIPSCPeer::getUseSlot()
{
	return  m_useSlot;
}

void CIPSCPeer::getWirelineAuthentication(char* pPacket, DWORD &size)
{
	CHMAC_SHA1 sha1;
	unsigned char digest[20];
	unsigned char kw[40];
	memset(kw, 0, sizeof(kw));
	memcpy_s(kw, sizeof(kw), VenderKey, sizeof(VenderKey));
	sha1.HMAC_SHA1((unsigned char*)pPacket, size, kw, sizeof(kw), digest);

	memcpy_s(pPacket + size, 10, AuthenticId, AUTHENTIC_ID_SIZE);
	size += 4;
	memcpy_s(pPacket + size, 20, digest, 10);
	size += 10;
}

//void PASCAL CIPSCPeer::Timerwait_LE_PEER_REGISTRATION_RESPONSE(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
//{
//	CIPSCPeer* p = (CIPSCPeer *)dwUser;
//	p->Timerwait_LE_PEER_REGISTRATION_RESPONSE_Proc();
//}
//
//void PASCAL CIPSCPeer::Timerwait_LE_PEER_KEEP_ALIVE_REQUEST(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
//{
//	CIPSCPeer* p = (CIPSCPeer *)dwUser;
//	p->Timerwait_LE_PEER_KEEP_ALIVE_REQUEST_Proc();
//}
//
//void PASCAL CIPSCPeer::Timerwait_LE_PEER_KEEP_ALIVE_RESPONSE(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
//{
//	CIPSCPeer* p = (CIPSCPeer *)dwUser;
//	p->Timerwait_LE_PEER_KEEP_ALIVE_RESPONSE_Proc();
//}
//
//void CIPSCPeer::Timerwait_LE_PEER_REGISTRATION_RESPONSE_Proc()
//{
//	if (m_statusWaitPeerRegistrationResponse)
//	{
//		sprintf_s(m_reportMsg, "PEER:%luLE注册正常", m_ulPeerID);
//		sendLogToWindow();
//	}
//	else
//	{
//		sprintf_s(m_reportMsg, "PEER:%luLE注册异常", m_ulPeerID);
//		sendLogToWindow();
//	}
//	m_statusWaitPeerRegistrationResponse = 0;
//}
//
//void CIPSCPeer::Timerwait_LE_PEER_KEEP_ALIVE_REQUEST_Proc()
//{
//	if (m_statusWaitPeerAliveRequest)
//	{
//		sprintf_s(m_reportMsg, "PEER:%lu正常发送心跳包", m_ulPeerID);
//		sendLogToWindow();
//	}
//	else
//	{
//		sprintf_s(m_reportMsg, "PEER:%lu异常发送心跳包", m_ulPeerID);
//		sendLogToWindow();
//	}
//	m_statusWaitPeerAliveRequest = 0;
//}
//
//void CIPSCPeer::Timerwait_LE_PEER_KEEP_ALIVE_RESPONSE_Proc()
//{
//	if (m_statusWaitPeerAliveResponse)
//	{
//		sprintf_s(m_reportMsg, "PEER:%lu心跳包正常", m_ulPeerID);
//		sendLogToWindow();
//	}
//	else
//	{
//		sprintf_s(m_reportMsg, "PEER:%lu心跳包异常", m_ulPeerID);
//		sendLogToWindow();
//	}
//	m_statusWaitPeerAliveResponse = 0;
//}

void PASCAL CIPSCPeer::PeerStatusCheck(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	//printf_s("1");
	CIPSCPeer* p = (CIPSCPeer*)dwUser;
	if (p)
	{
		if (p->m_bKillTimer)
		{
			timeKillEvent(wTimerID);
		}
		else
		{
			p->PeerStatusCheckProc();
		}
	}
	else
	{
		timeKillEvent(wTimerID);
	}
}

void CIPSCPeer::PeerStatusCheckProc()
{
	long dif = GetTickCount() - m_startTickCount;
	switch (m_peerStatus)
	{
	case PEER_STATUS_ALIVE_REQUES:
	{
									 if (dif > CONFIG_PEER_HEART_AND_REG_TIME)
									 {
										 HandlePacket(LE_PEER_KEEP_ALIVE_REQUEST_LOCAL, NULL, 0, 0);
									 }
	}
		break;
	case PEER_STATUS_ALIVE_RESPONSE:
	{
									   if (dif > WAIT_LE_PEER_KEEP_ALIVE_RESPONSE_TIMER)
									   {
										   //sprintf_s(m_reportMsg, "PEER:%lu心跳包异常", m_ulPeerID);
										   //sendLogToWindow();
										   HandlePacket(LE_PEER_REGISTRATION_REQUEST_LOCAL, NULL, 0, 0);
									   }
	}
		break;
	case PEER_STATUS_REGIS_RESPONSE:
	{
									   if (dif > CONFIG_PEER_HEART_AND_REG_TIME)
									   {
										   //sprintf_s(m_reportMsg, "PEER:%luLE注册异常,TickCount:%lu,this:%lu", m_ulPeerID, m_startTickCount, (DWORD)this);
										   //sendLogToWindow();
										   HandlePacket(LE_PEER_REGISTRATION_REQUEST_LOCAL, NULL, 0, 0);
									   }
	}
		break;
	default:
		break;
	}
}

void CIPSCPeer::destroy()
{
	m_bKillTimer = true;
	if (0 != m_timerIdPeerStatusCheck)
	{
		if (timeKillEvent(m_timerIdPeerStatusCheck))
		{

		}
		else
		{
			//sprintf_s(m_reportMsg, "timeKillEvent fail");
			//sendLogToWindow();
		}
	}
}

void CIPSCPeer::getCallRequestRltInfo(DECLINE_REASON_CODE_INFO &declineReasonCodeInfo)
{
	unsigned char value = declineReasonCodeInfo.Value;
	memset(&declineReasonCodeInfo, 0, sizeof(DECLINE_REASON_CODE_INFO));
	declineReasonCodeInfo.Value = value;
	declineReasonCodeInfo.BhaveGet = true;
	DWORD recordType = CONFIG_RECORD_TYPE;
	switch (declineReasonCodeInfo.Value)
	{
	case 0x03:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "Race Condition Failure");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "Call Setup request is rejected during Arbitration.");
				 declineReasonCodeInfo.NewCallRetry = true;
				 declineReasonCodeInfo.HangCallRetry = true;
				 declineReasonCodeInfo.RetryOfIPSC = true;
	}
		break;
	case 0x05:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "Destination Slot Busy Failure");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "The channel which the third party application is accessing is busy.");
				 declineReasonCodeInfo.NewCallRetry = true;
				 declineReasonCodeInfo.HangCallRetry = true;
				 declineReasonCodeInfo.RetryOfIPSC = true;
	}
		break;
	case 0x06:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "Destination Group Busy Failure");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "WL Call Setup request is declined because the destination Group is busy on another channel.");
	}
		break;
	case 0x07:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "All Channels Busy Failure");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "WL Call Setup request is declined because all the channels at the site are busy.The rest channel is busy.");
				 declineReasonCodeInfo.NewCallRetry = true;
				 declineReasonCodeInfo.HangCallRetry = true;
	}
		break;
	case 0x08:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "OTA Repeat Disabled Failure");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "WL Call Setup request is declined because repeater where the request is sent is momentarily disabled by a system monitoring application");
	}
		break;

	case 0x09:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "Signal Interference Failure");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "WL Call Setup request is declined because repeater where the request is sent is suffering FCC type I or II interference.");
	}
		break;
	case 0x0a:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "CWID In Progress Failure");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "The WL Call Setup request is declined because the repeater where the request is sent is transmitting CWID.");
	}
		break;
	case 0x0b:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "TOT Expiry Premature Call End Failure");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "The call sending by the third party application is ended because of the TOT timer expiry.");
	}
		break;
	case 0x0c:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "Transmit Interrupted Call Failure");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "The WL Call Setup request w / interrupt access is failed to interrupt the ongoing OTA interrupt voice call.");
				 declineReasonCodeInfo.NewCallRetry = true;
				 declineReasonCodeInfo.HangCallRetry = true;
				 declineReasonCodeInfo.RetryOfIPSC = true;
	}
		break;
	case 0x0d:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "Higher Priority Call Takeover Failure");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "The call being sent from the third party application is preempted by another call with higher priority such as Emergency call.");
	}
		break;

	case 0x81:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "Local Group Call not allowed");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "The WL Call Setup request for starting a Local Group call is declined because the site where the request is sent is reserved for Wide Area or Private calls.");
	}
		break;
	case 0x82:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "Non-Rest Channel Repeater");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "The WL Call Setup request is received on the non - rest channel repeater.");
				 declineReasonCodeInfo.NewCallRetry = true;
	}
		break;
	case 0x83:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "Destination Site / Sites Busy");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "The WL Call Setup request to start a wide area group call is declined because the destination sites of the group do not have channels available");
	}
		break;
	case 0x84:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "The repeater end the call–due to underrun");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "The repeater,to which the third party application is sending the call, ends the call due to jitter buffer under - runs continuously for over 720ms. This may due to the network congestion.");
	}
		break;
	case 0x85:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "Undefined Call Failure");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "Any other failures.");
				 declineReasonCodeInfo.NewCallRetry = true;
				 declineReasonCodeInfo.HangCallRetry = true;
				 declineReasonCodeInfo.RetryOfIPSC = true;
	}
		break;

	case 0xa6:
	{
				 strcpy_s(declineReasonCodeInfo.ReasonCode, "All call is ongoing or in-progress");
				 strcpy_s(declineReasonCodeInfo.FailureScenarios, "progress is declined because an All Call is ongoing.");
	}
		break;
	default:
		break;
	}
}

void CIPSCPeer::setUseSlot(unsigned char value)
{
	m_useSlot = (SlotNumber_e)value;
}

bool CIPSCPeer::isSame(CIPSCPeer *pPeer)
{
	if (pPeer->GetPeerID() == m_ulPeerID)
	{
		sockaddr_in sockAddrIn = pPeer->GetPeerAddressOfSockaddrin();
		if (sockAddrIn.sin_port == m_PeerAddr.sin_port
			&& sockAddrIn.sin_addr.S_un.S_addr == m_PeerAddr.sin_addr.S_un.S_addr)
		{
			return true;
		}
	}
	return false;
}

bool CIPSCPeer::getbFirstWlRegistration()
{
	return m_bFirstWlRegistration;
}

bool CIPSCPeer::getbFirstInit()
{
	return m_bFirstInit;
}
