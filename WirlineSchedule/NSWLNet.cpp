#include "stdafx.h"
#include <process.h>
#include "NSWLNet.h"
#include "NSLog.h"
#include "NSWLPeer.h"
#include "NSSerial.h"
#include "NSRecordFile.h"
#include "HMAC_SHA1.h"
#include "NSSound.h"
#include "Manager.h"
#include "WLNet.h"
#include "NSManager.h"

#define AUTHENTIC_ID_SIZE	4
#define VENDER_KEY_SIZE		20
static const unsigned char AuthenticId[AUTHENTIC_ID_SIZE] = { 0x01, 0x02, 0x00, 0x0d };
static const unsigned char VenderKey[VENDER_KEY_SIZE] = { 0x6b, 0xe5, 0xff, 0x95, 0x6a, 0xb5, 0xe8, 0x82, 0xa8, 0x6f, 0x29, 0x5f, 0x9d, 0x9d, 0x5e, 0xcf, 0xe6, 0x57, 0x61, 0x5a };

#define SEND_VOICE_INTERVAL 60

NSWLNet::NSWLNet(NSManager* pManager)
:m_pMasterXqttnet(NULL)
, m_pLog(NSLog::instance())
, m_pWorkThread(NULL)
, m_pAmbeDataThread(NULL)
//, m_getSerailThread(NULL)
, m_pTimeOutThread(NULL)
, m_bThreadWork(false)
, m_leStatus(STARTING)
, m_xnl_status_enum(WAITLE)
, m_workItems(NULL)
, m_mutexWorkItems(INITLOCKER())
, m_workTimeOutItems(NULL)
, m_mutexWorkTimeOutItems(INITLOCKER())
, m_peers(NULL)
, m_mutexPeers(INITLOCKER())
, m_sitePeer(NULL)
, m_pSerial(new NSSerial())
, m_ambes(NULL)
, m_mutexAmbes(INITLOCKER())
, m_mutexRecords(INITLOCKER())
, m_records(NULL)
, m_pCheckRecordsThread(NULL)
, m_pManager(pManager)
, m_currentSendVoicePeer(NULL)
, m_callThreadStatus(Call_Thread_Status_Idle)
, m_callId(1)
, m_pCallThread(NULL)
, m_burstType('\0')
, m_SequenceNumber(0)
, m_Timestamp(0)
, m_ringAmbeSend(createRingBuffer(RING_SIZE_AMBE_SEND + 1, sizeof(send_ambe_voice_encoded_frames_t)))
, m_mutexAmbeSend(INITLOCKER())
, m_mutexSend(INITLOCKER())
, m_TxSubCount(0)
, m_callThreadTimer(NULL)
, m_callBackPeers(NULL)
, m_mutexCallBackPeers(INITLOCKER())
, m_localRecordFile(NULL)
{
	netInit();
	memset(&m_localIpList, 0, sizeof(local_ip_list_t));
	/*获取当前ip list*/
	GetIpList(&m_localIpList);
	g_repeater_net_mode = WL;
	memset(&m_makeCallParam, 0, sizeof(make_call_param_t));
	memset(&m_startAmbe, 0, sizeof(send_ambe_voice_encoded_frames_t));
	memset(&m_burstAmbe, 0, sizeof(send_ambe_voice_encoded_frames_t));
	initVoiceBurst();
	m_timerId = timeSetEvent(SEND_VOICE_INTERVAL, 1, SendAmbeDataProc, (DWORD)this, TIME_PERIODIC);
	memset(&CurCallCmd, 0, sizeof(CALL_OPERATE_PARAM));
}

NSWLNet::~NSWLNet()
{
	clearRingAmbeSend();
	clearMasterXqttnet();
	allThreadStop();
	if (m_pSerial) delete m_pSerial;
	m_pSerial = NULL;
	clearWorkItems();
	clearWorkTimeOutItems();
	clearCallBacks();
	clearPeers();
	clearAmbes();
	clearRecords();
	timeKillEvent(m_timerId);
	if (m_localRecordFile)
	{
		delete m_localRecordFile;
		m_localRecordFile = NULL;
	}
	DELETELOCKER(m_mutexWorkItems);
	DELETELOCKER(m_mutexWorkTimeOutItems);
	DELETELOCKER(m_mutexPeers);
	DELETELOCKER(m_mutexAmbes);
	DELETELOCKER(m_mutexRecords);
	DELETELOCKER(m_mutexAmbeSend);
	DELETELOCKER(m_mutexSend);
	DELETELOCKER(m_mutexCallBackPeers);
#ifdef WIN32
	WSACleanup();
#endif // WIN32
}

int NSWLNet::StartNet(StartNetParam* p)
{
	if (NULL == p)
	{
		return -1;
	}
	if (NULL == m_pMasterXqttnet)
	{
		/*建立相关线程*/
		int ret = allThreadStart();
		//return 0;
		if (0 != ret)
		{
			return -1;
		}

		m_netParam = *p;

		/*决定当前的工作模式*/
		g_network_mode = 0x0000;
		g_network_mode = g_network_mode | Mode_Net_WL;
		if (IPSC == m_netParam.work_mode)
		{
			g_network_mode = g_network_mode | Mode_Work_IPSC;
		}
		else if (CPC == m_netParam.work_mode)
		{
			g_network_mode = g_network_mode | Mode_Work_CPC;
		}
		else
		{
			g_network_mode = g_network_mode | Mode_Work_LCP;
		}

		if (0 != m_netParam.master_firewall_time)
		{
			g_timing_alive_time_master = m_netParam.master_firewall_time;
		}
		if (0 != m_netParam.peer_firewall_time)
		{
			g_timing_alive_time_peer = m_netParam.peer_firewall_time;
		}
		if (0 != m_netParam.hang_time)
		{
			g_hang_time = m_netParam.hang_time;
		}
		m_pMasterXqttnet = connectServerUdp(m_netParam.master_ip, m_netParam.master_port, m_netParam.local_ip, m_netParam.local_port, &m_sockaddrMaster);

		if (NULL == m_pMasterXqttnet)
		{
			m_pLog->AddLog("connectServer fail");
			return -1;
		}

		StartSerialParam param = { 0 };
		param.param = this;
		param.pMasterXqttnet = m_pMasterXqttnet;
		param.pNetParam = &m_netParam;
		param.pSerialCallBack = &SetSerialNumberCallback;
		param.pSockaddrMaster = &m_sockaddrMaster;
		if (0 != m_pSerial->Start(&param))
		{
			m_pLog->AddLog("m_pSerial Start fail");
			return -1;
		}

		/*设置数据回调*/
		m_pMasterXqttnet->param = this;
		m_pMasterXqttnet->_netOnDisconn = &OnDisconn;
		m_pMasterXqttnet->_netOnError = &OnError;
		m_pMasterXqttnet->_netOnRecv = &OnRecive;
		//m_pMasterXqttnet->_netOnSendComplete = &OnSendComplete; //no use
		if (STARTING == LeStatus())
		{

			work_item_t* p = new work_item_t;
			memset(p, 0, sizeof(work_item_t));
			Build_WorkItem_LE_90(p);
			AddWorkItem(p);
			//int size = sizeof(work_item_t);
			//char temp[64] = { 0 };
			//sprintf_s(temp, "work_item_t size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_90);
			//sprintf_s(temp, "T_LE_PROTOCOL_90 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_91);
			//sprintf_s(temp, "T_LE_PROTOCOL_91 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_92);
			//sprintf_s(temp, "T_LE_PROTOCOL_92 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_93);
			//sprintf_s(temp, "T_LE_PROTOCOL_93 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_94);
			//sprintf_s(temp, "T_LE_PROTOCOL_94 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_95);
			//sprintf_s(temp, "T_LE_PROTOCOL_95 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_96);
			//sprintf_s(temp, "T_LE_PROTOCOL_96 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_97);
			//sprintf_s(temp, "T_LE_PROTOCOL_97 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_98);
			//sprintf_s(temp, "T_LE_PROTOCOL_98 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_99);
			//sprintf_s(temp, "T_LE_PROTOCOL_99 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_9A);
			//sprintf_s(temp, "T_LE_PROTOCOL_9A size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_90_LCP);
			//sprintf_s(temp, "T_LE_PROTOCOL_90_LCP size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_91_LCP);
			//sprintf_s(temp, "T_LE_PROTOCOL_91_LCP size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_92_LCP);
			//sprintf_s(temp, "T_LE_PROTOCOL_92_LCP size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_93_LCP);
			//sprintf_s(temp, "T_LE_PROTOCOL_93_LCP size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_94);
			//sprintf_s(temp, "T_LE_PROTOCOL_94_LCP size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_95);
			//sprintf_s(temp, "T_LE_PROTOCOL_95_LCP size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_96_LCP);
			//sprintf_s(temp, "T_LE_PROTOCOL_96_LCP size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_97_LCP);
			//sprintf_s(temp, "T_LE_PROTOCOL_97_LCP size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_98_LCP);
			//sprintf_s(temp, "T_LE_PROTOCOL_98_LCP size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_99_LCP);
			//sprintf_s(temp, "T_LE_PROTOCOL_99_LCP size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_LE_PROTOCOL_9A_LCP);
			//sprintf_s(temp, "T_LE_PROTOCOL_9A_LCP size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_WL_PROTOCOL_01);
			//sprintf_s(temp, "T_WL_PROTOCOL_01 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_WL_PROTOCOL_02);
			//sprintf_s(temp, "T_WL_PROTOCOL_02 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_WL_PROTOCOL_03);
			//sprintf_s(temp, "T_WL_PROTOCOL_03 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_WL_PROTOCOL_11);
			//sprintf_s(temp, "T_WL_PROTOCOL_11 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_WL_PROTOCOL_12);
			//sprintf_s(temp, "T_WL_PROTOCOL_12 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_WL_PROTOCOL_13);
			//sprintf_s(temp, "T_WL_PROTOCOL_13 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_WL_PROTOCOL_16);
			//sprintf_s(temp, "T_WL_PROTOCOL_16 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_WL_PROTOCOL_18);
			//sprintf_s(temp, "T_WL_PROTOCOL_18 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_WL_PROTOCOL_19);
			//sprintf_s(temp, "T_WL_PROTOCOL_19 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_WL_PROTOCOL_20);
			//sprintf_s(temp, "T_WL_PROTOCOL_20 size %d bytes", size);
			//m_pLog->AddLog(temp);

			//size = sizeof(T_WL_PROTOCOL_21);
			//sprintf_s(temp, "T_WL_PROTOCOL_21 size %d bytes", size);
			//m_pLog->AddLog(temp);
		}
	}
	return 0;
}

void NSWLNet::GetSerial(char* &pSerial, int &length)
{
	length = 0;
	if (NULL == pSerial || GET_SERIAL_SUCCESS != GetXnlStatus())
	{
		return;
	}
	length = sizeof(m_serialNumber);
	memcpy(pSerial, m_serialNumber, length);
}

void NSWLNet::OnRecive(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len)
{
	NSWLNet* p = (NSWLNet*)pNet->param;
	if (p)
	{
		p->onRecive(pNet, pNetClient, pData, len);
	}
}

DWORD NSWLNet::Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90* pData)
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
	/*currentLinkProtocolVersion*/
	*((WORD*)(&pPacket[10])) = htons(pData->currentLinkProtocolVersion);
	/*oldestLinkProtocolVersion*/
	*((WORD*)(&pPacket[12])) = htons(pData->oldestLinkProtocolVersion);
	size = 14;
	return size;
}

DWORD NSWLNet::Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90_LCP* pData)
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
	/*leadingChannelID*/
	pPacket[11] = pData->leadingChannelID;
	/*currentLinkProtocolVersion*/
	*((WORD*)(&pPacket[12])) = htons(pData->currentLinkProtocolVersion);
	/*oldestLinkProtocolVersion*/
	*((WORD*)(&pPacket[14])) = htons(pData->oldestLinkProtocolVersion);
	size = 16;
	return size;
}

void NSWLNet::OnDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode)
{
	NSWLNet* p = (NSWLNet*)pNet->param;
	if (p)
	{
		p->onDisconn(pNet, pNetClient, errCode);
	}
}

void NSWLNet::OnSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient)
{
	NSWLNet* p = (NSWLNet*)pNet->param;
	if (p)
	{
		p->onSendComplete(pNet, pNetClient);
	}
}

void NSWLNet::OnError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode)
{
	NSWLNet* p = (NSWLNet*)pNet->param;
	if (p)
	{
		p->onError(pNet, pNetClient, errCode);
	}
}

void NSWLNet::onRecive(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len)
{

	work_item_t *pItem = new work_item_t;
	memset(pItem, 0, sizeof(work_item_t));
	pItem->type = Recive;
	moto_protocol_t* pProtocol = &pItem->data.recive_data.protocol;
	char Opcode = '\0';
	Opcode = *pData;
	//char temp[64] = { 0 };
	//sprintf_s(temp, "onRecive Opcode: 0x%02x", Opcode);
	//m_pLog->AddLog(temp);
	item_oprate_enum OperateFlag = Oprate_Add;
	switch (Opcode)
	{
	case LE_MASTER_PEER_REGISTRATION_RESPONSE:
	{
												 if (LCP == m_netParam.work_mode)
												 {
													 pProtocol->le_lcp.PROTOCOL_91_LCP.length = len;
													 Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE((char*)pData, pProtocol->le_lcp.PROTOCOL_91_LCP);
												 }
												 else
												 {
													 pProtocol->le.PROTOCOL_91.length = len;
													 Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE((char*)pData, pProtocol->le.PROTOCOL_91);
												 }
	}
		break;
	case LE_NOTIFICATION_MAP_BROADCAST:
	{
										  if (LCP == m_netParam.work_mode)
										  {
											  pProtocol->le_lcp.PROTOCOL_93_LCP.length = len;
											  Unpack_LE_NOTIFICATION_MAP_BROADCAST((char*)pData, pProtocol->le_lcp.PROTOCOL_93_LCP);
										  }
										  else
										  {
											  pProtocol->le.PROTOCOL_93.length = len;
											  Unpack_LE_NOTIFICATION_MAP_BROADCAST((char*)pData, pProtocol->le.PROTOCOL_93);
										  }
	}
		break;
	case LE_PEER_REGISTRATION_REQUEST:
	{
										 if (LCP == m_netParam.work_mode)
										 {
											 pProtocol->le_lcp.PROTOCOL_94_LCP.length = len;
											 Unpack_LE_PEER_REGISTRATION_REQUEST((char*)pData, pProtocol->le_lcp.PROTOCOL_94_LCP);
										 }
										 else
										 {
											 pProtocol->le.PROTOCOL_94.length = len;
											 Unpack_LE_PEER_REGISTRATION_REQUEST((char*)pData, pProtocol->le.PROTOCOL_94);
										 }
	}
		break;
	case LE_PEER_REGISTRATION_RESPONSE:
	{
										  if (LCP == m_netParam.work_mode)
										  {
											  pProtocol->le_lcp.PROTOCOL_95_LCP.length = len;
											  Unpack_LE_PEER_REGISTRATION_RESPONSE((char*)pData, pProtocol->le_lcp.PROTOCOL_95_LCP);
										  }
										  else
										  {
											  pProtocol->le.PROTOCOL_95.length = len;
											  Unpack_LE_PEER_REGISTRATION_RESPONSE((char*)pData, pProtocol->le.PROTOCOL_95);
										  }
	}
		break;
	case LE_MASTER_KEEP_ALIVE_RESPONSE:
	{
										  if (LCP == m_netParam.work_mode)
										  {
											  pProtocol->le_lcp.PROTOCOL_97_LCP.length = len;
											  Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE((char*)pData, pProtocol->le_lcp.PROTOCOL_97_LCP);
										  }
										  else
										  {
											  pProtocol->le.PROTOCOL_97.length = len;
											  Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE((char*)pData, pProtocol->le.PROTOCOL_97);
										  }
	}
		break;
	case LE_PEER_KEEP_ALIVE_REQUEST:
	{
									   if (LCP == m_netParam.work_mode)
									   {
										   pProtocol->le_lcp.PROTOCOL_98_LCP.length = len;
										   Unpack_LE_PEER_KEEP_ALIVE_REQUEST((char*)pData, pProtocol->le_lcp.PROTOCOL_98_LCP);
									   }
									   else
									   {
										   pProtocol->le.PROTOCOL_98.length = len;
										   Unpack_LE_PEER_KEEP_ALIVE_REQUEST((char*)pData, pProtocol->le.PROTOCOL_98);
									   }
	}
		break;
	case LE_PEER_KEEP_ALIVE_RESPONSE:
	{
										if (LCP == m_netParam.work_mode)
										{
											pProtocol->le_lcp.PROTOCOL_99_LCP.length = len;
											Unpack_LE_PEER_KEEP_ALIVE_RESPONSE((char*)pData, pProtocol->le_lcp.PROTOCOL_99_LCP);
										}
										else
										{
											pProtocol->le.PROTOCOL_99.length = len;
											Unpack_LE_PEER_KEEP_ALIVE_RESPONSE((char*)pData, pProtocol->le.PROTOCOL_99);
										}
	}
		break;
	case WL_PROTOCOL:
	{
						int index = 0;
						moto_protocol_wl_t* pWl = &pProtocol->wl;
						pWl->PROTOCOL_01.Opcode = pData[index];
						index += sizeof(unsigned char);
						pWl->PROTOCOL_01.peerID = ntohl(*(unsigned long*)&pData[index]);
						index += sizeof(unsigned long);
						pWl->PROTOCOL_01.wirelineOpcode = pData[index];
						index += sizeof(unsigned char);
						Unpack_WL(pData, len, pWl, OperateFlag);
	}
		break;
	case LE_XNL:
	{
				   m_pSerial->RECV_LE_XCMP_XNL(pItem, pData);
				   m_pSerial->AddXnlItem(pItem);
				   OperateFlag = Oprate_Other;
	}
		break;
	case P2P_GRP_VOICE_CALL:
	case P2P_PVT_VOICE_CALL:
	case P2P_GRP_DATA_CALL:
	case P2P_PVT_DATA_CALL:
	case LE_DEREGISTRATION_RESPONSE:
	case LE_DEREGISTRATION_REQUEST:
	case LE_MASTER_KEEP_ALIVE_REQUEST:
	case LE_NOTIFICATION_MAP_REQUEST:
	case LE_MASTER_PEER_REGISTRATION_REQUEST:
	default:
		OperateFlag = Oprate_Del;
		break;
	}
	if (Oprate_Add == OperateFlag)
	{
		AddWorkItem(pItem);
	}
	else if (Oprate_Del == OperateFlag)
	{
		delete pItem;
		pItem = NULL;

	}
}

void NSWLNet::onDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode)
{
	m_pLog->AddLog("onDisconn,errCode:%d", errCode);
	clearMasterXqttnet();
}

void NSWLNet::onSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient)
{
	m_pLog->AddLog("onSendComplete");
}

void NSWLNet::onError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode)
{
	m_pLog->AddLog("onError");
	clearMasterXqttnet();
}

int NSWLNet::allThreadStart()
{
	if (m_bThreadWork)
	{
		return 0;
	}
	unsigned int pThreadId = 0;
	/*工作线程*/
	m_pWorkThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		WorkThreadProc,
		this,
		CREATE_SUSPENDED,
		&pThreadId
		);
	if (NULL == m_pWorkThread)
	{
		m_pLog->AddLog("WorkThreadProc create fail");
		return -1;
	}
	/*超时线程*/
	m_pTimeOutThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		TimeOutThreadProc,
		this,
		CREATE_SUSPENDED,
		&pThreadId
		);
	if (NULL == m_pTimeOutThread)
	{
		m_pLog->AddLog("TimeOutThreadProc create fail");
		return -1;
	}
	/*Ambe数据处理线程*/
	m_pAmbeDataThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		AmbeDataThreadProc,
		this,
		CREATE_SUSPENDED,
		&pThreadId
		);
	if (NULL == m_pAmbeDataThread)
	{
		m_pLog->AddLog("AmbeDataThreadProc create fail");
		return -1;
	}
	/*获取序列号线程*/
	m_pCheckRecordsThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		CheckRecordsThreadProc,
		this,
		CREATE_SUSPENDED,
		&pThreadId
		);
	if (NULL == m_pCheckRecordsThread)
	{
		m_pLog->AddLog("CheckRecordsThreadProc create fail");
		return -1;
	}
	/*通话线程*/
	m_pCallThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		CallThreadProc,
		this,
		CREATE_SUSPENDED,
		&pThreadId
		);
	if (NULL == m_pCallThread)
	{
		m_pLog->AddLog("CallThreadProc create fail");
		return -1;
	}
	///*获取序列号线程*/
	//m_getSerailThread = (HANDLE)_beginthreadex(
	//	NULL,
	//	0,
	//	GetSerialThreadProc,
	//	this,
	//	CREATE_SUSPENDED,
	//	&pThreadId
	//	);
	//if (NULL == m_getSerailThread)
	//{
	//	m_pLog->AddLog("GetSerialThreadProc create fail");
	//	return -1;
	//}
	/*启动线程*/
	m_bThreadWork = true;
	ResumeThread(m_pCheckRecordsThread);
	ResumeThread(m_pAmbeDataThread);
	//ResumeThread(m_getSerailThread);
	ResumeThread(m_pTimeOutThread);
	ResumeThread(m_pWorkThread);
	ResumeThread(m_pCallThread);
	return 0;
}

void NSWLNet::allThreadStop()
{
	m_bThreadWork = false;
	/*停止工作线程*/
	if (NULL != m_pWorkThread)
	{
		WaitForSingleObject(m_pWorkThread, 1000);
		CloseHandle(m_pWorkThread);
		m_pWorkThread = NULL;
	}
	/*停止超时线程*/
	if (NULL != m_pTimeOutThread)
	{
		WaitForSingleObject(m_pTimeOutThread, 1000);
		CloseHandle(m_pTimeOutThread);
		m_pTimeOutThread = NULL;
	}
	/*停止Ambe数据处理线程*/
	if (NULL != m_pAmbeDataThread)
	{
		WaitForSingleObject(m_pAmbeDataThread, 1000);
		CloseHandle(m_pAmbeDataThread);
		m_pAmbeDataThread = NULL;
	}
	if (NULL != m_pCheckRecordsThread)
	{
		WaitForSingleObject(m_pCheckRecordsThread, 1000);
		CloseHandle(m_pCheckRecordsThread);
		m_pCheckRecordsThread = NULL;
	}

	if (NULL != m_pCallThread)
	{
		ContinueCallThread();
		WaitForSingleObject(m_pCallThread, 1000);
		CloseHandle(m_pCallThread);
		m_pCallThread = NULL;
	}
	///*停止获取序列号线程*/
	//if (NULL != m_getSerailThread)
	//{
	//	WaitForSingleObject(m_getSerailThread, 1000);
	//	CloseHandle(m_getSerailThread);
	//	m_getSerailThread = NULL;
	//}
}

unsigned int __stdcall NSWLNet::WorkThreadProc(void* pArguments)
{
	NSWLNet* p = (NSWLNet*)pArguments;
	if (p)
	{
		p->WorkThread();
	}
	return 0;
}

void NSWLNet::WorkThread()
{
	m_pLog->AddLog("WorkThread start");
	work_mode_enum workMode = m_netParam.work_mode;
	find_peer_condition_t condition = { 0 };
	item_oprate_enum OpreateFlag = Oprate_Del;
	char Opcode = '\0';
	char temp[1024] = { 0 };
	while (m_bThreadWork)
	{
		work_item_t* curItem = PopWorkItem();
		if (NULL == curItem)
		{
			Sleep(SLEEP_WORK_THREAD);
			continue;
		}
		OpreateFlag = Oprate_Del;
		work_item_type_enum type = curItem->type;
		switch (type)
		{
		case Recive:
		{

					   Opcode = curItem->data.recive_data.protocol.le.PROTOCOL_90.Opcode;
					   //sprintf_s(temp, "WorkThread Recive Opcode:0x%02x", Opcode);
					   //m_pLog->AddLog(temp);
					   switch (LeStatus())
					   {
					   case STARTING:
					   {
										Handle_Le_Status_Starting_Recive(Opcode, curItem, OpreateFlag);
					   }
						   break;
					   case WAITFOR_LE_NOTIFICATION_MAP_BROADCAST:
					   {
																	 Handle_Le_Status_WaitMap_Recive(Opcode, curItem, OpreateFlag);
					   }
						   break;
					   case ALIVE:
					   {
									 Handle_Le_Status_Alive_Recive(Opcode, curItem, OpreateFlag);
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
		if (Oprate_Add == OpreateFlag)
		{
			AddWorkTimeOutItem(curItem);
		}
		else if (Oprate_Del == OpreateFlag)
		{
			//sprintf_s(temp, "WorkThread %s Opcode:0x%02x Delete", (Recive == curItem->type) ? "Recive" : "Send", Opcode);
			//m_pLog->AddLog(temp);
			delete curItem;
			curItem = NULL;
		}
	}
	m_pLog->AddLog("WorkThread exit");
}

unsigned int __stdcall NSWLNet::TimeOutThreadProc(void* pArguments)
{
	NSWLNet* p = (NSWLNet*)pArguments;
	if (p)
	{
		p->TimeOutThread();
	}
	return 0;
}

void NSWLNet::TimeOutThread()
{
	m_pLog->AddLog("TimeOutThread start");
	work_mode_enum workMode = m_netParam.work_mode;
	find_peer_condition_t condition = { 0 };
	char Opcode = '\0';
	char temp[1024] = { 0 };
	work_item_type_enum type;
	send_data_t* pSendData = NULL;
	recive_data_t* pReciveData = NULL;
	item_oprate_enum OpreateFlag = Oprate_Del;
	while (m_bThreadWork)
	{
		work_item_t* curItem = PopWorkTimeOutItem();
		if (NULL == curItem)
		{
			Sleep(SLEEP_TIMEOUT_THREAD);
			continue;
		}
		OpreateFlag = Oprate_Del;
		type = curItem->type;
		switch (type)
		{
		case Send:
		{
					 pSendData = &curItem->data.send_data;
					 Opcode = pSendData->protocol.le.PROTOCOL_90.Opcode;
					 //sprintf_s(temp, "TimeOutThread Opcode Send:0x%02x", Opcode);
					 //m_pLog->AddLog(temp);

					 switch (LeStatus())
					 {
					 case STARTING://处理0x90
					 {
									   Handle_Le_Status_Starting_TimeOut_Send(Opcode, curItem, OpreateFlag);
					 }
						 break;
					 case WAITFOR_LE_NOTIFICATION_MAP_BROADCAST://处理0x92
					 {
																	Handle_Le_Status_WaitMap_TimeOut_Send(Opcode, curItem, OpreateFlag);
					 }
						 break;
					 case ALIVE://处理 0x94 0x96 0x98 0x99 0xb2_01
					 {
									Handle_Le_Status_Alive_TimeOut_Send(Opcode, curItem, OpreateFlag);
					 }
						 break;
					 default:
						 break;
					 }
		}
			break;
		case Recive:
		{
					   pReciveData = &curItem->data.recive_data;
					   Opcode = pReciveData->protocol.le.PROTOCOL_90.Opcode;
					   //sprintf_s(temp, "TimeOutThread Opcode Recive:0x%02x", Opcode);
					   //m_pLog->AddLog(temp);
					   switch (LeStatus())
					   {
					   case STARTING://0x91
					   {
										 Handle_Le_Status_Starting_TimeOut_Recive(Opcode, curItem, OpreateFlag);
					   }
						   break;
					   case WAITFOR_LE_NOTIFICATION_MAP_BROADCAST://0x93
					   {
																	  Handle_Le_Status_WaitMap_TimeOut_Recive(Opcode, curItem, OpreateFlag);
					   }
						   break;
					   case ALIVE://0x95,0x97,0x98,0x99,0xb2_02
					   {
									  Handle_Le_Status_Alive_TimeOut_Recive(Opcode, curItem, OpreateFlag);
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
		if (Oprate_Del == OpreateFlag)
		{
			//sprintf_s(temp, "TimeOutThread %s Opcode:0x%02x Delete", (Recive == curItem->type) ? "Recive" : "Send", Opcode);
			//m_pLog->AddLog(temp);
			delete curItem;
			curItem = NULL;
		}
		/*规定时间范围内处理一次超时*/
		Sleep(SLEEP_TIMEOUT_THREAD);
	}
	m_pLog->AddLog("TimeOutThread exit");
}

unsigned int __stdcall NSWLNet::AmbeDataThreadProc(void* pArguments)
{
	NSWLNet* p = (NSWLNet*)pArguments;
	if (p)
	{
		p->AmbeDataThread();
	}
	return 0;
}

void NSWLNet::AmbeDataThread()
{
	m_pLog->AddLog("AmbeDataThread start");
	item_oprate_enum OpreateFlag = Oprate_Del;
	work_item_t* curItem = NULL;
	moto_protocol_wl_t* protocol = NULL;
	find_record_condition_t condition = { 0 };
	char wlOpcode = '\0';
	NSRecordFile* record = NULL;
	NSWLPeer* peer = NULL;
	find_peer_condition_t conditionPeer = { 0 };
	bool bFind = true;
	unsigned char src_slot = 0x00;
	unsigned long call_id = 0;
	unsigned char call_type = 0x00;
	unsigned long src_radio = 0;
	unsigned long target_radio = 0;
	unsigned long src_peer_id = 0;
	while (m_bThreadWork)
	{
		curItem = PopAmbesItem();
		if (NULL == curItem)
		{
			Sleep(SLEEP_AMBE_THREAD);
			continue;
		}
		protocol = &curItem->data.recive_data.protocol.wl;
		T_WL_PROTOCOL_18* param = &protocol->PROTOCOL_18;
		wlOpcode = param->wirelineOpcode;
		src_slot = param->slotNumber;
		call_id = param->callID;
		call_type = param->callType;
		src_radio = param->sourceID;
		target_radio = param->targetID;
		src_peer_id = param->peerID;
		conditionPeer.peer_id = src_peer_id;
		peer = FindPeersItem(&conditionPeer);
		switch (wlOpcode)
		{
		case WL_VC_VOICE_START:
		{
								  T_WL_PROTOCOL_18* handleData = &protocol->PROTOCOL_18;
								  record = new NSRecordFile(m_pManager, peer);
								  record->src_peer_id = src_peer_id;
								  record->src_radio = src_radio;
								  record->target_radio = target_radio;
								  record->call_id = call_id;
								  record->call_type = call_type;
								  record->src_slot = src_slot;
								  record->setCallStatus(VOICE_START);
								  AddRecordsItem(record);
		}
			break;
		case WL_VC_VOICE_BURST:
		{
								  T_WL_PROTOCOL_21 *handleData = &protocol->PROTOCOL_21;
								  char voiceFrame[21] = { 0 };
								  Handle_WL_PROTOCOL_21_Ambe_Data(voiceFrame, handleData->AMBEVoiceEncodedFrames.data);
								  condition.call_id = call_id;
								  condition.src_radio = src_radio;
								  condition.target_radio = target_radio;
								  record = FindOrAddRecordsItem(&condition, bFind, peer);
								  if (!bFind)
								  {
									  record->src_peer_id = src_peer_id;
									  record->src_radio = src_radio;
									  record->target_radio = target_radio;
									  record->call_id = call_id;
									  record->call_type = call_type;
									  record->src_slot = src_slot;
									  record->setCallStatus(VOICE_START);
								  }
								  record->setCallStatus(VOICE_BURST);
								  record->WriteVoiceFrame(voiceFrame, 21);
								  record->src_rssi = handleData->rawRssiValue;
		}
			break;
		case WL_VC_VOICE_END_BURST:
		{
									  condition.call_id = call_id;
									  condition.src_radio = src_radio;
									  condition.target_radio = target_radio;
									  record = FindRecordsItem(&condition);
									  if (record)
									  {
										  record->setCallStatus(VOICE_END_BURST);
										  /*写入数据库*/
										  record->WriteToDb();
									  }
		}
			break;
		case WL_VC_CALL_SESSION_STATUS:
		{
										  T_WL_PROTOCOL_20* handleData = &protocol->PROTOCOL_20;
										  unsigned char callSessionStatus = handleData->callSessionStatus;
										  condition.call_id = call_id;
										  condition.src_radio = src_radio;
										  condition.target_radio = target_radio;
										  m_pLog->AddLog("%s From %lu To %lu", (Call_Session_End == callSessionStatus) ? "Call_Session_End" : "Call_Session_Call_Hang", src_radio, target_radio);
										  if (Call_Session_Call_Hang == callSessionStatus)
										  {
											  /*增加到回叫队列*/
											  AddCallBacksItem(peer);
										  }
										  record = FindRecordsItem(&condition);
										  if (record)
										  {
											  if (Call_Session_End == callSessionStatus)
											  {
												  /*更新通话状态*/
												  record->setCallStatus(CALL_SESSION_STATUS_END);
												  /*更新通话状态到数据库*/
												  record->WriteToDb();
												  /*从处理容器中移除*/
												  RemoveRecordsItem(record);
												  /*删除此记录*/
												  delete record;
												  record = NULL;
											  }
											  else if (Call_Session_Call_Hang == callSessionStatus)
											  {
												  /*更新通话状态*/
												  record->setCallStatus(CALL_SESSION_STATUS_HANG);
											  }
										  }
										  /*如果是由本地发出的通话*/
										  else if (src_radio == m_netParam.local_radio_id)
										  {
											  if (m_localRecordFile)
											  {
												  if (Call_Session_End == callSessionStatus)
												  {
													  /*更新通话状态*/
													  m_localRecordFile->setCallStatus(CALL_SESSION_STATUS_END);
													  /*更新通话状态到数据库*/
													  m_localRecordFile->WriteToDb();
													  /*删除此记录*/
													  delete m_localRecordFile;
													  m_localRecordFile = NULL;
												  }
												  else if (Call_Session_Call_Hang == callSessionStatus)
												  {
													  /*更新通话状态*/
													  memcpy(m_localRecordFile->SessionId, CurCallCmd.SessionId, SESSION_SIZE);
													  m_localRecordFile->setCallStatus(CALL_SESSION_STATUS_HANG);
												  }
											  }
										  }
		}
			break;
		default:
			break;
		}
		if (Oprate_Del == OpreateFlag)
		{
			delete curItem;
			curItem = NULL;
		}
	}
	m_pLog->AddLog("AmbeDataThread exit");
}

//unsigned int __stdcall WLNet::GetSerialThreadProc(void* pArguments)
//{
//	WLNet* p = (WLNet*)pArguments;
//	if (p)
//	{
//
//		p->GetSerialThread();
//	}
//	return 0;
//}
//
//void WLNet::GetSerialThread()
//{
//	NSLog* m_log = m_pLog;
//	m_log->AddLog("get serial Thread start");
//	while (m_bThreadWork)
//	{
//		work_item_t* currentItem = PopSerialItem();
//		if (NULL == currentItem)
//		{
//			if (XNL_CONNECT == GetXnlStatus() || WAITFOR_XNL_MASTER_STATUS_BROADCAST == GetXnlStatus()
//				|| WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY == GetXnlStatus() || WAITFOR_XNL_DEVICE_CONNECT_REPLY == GetXnlStatus())
//			{
//				work_item_t* pItem = new work_item_t;
//				memset(pItem, 0, sizeof(work_item_t));
//				SEND_LE_XCMP_XNL_DEVICE_MASTER_QUERY(pItem);
//				AddXnlItem(pItem);
//				SetXnlStatus(WAITFOR_XNL_MASTER_STATUS_BROADCAST);   //等待接收 0x0002
//			}
//			if (WAITFOR_XNL_XCMP_READ_SERIAL == GetXnlStatus() || WAITFOR_XNL_XCMP_READ_SERIAL_RESULT == GetXnlStatus())
//			{
//				work_item_t* pItem = new work_item_t;
//				memset(pItem, 0, sizeof(work_item_t));
//				SEND_LE_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(pItem);
//				AddXnlItem(pItem);
//				SetXnlStatus(WAITFOR_XNL_XCMP_READ_SERIAL_RESULT);   //等待接收序列号
//			}
//			Sleep(SLEEP_GETSERIAL_THREAD);
//			continue;
//		}
//		work_item_type_enum type = currentItem->type;
//		switch (type)
//		{
//		case Send:
//			SendXnlToMaster(currentItem);
//			break;
//		case Recive:
//		{
//					   unsigned short opcode = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_MASTER_QUERY.Opcode;
//					   switch (GetXnlStatus())
//					   {
//					   case WAITFOR_XNL_MASTER_STATUS_BROADCAST:  //等待接收0x0002
//						   if (LE_XNL_MASTER_STATUS_BRDCST == opcode)
//						   {
//							   work_item_t* pItem = new work_item_t;
//							   memset(pItem, 0, sizeof(work_item_t));
//							   pItem->data.send_data.protocol.xcmp.Payload.DEVICE_AUTH_KEY_REQUEST.DesAddress = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_SYSMAP_BRDCST.SourceAddress;
//							   SEND_LE_XCMP_XNL_DEVICE_AUTH_KEY_REQUEST(pItem);
//							   AddXnlItem(pItem);
//							   SetXnlStatus(WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY);   //等待接收 0x0005
//							   break;
//						   }
//
//					   case WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY:
//						   if (LE_XNL_DEVICE_AUTH_KEY_REPLY == opcode)
//						   {
//							   work_item_t* pItem = new work_item_t;
//							   memset(pItem, 0, sizeof(work_item_t));
//							   pItem->data.send_data.protocol.xcmp.Payload.DEVICE_CONN_REQUEST.SourceAddress = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_AUTH_KEY_REPLY.TemporaryXnlAddress;
//							   unsigned char * unAuth = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_AUTH_KEY_REPLY.UnencryptedAuthenticationValue;
//							   unsigned char*  enAuth = pItem->data.recive_data.protocol.xcmp.Payload.DEVICE_CONN_REQUEST.EncryptedAuthenticationValue;
//							   EncryptAuthenticationValue(unAuth, enAuth);
//							   SEND_LE_XCMP_XNL_DEVICE_CONN_REQUEST(pItem);
//							   AddXnlItem(pItem);
//							   SetXnlStatus(WAITFOR_XNL_DEVICE_CONNECT_REPLY);   //等待接收 0x0007
//							   break;
//						   }
//
//					   case WAITFOR_XNL_DEVICE_CONNECT_REPLY:
//						   if (LE_XNL_DEVICE_CONN_REPLY == opcode)
//						   {
//							   if (LE_XNL_XCMP_CONNECT_SUCCESS == currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_CONN_REPLY.ResultCode)
//							   {
//								   work_item_t* pItem = new work_item_t;
//								   memset(pItem, 0, sizeof(work_item_t));
//								   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.TrascationId = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;
//								   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.XnlFlags = ++(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags);
//								   SEND_LE_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(pItem);
//								   AddXnlItem(pItem);
//								   SetXnlStatus(WAITFOR_XNL_XCMP_READ_SERIAL_RESULT);   //等待接收序列号
//							   }
//							   else
//							   {
//								   SetXnlStatus(XNL_CONNECT);// 连接失败后重新开始连接
//							   }
//						   }
//					   case WAITFOR_XNL_XCMP_READ_SERIAL_RESULT:
//						   if (LE_XNL_DATA_MSG == opcode)
//						   {
//							   work_item_t* pItem = new work_item_t;
//							   memset(pItem, 0, sizeof(work_item_t));
//							   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.XnlFlags = ++(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags);
//							   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.TrascationId = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;
//							   SEND_LE_XCMP_XNL_DATA_MSG_ACK(pItem);
//							   AddXnlItem(pItem);
//							   //若取到序列号，此处直接跳出该线程；若没有取得序列号，则继续发送获取序列号
//							   unsigned short payloadOpcode = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.Opcode;
//							   char result = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.Result;
//							   if (LE_XNL_XCMP_RADIO_STATUS_REPLY == payloadOpcode && LE_XNL_XCMP_READ_SERIAL_SUCCESS == result)
//							   {
//								   //memcpy(m_serialNumber, &(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.ProductSerialNumber[0]), sizeof(m_serialNumber));
//								   unsigned char* pSerial = &(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.ProductSerialNumber[0]);
//								   SetSerialNumber(pSerial);
//								   SetXnlStatus(GET_SERIAL_SUCCESS);
//							   }
//							   else
//							   {
//								   SetXnlStatus(WAITFOR_XNL_XCMP_READ_SERIAL);
//							   }
//
//						   }
//						   break;
//					   case GET_SERIAL_SUCCESS:
//						   break;
//					   default:
//						   if (LE_XNL_DATA_MSG == opcode)    //接收xnl_data_msg 后，回复xnl_data_msg_ack
//						   {
//							   work_item_t* pItem = new work_item_t;
//							   memset(pItem, 0, sizeof(work_item_t));
//							   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.XnlFlags = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags;
//							   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.TrascationId = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;
//							   SEND_LE_XCMP_XNL_DATA_MSG_ACK(pItem);
//							   AddXnlItem(pItem);
//						   }
//						   break;
//					   }
//		}
//			break;
//		default:
//			break;
//		}
//	}
//	m_log->AddLog("get serial Thread end");
//}

void NSWLNet::clearMasterXqttnet()
{
	if (NULL != m_pMasterXqttnet)
	{
		m_pMasterXqttnet->param = NULL;
		m_pMasterXqttnet->_netOnDisconn = NULL;
		m_pMasterXqttnet->_netOnError = NULL;
		m_pMasterXqttnet->_netOnRecv = NULL;
		m_pMasterXqttnet = NULL;
		SetLeStatus(STARTING);
		SetXnlStatus(WAITLE);
	}
}

void NSWLNet::SetLeStatus(le_status_enum value)
{
	if (m_leStatus != value)
	{
		le_status_enum old = m_leStatus;
		char temp[64] = { 0 };
		sprintf_s(temp, "=====Le Status From %d To %d=====", m_leStatus, value);
		m_pLog->AddLog(temp);
		m_leStatus = value;
		if (m_leStatus > WAITFOR_MAP_REQUEST_TX)
		{
			if (g_manager)
			{
				g_manager->setLEStatus(WL_SYSTEM_CONNECT);
			}
		}
		else
		{
			if (g_manager)
			{
				g_manager->setLEStatus(WL_SYSTEM_DISCONNECT);
			}
		}

		FieldValue info(FieldValue::TInt);
		/*连接成功*/
		if (ALIVE == m_leStatus)
		{
			if (g_pNet)
			{
				info.setInt(REPEATER_CONNECT);
				g_pNet->wlInfo(GET_TYPE_CONN, info, "");
			}
		}
		/*断开连接*/
		if (ALIVE == old)
		{
			if (g_pNet)
			{
				info.setInt(REPEATER_DISCONNECT);
				g_pNet->wlInfo(GET_TYPE_CONN, info, "");
			}
		}
	}
}

void NSWLNet::clearWorkItems()
{
	work_item_t* p = NULL;
	TRYLOCK(m_mutexWorkItems);
	pLinkItem item = popFront(&m_workItems);
	while (item)
	{
		if (NULL != item->data)
		{
			p = (work_item_t*)item->data;
#if _DEBUG
			char temp[128] = { 0 };
			unsigned char Opcode = '\0';
			if (Recive == p->type)
			{
				Opcode = p->data.recive_data.protocol.le.PROTOCOL_90.Opcode;
				sprintf_s(temp, "Recive Opcode:0x%02x Delete", Opcode);
			}
			else
			{
				Opcode = p->data.send_data.protocol.le.PROTOCOL_90.Opcode;
				sprintf_s(temp, "Send Opcode:0x%02x Delete", Opcode);
			}
			m_pLog->AddLog(temp);
#endif // _DEBUG

			delete p;
			p = NULL;
			item->data = NULL;
		}
		freeList(item);
		item = popFront(&m_workItems);
	}
	m_workItems = NULL;
	RELEASELOCK(m_mutexWorkItems);
}

le_status_enum NSWLNet::LeStatus()
{
	return m_leStatus;
}

xnl_status_enum NSWLNet::GetXnlStatus()
{
	return m_pSerial->GetXnlStatus();
}

void NSWLNet::SetXnlStatus(xnl_status_enum value)
{
	//if (m_xnl_status_enum != value)
	//{
	//	char temp[64] = { 0 };
	//	sprintf_s(temp, "=====Serial Status From %d To %d=====", m_xnl_status_enum, value);
	//	m_pLog->AddLog(temp);
	//	m_xnl_status_enum = value;
	//}
	m_pSerial->SetXnlStatus(value);
}

void NSWLNet::AddWorkItem(work_item_t* p)
{
	if (Send == p->type)
	{
		SendDataToMaster(p);
	}
	else
	{
		TRYLOCK(m_mutexWorkItems);
		appendData(&m_workItems, p);
		RELEASELOCK(m_mutexWorkItems);
	}

}

void NSWLNet::Build_WorkItem_LE_90(work_item_t* p)
{
	if (NULL == p)
	{
		return;
	}
	p->type = Send;
	p->data.send_data.send_to = &m_sockaddrMaster;
	p->data.send_data.timeout_try = 0xffff;//0x90为所有package的开始,理论重发次数无限
	if (LCP == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_90_LCP *pNetworkData = NULL;
		send_data_t* pSendData = &p->data.send_data;
		pNetworkData = &pSendData->protocol.le_lcp.PROTOCOL_90_LCP;
		pNetworkData->currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
		pNetworkData->leadingChannelID = LEADING_CHANNEL_ID;
		pNetworkData->oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
		pNetworkData->Opcode = LE_MASTER_PEER_REGISTRATION_REQUEST;
		pNetworkData->peerID = m_netParam.local_peer_id;
		pNetworkData->peerMode = LCP_MODE;
		pNetworkData->peerServices = LCP_SERVICES;
		pSendData->net_lenth = Build_LE_MASTER_PEER_REGISTRATION_REQUEST(pSendData->net_data, pNetworkData);
	}
	else
	{
		T_LE_PROTOCOL_90 *pNetworkData = NULL;
		send_data_t* pSendData = &p->data.send_data;
		pNetworkData = &pSendData->protocol.le.PROTOCOL_90;
		pNetworkData->Opcode = LE_MASTER_PEER_REGISTRATION_REQUEST;
		pNetworkData->peerID = m_netParam.local_peer_id;
		pNetworkData->currentLinkProtocolVersion = CPC_CURRENTLPVERSION;
		pNetworkData->oldestLinkProtocolVersion = CPC_OLDESTPVERSION;
		pNetworkData->peerMode = CPC_MODE;
		pNetworkData->peerServices = CPC_SERVICES;
		pSendData->net_lenth = Build_LE_MASTER_PEER_REGISTRATION_REQUEST(pSendData->net_data, pNetworkData);
	}
}

work_item_t* NSWLNet::PopWorkItem()
{
	work_item_t* p = NULL;
	TRYLOCK(m_mutexWorkItems);
	pLinkItem item = popFront(&m_workItems);
	if (NULL != item)
	{
		if (NULL != item->data)
		{
			p = (work_item_t*)item->data;
		}
		freeList(item);
	}
	RELEASELOCK(m_mutexWorkItems);
	return p;
}

void NSWLNet::AddWorkTimeOutItem(work_item_t* p)
{
	TRYLOCK(m_mutexWorkTimeOutItems);
	appendData(&m_workTimeOutItems, p);
	RELEASELOCK(m_mutexWorkTimeOutItems);
}

work_item_t* NSWLNet::PopWorkTimeOutItem()
{
	work_item_t* p = NULL;
	TRYLOCK(m_mutexWorkTimeOutItems);
	pLinkItem item = popFront(&m_workTimeOutItems);
	if (NULL != item)
	{
		if (NULL != item->data)
		{
			p = (work_item_t*)item->data;
		}
		freeList(item);
	}
	RELEASELOCK(m_mutexWorkTimeOutItems);
	return p;
}

void NSWLNet::clearWorkTimeOutItems()
{
	work_item_t* p = NULL;
	TRYLOCK(m_mutexWorkTimeOutItems);
	pLinkItem item = popFront(&m_workTimeOutItems);
	while (item)
	{
		if (NULL != item->data)
		{
			p = (work_item_t*)item->data;
#if _DEBUG
			char temp[128] = { 0 };
			unsigned char Opcode = '\0';
			if (Recive == p->type)
			{
				Opcode = p->data.recive_data.protocol.le.PROTOCOL_90.Opcode;
				sprintf_s(temp, "TimeOut Recive Opcode:0x%02x Delete", Opcode);
			}
			else
			{
				Opcode = p->data.send_data.protocol.le.PROTOCOL_90.Opcode;
				sprintf_s(temp, "TimeOut Send Opcode:0x%02x Delete", Opcode);
			}
			m_pLog->AddLog(temp);
#endif // _DEBUG

			delete p;
			p = NULL;
			item->data = NULL;
		}
		freeList(item);
		item = popFront(&m_workTimeOutItems);
	}
	m_workTimeOutItems = NULL;
	RELEASELOCK(m_mutexWorkTimeOutItems);
}

void NSWLNet::Build_WorkItem_LE_92(work_item_t* p)
{
	if (NULL == p)
	{
		return;
	}
	p->type = Send;
	p->data.send_data.send_to = &m_sockaddrMaster;
	if (LCP == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_92_LCP *pNetworkData = NULL;
		send_data_t* pSendData = &p->data.send_data;
		pNetworkData = &pSendData->protocol.le_lcp.PROTOCOL_92_LCP;
		pNetworkData->acceptedLinkProtocolVersion = LCP_CURRENTLPVERSION;
		pNetworkData->mapType = MAP_TYPE;
		pNetworkData->oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
		pNetworkData->Opcode = LE_NOTIFICATION_MAP_REQUEST;
		pNetworkData->peerID = m_netParam.local_peer_id;
		pSendData->net_lenth = Build_LE_NOTIFICATION_MAP_REQUEST(pSendData->net_data, pNetworkData);
	}
	else
	{
		T_LE_PROTOCOL_92 *pNetworkData = NULL;
		send_data_t* pSendData = &p->data.send_data;
		pNetworkData = &pSendData->protocol.le.PROTOCOL_92;
		pNetworkData->Opcode = LE_NOTIFICATION_MAP_REQUEST;
		pNetworkData->peerID = m_netParam.local_peer_id;
		pSendData->net_lenth = Build_LE_NOTIFICATION_MAP_REQUEST(pSendData->net_data, pNetworkData);
	}
}

DWORD NSWLNet::Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92_LCP* pData)
{
	DWORD size;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	/*mapType*/
	pPacket[5] = pData->mapType;
	/*acceptedLinkProtocolVersion*/
	*((WORD*)(&pPacket[6])) = htons(pData->acceptedLinkProtocolVersion);
	/*oldestLinkProtocolVersion*/
	*((WORD*)(&pPacket[8])) = htons(pData->oldestLinkProtocolVersion);
	size = 10;
	return size;
}

DWORD NSWLNet::Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92* pData)
{
	DWORD size;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	size = 5;
	return size;
}

void NSWLNet::Handle_MapBroadcast(T_LE_PROTOCOL_93* p, T_LE_PROTOCOL_93_LCP* pLcp)
{
	wl_peer_build_param_t param;
	find_peer_condition_t condition;
	pLinkList newPeers = NULL;
	/*增加主中继到新map*/
	param.addr = inet_addr(m_netParam.master_ip);
	param.peerId = m_netParam.matser_peer_id;
	param.pNet = this;
	param.pNetParam = &m_netParam;
	param.port = m_netParam.master_port;
	condition.peer_id = m_netParam.matser_peer_id;
	NSWLPeer* peer = FindPeersItemAndRemove(&condition);
	if (NULL == peer)
	{
		peer = new NSWLPeer(&param);

	}
	appendData(&newPeers, peer);
	peer->WL_REGISTRATION();
	if (LCP == m_netParam.work_mode)
	{
		int mapNums = pLcp->mapNums;
		for (int i = 0; i < mapNums; i++)
		{
			System_Wide_Map_t* pMapInfo = &pLcp->mapPayload.wideMapPeers[i];
			param.addr = pMapInfo->remoteIPAddr;
			param.peerId = pMapInfo->remotePeerID;
			param.pNet = this;
			param.pNetParam = &m_netParam;
			param.port = pMapInfo->remotePort;
			if (!FindLocalIP(param.addr))
			{
				condition.peer_id = param.peerId;
				NSWLPeer* peer = FindPeersItemAndRemove(&condition);
				if (NULL == peer)
				{
					peer = new NSWLPeer(&param);
					peer->LE_PEER_REGISTRATION();
				}
				appendData(&newPeers, peer);
				peer->WL_REGISTRATION();
			}
		}
	}
	else
	{
		int mapNums = p->mapNums;
		for (int i = 0; i < mapNums; i++)
		{
			map_peer_info_t* pMapInfo = &p->mapPeers[i];
			param.addr = pMapInfo->remoteIPAddr;
			param.peerId = pMapInfo->remotePeerID;
			param.pNet = this;
			param.pNetParam = &m_netParam;
			param.port = pMapInfo->remotePort;
			if (!FindLocalIP(param.addr))
			{
				condition.peer_id = param.peerId;
				NSWLPeer* peer = FindPeersItemAndRemove(&condition);
				if (NULL == peer)
				{
					peer = new NSWLPeer(&param);
					peer->LE_PEER_REGISTRATION();
				}
				appendData(&newPeers, peer);
				peer->WL_REGISTRATION();
			}
		}
	}
	/*更新map表*/
	clearPeers();
	TRYLOCK(m_mutexPeers);
	m_peers = newPeers;
	RELEASELOCK(m_mutexPeers);
}

void NSWLNet::Build_WorkItem_LE_96(work_item_t* p, unsigned long timing)
{
	if (NULL == p)
	{
		return;
	}
	char temp[64] = { 0 };
	sprintf_s(temp, "LE_MASTER_PEER_KEEP_ALIVE_REQUEST Send After %.1fs", timing / 1000.0f);
	m_pLog->AddLog(temp);
	p->type = Send;
	p->data.send_data.send_to = &m_sockaddrMaster;
	p->data.send_data.timeout_try = TIMEOUT_TRY_COUNT;
	if (0 != timing)
	{
		p->data.send_data.timing = GetTickCount() + timing;
	}
	if (LCP == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_96_LCP *pNetworkData = NULL;
		send_data_t* pSendData = &p->data.send_data;
		pNetworkData = &pSendData->protocol.le_lcp.PROTOCOL_96_LCP;
		pNetworkData->currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
		pNetworkData->leadingChannelID = LEADING_CHANNEL_ID;
		pNetworkData->oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
		pNetworkData->Opcode = LE_MASTER_KEEP_ALIVE_REQUEST;
		pNetworkData->peerID = m_netParam.local_peer_id;
		pNetworkData->peerMode = LCP_MODE;
		pNetworkData->peerServices = LCP_SERVICES;
		pSendData->net_lenth = Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(pSendData->net_data, pNetworkData);
	}
	else if (CPC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_96 *pNetworkData = NULL;
		send_data_t* pSendData = &p->data.send_data;
		pNetworkData = &pSendData->protocol.le.PROTOCOL_96;
		pNetworkData->Opcode = LE_MASTER_KEEP_ALIVE_REQUEST;
		pNetworkData->peerID = m_netParam.local_peer_id;
		pNetworkData->currentLinkProtocolVersion = CPC_CURRENTLPVERSION;
		pNetworkData->oldestLinkProtocolVersion = CPC_OLDESTPVERSION;
		pNetworkData->peerMode = CPC_MODE;
		pNetworkData->peerServices = CPC_SERVICES;
		pSendData->net_lenth = Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(pSendData->net_data, pNetworkData);
	}
	else
	{
		T_LE_PROTOCOL_96 *pNetworkData = NULL;
		send_data_t* pSendData = &p->data.send_data;
		pNetworkData = &pSendData->protocol.le.PROTOCOL_96;
		pNetworkData->Opcode = LE_MASTER_KEEP_ALIVE_REQUEST;
		pNetworkData->peerID = m_netParam.local_peer_id;
		pNetworkData->currentLinkProtocolVersion = IPSC_CURRENTLPVERSION;
		pNetworkData->oldestLinkProtocolVersion = IPSC_OLDESTPVERSION;
		pNetworkData->peerMode = IPSC_MODE;
		pNetworkData->peerServices = IPSC_SERVICES;
		pSendData->net_lenth = Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(pSendData->net_data, pNetworkData);
	}
}

DWORD NSWLNet::Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96* pData)
{
	DWORD size;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	/*peerMode*/
	pPacket[5] = pData->peerMode;
	/*peerServices*/
	*((DWORD*)(&pPacket[6])) = htonl(pData->peerServices);
	/*currentLinkProtocolVersion*/
	*((WORD*)(&pPacket[10])) = htons(pData->currentLinkProtocolVersion);
	/*oldestLinkProtocolVersion*/
	*((WORD*)(&pPacket[12])) = htons(pData->oldestLinkProtocolVersion);
	size = 14;
	return size;
}

DWORD NSWLNet::Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96_LCP* pData)
{
	DWORD size;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	/*peerMode*/
	*((WORD*)(&pPacket[5])) = htons(pData->peerMode);
	/*peerServices*/
	*((DWORD*)(&pPacket[7])) = htonl(pData->peerServices);
	/*leadingChannelID*/
	pPacket[11] = pData->leadingChannelID;
	/*currentLinkProtocolVersion*/
	*((WORD*)(&pPacket[12])) = htons(pData->currentLinkProtocolVersion);
	/*oldestLinkProtocolVersion*/
	*((WORD*)(&pPacket[14])) = htons(pData->oldestLinkProtocolVersion);
	size = 16;
	return size;
}

void NSWLNet::SendDataToMaster(work_item_t* p, unsigned long timeOut /*= TIMEOUT_LE*/)
{
	sendWorkItemNetData(p);
	send_data_t* pSend = &p->data.send_data;
	if (0 != timeOut)
	{
		pSend->timeout_send = GetTickCount() + timeOut;
	}
	AddWorkTimeOutItem(p);
}

void NSWLNet::sendWorkItemNetData(work_item_t* p)
{
	send_data_t* pSend = &p->data.send_data;
	//sendDataUdp(m_pMasterXqttnet, pSend->net_data, pSend->net_lenth, (SOCKADDR_IN*)pSend->send_to, sizeof(SOCKADDR_IN));
	sendNetDataBase(pSend->net_data, pSend->net_lenth, pSend->send_to);
}

void NSWLNet::clearPeers()
{
	TRYLOCK(m_mutexPeers);
	pLinkItem item = popFront(&m_peers);
	while (item)
	{
		if (NULL != item->data)
		{
			delete (NSWLPeer*)item->data;
			item->data = NULL;
		}
		freeList(item);
		item = popFront(&m_peers);
	}
	m_peers = NULL;
	RELEASELOCK(m_mutexPeers);
}

void NSWLNet::AddPeersItem(NSWLPeer* p)
{
	TRYLOCK(m_mutexPeers);
	appendData(&m_peers, p);
	RELEASELOCK(m_mutexPeers);
}

NSWLPeer* NSWLNet::FindPeersItem(find_peer_condition_t* pCondition)
{
	NSWLPeer* p = NULL;
	TRYLOCK(m_mutexPeers);
	pLinkItem item = findItem(m_peers, pCondition, &FuncFindPeer);
	if (NULL != item)
	{
		if (NULL != item->data)
		{
			p = (NSWLPeer*)item->data;
		}
	}
	RELEASELOCK(m_mutexPeers);
	return p;
}

bool NSWLNet::FuncFindPeer(const void* pValue, const void* pCondition)
{
	bool rlt = false;
	NSWLPeer* p = (NSWLPeer*)pValue;
	if (p)
	{
		find_peer_condition_t* pCndition = (find_peer_condition_t*)pCondition;
		if (pCndition)
		{
			rlt = (pCndition->peer_id == p->PeerId());
		}
	}
	return rlt;
}

void NSWLNet::SetSitePeer(NSWLPeer* pVlaue)
{
	m_sitePeer = pVlaue;
}

NSWLPeer* NSWLNet::FindOrAddPeersItem(wl_peer_build_param_t* param)
{
	find_peer_condition_t condition = { 0 };
	condition.peer_id = param->peerId;
	NSWLPeer* p = FindPeersItem(&condition);
	if (NULL == p)
	{
		p = new NSWLPeer(param);
		AddPeersItem(p);
	}
	return p;
}

void NSWLNet::Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91& networkData)
{
	/*Opcode*/
	networkData.Opcode = pData[0];
	/*peerID*/
	networkData.peerID = ntohl(*((DWORD*)(&pData[1])));
	/*peerMode*/
	networkData.peerMode = pData[5];
	/*peerServices*/
	networkData.peerServices = ntohl(*((DWORD*)(&pData[6])));
	/*numPeers*/
	networkData.numPeers = ntohs(*((WORD*)(&pData[10])));
	/*acceptedLinkProtocolVersion*/
	networkData.acceptedLinkProtocolVersion = ntohs(*((WORD*)(&pData[12])));
	/*oldestLinkProtocolVersion*/
	networkData.oldestLinkProtocolVersion = ntohs(*((WORD*)(&pData[14])));
}

void NSWLNet::Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91_LCP& networkData)
{
	/*Opcode*/
	networkData.Opcode = pData[0];
	/*peerID*/
	networkData.peerID = ntohl(*((DWORD*)(&pData[1])));
	/*peerMode*/
	networkData.peerMode = ntohs(*((WORD*)(&pData[5])));
	/*peerServices*/
	networkData.peerServices = ntohl(*((DWORD*)(&pData[7])));
	/*leadingChannelID*/
	networkData.leadingChannelID = pData[11];
	/*numPeers*/
	networkData.numPeers = ntohs(*((WORD*)(&pData[12])));
	/*acceptedLinkProtocolVersion*/
	networkData.acceptedLinkProtocolVersion = ntohs(*((WORD*)(&pData[14])));
	/*oldestLinkProtocolVersion*/
	networkData.oldestLinkProtocolVersion = ntohs(*((WORD*)(&pData[16])));
}

void NSWLNet::Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93& networkData)
{
	int dataLength = networkData.length;
	/*Opcode*/
	networkData.Opcode = pData[0];
	/*peerID*/
	networkData.peerID = ntohl(*((DWORD*)(&pData[1])));
	/*mapLength*/
	networkData.mapLength = ntohs(*((WORD*)(&pData[5])));
	/*maps*/
	dataLength -= 7;
	unsigned short index = 0;
	while (dataLength)
	{
		//sprintf_s(m_reportMsg, "22");
		//sendLogToWindow();
		//networkData.mapPeers[index].remotePeerID = ntohl(*((DWORD*)(&pData[7 + (index * 11)])));
		//networkData.mapPeers[index].remoteIPAddr = ntohl(*((DWORD*)(&pData[11 + (index * 11)])));
		//networkData.mapPeers[index].remotePort = ntohs(*((WORD*)(&pData[15 + (index * 11)])));
		networkData.mapPeers[index].remotePeerID = ntohl(*((DWORD*)(&pData[7 + (index * 11)])));
		networkData.mapPeers[index].remoteIPAddr = (*((DWORD*)(&pData[11 + (index * 11)])));
		networkData.mapPeers[index].remotePort = ntohs(*((WORD*)(&pData[15 + (index * 11)])));
		networkData.mapPeers[index].peerMode = pData[17 + (index * 11)];
		dataLength -= 11;
		index++;
	}
	/*mapNums;*/
	networkData.mapNums = index;
}

void NSWLNet::Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93_LCP& networkData)
{
	int dataLength = networkData.length;
	/*Opcode*/
	networkData.Opcode = pData[0];
	/*peerID*/
	networkData.peerID = ntohl(*((DWORD*)(&pData[1])));
	/*mapType*/
	networkData.mapType = pData[5];
	/*mapLength*/
	networkData.mapLength = ntohs(*((WORD*)(&pData[6])));
	/*Map Payload*/
	char mapType = networkData.mapType;
	unsigned short index = 0;
	int length = dataLength;
	if (MAP_TYPE == mapType)
	{
		length -= 12;
		while (length)
		{
			// 			sprintf_s(m_reportMsg, "23");
			// 			sendLogToWindow();
			//networkData.mapPayload.wideMapPeers[index].remotePeerID = ntohl(*((DWORD*)(&pData[8 + (index * 13)])));
			//networkData.mapPayload.wideMapPeers[index].remoteIPAddr = ntohl(*((DWORD*)(&pData[12 + (index * 13)])));
			//networkData.mapPayload.wideMapPeers[index].remotePort = ntohs(*((WORD*)(&pData[16 + (index * 13)])));
			//networkData.mapPayload.wideMapPeers[index].peerMode = ntohs(*((WORD*)(&pData[18 + (index * 13)])));
			networkData.mapPayload.wideMapPeers[index].remotePeerID = ntohl(*((DWORD*)(&pData[8 + (index * 13)])));
			networkData.mapPayload.wideMapPeers[index].remoteIPAddr = (*((DWORD*)(&pData[12 + (index * 13)])));
			networkData.mapPayload.wideMapPeers[index].remotePort = ntohs(*((WORD*)(&pData[16 + (index * 13)])));
			networkData.mapPayload.wideMapPeers[index].peerMode = ntohs(*((WORD*)(&pData[18 + (index * 13)])));
			networkData.mapPayload.wideMapPeers[index].leadingChannelID = pData[20 + (index * 13)];
			length -= 13;
			index++;
		}
	}
	/*mapNums;*/
	networkData.mapNums = index;
	/*acceptedLinkProtocolVersion*/
	networkData.acceptedLinkProtocolVersion = ntohs(*((WORD*)(&pData[dataLength - 4])));
	/*oldestLinkProtocolVersion*/
	networkData.oldestLinkProtocolVersion = ntohs(*((WORD*)(&pData[dataLength - 2])));
}

void NSWLNet::Unpack_LE_PEER_REGISTRATION_REQUEST(char* pData, T_LE_PROTOCOL_94& networkData)
{
	/*Opcode*/
	networkData.Opcode = pData[0];
	/*peerID*/
	networkData.peerID = ntohl(*((DWORD*)(&pData[1])));
	/*currentLinkProtocolVersion*/
	networkData.currentLinkProtocolVersion = ntohs(*((WORD*)(&pData[5])));
	/*oldestLinkProtocolVersion*/
	networkData.oldestLinkProtocolVersion = ntohs(*((WORD*)(&pData[7])));
}

void NSWLNet::Unpack_LE_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_95& networkData)
{
	/*Opcode*/
	networkData.Opcode = pData[0];
	/*peerID*/
	networkData.peerID = ntohl(*((DWORD*)(&pData[1])));
	/*currentLinkProtocolVersion*/
	networkData.currentLinkProtocolVersion = ntohs(*((WORD*)(&pData[5])));
	/*oldestLinkProtocolVersion*/
	networkData.oldestLinkProtocolVersion = ntohs(*((WORD*)(&pData[7])));
}

void NSWLNet::Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98& networkData)
{
	/*Opcode*/
	networkData.Opcode = pData[0];
	/*peerID*/
	networkData.peerID = ntohl(*((DWORD*)(&pData[1])));
	/*peerMode*/
	networkData.peerMode = pData[5];
	/*peerServices*/
	networkData.peerServices = ntohl(*((DWORD*)(&pData[6])));
}

void NSWLNet::Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98_LCP& networkData)
{
	/*Opcode*/
	networkData.Opcode = pData[0];
	/*peerID*/
	networkData.peerID = ntohl(*((DWORD*)(&pData[1])));
	/*peerMode*/
	networkData.peerMode = ntohs(*((WORD*)(&pData[5])));
	/*peerServices*/
	networkData.peerServices = ntohl(*((DWORD*)(&pData[7])));
	/*currentLinkProtocolVersion*/
	networkData.currentLinkProtocolVersion = ntohs(*((WORD*)(&pData[11])));
	/*oldestLinkProtocolVersion*/
	networkData.oldestLinkProtocolVersion = ntohs(*((WORD*)(&pData[13])));
}

void NSWLNet::Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99& networkData)
{
	/*Opcode*/
	networkData.Opcode = pData[0];
	/*peerID*/
	networkData.peerID = ntohl(*((DWORD*)(&pData[1])));
	/*peerMode*/
	networkData.peerMode = pData[5];
	/*peerServices*/
	networkData.peerServices = ntohl(*((DWORD*)(&pData[6])));
}

void NSWLNet::Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99_LCP& networkData)
{
	/*Opcode*/
	networkData.Opcode = pData[0];
	/*peerID*/
	networkData.peerID = ntohl(*((DWORD*)(&pData[1])));
	/*peerMode*/
	networkData.peerMode = ntohs(*((WORD*)(&pData[5])));
	/*peerServices*/
	networkData.peerServices = ntohl(*((DWORD*)(&pData[7])));
	/*currentLinkProtocolVersion*/
	networkData.currentLinkProtocolVersion = ntohs(*((WORD*)(&pData[11])));
	/*oldestLinkProtocolVersion*/
	networkData.oldestLinkProtocolVersion = ntohs(*((WORD*)(&pData[13])));
}

void NSWLNet::Unpack_WL_REGISTRATION_GENERAL_OPS(char* pData, T_WL_PROTOCOL_03& networkData)
{
	/*registrationSlotNumber*/
	networkData.registrationSlotNumber = pData[6];
	/*registrationPduID*/
	networkData.registrationPduID = ntohl(*((DWORD*)(&pData[7])));
	/*registrationOperationOpcode*/
	networkData.registrationOperationOpcode = pData[11];
	/*Current / Accepted Wireline Protocol Version*/
	networkData.currentLinkProtocolVersion = pData[12];
	/*Oldest Wireline Protocol Version*/
	networkData.oldestLinkProtocolVersion = pData[13];
}

void NSWLNet::Unpack_WL_REGISTRATION_STATUS(char* pData, T_WL_PROTOCOL_02& networkData)
{
	/*registrationPduID*/
	networkData.registrationPduID = ntohl(*((DWORD*)(&pData[6])));
	/*registrationID (Slot 1)*/
	networkData.registrationIDSlot1 = ntohs(*((WORD*)(&pData[10])));
	/*registrationID (Slot 2)*/
	networkData.registrationIDSlot2 = ntohs(*((WORD*)(&pData[12])));
	/*registrationStatus*/
	networkData.registrationStatus = pData[14];
	/*registrationStatusCode*/
	networkData.registrationStatusCode = pData[15];
	/*Current / Accepted Wireline Protocol Version*/
	networkData.currentLinkProtocolVersion = pData[16];
	/*Oldest Wireline Protocol Version*/
	networkData.oldestLinkProtocolVersion = pData[17];
}

void NSWLNet::Unpack_WL_CHNL_STATUS(char* pData, T_WL_PROTOCOL_11& networkData)
{
	/*slotNumber*/
	networkData.slotNumber = pData[6];
	/*statusPduID*/
	networkData.statusPduID = ntohl(*((DWORD*)(&pData[7])));
	/*conventionalchannelStatus*/
	networkData.conventionalchannelStatus = pData[11];
	/*restChannelStatus*/
	networkData.restChannelStatus = pData[12];
	/*typeOfCall*/
	networkData.restChannelStatus = pData[13];
	/*Current / Accepted Wireline Protocol Version*/
	networkData.currentLinkProtocolVersion = pData[14];
	/*Oldest Wireline Protocol Version*/
	networkData.oldestLinkProtocolVersion = pData[15];
}

void NSWLNet::Unpack_WL_CHNL_STATUS_QUERY(char* pData, T_WL_PROTOCOL_12& networkData)
{
	/*slotNumber*/
	networkData.slotNumber = pData[6];
	/*Current / Accepted Wireline Protocol Version*/
	networkData.currentLinkProtocolVersion = pData[7];
	/*Oldest Wireline Protocol Version*/
	networkData.oldestLinkProtocolVersion = pData[8];
}

void NSWLNet::Unpack_WL_VC_CHNL_CTRL_STATUS(char* pData, T_WL_PROTOCOL_16& networkData)
{
	/*slotNumber*/
	networkData.slotNumber = pData[6];
	/*callID*/
	networkData.callID = ntohl(*((DWORD*)(&pData[7])));
	/*callType*/
	networkData.callType = pData[11];
	/*chnCtrlstatus*/
	networkData.chnCtrlstatus = pData[12];
	/*DeclineReasonCode*/
	networkData.DeclineReasonCode = pData[13];
	/*Current / Accepted Wireline Protocol Version*/
	networkData.currentLinkProtocolVersion = pData[14];
	/*Oldest Wireline Protocol Version*/
	networkData.oldestLinkProtocolVersion = pData[15];
}

void NSWLNet::Unpack_WL_VC_VOICE_START(char* pData, T_WL_PROTOCOL_18& networkData)
{
	/*slotNumber*/
	networkData.slotNumber = pData[6];
	/*callID*/
	networkData.callID = ntohl(*((DWORD*)(&pData[7])));
	/*callType*/
	networkData.callType = pData[11];
	/*source ID*/
	networkData.sourceID = ntohl(*((DWORD*)(&pData[12])));
	/*target ID*/
	networkData.targetID = ntohl(*((DWORD*)(&pData[16])));
	/*callAttributes*/
	networkData.callAttributes = pData[20];
	/*RESERVED*/
	/*MFID*/
	networkData.MFID = pData[22];
	/*serviceOption*/
	networkData.serviceOption = pData[23];
	/*Current / Accepted Wireline Protocol Version*/
	networkData.currentLinkProtocolVersion = pData[24];
	/*Oldest Wireline Protocol Version*/
	networkData.oldestLinkProtocolVersion = pData[25];
}

void NSWLNet::Unpack_WL_VC_VOICE_END_BURST(char* pData, T_WL_PROTOCOL_19& networkData)
{
	/*slotNumber*/
	networkData.slotNumber = pData[6];
	/*callID*/
	networkData.callID = ntohl(*((DWORD*)(&pData[7])));
	/*callType*/
	networkData.callType = pData[11];
	/*source ID*/
	networkData.sourceID = ntohl(*((DWORD*)(&pData[12])));
	/*target ID*/
	networkData.targetID = ntohl(*((DWORD*)(&pData[16])));
	/*RTP Information Field*/
	networkData.RTPInformationField.header = pData[20];
	networkData.RTPInformationField.MPT = pData[21];
	networkData.RTPInformationField.SequenceNumber = ntohs(*((WORD*)(&pData[22])));
	networkData.RTPInformationField.Timestamp = ntohl(*((DWORD*)(&pData[24])));
	networkData.RTPInformationField.SSRC = ntohl(*((DWORD*)(&pData[28])));
	/*burstType*/
	networkData.burstType = pData[32];
	/*RESERVED*/
	/*MFID*/
	networkData.MFID = pData[34];
	/*serviceOption*/
	networkData.serviceOption = pData[35];
	/*Current / Accepted Wireline Protocol Version*/
	networkData.currentLinkProtocolVersion = pData[36];
	/*Oldest Wireline Protocol Version*/
	networkData.oldestLinkProtocolVersion = pData[37];
}

void NSWLNet::Unpack_WL_VC_CALL_SESSION_STATUS(char* pData, T_WL_PROTOCOL_20& networkData)
{
	/*slotNumber*/
	networkData.slotNumber = pData[6];
	/*callID*/
	networkData.callID = ntohl(*((DWORD*)(&pData[7])));
	/*callType*/
	networkData.callType = pData[11];
	/*source ID*/
	networkData.sourceID = ntohl(*((DWORD*)(&pData[12])));
	/*target ID*/
	networkData.targetID = ntohl(*((DWORD*)(&pData[16])));
	/*RESERVED*/
	/*callSessionStatus*/
	networkData.callSessionStatus = pData[24];
	/*Current / Accepted Wireline Protocol Version*/
	networkData.currentLinkProtocolVersion = pData[25];
	/*Oldest Wireline Protocol Version*/
	networkData.oldestLinkProtocolVersion = pData[26];
}

void NSWLNet::Unpack_WL_VC_VOICE_BURST(char* pData, T_WL_PROTOCOL_21& networkData)
{
	/*slotNumber*/
	networkData.slotNumber = pData[6];
	/*callID*/
	networkData.callID = ntohl(*((DWORD*)(&pData[7])));
	/*callType*/
	networkData.callType = pData[11];
	/*source ID*/
	networkData.sourceID = ntohl(*((DWORD*)(&pData[12])));
	/*target ID*/
	networkData.targetID = ntohl(*((DWORD*)(&pData[16])));
	/**/
	networkData.callAttributes = pData[20];
	/*RTP Information Field*/
	networkData.RTPInformationField.header = pData[22];
	networkData.RTPInformationField.MPT = pData[23];
	networkData.RTPInformationField.SequenceNumber = ntohs(*((WORD*)(&pData[24])));
	networkData.RTPInformationField.Timestamp = ntohl(*((DWORD*)(&pData[26])));
	networkData.RTPInformationField.SSRC = ntohl(*((DWORD*)(&pData[30])));
	/*burstType*/
	networkData.burstType = pData[34];
	/*RESERVED*/
	/*MFID*/
	networkData.MFID = pData[36];
	/*serviceOption*/
	networkData.serviceOption = pData[37];
	/*algorithmID*/
	networkData.algorithmID = pData[38];
	/*keyID*/
	networkData.keyID = pData[39];
	/*IV*/
	networkData.IV = ntohl(*((DWORD*)(&pData[40])));
	/*AMBEVoiceEncodedFrames*/
	networkData.AMBEVoiceEncodedFrames = *((AMBE_VOICE_ENCODED_FRAMES*)(&pData[44]));
	/*rawRssiValue*/
	networkData.rawRssiValue = ntohs(*((WORD*)(&pData[64])));
	/*Current / Accepted Wireline Protocol Version*/
	networkData.currentLinkProtocolVersion = pData[66];
	/*Oldest Wireline Protocol Version*/
	networkData.oldestLinkProtocolVersion = pData[67];
}

void NSWLNet::Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_97& networkData)
{
	int index = 0;
	//unsigned char Opcode;
	networkData.Opcode = pData[index];
	index += (sizeof(unsigned char));
	//unsigned long peerID;
	networkData.peerID = ntohl(*(unsigned long*)(&pData[index]));
	index += (sizeof(unsigned long));
	//unsigned char peerMode;
	networkData.peerMode = pData[index];
	index += (sizeof(unsigned char));
	//unsigned long peerServices;
	networkData.peerServices = ntohl(*(unsigned long*)(&pData[index]));
	index += (sizeof(unsigned long));
	//unsigned short acceptedLinkProtocolVersion;
	networkData.acceptedLinkProtocolVersion = ntohs(*(unsigned short*)(&pData[index]));
	index += (sizeof(unsigned short));
	//unsigned short oldestLinkProtocolVersion;
	networkData.oldestLinkProtocolVersion = ntohs(*(unsigned short*)(&pData[index]));
	index += (sizeof(unsigned short));
}

void NSWLNet::Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_97_LCP& networkData)
{
	int index = 0;
	//unsigned char Opcode;
	networkData.Opcode = pData[index];
	index += (sizeof(unsigned char));
	//unsigned long peerID;
	networkData.peerID = ntohl(*(unsigned long*)(&pData[index]));
	index += (sizeof(unsigned long));
	//unsigned short peerMode;
	networkData.peerMode = ntohs(*(unsigned short*)(&pData[index]));
	index += (sizeof(unsigned short));
	//unsigned long peerServices;
	networkData.peerServices = ntohl(*(unsigned long*)(&pData[index]));
	index += (sizeof(unsigned long));
	//unsigned char leadingChannelID;
	networkData.leadingChannelID = pData[index];
	index += (sizeof(unsigned char));
	//unsigned short currentLinkProtocolVersion;
	networkData.currentLinkProtocolVersion = ntohs(*(unsigned short*)(&pData[index]));
	index += (sizeof(unsigned short));
	//unsigned short oldestLinkProtocolVersion;
	networkData.oldestLinkProtocolVersion = ntohs(*(unsigned short*)(&pData[index]));
	index += (sizeof(unsigned short));
}

void NSWLNet::GetIpList(void * p)
{
	local_ip_list_t* pLocalIpList = (local_ip_list_t*)p;
	if (NULL == pLocalIpList)
	{
		return;
	}
	pLocalIpList->count = 0;
	//char ip[255][200];
	char host_name[255];
	//获取本地主机名称
	if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR) {
		return;
	}
	//printf("Host name is: %s\n", host_name);
	//从主机名数据库中得到对应的“主机”
	struct hostent *phe = gethostbyname(host_name);
	if (phe == 0)
	{
		return;
	}
	//struct in_addr addr;
	unsigned short nIpCount = 0;
	char temp[128] = { 0 };
	char tempIp[16] = { 0 };
	//循环得出本地机器所有IP地址
	for (int i = 0; phe->h_addr_list[i] != 0; ++i)
	{
		//memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		memcpy(&pLocalIpList->iplist[i], phe->h_addr_list[i], sizeof(unsigned long));
		nIpCount++;
		//strcpy(tempIp, inet_ntoa(addr));
		//sprintf_s(temp, "%s:%lu:%lu", tempIp, inet_addr(tempIp), pLocalIpList->iplist[i]);
		//m_pLog->AddLog(temp);
	}
	pLocalIpList->count = nIpCount;
}

bool NSWLNet::FindLocalIP(unsigned long addr)
{
	bool rlt = false;
	for (int i = 0; i < m_localIpList.count; i++)
	{
		if (addr == m_localIpList.iplist[i])
		{
			rlt = true;
			break;
		}
	}
	return rlt;
}

NSWLPeer* NSWLNet::FindPeersItemAndRemove(find_peer_condition_t* pCondition)
{
	NSWLPeer* p = NULL;
	TRYLOCK(m_mutexPeers);
	pLinkItem item = findItem(m_peers, pCondition, &FuncFindPeer);
	if (NULL != item)
	{
		if (NULL != item->data)
		{
			p = (NSWLPeer*)item->data;
		}
		removeItem(&m_peers, p);
		item->pNext = NULL;
		freeList(item);
	}
	RELEASELOCK(m_mutexPeers);
	return p;
}

void NSWLNet::Handle_Le_Status_Starting_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag)
{
	OpreateFlag = Oprate_Del;
	work_mode_enum workMode = m_netParam.work_mode;
	if (LE_MASTER_PEER_REGISTRATION_RESPONSE == Opcode)
	{
		int ExpectedPeers = 0;
		if (LCP == workMode)
		{
			T_LE_PROTOCOL_91_LCP* pProtocol = &curItem->data.recive_data.protocol.le_lcp.PROTOCOL_91_LCP;
			ExpectedPeers = pProtocol->numPeers;
			m_netParam.matser_peer_id = pProtocol->peerID;
		}
		else
		{
			T_LE_PROTOCOL_91* pProtocol = &curItem->data.recive_data.protocol.le.PROTOCOL_91;
			ExpectedPeers = pProtocol->numPeers;
			m_netParam.matser_peer_id = pProtocol->peerID;
		}
		if (0 == ExpectedPeers)
		{
			/*主中继进行WireLine注册*/
			wl_peer_build_param_t param = { 0 };
			param.addr = inet_addr(m_netParam.master_ip);
			param.peerId = m_netParam.matser_peer_id;
			param.pNet = this;
			param.pNetParam = &m_netParam;
			param.port = m_netParam.master_port;
			NSWLPeer* p = FindOrAddPeersItem(&param);
			p->WL_REGISTRATION();
			/*发送主中继心跳包*/
			work_item_t* temp = new work_item_t;
			memset(temp, 0, sizeof(work_item_t));
			Build_WorkItem_LE_96(temp);
			AddWorkItem(temp);
			SetLeStatus(ALIVE);
		}
		else
		{
			/*组建0x92并发送*/
			work_item_t* temp = new work_item_t;
			memset(temp, 0, sizeof(work_item_t));
			Build_WorkItem_LE_92(temp);
			AddWorkItem(temp);
			SetLeStatus(WAITFOR_LE_NOTIFICATION_MAP_BROADCAST);
		}
		SetXnlStatus(XNL_CONNECT);
		OpreateFlag = Oprate_Add;
	}

}

void NSWLNet::Handle_Le_Status_WaitMap_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag)
{
	OpreateFlag = Oprate_Del;
	work_mode_enum workMode = m_netParam.work_mode;
	if (LE_NOTIFICATION_MAP_BROADCAST == Opcode)
	{
		T_LE_PROTOCOL_93* pProtocol = NULL;
		T_LE_PROTOCOL_93_LCP* pProtocolLcp = NULL;
		if (LCP == workMode)
		{
			pProtocolLcp = &curItem->data.recive_data.protocol.le_lcp.PROTOCOL_93_LCP;
		}
		else
		{
			pProtocol = &curItem->data.recive_data.protocol.le.PROTOCOL_93;
		}
		/*解析map表*/
		Handle_MapBroadcast(pProtocol, pProtocolLcp);
		/*发送主中继心跳包*/
		work_item_t* temp = new work_item_t;
		memset(temp, 0, sizeof(work_item_t));
		Build_WorkItem_LE_96(temp);
		AddWorkItem(temp);
		SetLeStatus(ALIVE);
		OpreateFlag = Oprate_Add;
	}
}

void NSWLNet::Handle_Le_Status_Alive_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag)
{
	OpreateFlag = Oprate_Add;
	work_mode_enum workMode = m_netParam.work_mode;
	find_peer_condition_t condition = { 0 };
	NSWLPeer* pPeer = NULL;
	switch (Opcode)
	{
	case P2P_GRP_VOICE_CALL:
	case P2P_PVT_VOICE_CALL:
	{
							   /*P2P语音数据,不做任何处理*/
							   OpreateFlag = Oprate_Del;
	}
		break;
	case LE_MASTER_KEEP_ALIVE_RESPONSE:
	{
										  /*主中继心跳包的回复,等待TimeOut处理*/
	}
		break;
	case LE_PEER_REGISTRATION_REQUEST:
	{
										 /*非主中继LE注册请求*/
										 T_LE_PROTOCOL_94* pProtocol = NULL;
										 if (LCP == workMode)
										 {
											 pProtocol = &curItem->data.recive_data.protocol.le_lcp.PROTOCOL_94_LCP;
										 }
										 else
										 {
											 pProtocol = &curItem->data.recive_data.protocol.le.PROTOCOL_94;
										 }
										 condition.peer_id = pProtocol->peerID;
										 pPeer = FindPeersItem(&condition);
										 if (pPeer)
										 {
											 pPeer->Handle_LE_PEER_REGISTRATION_REQUEST_Recive();
										 }
										 OpreateFlag = Oprate_Del;
	}
		break;
	case LE_PEER_KEEP_ALIVE_REQUEST:
	{
									   /*非主中继心跳包请求,等待TimeOut处理*/
	}
		break;
	case LE_PEER_KEEP_ALIVE_RESPONSE:
	{
										/*非主中继心跳包回复,等待TimeOut处理*/
	}
		break;
	case LE_PEER_REGISTRATION_RESPONSE:
	{
										  /*非主中继LE注册返回,等待TimeOut处理*/
	}
		break;
	case WL_PROTOCOL:
	{
						/*收到wireline相关信息*/
						unsigned char wirelineOpcode = curItem->data.recive_data.protocol.wl.PROTOCOL_01.wirelineOpcode;
						//moto_protocol_wl_t* protocol = &curItem->data.recive_data.protocol.wl;
						Handle_Wl_Status_Alive_Recive(wirelineOpcode, curItem, OpreateFlag);
	}
		break;
	case LE_NOTIFICATION_MAP_BROADCAST:
	{
										  /*Map表通知*/
										  T_LE_PROTOCOL_93* pProtocol = NULL;
										  T_LE_PROTOCOL_93_LCP* pProtocolLcp = NULL;
										  if (LCP == workMode)
										  {
											  pProtocolLcp = &curItem->data.recive_data.protocol.le_lcp.PROTOCOL_93_LCP;
										  }
										  else
										  {
											  pProtocol = &curItem->data.recive_data.protocol.le.PROTOCOL_93;
										  }
										  /*解析map表*/
										  Handle_MapBroadcast(pProtocol, pProtocolLcp);
	}
		break;
	default:
		OpreateFlag = Oprate_Del;
		break;
	}
}

void NSWLNet::Handle_Le_Status_Starting_TimeOut_Send(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag)
{
	if (NULL == curItem)
	{
		return;
	}
	switch (Opcode)
	{
	case LE_MASTER_PEER_REGISTRATION_REQUEST:
	{
												HandleRetryAndTimingSend(curItem, OpreateFlag);
	}
		break;
	default:
		break;
	}
}

void NSWLNet::Handle_Le_Status_WaitMap_TimeOut_Send(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag)
{
	if (NULL == curItem)
	{
		return;
	}
	switch (Opcode)
	{
	case LE_NOTIFICATION_MAP_REQUEST:
	{

										if (!HandleRetryAndTimingSend(curItem, OpreateFlag))
										{
											/*0x92尝试次数完毕,重新发送0x90*/
											SetLeStatus(STARTING);
											SetXnlStatus(WAITLE);
											work_item_t* p = new work_item_t;
											memset(p, 0, sizeof(work_item_t));
											Build_WorkItem_LE_90(p);
											AddWorkItem(p);
										}

	}
		break;
	default:
		break;
	}
}

void NSWLNet::Handle_Le_Status_Alive_TimeOut_Send(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag)
{
	if (NULL == curItem)
	{
		return;
	}
	send_data_t* pSend = &curItem->data.send_data;
	switch (Opcode)
	{
	case  LE_MASTER_KEEP_ALIVE_REQUEST:
	{

										  if (!HandleRetryAndTimingSend(curItem, OpreateFlag))
										  {
											  /*0x96尝试次数完毕,重新发送0x90*/
											  SetLeStatus(STARTING);
											  SetXnlStatus(WAITLE);
											  work_item_t* p = new work_item_t;
											  memset(p, 0, sizeof(work_item_t));
											  Build_WorkItem_LE_90(p);
											  AddWorkItem(p);
										  }
	}
		break;
	case LE_PEER_REGISTRATION_REQUEST:
	{

										 HandleRetryAndTimingSend(curItem, OpreateFlag);
	}
		break;
	case LE_PEER_KEEP_ALIVE_REQUEST:
	{
									   if (!HandleRetryAndTimingSend(curItem, OpreateFlag))
									   {
										   NSWLPeer* peer = (NSWLPeer*)pSend->pFrom;
										   if (peer)
										   {
											   peer->LE_PEER_REGISTRATION();
										   }
									   }
	}
		break;
	case WL_PROTOCOL:
	{

						Handle_Wl_Status_Alive_TimeOut_Send(pSend->protocol.wl.PROTOCOL_01.wirelineOpcode, curItem, OpreateFlag);
	}
		break;
	default:
		break;
	}
}

bool NSWLNet::HandleRetryAndTimingSend(work_item_t* curItem, item_oprate_enum &OpreateFlag)
{
	bool rlt = true;
	send_data_t* pSend = &curItem->data.send_data;
	if (0 != pSend->timing)
	{
		OpreateFlag = Oprate_Other;
		if (pSend->timing <= GetTickCount())
		{
			pSend->timing = 0;
			AddWorkItem(curItem);
		}
		else
		{
			AddWorkTimeOutItem(curItem);
		}
	}
	else if (0 != pSend->timeout_send)
	{
		if (pSend->timeout_send < GetTickCount())
		{
			if (0 < (pSend->timeout_try--))
			{
				OpreateFlag = Oprate_Other;
				pSend->timeout_send = 0;
				AddWorkItem(curItem);
			}
			else
			{
				rlt = false;
			}
		}
		else
		{
			/*放在下一轮判断*/
			OpreateFlag = Oprate_Other;
			AddWorkTimeOutItem(curItem);
		}
	}
	return rlt;
}

void NSWLNet::Handle_Wl_Status_Alive_TimeOut_Send(const char wirelineOpcode, work_item_t* curItem, item_oprate_enum &OpreateFlag)
{
	unsigned char Opcode = wirelineOpcode;
	switch (Opcode)
	{
	case WL_REGISTRATION_REQUEST:
	{
									HandleRetryAndTimingSend(curItem, OpreateFlag);
	}
		break;
	default:
		break;
	}
}

void NSWLNet::Handle_Wl_Status_Alive_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag)
{
	OpreateFlag = Oprate_Del;
	switch (Opcode)
	{
	case WL_REGISTRATION_STATUS:
	{
								   /*等待TimeOut线程处理*/
								   OpreateFlag = Oprate_Add;
	}
		break;
	case WL_VC_VOICE_START:
	case WL_VC_VOICE_END_BURST:
	case WL_VC_VOICE_BURST:
	case WL_VC_CALL_SESSION_STATUS:
	{
									  /*通话开始、结束、语音数据、挂起或者完成,交给Ambe线程处理*/
									  OpreateFlag = Oprate_Other;
									  AddAmbesItem(curItem);
	}
		break;
	case WL_VC_CHNL_CTRL_STATUS:
	{
								   T_WL_PROTOCOL_16* protocol = &curItem->data.recive_data.protocol.wl.PROTOCOL_16;
								   unsigned long peerID = protocol->peerID;
								   unsigned char slotNumber = protocol->slotNumber;
								   unsigned long callID = protocol->callID;
								   m_callFailInfo.Value = protocol->DeclineReasonCode;
								   find_peer_condition_t condition = { 0 };
								   condition.peer_id = peerID;
								   NSWLPeer* peer = FindPeersItem(&condition);
								   getCallRequestRltInfo(m_callFailInfo);
								   unsigned char chnCtrlstatus = protocol->chnCtrlstatus;
								   setCallId(callID);
								   switch (chnCtrlstatus)
								   {
								   case Channel_Control_Request_Status_Grant:
								   {
																				m_pLog->AddLog("Channel_Control_Request_Status_Grant");
																				/*设定当前peer为发送voice peer*/
																				if (CallThreadStatus() == Call_Thread_Wait_Reply)
																				{
																					if (peer)
																					{
																						peer->setSlotNumber((slot_number_enum)slotNumber);
																						setCurrentSendVoicePeer(peer);
																						setCallThreadStatus(Call_Thread_Send_Burst);
																					}
																				}
								   }
									   break;
								   case Channel_Control_Request_Status_Declined:
								   {
																				   /*请求通话失败*/
																				   if (Call_Thread_Status_Idle != CallThreadStatus() ||
																					   Call_Thread_Author_No != CallThreadStatus())
																				   {
																					   m_pLog->AddLog("Channel_Control_Request_Status_Declined ReasonCode:%s,FailureScenarios:%s", m_callFailInfo.ReasonCode, m_callFailInfo.FailureScenarios);
																					   setCallThreadStatus(Call_Thread_Call_Fail);
																				   }
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
}

void NSWLNet::Handle_Le_Status_Starting_TimeOut_Recive(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag)
{
	if (NULL == curItem)
	{
		return;
	}
	switch (Opcode)
	{
	case LE_MASTER_PEER_REGISTRATION_RESPONSE:
	{
												 /*删除0x90*/
												 findTimeOutItemAndDelete(m_netParam.matser_peer_id, LE_MASTER_PEER_REGISTRATION_REQUEST, 0x00, Send);
	}
		break;
	default:
		break;
	}
}

void NSWLNet::Handle_Le_Status_WaitMap_TimeOut_Recive(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag)
{
	if (NULL == curItem)
	{
		return;
	}
	switch (Opcode)
	{
	case LE_NOTIFICATION_MAP_BROADCAST:
	{
										  /*删除0x92*/
										  findTimeOutItemAndDelete(m_netParam.matser_peer_id, LE_NOTIFICATION_MAP_REQUEST, 0x00, Send);
	}
		break;
	default:
		break;
	}
}

void NSWLNet::Handle_Le_Status_Alive_TimeOut_Recive(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag)
{
	work_mode_enum workMode = m_netParam.work_mode;
	find_peer_condition_t condition = { 0 };
	NSWLPeer* peer = NULL;
	if (NULL == curItem)
	{
		return;
	}
	switch (Opcode)
	{
	case LE_PEER_REGISTRATION_RESPONSE:
	{
										  T_LE_PROTOCOL_95* protocol = NULL;
										  if (LCP == workMode)
										  {
											  protocol = &curItem->data.recive_data.protocol.le_lcp.PROTOCOL_95_LCP;
										  }
										  else
										  {
											  protocol = &curItem->data.recive_data.protocol.le.PROTOCOL_95;
										  }
										  /*删除0x94*/
										  findTimeOutItemAndDelete(protocol->peerID, LE_PEER_REGISTRATION_REQUEST, 0x00, Send);
										  /*定时发送0x98*/
										  condition.peer_id = protocol->peerID;
										  peer = FindPeersItem(&condition);
										  if (peer)
										  {
											  peer->Handle_LE_PEER_REGISTRATION_RESPONSE_Recive();
										  }
	}
		break;
	case LE_MASTER_KEEP_ALIVE_RESPONSE:
	{
										  char temp[64] = { 0 };
										  sprintf_s(temp, "LE_MASTER_KEEP_ALIVE_RESPONSE Recive");
										  m_pLog->AddLog(temp);
										  /*删除0x96*/
										  findTimeOutItemAndDelete(m_netParam.matser_peer_id, LE_MASTER_KEEP_ALIVE_REQUEST, 0x00, Send);
										  /*定时发送0x96*/
										  work_item_t* p = new work_item_t;
										  memset(p, 0, sizeof(work_item_t));
										  Build_WorkItem_LE_96(p, g_timing_alive_time_master);
										  AddWorkTimeOutItem(p);
	}
		break;
	case LE_PEER_KEEP_ALIVE_REQUEST:
	{

									   T_LE_PROTOCOL_98* protocol = NULL;
									   T_LE_PROTOCOL_98_LCP* protocol_lcp = NULL;
									   bool is3rd = false;
									   if (LCP == workMode)
									   {
										   protocol_lcp = &curItem->data.recive_data.protocol.le_lcp.PROTOCOL_98_LCP;
										   if (protocol_lcp->peerMode & 0x0080) is3rd = true;
										   /*删除0x98*/
										   findTimeOutItemAndDelete(protocol_lcp->peerID, LE_PEER_KEEP_ALIVE_REQUEST, 0x00, Send);
										   condition.peer_id = protocol_lcp->peerID;
										   peer = FindPeersItem(&condition);
										   if (peer)
										   {
											   peer->setRemote3rdParty(is3rd);
											   peer->Handle_LE_PEER_KEEP_ALIVE_REQUEST_Recive();
										   }
									   }
									   else
									   {
										   protocol = &curItem->data.recive_data.protocol.le.PROTOCOL_98;
										   if (protocol->peerServices & 0x00002000) is3rd = true;
										   /*删除0x98*/
										   findTimeOutItemAndDelete(protocol->peerID, LE_PEER_KEEP_ALIVE_REQUEST, 0x00, Send);
										   condition.peer_id = protocol->peerID;
										   peer = FindPeersItem(&condition);
										   if (peer)
										   {
											   peer->setRemote3rdParty(is3rd);
											   peer->Handle_LE_PEER_KEEP_ALIVE_REQUEST_Recive();
										   }

									   }
	}
		break;
	case LE_PEER_KEEP_ALIVE_RESPONSE:
	{
										T_LE_PROTOCOL_99* protocol = NULL;
										T_LE_PROTOCOL_99_LCP* protocol_lcp = NULL;
										if (LCP == workMode)
										{
											protocol_lcp = &curItem->data.recive_data.protocol.le_lcp.PROTOCOL_99_LCP;
											/*删除0x98*/
											findTimeOutItemAndDelete(protocol_lcp->peerID, LE_PEER_KEEP_ALIVE_REQUEST, 0x00, Send);
											condition.peer_id = protocol_lcp->peerID;
											peer = FindPeersItem(&condition);
											if (peer)
											{
												peer->Handle_LE_PEER_KEEP_ALIVE_RESPONSE_Recive();
											}
										}
										else
										{
											protocol = &curItem->data.recive_data.protocol.le.PROTOCOL_99;
											/*删除0x98*/
											findTimeOutItemAndDelete(protocol->peerID, LE_PEER_KEEP_ALIVE_REQUEST, 0x00, Send);
											condition.peer_id = protocol->peerID;
											peer = FindPeersItem(&condition);
											if (peer)
											{
												peer->Handle_LE_PEER_KEEP_ALIVE_RESPONSE_Recive();
											}
										}
	}
		break;
	case WL_PROTOCOL:
	{
						moto_protocol_wl_t* protocol = &curItem->data.recive_data.protocol.wl;
						Handle_Wl_Status_Alive_TimeOut_Recive(protocol->PROTOCOL_01.wirelineOpcode, protocol, OpreateFlag);
	}
		break;
	default:
		break;
	}
}

void NSWLNet::findTimeOutItemAndDelete(unsigned long peerId, const char Opcode, const char wlOpcode, work_item_type_enum type)
{
	find_item_condition_t condition = { 0 };
	condition.peerId = peerId;
	condition.Opcode = Opcode;
	condition.wlOpcode = wlOpcode;
	condition.type = type;
	work_item_t* p = NULL;
	TRYLOCK(m_mutexWorkTimeOutItems);
	pLinkItem item = findItem(m_workTimeOutItems, &condition, &FuncFindItem);
	while (item)
	{
		p = (work_item_t*)item->data;
		if (p)
		{
#if _DEBUG
			//char temp[128] = { 0 };
			//unsigned char Opcode = '\0';
			//if (Recive == p->type)
			//{
			//	Opcode = p->data.recive_data.protocol.le.PROTOCOL_90.Opcode;
			//	sprintf_s(temp, "Rely Success TimeOut Recive Opcode:0x%02x Delete", Opcode);
			//}
			//else
			//{
			//	Opcode = p->data.send_data.protocol.le.PROTOCOL_90.Opcode;
			//	sprintf_s(temp, "Rely Success TimeOut Send Opcode:0x%02x Delete", Opcode);
			//}
			//m_pLog->AddLog(temp);
#endif // _DEBUG
			removeItem(&m_workTimeOutItems, p);
			delete p;
			p = NULL;
		}
		item->pNext = NULL;
		freeList(item);
		item = findItem(m_workTimeOutItems, &condition, &FuncFindItem);
	}
	RELEASELOCK(m_mutexWorkTimeOutItems);
}

bool NSWLNet::FuncFindItem(const void* pValue, const void* pCondition)
{

	work_item_t* p = (work_item_t*)pValue;
	find_item_condition_t* condition = (find_item_condition_t*)pCondition;
	bool rlt = false;
	if (NULL == pValue || NULL == pCondition)
	{
		return rlt;
	}
	unsigned char Opcode, wlOpcode;
	unsigned long peerId;
	work_item_type_enum type = p->type;
	bool bNeedCompilePeerId = true;

	if (Recive == p->type)
	{
		Opcode = p->data.recive_data.protocol.le.PROTOCOL_90.Opcode;
		peerId = p->data.recive_data.protocol.le.PROTOCOL_90.peerID;
		wlOpcode = p->data.recive_data.protocol.wl.PROTOCOL_01.wirelineOpcode;
	}
	else
	{
		Opcode = p->data.send_data.protocol.le.PROTOCOL_90.Opcode;
		wlOpcode = p->data.send_data.protocol.wl.PROTOCOL_01.wirelineOpcode;
		NSWLPeer* peer = (NSWLPeer*)p->data.send_data.pFrom;
		if (peer)
		{
			peerId = peer->PeerId();
		}
		else
		{
			/*此命令为master发出或向master发出*/
			bNeedCompilePeerId = false;
		}
	}

	if (ReciveOrSend == condition->type || type == condition->type)
	{
		if (Opcode == condition->Opcode)
		{
			if (0x00 != condition->wlOpcode)
			{
				rlt = (wlOpcode == condition->wlOpcode);
			}
			else
			{
				if (bNeedCompilePeerId)
				{
					rlt = (peerId == condition->peerId);
				}
				else
				{
					rlt = true;
				}
			}

		}
	}
	return rlt;
}

void NSWLNet::findItemAndDelete(unsigned long peerId, const char Opcode, const char wlOpcode)
{
	find_item_condition_t condition = { 0 };
	condition.peerId = peerId;
	condition.Opcode = Opcode;
	condition.wlOpcode = wlOpcode;
	work_item_t* p = NULL;
	TRYLOCK(m_mutexWorkItems);
	pLinkItem item = findItem(m_workItems, &condition, &FuncFindItem);
	while (item)
	{
		p = (work_item_t*)item->data;
		if (p)
		{
#if _DEBUG
			//char temp[128] = { 0 };
			//unsigned char Opcode = '\0';
			//if (Recive == p->type)
			//{
			//	Opcode = p->data.recive_data.protocol.le.PROTOCOL_90.Opcode;
			//	sprintf_s(temp, "Rely Success Recive Opcode:0x%02x Delete", Opcode);
			//}
			//else
			//{
			//	Opcode = p->data.send_data.protocol.le.PROTOCOL_90.Opcode;
			//	sprintf_s(temp, "Rely Success Send Opcode:0x%02x Delete", Opcode);
			//}
			//m_pLog->AddLog(temp);
#endif // _DEBUG
			removeItem(&m_workItems, p);
			delete p;
			p = NULL;
		}
		item->pNext = NULL;
		freeList(item);
		item = findItem(m_workItems, &condition, &FuncFindItem);
	}
	RELEASELOCK(m_mutexWorkItems);
}

void NSWLNet::SetSerialNumberCallback(unsigned char* pData, void* pArguments)
{
	NSWLNet* p = (NSWLNet*)pArguments;
	if (p)
	{
		p->SetSerialNumber(pData);
	}
}

//work_item_t * WLNet::PopSerialItem()
//{
//	work_item_t* p = NULL;
//	TRYLOCK(m_serialMutex);
//	pLinkItem it = popFront(&m_serialLink);
//	if (NULL != it)
//	{
//		if (NULL != it->data)
//		{
//			p = (work_item_t*)it->data;
//		}
//		freeList(it);
//	}
//	RELEASELOCK(m_serialMutex);
//	return p;
//}
//
void NSWLNet::SetSerialNumber(unsigned char* pSerial)
{
	memcpy(m_serialNumber, pSerial, sizeof(m_serialNumber));
	char temp[11] = { 0 };
	memcpy(temp, m_serialNumber, sizeof(m_serialNumber));
	repeaterSerial = temp;
	if (g_pNet)
	{
		g_pNet->wlSendSerial();
	}
}

void NSWLNet::Handle_Wl_Status_Alive_TimeOut_Recive(const char wlOpcode, moto_protocol_wl_t* protocol, item_oprate_enum &OpreateFlag)
{
	NSWLPeer* peer = NULL;
	find_peer_condition_t condition = { 0 };
	wl_reg_status wlRegStatus = WL_REG_UNKNOWN;
	switch (wlOpcode)
	{
	case WL_REGISTRATION_STATUS:
	{
								   T_WL_PROTOCOL_02* info = &protocol->PROTOCOL_02;
								   unsigned long peerID = info->peerID;
								   if (0x00 == info->registrationStatus)
								   {
									   wlRegStatus = WL_REG_SUCCESS;
								   }
								   else
								   {
									   wlRegStatus = WL_REG_FAIl;
								   }
								   condition.peer_id = peerID;
								   peer = FindPeersItem(&condition);
								   if (peer)
								   {
									   peer->setWlRegStatus(wlRegStatus);
								   }
								   /*删除0x01*/
								   findTimeOutItemAndDelete(peerID, WL_PROTOCOL, WL_REGISTRATION_REQUEST, Send);
	}
		break;
	default:
		break;
	}
}

void NSWLNet::Unpack_WL(const char* pData, int len, moto_protocol_wl_t* &p, item_oprate_enum &OpreateFlag)
{
	unsigned char wlOpcode = p->PROTOCOL_01.wirelineOpcode;
	char temp[64];
	switch (wlOpcode)
	{
	case WL_REGISTRATION_REQUEST:
	{
									/*理论上来说不存在向第三方应用进行WL注册*/
									sprintf_s(temp, "WL_REGISTRATION_REQUEST no handle");
									m_pLog->AddLog(temp);
									OpreateFlag = Oprate_Del;
	}
		break;
	case WL_REGISTRATION_STATUS:
	{
								   p->PROTOCOL_02.length = len;
								   Unpack_WL_REGISTRATION_STATUS((char*)pData, p->PROTOCOL_02);
	}
		break;
	case WL_REGISTRATION_GENERAL_OPS:
	{
										p->PROTOCOL_03.length = len;
										Unpack_WL_REGISTRATION_GENERAL_OPS((char*)pData, p->PROTOCOL_03);
	}
		break;
	case WL_CHNL_STATUS:
	{
						   p->PROTOCOL_11.length = len;
						   Unpack_WL_CHNL_STATUS((char*)pData, p->PROTOCOL_11);
	}
		break;
	case WL_CHNL_STATUS_QUERY:
	{
								 p->PROTOCOL_12.length = len;
								 Unpack_WL_CHNL_STATUS_QUERY((char*)pData, p->PROTOCOL_12);
	}
		break;
	case WL_VC_CHNL_CTRL_REQUEST:
	{
									/*理论上来说不存在向第三方应用进行通话申请*/
									sprintf_s(temp, "WL_VC_CHNL_CTRL_REQUEST no handle");
									m_pLog->AddLog(temp);
									OpreateFlag = Oprate_Del;
	}
		break;
	case WL_VC_CHNL_CTRL_STATUS:
	{
								   p->PROTOCOL_16.length = len;
								   Unpack_WL_VC_CHNL_CTRL_STATUS((char*)pData, p->PROTOCOL_16);
	}
		break;
	case WL_VC_VOICE_START:
	{
							  p->PROTOCOL_18.length = len;
							  Unpack_WL_VC_VOICE_START((char*)pData, p->PROTOCOL_18);
	}
		break;
	case WL_VC_VOICE_END_BURST:
	{
								  p->PROTOCOL_19.length = len;
								  Unpack_WL_VC_VOICE_END_BURST((char*)pData, p->PROTOCOL_19);
	}
		break;
	case WL_VC_CALL_SESSION_STATUS:
	{
									  p->PROTOCOL_20.length = len;
									  Unpack_WL_VC_CALL_SESSION_STATUS((char*)pData, p->PROTOCOL_20);
	}
		break;
	case WL_VC_VOICE_BURST:
	{
							  p->PROTOCOL_21.length = len;
							  Unpack_WL_VC_VOICE_BURST((char*)pData, p->PROTOCOL_21);
	}
		break;
	default:
		OpreateFlag = Oprate_Del;
		break;
	}
}

void NSWLNet::clearAmbes()
{
	work_item_t* p = NULL;
	TRYLOCK(m_mutexAmbes);
	pLinkItem item = popFront(&m_ambes);
	while (item)
	{
		if (NULL != item->data)
		{
			p = (work_item_t*)item->data;
			delete p;
			p = NULL;
			item->data = NULL;
		}
		freeList(item);
		item = popFront(&m_ambes);
	}
	m_ambes = NULL;
	RELEASELOCK(m_mutexAmbes);
}

void NSWLNet::AddAmbesItem(work_item_t* p)
{
	TRYLOCK(m_mutexAmbes);
	appendData(&m_ambes, p);
	RELEASELOCK(m_mutexAmbes);
}

work_item_t* NSWLNet::PopAmbesItem()
{
	work_item_t* p = NULL;
	TRYLOCK(m_mutexAmbes);
	pLinkItem it = popFront(&m_ambes);
	if (NULL != it)
	{
		if (NULL != it->data)
		{
			p = (work_item_t*)it->data;
		}
		freeList(it);
	}
	RELEASELOCK(m_mutexAmbes);
	return p;
}

void NSWLNet::AddRecordsItem(NSRecordFile* p)
{
	TRYLOCK(m_mutexRecords);
	appendData(&m_records, p);
	RELEASELOCK(m_mutexRecords);
}

void NSWLNet::clearRecords()
{
	NSRecordFile* p = NULL;
	TRYLOCK(m_mutexRecords);
	pLinkItem item = popFront(&m_records);
	while (item)
	{
		if (NULL != item->data)
		{
			p = (NSRecordFile*)item->data;
			delete p;
			p = NULL;
			item->data = NULL;
		}
		freeList(item);
		item = popFront(&m_records);
	}
	m_records = NULL;
	RELEASELOCK(m_mutexRecords);
}

void NSWLNet::Handle_WL_PROTOCOL_21_Ambe_Data(char *dst, const char* pAmbePacket)
{
	char* voiceFrame1 = &dst[0];
	char* voiceFrame2 = &dst[7];
	char* voiceFrame3 = &dst[14];

	voiceFrame1[0] = pAmbePacket[1];
	voiceFrame1[1] = pAmbePacket[2];
	voiceFrame1[2] = pAmbePacket[3];
	voiceFrame1[3] = pAmbePacket[4];
	voiceFrame1[4] = pAmbePacket[5];
	voiceFrame1[5] = pAmbePacket[6];
	voiceFrame1[6] = (pAmbePacket[7]) & 0x80;

	voiceFrame2[0] = ((pAmbePacket[7]) << 2) | (((pAmbePacket[8]) >> 6) & 0x03);
	voiceFrame2[1] = ((pAmbePacket[8]) << 2) | (((pAmbePacket[9]) >> 6) & 0x03);
	voiceFrame2[2] = ((pAmbePacket[9]) << 2) | (((pAmbePacket[10]) >> 6) & 0x03);
	voiceFrame2[3] = ((pAmbePacket[10]) << 2) | (((pAmbePacket[11]) >> 6) & 0x03);
	voiceFrame2[4] = ((pAmbePacket[11]) << 2) | (((pAmbePacket[12]) >> 6) & 0x03);
	voiceFrame2[5] = ((pAmbePacket[12]) << 2) | (((pAmbePacket[13]) >> 6) & 0x03);
	voiceFrame2[6] = ((pAmbePacket[13]) << 2) & 0x80;

	voiceFrame3[0] = ((pAmbePacket[13]) << 4) | (((pAmbePacket[14]) >> 4) & 0x0F);
	voiceFrame3[1] = ((pAmbePacket[14]) << 4) | (((pAmbePacket[15]) >> 4) & 0x0F);
	voiceFrame3[2] = ((pAmbePacket[15]) << 4) | (((pAmbePacket[16]) >> 4) & 0x0F);
	voiceFrame3[3] = ((pAmbePacket[16]) << 4) | (((pAmbePacket[17]) >> 4) & 0x0F);
	voiceFrame3[4] = ((pAmbePacket[17]) << 4) | (((pAmbePacket[18]) >> 4) & 0x0F);
	voiceFrame3[5] = ((pAmbePacket[18]) << 4) | (((pAmbePacket[19]) >> 4) & 0x0F);
	voiceFrame3[6] = ((pAmbePacket[19]) << 4) & 0x80;
}

NSRecordFile* NSWLNet::FindOrAddRecordsItem(find_record_condition_t* condition, bool &bFind, NSWLPeer* peer)
{
	bFind = true;
	NSRecordFile* p = FindRecordsItem(condition);
	if (NULL == p)
	{
		bFind = false;
		p = new NSRecordFile(m_pManager, peer);
		AddRecordsItem(p);
	}
	return p;
}

NSRecordFile* NSWLNet::FindRecordsItem(find_record_condition_t* condition)
{
	NSRecordFile* p = NULL;
	TRYLOCK(m_mutexRecords);
	pLinkItem item = findItem(m_records, condition, &FuncFindRecord);
	if (NULL != item)
	{
		if (NULL != item->data)
		{
			p = (NSRecordFile*)item->data;
		}
	}
	RELEASELOCK(m_mutexRecords);
	return p;
}

bool NSWLNet::FuncFindRecord(const void* pValue, const void* pCondition)
{
	bool rlt = false;
	NSRecordFile* p = (NSRecordFile*)pValue;
	if (p)
	{
		find_record_condition_t* pCndition = (find_record_condition_t*)pCondition;
		if (pCndition)
		{
			rlt = (pCndition->call_id == p->call_id && pCndition->src_radio == p->src_radio && pCndition->target_radio == p->target_radio);
		}
	}
	return rlt;
}

void NSWLNet::RemoveRecordsItem(NSRecordFile* p)
{
	TRYLOCK(m_mutexRecords);
	pLinkItem item = removeItem(&m_records, p);
	if (item)
	{
		item->pNext = NULL;
		freeList(item);
		item = NULL;
	}
	RELEASELOCK(m_mutexRecords);
}

unsigned int __stdcall NSWLNet::CheckRecordsThreadProc(void* pArguments)
{
	NSWLNet* p = (NSWLNet*)pArguments;
	if (p)
	{
		p->CheckRecordsThread();
	}
	return 0;
}

void NSWLNet::CheckRecordsThread()
{
	m_pLog->AddLog("CheckRecordsThread Start");
	NSRecordFile* record = NULL;
	pLinkItem curItem = NULL;
	while (m_bThreadWork)
	{
		TRYLOCK(m_mutexRecords);
		curItem = m_records;
		while (curItem)
		{
			record = (NSRecordFile*)curItem->data;
			if (record)
			{
				if (record->TimeOut())
				{
					if (0 != record->Length())
					{
						record->WriteToDb();
					}
					if (VOICE_BURST == record->CallStatus() ||
						VOICE_END_BURST == record->CallStatus() ||
						VOICE_START == record->CallStatus())
					{
						g_pNet->wlCall(record->call_type, record->src_radio, record->target_radio, OPERATE_CALL_END, (record->call_type == g_playCalltype && record->target_radio == g_playTargetId));
					}
					removeItem(&m_records, record);
					curItem->pNext = NULL;
					freeList(curItem);
					curItem = m_records;
					delete record;
					record = NULL;
				}
				else
				{
					curItem = curItem->pNext;
				}
			}
			else
			{
				removeItem(&m_records, record);
				curItem->pNext = NULL;
				freeList(curItem);
				curItem = m_records;
			}
		}
		RELEASELOCK(m_mutexRecords);
		Sleep(SLEEP_CHECK_AMBE_THREAD);
	}
	m_pLog->AddLog("CheckRecordsThread End");
}

work_item_t* NSWLNet::findTimeOutItemAndRemove(unsigned long peerId, const char Opcode, const char wlOpcode, work_item_type_enum type)
{
	work_item_t* rlt = NULL;
	find_item_condition_t condition = { 0 };
	condition.peerId = peerId;
	condition.Opcode = Opcode;
	condition.wlOpcode = wlOpcode;
	condition.type = type;
	work_item_t* p = NULL;
	TRYLOCK(m_mutexWorkTimeOutItems);
	pLinkItem item = findItem(m_workTimeOutItems, &condition, &FuncFindItem);
	if (item)
	{
		if (item->data)
		{
			rlt = (work_item_t*)item->data;
			removeItem(&m_workTimeOutItems, rlt);
		}
		item->pNext = NULL;
		freeList(item);
	}
	RELEASELOCK(m_mutexWorkTimeOutItems);
	return rlt;
}

void NSWLNet::getCallRequestRltInfo(decline_reason_code_info_t &declineReasonCodeInfo)
{
	unsigned char value = declineReasonCodeInfo.Value;
	memset(&declineReasonCodeInfo, 0, sizeof(decline_reason_code_info_t));
	declineReasonCodeInfo.Value = value;
	declineReasonCodeInfo.BhaveGet = true;
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

void NSWLNet::setCurrentSendVoicePeer(NSWLPeer* value)
{
	m_currentSendVoicePeer = value;
}

NSWLPeer* NSWLNet::CurrentSendVoicePeer()
{
	return m_currentSendVoicePeer;
}

call_thread_status_enum NSWLNet::CallThreadStatus()
{
	return m_callThreadStatus;
}

void NSWLNet::CallStopUnnormal()
{
	if (Call_Thread_Send_Burst == CallThreadStatus())
	{
		setCallThreadStatus(Call_Thread_Call_Fail);
	}

}

void NSWLNet::setCallThreadStatus(call_thread_status_enum value)
{
	if (value != m_callThreadStatus)
	{
		m_pLog->AddLog("====Call Thread Status From %d To %d====", m_callThreadStatus, value);
		m_callThreadStatus = value;
		ContinueCallThread();
	}
}

unsigned long NSWLNet::CallId()
{
	return m_callId;
}

void NSWLNet::setCallId(unsigned long value)
{
	m_callId = value;
}

unsigned int __stdcall NSWLNet::CallThreadProc(void* pArguments)
{
	NSWLNet* p = (NSWLNet*)pArguments;
	if (p)
	{
		p->CallThread();
	}
	return 0;
}

void NSWLNet::CallThread()
{
	m_pLog->AddLog("CallThread Start");
	m_callThreadTimer = CreateEvent(NULL, FALSE, FALSE, NULL);
	while (m_bThreadWork)
	{
		NSWLPeer* peer = CurrentSendVoicePeer();
		call_thread_status_enum status = CallThreadStatus();
		switch (status)
		{
		case Call_Thread_Author_No:
		case Call_Thread_Status_Idle:
		case Call_Thread_Wait_Reply:
		case Call_Thread_Send_Burst:
		case Call_Thread_Send_End:
		{
									 /*do nothing*/
		}
			break;
		case Call_Thread_Send_Request:
		{

										 ReadyMakeCall();
										 Make_Call(&m_makeCallParam);
										 /*准备起始语音包*/
										 AddRingAmbeItem(&m_startAmbe);
										 /*开始录音*/
										 g_pNSSound->setMicStatus(Mic_Start);
		}
			break;
		case Call_Thread_Call_Fail:
		{
									  //g_pNet->wlRequestCallEnd(CurCallCmd);
									  g_pNet->wlCallStatus(CurCallCmd.callType, m_netParam.local_radio_id, CurCallCmd.tartgetId, STATUS_CALL_END | REMOTE_CMD_FAIL, CurCallCmd.SessionId);
									  g_pNSSound->setMicStatus(Mic_Stop);
									  setCallThreadStatus(Call_Thread_Status_Idle);
		}
			break;
		default:
			break;
		}
		WaitForSingleObject(m_callThreadTimer, INFINITE);
	}
	m_callThreadTimer = NULL;
	m_pLog->AddLog("CallThread End");
}


void NSWLNet::Make_Call(make_call_param_t* p)
{
	if (p == NULL || p->targetID == 0)
	{
		m_pLog->AddLog("invalid call param");
		setCallThreadStatus(Call_Thread_Call_Fail);
	}
	else
	{

		p->callID = CallId() + 1;
		work_mode_enum work_mode = m_netParam.work_mode;
		if (IPSC == work_mode)
		{
			p->slotNumber = SLOT1;
		}
		else
		{
			p->slotNumber = NULL_SLOT;
		}
		find_peer_condition_t condition = { 0 };
		condition.peer_id = m_netParam.matser_peer_id;
		NSWLPeer* peer = FindPeersItem(&condition);
		if (peer)
		{
			if (WL_REG_SUCCESS == peer->WlRegStatus())
			{
				//首先判断是否为callback
				peer = FindCallBacksItemAndRemove(p);
				if (peer)
				{
					m_pLog->AddLog("This is a CallBack");
					p->slotNumber = peer->SlotNumber();
					work_item_t* item = peer->Build_WL_VC_CHNL_CTRL_REQUEST(p);
					sendWorkItemNetData(item);
					setCallThreadStatus(Call_Thread_Wait_Reply);
					if (item)
					{
						delete item;
						item = NULL;
					}
				}
				else
				{
					m_pLog->AddLog("This is a NewCall");
					if (IPSC == work_mode)
					{
						find_peer_condition_t condition = { 0 };
						condition.peer_id = m_netParam.matser_peer_id;
						peer = FindPeersItem(&condition);
					}
					else
					{
						peer = SitePeer();
					}
					if (peer)
					{
						work_item_t* item = peer->Build_WL_VC_CHNL_CTRL_REQUEST(p);
						sendWorkItemNetData(item);
						setCallThreadStatus(Call_Thread_Wait_Reply);
						if (item)
						{
							delete item;
							item = NULL;
						}
					}
					else
					{
						m_pLog->AddLog("peer is null,maybe no map broadcast,wait little minuts try");
						setCallThreadStatus(Call_Thread_Call_Fail);
					}
				}
				p->peer = peer;
			}
			else
			{
				m_pLog->AddLog("peer %lu wl reg fail or unreg", peer->PeerId());
				setCallThreadStatus(Call_Thread_Call_Fail);
			}
		}
		else
		{
			m_pLog->AddLog("peer is null,maybe no map broadcast,wait little minuts try");
			setCallThreadStatus(Call_Thread_Call_Fail);
		}
	}
}

NSWLPeer* NSWLNet::FindCallBacksItemAndRemove(make_call_param_t* p)
{
	NSWLPeer* peer = NULL;
	NSWLPeer* temp = NULL;
	pLinkItem item = NULL;
	TRYLOCK(m_mutexCallBackPeers);
	pLinkList head = m_callBackPeers;
	while (head)
	{
		if (head->data)
		{
			temp = (NSWLPeer*)head->data;
			NSRecordFile* record = temp->RecordFile();
			if (record)
			{
				if (record->call_type == GROUP_CALL &&
					p->callType == record->call_type)
				{
					if (record->target_radio == p->targetID)
					{
						peer = temp;
						break;
					}
				}
				else if (record->call_type == PRIVATE_CALL &&
					p->callType == record->call_type)
				{
					/*目标一致*/
					if (record->src_radio == m_netParam.local_radio_id &&
						record->target_radio == p->targetID)
					{
						peer = temp;
						break;
					}
					/*目标和源对调*/
					else if (record->src_radio == p->targetID &&
						record->target_radio == m_netParam.local_radio_id)
					{
						peer = temp;
						break;
					}
				}
				head = head->pNext;
			}
			else
			{
				item = removeItem(&m_callBackPeers, temp);
				if (item)
				{
					head = item->pNext;
					item->pNext = NULL;
					freeList(item);
					item = NULL;
				}
				else
				{
					head = NULL;
				}
			}
		}
		else
		{
			item = removeItem(&m_callBackPeers, NULL);
			if (item)
			{
				head = item->pNext;
				item->pNext = NULL;
				freeList(item);
				item = NULL;
			}
			else
			{
				head = NULL;
			}
		}
	}
	/*移除寻找成功的回叫peer*/
	if (peer)
	{
		item = removeItem(&m_callBackPeers, peer);
		if (item)
		{
			item->pNext = NULL;
			freeList(item);
			item = NULL;
		}
	}
	RELEASELOCK(m_mutexCallBackPeers);
	return peer;
}

NSWLPeer* NSWLNet::SitePeer()
{
	return m_sitePeer;
}

void NSWLNet::CallStart(make_call_param_t *p)
{
	m_makeCallParam.callType = p->callType;
	m_makeCallParam.targetID = p->targetID;
	setCallThreadStatus(Call_Thread_Send_Request);
}

void NSWLNet::clearRingAmbeSend()
{
	TRYLOCK(m_mutexAmbeSend);
	freeRingBuffer(m_ringAmbeSend);
	m_ringAmbeSend = NULL;
	RELEASELOCK(m_mutexAmbeSend);
}

int NSWLNet::sizeRingAmbeSend()
{
	int rlt = 0;
	TRYLOCK(m_mutexAmbeSend);
	rlt = getRingBufferSize(m_ringAmbeSend);
	RELEASELOCK(m_mutexAmbeSend);
	return rlt;
}

void NSWLNet::AddRingAmbeItem(send_ambe_voice_encoded_frames_t* item)
{
	TRYLOCK(m_mutexAmbeSend);
	push(m_ringAmbeSend, item);
	RELEASELOCK(m_mutexAmbeSend);
}

void NSWLNet::PopRingAmbeItem(send_ambe_voice_encoded_frames_t* item)
{
	TRYLOCK(m_mutexAmbeSend);
	pop(m_ringAmbeSend, item);
	RELEASELOCK(m_mutexAmbeSend);
}

void NSWLNet::ResetRingAmbe()
{
	send_ambe_voice_encoded_frames_t item;
	while (0 != sizeRingAmbeSend())
	{
		PopRingAmbeItem(&item);
	}
}

int NSWLNet::sendNetDataBase(const char* pData, int len, void* send_to)
{
	if (NULL == m_pMasterXqttnet)
	{
		m_pLog->AddLog("sendNetDataBase fail,m_pMasterXqttnet is null");
		return 0;
	}
	int rlt = 0;
	TRYLOCK(m_mutexSend);
	rlt = sendDataUdp(m_pMasterXqttnet, pData, len, (SOCKADDR_IN*)send_to, sizeof(SOCKADDR_IN));
	RELEASELOCK(m_mutexSend);
	return rlt;
}

void NSWLNet::Build_T_WL_PROTOCOL_21(T_WL_PROTOCOL_21& networkData, bool start)
{
	NSWLPeer* peer = CurrentSendVoicePeer();
	networkData.algorithmID = ALGORITHMID;
	if (m_burstType == (char)BURST_T)
	{
		m_burstType = BURST_A;
	}
	networkData.burstType = m_burstType;
	m_burstType++;
	networkData.callAttributes = CALL_ATTRIBUTES;
	networkData.callID = CallId();
	networkData.callType = m_makeCallParam.callType;
	networkData.currentLinkProtocolVersion = Wireline_Protocol_Version;
	networkData.IV = VALUE_IV;
	networkData.keyID = KEY_ID;
	networkData.MFID = VALUE_MFID;
	networkData.oldestLinkProtocolVersion = Wireline_Protocol_Version;
	networkData.Opcode = WL_PROTOCOL;
	networkData.peerID = m_netParam.local_peer_id;
	networkData.rawRssiValue = VALUE_RSSI;
	networkData.RTPInformationField.header = BURST_RTP_HEADER;
	networkData.sourceID = m_netParam.local_radio_id;
	networkData.targetID = m_makeCallParam.targetID;
	if (peer)
	{
		networkData.slotNumber = peer->SlotNumber();
	}
	else
	{
		networkData.slotNumber = NULL_SLOT;
	}
	if (start)
	{
		networkData.RTPInformationField.MPT = BURST_START_RTP_MPT;
	}
	else
	{
		networkData.RTPInformationField.MPT = BURST_NORMAL_RTP_MPT;
	}
	networkData.RTPInformationField.SequenceNumber = m_SequenceNumber;
	m_SequenceNumber++;
	networkData.RTPInformationField.SSRC = BURST_RTP_SSRC;
	networkData.RTPInformationField.Timestamp = m_Timestamp;
	m_Timestamp += 480;
	networkData.serviceOption = BURST_SERVICEOPTION;
	networkData.wirelineOpcode = WL_VC_VOICE_BURST;
}

void NSWLNet::initVoiceBurst()
{
	m_startAmbe.start = true;
	m_burstAmbe.start = false;
}

short NSWLNet::Build_WL_VC_VOICE_BURST(CHAR* pPacket, T_WL_PROTOCOL_21* pData)
{
	short size = 0;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	/*wirelineOpcode*/
	pPacket[5] = pData->wirelineOpcode;
	/*slotNumber*/
	pPacket[6] = pData->slotNumber;
	/*callID*/
	*((DWORD*)(&pPacket[7])) = htonl(pData->callID);
	/*callType*/
	pPacket[11] = pData->callType;
	/*source ID*/
	*((DWORD*)(&pPacket[12])) = htonl(pData->sourceID);
	/*target ID*/
	*((DWORD*)(&pPacket[16])) = htonl(pData->targetID);
	/*callAttributes*/
	pPacket[20] = pData->callAttributes;
	/*RESERVED*/
	pPacket[21] = 0x00;
	/*RTP Information Field*/
	pPacket[22] = pData->RTPInformationField.header;
	pPacket[23] = pData->RTPInformationField.MPT;
	*((WORD*)(&pPacket[24])) = htons(pData->RTPInformationField.SequenceNumber);
	*((DWORD*)(&pPacket[26])) = htonl(pData->RTPInformationField.Timestamp);
	*((DWORD*)(&pPacket[30])) = htonl(pData->RTPInformationField.SSRC);
	/*burstType*/
	pPacket[34] = pData->burstType;
	/*RESERVED*/
	pPacket[35] = 0x00;
	/*MFID*/
	pPacket[36] = pData->MFID;
	/*serviceOptions*/
	pPacket[37] = pData->serviceOption;
	/*algorithmID*/
	pPacket[38] = pData->algorithmID;
	/*keyID*/
	pPacket[39] = pData->keyID;
	/*IV*/
	*((DWORD*)(&pPacket[40])) = htonl(pData->IV);
	/*AMBE voice encoded frames*/

	/*原来两者都是通过逐字拷贝来实现的。
	但是“等号赋值”被编译器翻译成一连串
	的MOV指令，而memcpy则是一个循环。“等
	号赋值”比memcpy快，并不是快在拷贝方
	式上，而是快在程序流程上。测试发现，
	“等号赋值”的长度必须小于等于128，
	并且是机器字长的倍数，才会被编译成连
	续MOV形式，否则会被编译成调用memcpy。
	而同样的，如果memcpy复制的长度小于等
	于128且是机器字长的整数倍，会被编译成
	MOV形式。所以，无论你的代码中如何写，
	编译器都会做好优化工作。*/
	//memcpy(&pPacket[44], pData->AMBEVoiceEncodedFrames, 20);
	*((AMBE_VOICE_ENCODED_FRAMES*)(&pPacket[44])) = pData->AMBEVoiceEncodedFrames;
	/*rawRssiValue*/
	*((WORD*)(&pPacket[64])) = htons(pData->rawRssiValue);
	/*Current / Accepted Wireline Protocol Version*/
	pPacket[66] = pData->currentLinkProtocolVersion;
	/*Oldest Wireline Protocol Version*/
	pPacket[67] = pData->oldestLinkProtocolVersion;
	size = 68;
	/*getWirelineAuthentication*/
	getWirelineAuthentication(pPacket, size);
	return size;
}

void NSWLNet::getWirelineAuthentication(char* pPacket, short &size)
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

void NSWLNet::ReadyMakeCall()
{
	/*初始化通话相关参数*/
	m_TxSubCount = 0;
	m_burstType = BURST_A;
	m_SequenceNumber = 1;
	m_Timestamp = 0;
	ResetRingAmbe();
}

void NSWLNet::CallStop()
{
	if (Call_Thread_Status_Idle != CallThreadStatus())
	{
		setCallThreadStatus(Call_Thread_Send_End);
	}
	else
	{
		m_pLog->AddLog("no call need stop");
		g_pNet->wlCallStatus(CurCallCmd.callType, m_netParam.local_radio_id, CurCallCmd.tartgetId, STATUS_CALL_END | REMOTE_CMD_SUCCESS, CurCallCmd.SessionId);
	}
}

void NSWLNet::HandleAmbeData(void* pData, unsigned long length)
{
	unsigned char* pVoiceBytes = (unsigned char*)pData;
	char* pVoice = &m_burstAmbe.ambe.data[1];
	switch (m_TxSubCount)
	{
	case 0:
	{
			  pVoice[0] = *(pVoiceBytes);
			  pVoice[1] = *(pVoiceBytes + 1);
			  pVoice[2] = *(pVoiceBytes + 2);
			  pVoice[3] = *(pVoiceBytes + 3);
			  pVoice[4] = *(pVoiceBytes + 4);
			  pVoice[5] = *(pVoiceBytes + 5);
			  pVoice[6] = (*(pVoiceBytes + 6)) & 0x80;

			  m_TxSubCount = 1;
	}
		break;
	case 1:  //Note: Check that right shift shifts in zeros.
	{

				 pVoice[6] |= (*(pVoiceBytes)) >> 2;
				 pVoice[7] = ((*(pVoiceBytes)) << 6) | ((*(pVoiceBytes + 1)) >> 2);
				 pVoice[8] = ((*(pVoiceBytes + 1)) << 6) | ((*(pVoiceBytes + 2)) >> 2);
				 pVoice[9] = ((*(pVoiceBytes + 2)) << 6) | ((*(pVoiceBytes + 3)) >> 2);
				 pVoice[10] = ((*(pVoiceBytes + 3)) << 6) | ((*(pVoiceBytes + 4)) >> 2);
				 pVoice[11] = ((*(pVoiceBytes + 4)) << 6) | ((*(pVoiceBytes + 5)) >> 2);
				 pVoice[12] = (((*(pVoiceBytes + 5)) << 6) | ((*(pVoiceBytes + 6)) >> 2)) & 0xE0;

				 m_TxSubCount = 2;
	}
		break;
	case 2:
	{
			  pVoice[12] |= (*(pVoiceBytes)) >> 4;
			  pVoice[13] = ((*(pVoiceBytes)) << 4) | ((*(pVoiceBytes + 1)) >> 4);
			  pVoice[14] = ((*(pVoiceBytes + 1)) << 4) | ((*(pVoiceBytes + 2)) >> 4);
			  pVoice[15] = ((*(pVoiceBytes + 2)) << 4) | ((*(pVoiceBytes + 3)) >> 4);
			  pVoice[16] = ((*(pVoiceBytes + 3)) << 4) | ((*(pVoiceBytes + 4)) >> 4);
			  pVoice[17] = ((*(pVoiceBytes + 4)) << 4) | ((*(pVoiceBytes + 5)) >> 4);
			  pVoice[18] = (((*(pVoiceBytes + 5)) << 4) | ((*(pVoiceBytes + 6)) >> 4)) & 0xF8;

			  if (sizeRingAmbeSend() < RING_SIZE_AMBE_SEND)
			  {
				  //m_pLog->AddLog("add a frame");
				  AddRingAmbeItem(&m_burstAmbe);
			  }
			  else
			  {
				  m_pLog->AddLog("no add a frame,will end call");
				  setCallThreadStatus(Call_Thread_Call_Fail);
			  }
			  m_TxSubCount = 0;
	}
		break;
	}
}

void NSWLNet::ContinueCallThread()
{
	if (m_callThreadTimer)
	{
		SetEvent(m_callThreadTimer);
	}
}

void PASCAL NSWLNet::SendAmbeDataProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	NSWLNet* p = (NSWLNet *)dwUser;
	if (p)
	{
		p->SendAmbeData();
	}
}

void NSWLNet::SendAmbeData()
{
	call_thread_status_enum status = CallThreadStatus();
	NSWLPeer* peer = CurrentSendVoicePeer();
	if (Call_Thread_Send_Burst == status)
	{
		if (peer)
		{
			if (sizeRingAmbeSend() > 0)
			{
				//m_pLog->AddLog("pop a frame");
				send_ambe_voice_encoded_frames_t voice = { 0 };
				PopRingAmbeItem(&voice);
				Build_T_WL_PROTOCOL_21(m_vcBurst, voice.start);
				if (voice.start)
				{
					//g_pNet->wlCallStatus(CurCallCmd.callType, m_netParam.local_radio_id, CurCallCmd.tartgetId, STATUS_CALL_START | REMOTE_CMD_SUCCESS, CurCallCmd.SessionId);
					if (m_localRecordFile)
					{
						delete m_localRecordFile;
						m_localRecordFile = NULL;
					}
					m_localRecordFile = new NSRecordFile(m_pManager, peer);
					m_localRecordFile->src_peer_id = m_netParam.local_peer_id;
					m_localRecordFile->src_radio = m_netParam.local_radio_id;
					m_localRecordFile->target_radio = m_makeCallParam.targetID;
					m_localRecordFile->call_id = CallId();
					m_localRecordFile->call_type = m_makeCallParam.callType;
					m_localRecordFile->src_slot = peer->SlotNumber();
					memcpy(m_localRecordFile->SessionId, CurCallCmd.SessionId, SESSION_SIZE);
					m_localRecordFile->setCallStatus(VOICE_START);
				}
				else
				{
					/*将数据写入数据库*/
					if (m_localRecordFile)
					{
						char voiceFrame[21] = { 0 };
						Handle_WL_PROTOCOL_21_Ambe_Data(voiceFrame, voice.ambe.data);
						m_localRecordFile->WriteVoiceFrame(voiceFrame, 21, false);
						m_localRecordFile->setCallStatus(VOICE_BURST);
						m_localRecordFile->src_rssi = m_vcBurst.rawRssiValue;
					}
				}
				m_vcBurst.AMBEVoiceEncodedFrames = voice.ambe;
				m_sendBuffer.net_length = Build_WL_VC_VOICE_BURST(m_sendBuffer.net_data, &m_vcBurst);
				sendNetDataBase(m_sendBuffer.net_data, m_sendBuffer.net_length, &peer->m_sockaddr);
			}
			else
			{

					m_pLog->AddLog("not pop a frame,will send empty 60ms ambe");
					Build_T_WL_PROTOCOL_21(m_vcBurst, false);
					m_vcBurst.AMBEVoiceEncodedFrames = m_startAmbe.ambe;
					m_sendBuffer.net_length = Build_WL_VC_VOICE_BURST(m_sendBuffer.net_data, &m_vcBurst);
					sendNetDataBase(m_sendBuffer.net_data, m_sendBuffer.net_length, &peer->m_sockaddr);
			}
		}
		else
		{
			m_pLog->AddLog("peer is null");
		}
	}
	else if (Call_Thread_Send_End == status)
	{
		if (peer)
		{
			if (m_burstType != (char)BURST_T)
			{
				Build_T_WL_PROTOCOL_21(m_vcBurst, false);
				//memcpy(&m_sendBuffer.net_data[44], &m_startAmbe.ambe.data[0], sizeof(m_startAmbe.ambe.data));
				m_vcBurst.AMBEVoiceEncodedFrames = m_startAmbe.ambe;
				m_sendBuffer.net_length = Build_WL_VC_VOICE_BURST(m_sendBuffer.net_data, &m_vcBurst);
				sendNetDataBase(m_sendBuffer.net_data, m_sendBuffer.net_length, &peer->m_sockaddr);
				//m_pLog->AddLog("SendAmbeData Call_Thread_Send_End_Fill");
			}
			else
			{
				Build_T_WL_PROTOCOL_19(m_vcEnd);
				m_sendBuffer.net_length = Build_WL_VC_VOICE_END_BURST(m_sendBuffer.net_data, &m_vcEnd);
				sendNetDataBase(m_sendBuffer.net_data, m_sendBuffer.net_length, &peer->m_sockaddr);
				if (m_localRecordFile)
				{
					m_localRecordFile->setCallStatus(VOICE_END_BURST);
					m_localRecordFile->WriteToDb();
				}
				g_pNSSound->setMicStatus(Mic_Stop);
				setCallThreadStatus(Call_Thread_Status_Idle);
			}
		}
		else
		{
			m_pLog->AddLog("peer is null");
			g_pNSSound->setMicStatus(Mic_Stop);
			setCallThreadStatus(Call_Thread_Status_Idle);
		}

	}
}

short NSWLNet::Build_WL_VC_VOICE_END_BURST(CHAR* pPacket, T_WL_PROTOCOL_19* pData)
{
	short size = 0;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	/*wirelineOpcode*/
	pPacket[5] = pData->wirelineOpcode;
	/*slotNumber*/
	pPacket[6] = pData->slotNumber;
	/*callID*/
	*((DWORD*)&pPacket[7]) = htonl(pData->callID);
	/*callType*/
	pPacket[11] = pData->callType;
	/*source ID*/
	*((DWORD*)&pPacket[12]) = htonl(pData->sourceID);
	/*target ID*/
	*((DWORD*)&pPacket[16]) = htonl(pData->targetID);
	/*RTP Information Field*/
	pPacket[20] = pData->RTPInformationField.header;
	pPacket[21] = pData->RTPInformationField.MPT;
	*((WORD*)(&pPacket[22])) = htons(pData->RTPInformationField.SequenceNumber);
	*((DWORD*)(&pPacket[24])) = htonl(pData->RTPInformationField.Timestamp);
	*((DWORD*)(&pPacket[28])) = htonl(pData->RTPInformationField.SSRC);
	/*burstType*/
	pPacket[32] = pData->burstType;
	/*RESERVED*/
	/*MFID*/
	pPacket[34] = pData->MFID;
	/*serviceOption*/
	pPacket[35] = pData->serviceOption;//clear call
	/*Current / Accepted Wireline Protocol Version*/
	pPacket[36] = pData->currentLinkProtocolVersion;
	/*Oldest Wireline Protocol Version*/
	pPacket[37] = pData->oldestLinkProtocolVersion;
	size = 38;
	/*getWirelineAuthentication*/
	getWirelineAuthentication(pPacket, size);
	return size;
}

void NSWLNet::Build_T_WL_PROTOCOL_19(T_WL_PROTOCOL_19& networkData)
{
	NSWLPeer* peer = CurrentSendVoicePeer();
	networkData.burstType = m_burstType;
	networkData.callID = CallId();
	networkData.callType = m_makeCallParam.callType;
	networkData.currentLinkProtocolVersion = Wireline_Protocol_Version;
	networkData.MFID = VALUE_MFID;
	networkData.oldestLinkProtocolVersion = Wireline_Protocol_Version;
	networkData.Opcode = WL_PROTOCOL;
	networkData.peerID = m_netParam.local_peer_id;
	networkData.RTPInformationField.header = BURST_RTP_HEADER;
	networkData.RTPInformationField.MPT = BURST_END_RTP_MPT;
	networkData.RTPInformationField.SequenceNumber = m_SequenceNumber;
	networkData.RTPInformationField.SSRC = BURST_RTP_SSRC;
	networkData.RTPInformationField.Timestamp = m_Timestamp;
	networkData.serviceOption = BURST_SERVICEOPTION;
	if (peer)
	{
		networkData.slotNumber = peer->SlotNumber();
	}
	else
	{
		networkData.slotNumber = NULL_SLOT;
	}
	networkData.sourceID = m_netParam.local_radio_id;
	networkData.targetID = m_makeCallParam.targetID;
	networkData.wirelineOpcode = WL_VC_VOICE_END_BURST;
}

void NSWLNet::AddCallBacksItem(NSWLPeer* peer)
{
	if (NULL == peer)
	{
		return;
	}
	TRYLOCK(m_mutexCallBackPeers);
	appendData(&m_callBackPeers, peer);
	RELEASELOCK(m_mutexCallBackPeers);
}

void NSWLNet::clearCallBacks()
{
	TRYLOCK(m_mutexCallBackPeers);
	freeList(m_callBackPeers);
	m_callBackPeers = NULL;
	RELEASELOCK(m_mutexCallBackPeers);
}
