#include "stdafx.h"
#include "WLNet.h"
#include <string>


#define AUTHENTIC_ID_SIZE	4
#define VENDER_KEY_SIZE		20
static const unsigned char AuthenticId[AUTHENTIC_ID_SIZE] = { 0x01, 0x02, 0x00, 0x0d };
static const unsigned char VenderKey[VENDER_KEY_SIZE] = { 0x6b, 0xe5, 0xff, 0x95, 0x6a, 0xb5, 0xe8, 0x82, 0xa8, 0x6f, 0x29, 0x5f, 0x9d, 0x9d, 0x5e, 0xcf, 0xe6, 0x57, 0x61, 0x5a };


CWLNet::CWLNet(CMySQL *pDb,CManager *pManager)
: m_socket(INVALID_SOCKET)
, m_hWorkThread(INVALID_HANDLE_VALUE)
, m_bExit(TRUE)
, m_dwRecvMasterKeepAliveTime(0)
//add code by chenhaidong
, m_masterAddress(0)
, m_masterPort(0)
{
	//m_isRequestNewCall = false;
	//m_isFirstBurstA = true;
	m_burstType = BURST_A;
	m_SequenceNumber = 1;
	m_Timestamp = 0;

	m_cuurentSendType = SEND_TYPE_MIC;
	m_callSequenceNumber = 0x00;

	m_BytesSent = 0;

	m_pCurrentBuildSendvoice = NULL;

	m_TxSubCount = 0;
	m_TxBurstType = (IPSCBurstType)0;

	memset(m_addr, 0, sizeof(wchar_t)* 32);
	memset(m_port, 0, sizeof(wchar_t)* 8);
	memset(m_peerID, 0, sizeof(wchar_t)* 128);
	memset(m_NumberStr, 0, sizeof(wchar_t)* 8);

	m_CurrentRecvBuffer.buf = m_RxBuffer;
	m_CurrentRecvBuffer.len = sizeof(m_RxBuffer);

	if (!g_tool.GetApplicationPath(m_strSettingFilePath))
	{
		sprintf_s(m_reportMsg, "GetApplicationPath fail");
		sendLogToWindow();
	}
	else
	{
		wcscat_s(m_strSettingFilePath, L"\\sys.ini");
	}

	m_bIsSending = false;
	m_wlInitNewCallEvent = NULL;
	m_callStatus = CALL_IDLE;
	m_endRecordEvent = NULL;
	m_pCurrentSendVoicePeer = NULL;
	m_pSitePeer = NULL;
	m_retryRequestCallCount = REQUEST_CALL_REPEAT_FREQUENCY;
	m_pEventLoger = new WLRecord(pDb);
	m_pDb = pDb;
	//m_dongleIdleEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	m_pManager = pManager;
	m_dwChangeToCurrentTick = 0;
	m_pPlayCall = NULL;
}

CWLNet::~CWLNet()
{
	clearPeers();
	clearSendVoices();
	//StopNet();
}

DWORD WINAPI CWLNet::NetThreadProc(LPVOID pVoid)
{
	CWLNet* pThis = reinterpret_cast<CWLNet*>(pVoid);
	return pThis->NetThread();
}

DWORD CWLNet::NetThread()
{


	/************************************************************************/
	/* 注意事项
	/*
	Version 01.02 Motorola Solutions Confidential Proprietary 106
	1532 It is recommended that the third party application shall 1532 automatically retry when
	1533 receiving the following decline reason codes for a new call set up:
	1534  Non-Rest channel Repeater
	1535  Destination Slot Busy
	1536  Race Condition Failure
	1537  Undefined Call Failure
	/************************************************************************/


	/************************************************************************/
	/* IPSC
	MASTER
	/*1 A->R 0x90 启动定时器，超过规定时间则再次进行注册
	/*2 R->A 0x91 获取当前系统中的peer数量,为0则进入步骤5
	/*3 A->R 0x92 启动定时器，超过规定时间则进行步骤1
	/*4 R->A 0x93 获取当前系统中的所有peer的详细信息
	/*5 A->R 0x96 启动定时器，超过规定时间则进行步骤1
	/*6 R->A 0x97 刷新定时器
	OTHER PEERS
	/*4.1 A->R 0x94 启动定时器，超过规定的时间再次进行注册
	/*4.2 R->A 0x94 A->R 0x95 立即回复请求注册并启动定时器(CPS决定时长)
	/*4.3 A->R 0x98 时间到若还未收到心跳包，则发送心跳包,并启动定时器，超过规定时间则再次进行步骤4.1
	/*4.4 R->A 0x98 A->R 0x99 心跳包回应后立即进行步骤4.3
	WL(0xb2....)
	/*4.2.1 A->R 0x01 对非第三方PEER SLOT1进行wireline注册,启动定时器,超过时间再次进行wireline注册
	/*4.2.2 R->A 0x02 记录注册结果
	/*4.2.3 A->R 0x01 对非第三方PEER SLOT2进行wireline注册,启动定时器,超过时间再次进行wireline注册
	/*4.2.4 R->A 0x02 记录注册结果

	BURST_VOICE(0xb2....)
	/*1 A->R 0x13 请求初始化call,启动定时器，超过规定的时间再次进行请求
	/*2 R->A 0x16 记录请求结果，根据结果进行其他操作
	/*2.1 A->R 进行步骤3
	/*2.2 A->R 再次进行步骤1(当收到0x11是IDLE时再次请求可增加成功率)
	/*3 A->R 0x21 发送AMBE数据
	/*4 A->R 0x19 本次通话结束
	/*5 R->A 0x20 本次通话Call Hang
	/*5.1 R->A 0x18 存在call back
	/*5.2 R->A 0x21 AMBE数据
	/*5.3 R->A 0x19 本次通话结束
	/*5.4 R->A 0x20 本次通话Call Hang
	/*5.5 A->R 需要回复,进行步骤1
	/*5.6 A->R 0x20 本次通话Call End
	/*6 R->A 0x20 本次通话Call End
	注:1 1342 When a call stream ends, if the third party application wants to talk back, it shall wait for
	1343 the WL_VC_CALL_SESSION_STATUS with status of Call Hang before sending
	1344 WL_VC_CHNL_CTRL_REQUEST.
	当一个通话结束，第三方应用想进行回话。应该在收到CALL HANG 时再次使用不同的call id进行初始化call
	2 R->A 有可能存在0x18 0x19和0x20丢失的情况，分别用增加头、超时处理和Session Timer来应对以上情况。
	3 发送的目标均为非第三方的Repeater(可根据当前channel的状态，例如IDLE来决定向哪一个Repeater发送),另集群系统均为local area.
	/************************************************************************/

	/************************************************************************/
	/* CPC
	MASTER
	/*1 A->R 0x90 启动定时器，超过规定时间则再次进行注册
	/*2 R->A 0x91 获取当前系统中的peer数量,为0则进入步骤5
	/*3 A->R 0x92 启动定时器，超过规定时间则进行步骤1
	/*4 R->A 0x93 获取当前系统中的所有peer的详细信息
	/*5 A->R 0x96 启动定时器，超过规定时间则进行步骤1
	/*6 R->A 0x97 刷新定时器
	OTHER PEERS
	/*4.1 A->R 0x94 启动定时器，超过规定的时间再次进行注册(包括 peer id = 0)
	/*4.2 R->A 0x94 A->R 0x95 立即回复请求注册并启动定时器(CPS决定时长)
	/*4.3 A->R 0x98 时间到若还未收到心跳包，则发送心跳包,并启动定时器，超过规定时间则再次进行步骤4.1
	/*4.4 R->A 0x98 A->R 0x99 心跳包回应后立即进行步骤4.3
	WL(0xb2....)
	/*4.2.1 A->R 0x01 对非第三方PEER进行wireline注册,启动定时器,超过时间再次进行wireline注册(不包括 peer id = 0)
	/*4.2.2 R->A 0x02 记录注册结果

	BURST_VOICE(0xb2....)
	/*1 A->R 0x13 请求初始化call,启动定时器，超过规定的时间再次进行请求
	/*2 R->A 0x16 记录请求结果，根据结果进行其他操作
	/*2.1 A->R 进行步骤3
	/*2.2 A->R 再次进行步骤1(当收到0x11是IDLE时再次请求可增加成功率)
	/*3 A->R 0x21 发送AMBE数据
	/*4 A->R 0x19 本次通话结束
	/*5 R->A 0x20 本次通话Call Hang
	/*5.1 R->A 0x18 存在call back
	/*5.2 R->A 0x21 AMBE数据
	/*5.3 R->A 0x19 本次通话结束
	/*5.4 R->A 0x20 本次通话Call Hang
	/*5.5 A->R 需要回复,进行步骤1(此处因发送给Last Peer而不是The Site Peer)
	/*5.6 A->R 0x20 本次通话Call End
	/*6 R->A 0x20 本次通话Call End
	注:1 1342 When a call stream ends, if the third party application wants to talk back, it shall wait for
	1343 the WL_VC_CALL_SESSION_STATUS with status of Call Hang before sending
	1344 WL_VC_CHNL_CTRL_REQUEST.
	当一个通话结束，第三方应用想进行回话。应该在收到CALL HANG 时再次使用不同的call id进行初始化call
	2 R->A 有可能存在0x18 0x19和0x20丢失的情况，分别用增加头、超时处理和Session Timer来应对以上情况。
	3 发送的目标均为The Site Peer,另集群系统均为local area.具体的Repeater信息会在0x16(Granted)中。
	4(暂时不实装) 1476 The third party application first sends the CSBK
	1487 private call request to the Site Peer at Site 1. After the target radio responds, the third
	1488 party application directly initiates the private voice call request to the repeater peer at
	1489 Site 2 whose slot is used for the CSBK private call request transmission. The third party
	1490 application shall initiate the Private Voice Call request before the voice call session
	1491 changed from hang state to end state. The repeater’s call hang timer duration is
	1492 configurable in MOTOTRBO CPS.
	进行个呼的注意事项，其它通话部分与组呼一致
	/************************************************************************/

	/************************************************************************/
	/* LCP
	MASTER
	/*1 A->R 0x90 启动定时器，超过规定时间则再次进行注册
	/*2 R->A 0x91 获取当前系统中的peer数量,为0则进入步骤5
	/*3 A->R 0x92 启动定时器，超过规定时间则进行步骤1
	/*4 R->A 0x93 获取当前系统中的所有peer的详细信息
	/*5 A->R 0x96 启动定时器，超过规定时间则进行步骤1
	/*6 R->A 0x97 刷新定时器
	OTHER PEERS
	/*4.1 A->R 0x94 启动定时器，超过规定的时间再次进行注册(包括 peer id = 0)
	/*4.2 R->A 0x94 A->R 0x95 立即回复请求注册并启动定时器(CPS决定时长)
	/*4.3 A->R 0x98 时间到若还未收到心跳包，则发送心跳包,并启动定时器，超过规定时间则再次进行步骤4.1
	/*4.4 R->A 0x98 A->R 0x99 心跳包回应后立即进行步骤4.3
	WL(0xb2....)
	/*4.2.1 A->R 0x01 对非第三方PEER进行wireline注册,启动定时器,超过时间再次进行wireline注册(不包括 peer id = 0)
	/*4.2.2 R->A 0x02 记录注册结果

	BURST_VOICE(0xb2....)
	/*1 A->R 0x13 请求初始化call,启动定时器，超过规定的时间再次进行请求
	/*2 R->A 0x16 记录请求结果，根据结果进行其他操作
	/*2.1 A->R 进行步骤3
	/*2.2 A->R 再次进行步骤1(当收到0x11是IDLE时再次请求可增加成功率)
	/*3 A->R 0x21 发送AMBE数据
	/*4 A->R 0x19 本次通话结束
	/*5 R->A 0x20 本次通话Call Hang
	/*5.1 R->A 0x18 存在call back
	/*5.2 R->A 0x21 AMBE数据
	/*5.3 R->A 0x19 本次通话结束
	/*5.4 R->A 0x20 本次通话Call Hang
	/*5.5 A->R 需要回复,进行步骤1(此处因发送给Last Peer而不是The Site Peer)
	/*5.6 A->R 0x20 本次通话Call End
	/*6 R->A 0x20 本次通话Call End
	注:1 1342 When a call stream ends, if the third party application wants to talk back, it shall wait for
	1343 the WL_VC_CALL_SESSION_STATUS with status of Call Hang before sending
	1344 WL_VC_CHNL_CTRL_REQUEST.
	当一个通话结束，第三方应用想进行回话。应该在收到CALL HANG 时再次使用不同的call id进行初始化call
	2 R->A 有可能存在0x18 0x19和0x20丢失的情况，分别用增加头、超时处理和Session Timer来应对以上情况。
	3 发送的目标均为The Site Peer,另集群系统均为local area.具体的Repeater信息会在0x16(Granted)中。
	4(暂时不实装) 1476 The third party application first sends the CSBK
	1487 private call request to the Site Peer at Site 1. After the target radio responds, the third
	1488 party application directly initiates the private voice call request to the repeater peer at
	1489 Site 2 whose slot is used for the CSBK private call request transmission. The third party
	1490 application shall initiate the Private Voice Call request before the voice call session
	1491 changed from hang state to end state. The repeater’s call hang timer duration is
	1492 configurable in MOTOTRBO CPS.
	进行个呼的注意事项，其它通话部分与组呼一致
	/************************************************************************/

	int		rc = 0;
	DWORD	EventIndex = FIRSTPASS;

	// issue a first read request
	while (WSA_IO_PENDING != rc)
	{
		rc = IssueReadRequest();
		//LogEvent(StartListener, _T("Start To Receive Network Packet"));

		sprintf_s(m_reportMsg, "Start To Receive Network Packet");
		sendLogToWindow();

	}

	while (!m_bExit)
	{
		switch (m_WLStatus)
		{
			//发送0x90
		case STARTING:
			Net_RegisterLE(EventIndex);
			break;
			//确保0x90发送
		case WAITFOR_LE_MASTER_PEER_REGISTRATION_TX:
			Net_WAITFOR_LE_MASTER_PEER_REGISTRATION_TX(EventIndex);
			break;
			//解码0x91并发送0x92
		case WAITFOR_LE_MASTER_PEER_REGISTRATION_RESPONSE:
			Net_WAITFOR_LE_MASTER_PEER_REGISTRATION_RESPONSE(EventIndex);
			break;
			//确保0x92发送
		case WAITFOR_MAP_REQUEST_TX:
			Net_WAITFOR_MAP_REQUEST_TX(EventIndex);
			break;
			//解码0x93根据模式进行对应的注册
		case WAITFOR_LE_NOTIFICATION_MAP_BROADCAST:
			Net_WAITFOR_LE_NOTIFICATION_MAP_BROADCAST(EventIndex);
			break;
			//xnl connect start
		case XNL_CONNECT:
			Net_XNL_CONNECT(EventIndex);
			break;
		case WAITFOR_XNL_DEVICE_MASTER_QUERY_TX:
			Net_XNL_DEVICE_MASTER_QUERY_TX(EventIndex);
			break;
		case WAITFOR_XNL_MASTER_STATUS_BROADCAST:
			Net_WAITFOR_XNL_MASTER_STATUS_BROADCAST(EventIndex);
			break;
		case WAITFOR_XNL_DEVICE_AUTH_KEY_REQUEST_TX:
			Net_WAITFOR_XNL_DEVICE_AUTH_KEY_REQUEST_TX(EventIndex);
			break;
		case WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY:
			Net_WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY(EventIndex);
			break;
		case WAITFOR_XNL_DEVICE_CONNECT_REQUEST_TX:
			Net_WAITFOR_XNL_DEVICE_CONNECT_REQUEST_TX(EventIndex);
			break;
		case WAITFOR_XNL_DEVICE_CONNECT_REPLY:
			Net_WAITFOR_XNL_DEVICE_CONNECT_REPLY(EventIndex);
			break;
		case WAITFOR_XNL_DEVICE_SYSMAP_BROADCAST:
			Net_WAITFOR_XNL_DEVICE_SYSMAP_BROADCAST(EventIndex);
			break;
		case WAITFOR_XNL_DATA_MSG_DEVICE_INIT_1:
			Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_1(EventIndex);
			break;
		case WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2_TX:
			Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2_TX(EventIndex);
			break;
		case WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2:
			Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2(EventIndex);
			break;
		case WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3_TX:
			Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3_TX(EventIndex);
			break;
		case WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3:
			Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3(EventIndex);
			break;
		case WAITFOR_XNL_XCMP_READ_SERIAL:
			Net_XNL_XCMP_READ_SERIAL(EventIndex);
			break;
		case WAITFOR_XNL_XCMP_READ_SERIAL_RESULT:
			Net_WAITFOR_XNL_XCMP_READ_SERIAL_RESULT(EventIndex);

		case ALIVE:
			Net_MaintainAlive(EventIndex);
			break;
		case TRANSMITTING1:
			break;
		case TRANSMITTING2:
			break;
		case WAITINGFOR_LE_DEREGISTRATION_TXFREE:
			break;
		case WAITINGFOR_LE_DEREGISTRATION_TRANSMISSION:
			Net_WAITINGFOR_LE_DEREGISTRATION_TRANSMISSION(EventIndex);
			break;
		case BAILOUT:
			return -1;
		default:
			break;
		}

		EventIndex = WSAWaitForMultipleEvents(MAXEVENTS,
			m_eventArray,
			FALSE,
			NOACTIVITYTIMEOUT,
			FALSE);
		if (WSA_WAIT_FAILED == EventIndex){//Something really bad happened.
			rc = WSAGetLastError();
			return -2;
		}
	}

	return 0;
}

// void CWLNet::InitControlBuffer(DWORD dwSelfPeerId)
// {
// 	DWORD temp = dwSelfPeerId;
// 	CONFIG_LOCAL_PEER_ID = dwSelfPeerId;
// 	m_ControlProto[4] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_ControlProto[3] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_ControlProto[2] = (char)(temp & 0x000000FF);
// 	temp = temp >> 8;
// 	m_ControlProto[1] = (char)(temp & 0x000000FF);
// 
// 	if (CONFIG_RECORD_TYPE == IPSC)
// 	{
// 		m_ControlProto[5] = IPSC_PEERMODE_DEFAULT;
// 		m_ControlProto[6] = IPSC_PEERSERVICES_DEFAULT_4;
// 		m_ControlProto[7] = IPSC_PEERSERVICES_DEFAULT_3;
// 		m_ControlProto[8] = IPSC_PEERSERVICES_DEFAULT_2;
// 		m_ControlProto[9] = IPSC_PEERSERVICES_DEFAULT_1;
// 
// 		m_ControlProto[10] = IPSC_CURRENTLPVERSION_DEFAULT_2;
// 		m_ControlProto[11] = IPSC_CURRENTLPVERSION_DEFAULT_1;
// 		m_ControlProto[12] = IPSC_OLDESTLPVERSION_DEFAULT_2;
// 		m_ControlProto[13] = IPSC_OLDESTLPVERSION_DEFAULT_1;
// 	}
// 	else if (CONFIG_RECORD_TYPE == CPC)
// 	{
// 		m_ControlProto[5] = CPC_PEERMODE_DEFAULT;    // PEERMODE_DEFAULT
// 		m_ControlProto[6] = CPC_PEERSERVICES_DEFAULT_4;    // PEER SERVICE
// 		m_ControlProto[7] = CPC_PEERSERVICES_DEFAULT_3;
// 		m_ControlProto[8] = CPC_PEERSERVICES_DEFAULT_2;
// 		m_ControlProto[9] = CPC_PEERSERVICES_DEFAULT_1;
// 
// 		m_ControlProto[10] = CPC_CURRENTVERSION_DEFAULT_2;
// 		m_ControlProto[11] = CPC_CURRENTVERSION_DEFAULT_1;
// 		m_ControlProto[12] = CPC_OLDESTVERSION_DEFAULT_2;
// 		m_ControlProto[13] = CPC_OLDESTVERSION_DEFAULT_1;
// 	}
// 	else
// 	{
// 
// 		m_ControlProto[5] = LCP_PEERMODE_DEFAULT_1;
// 		m_ControlProto[6] = LCP_PEERMODE_DEFAULT_2;
// 
// 		m_ControlProto[7] = LCP_PEERSERVICES_DEFAULT_1;
// 		m_ControlProto[8] = LCP_PEERSERVICES_DEFAULT_2;
// 		m_ControlProto[9] = LCP_PEERSERVICES_DEFAULT_3;
// 		m_ControlProto[10] = LCP_PEERSERVICES_DEFAULT_4;
// 
// 		m_ControlProto[11] = 0x00;  // leadingChannelID
// 
// 		m_ControlProto[12] = LCP_CURRENTVERSION_DEFAULT_2;
// 		m_ControlProto[13] = LCP_CURRENTVERSION_DEFAULT_1;
// 
// 		m_ControlProto[14] = LCP_OLDESTVERSION_DEFAULT_2;
// 		m_ControlProto[15] = LCP_OLDESTVERSION_DEFAULT_1;
// 	}
// 
// 
// 	m_SendControlBuffer.buf = &m_ControlProto[0];
// 	m_SendControlBuffer.len = 0; //Not really necessary.
// }

BOOL CWLNet::StartNet(DWORD dwMasterIp
	, WORD wMasterPort
	, DWORD dwLocalIp
	, DWORD dwSelfPeerId
	, DWORD dwSelfRadioId
	//, int dwSelfSlot
	//, unsigned int dwSelfGroup
	, DWORD recType
	)
{
	if (g_net_connect)
	{
		StopNet();
	}
	sprintf_s(m_reportMsg, "StartNetWork");
	sendLogToWindow();


	//add code by chenhaidong
	m_wlInitNewCallEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_endRecordEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_masterAddress = dwMasterIp;
	m_masterPort = wMasterPort;
	m_dwMyRadioID = dwSelfRadioId;
	//m_TxSlot = dwSelfSlot;
	//m_dwMyRadioGroup = dwSelfGroup;
	//InitControlBuffer(dwSelfPeerId);
	CONFIG_LOCAL_PEER_ID = dwSelfPeerId;
	m_WLStatus = STARTING;
	m_bExit = FALSE;

	memset(&m_masterAddr, 0, sizeof(SOCKADDR_IN));
	m_masterAddr.sin_family = AF_INET;
	m_masterAddr.sin_port = htons(wMasterPort);
	m_masterAddr.sin_addr.s_addr = dwMasterIp;

	m_eventArray[RxEvent] = CreateEvent(NULL, TRUE, FALSE, NULL);		// rx event
	if (NULL == m_eventArray[RxEvent])
	{
		sprintf_s(m_reportMsg, "CreateEventFail:Rx Event");
		sendLogToWindow();

		StopNet();
		return FALSE;
	}
	m_RxOverlapped.hEvent = m_eventArray[RxEvent];

	m_eventArray[TxEvent] = CreateEvent(NULL, TRUE, FALSE, NULL);		// tx event
	if (NULL == m_eventArray[TxEvent])
	{
		sprintf_s(m_reportMsg, "CreateEventFail:Tx Event");
		sendLogToWindow();

		StopNet();
		return FALSE;
	}
	m_TxOverlapped.hEvent = m_eventArray[TxEvent];

	m_eventArray[TickEvent] = CreateEvent(NULL, TRUE, FALSE, NULL);		// Tick Event
	if (NULL == m_eventArray[TickEvent])
	{
		sprintf_s(m_reportMsg, "CreateEventFail:Tick Event");
		sendLogToWindow();

		StopNet();
		return FALSE;
	}

	WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	m_socket = WSASocket(AF_INET,
		SOCK_DGRAM,
		IPPROTO_UDP,
		NULL, 0,
		WSA_FLAG_OVERLAPPED);

	if (SOCKET_ERROR == m_socket)
	{
		sprintf_s(m_reportMsg, "WSASocket Failed, Reason:%d", GetLastError());
		sendLogToWindow();

		StopNet();
		return FALSE;
	}

	DWORD BufferSize = 1024 * 100;
	setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char*)&BufferSize, sizeof(DWORD));
	setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&BufferSize, sizeof(DWORD));

	memset(&m_locaAddr, 0, sizeof(m_locaAddr));
	m_locaAddr.sin_family = AF_INET;
	m_locaAddr.sin_addr.s_addr = dwLocalIp;

	for (int port = 40000; port <= 50000; port++)
	{
		m_locaAddr.sin_port = htons(port);
		int res = bind(m_socket, (struct sockaddr*)&m_locaAddr, sizeof(SOCKADDR_IN));
		if (SOCKET_ERROR == res)
		{
			if (port == 50000)
			{
				StopNet();
				sprintf_s(m_reportMsg, "bind error:%d", GetLastError());
				sendLogToWindow();

				return FALSE;
			}
			else
			{
				continue;
			}

		}
		else{
			m_hWorkThread = CreateThread(NULL, 0, NetThreadProc, this, 0, NULL);
			return TRUE;
		}
	}

	return FALSE;
}

void CWLNet::StopNet()
{

	sprintf_s(m_reportMsg, "↓start stop net↓");
	sendLogToWindow();

	if (m_bExit)
	{
		return;
	}
	m_bExit = TRUE;

	WSASetEvent(m_eventArray[TickEvent]);

	for (int i = 0; i < sizeof(m_eventArray) / sizeof(WSAEVENT); i++)
	{
		if (m_eventArray[i] != NULL)
		{
			CloseHandle(m_eventArray[i]);
		}
	}

	sprintf_s(m_reportMsg, "close socket");
	sendLogToWindow();

	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
	}

	sprintf_s(m_reportMsg, "close thread");
	sendLogToWindow();

	if (m_hWorkThread != INVALID_HANDLE_VALUE)
	{
		TerminateThread(m_hWorkThread, 2);
	}

	sprintf_s(m_reportMsg, "clean net");
	sendLogToWindow();

	WSACleanup();

	sprintf_s(m_reportMsg, "↑stop net success↑");
	sendLogToWindow();
}

int CWLNet::IssueReadRequest()
{
	int rc;
	m_dwByteRecevied = 0;
	m_dwRxFlags = 0;
	m_sizeAddress = sizeof (m_remoteAddr);
	rc = WSARecvFrom(m_socket,
		&m_CurrentRecvBuffer,
		1,
		&m_dwByteRecevied,
		&m_dwRxFlags,
		(SOCKADDR *)& m_remoteAddr,
		&m_sizeAddress,
		&m_RxOverlapped,
		NULL);
	if (0 != rc){//call did not complete immediately
		rc = WSAGetLastError();//WSA_IO_PENDING is OK.
	}
	return rc;
}

int CWLNet::SendToLE(const SOCKADDR_IN* pAddr)
{
	int rc;

	m_dwByteSent = 0;
	rc = WSASendTo(m_socket,
		&m_SendControlBuffer, 1,
		&m_dwByteSent, 0,
		(const struct sockaddr *)pAddr,
		sizeof(SOCKADDR_IN), &m_TxOverlapped,
		NULL);
	if (0 != rc){
		rc = WSAGetLastError();
	}
	return rc;
}

void CWLNet::Net_RegisterLE(DWORD eventIndex)
{
	WCHAR  str[8] = { 0 };
	swprintf_s(str, __TEXT("%d"), 0);
	WritePrivateProfileString(SYS_SECTION, CONNECT_RESULT, str, m_strSettingFilePath);
	WritePrivateProfileString(SYS_SECTION, PEER_COUNT, str, m_strSettingFilePath);
	WritePrivateProfileString(SYS_SECTION, LICENSE_RESULT, str, m_strSettingFilePath);

	sprintf_s(m_reportMsg, "OFFline");
	sendLogToWindow();

	int    rc;
	switch (eventIndex)
	{
	case FIRSTPASS:
	case TIMEOUT:		//Timeout. Transmission never completed.
		WSAResetEvent(m_eventArray[TxEvent]);
		//LogEvent(SendMasterRegistration, _T("Register To Master"));

		sprintf_s(m_reportMsg, "Register To Master");
		sendLogToWindow();

		//Build_LE_MASTER_PEER_REGISTRATION_REQUEST();
		//m_ControlProto[0] = LE_MASTER_PEER_REGISTRATION_REQUEST;
		if (CONFIG_RECORD_TYPE == LCP)
		{
			//m_SendControlBuffer.len = LE_MASTER_PEER_REGISTRATION_REQUEST_LCP_L;
			T_LE_PROTOCOL_90_LCP networkData = { 0 };
			networkData.currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
			networkData.leadingChannelID = LEADING_CHANNEL_ID;
			networkData.oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
			networkData.Opcode = LE_MASTER_PEER_REGISTRATION_REQUEST;
			networkData.peerID = CONFIG_LOCAL_PEER_ID;
			networkData.peerMode = LCP_MODE;
			networkData.peerServices = LCP_SERVICES;
			m_SendControlBuffer.buf = m_ControlProto;
			m_SendControlBuffer.len = Build_LE_MASTER_PEER_REGISTRATION_REQUEST(m_SendControlBuffer.buf, &networkData);
		}
		else
		{
			T_LE_PROTOCOL_90 networkData = { 0 };
			networkData.Opcode = LE_MASTER_PEER_REGISTRATION_REQUEST;
			networkData.peerID = CONFIG_LOCAL_PEER_ID;
			if (IPSC == CONFIG_RECORD_TYPE)
			{
				networkData.currentLinkProtocolVersion = IPSC_CURRENTLPVERSION;
				networkData.oldestLinkProtocolVersion = IPSC_OLDESTPVERSION;
				networkData.peerMode = IPSC_MODE;
				networkData.peerServices = IPSC_SERVICES;
			}
			else
			{
				networkData.currentLinkProtocolVersion = CPC_CURRENTLPVERSION;
				networkData.oldestLinkProtocolVersion = CPC_OLDESTPVERSION;
				networkData.peerMode = CPC_MODE;
				networkData.peerServices = CPC_SERVICES;
			}
			m_SendControlBuffer.buf = m_ControlProto;
			m_SendControlBuffer.len = Build_LE_MASTER_PEER_REGISTRATION_REQUEST(m_SendControlBuffer.buf, &networkData);
		}
		rc = SendToLE(&m_masterAddr);

		m_dwMasterRegisterTimer = GetTickCount();
		switch (rc)
		{
		case 0:               //Tx completed successfully immediately.
		case WSA_IO_PENDING:  //Completion is pending
			m_WLStatus = WAITFOR_LE_MASTER_PEER_REGISTRATION_TX;
			//strSysLog = _T("发送0x90(LE_MASTER_PEER_REGISTRATION_REQUEST)");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
			break;
		default:              //stay in this state and eventually retry.
			break;
		}
		break;

	case RxIndex: // ignore all incoming data
		WSAResetEvent(m_eventArray[RxEvent]);
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;

	default:
		break;
	}
}

void CWLNet::Net_DeRegisterLE()
{
	int rc;

	m_ControlProto[0] = LE_DEREGISTRATION_REQUEST;
	m_SendControlBuffer.len = LE_DEREGISTRATION_REQUEST_L;
	rc = SendToLE(&m_masterAddr);
	switch (rc)
	{
	case 0: // Tx completely
		m_WLStatus = BAILOUT;
		break;
	case WSA_IO_PENDING:
		m_WLStatus = WAITINGFOR_LE_DEREGISTRATION_TRANSMISSION;
		break;
	default:
		m_WLStatus = BAILOUT;
		break;
	}
}

void CWLNet::Net_WAITINGFOR_LE_DEREGISTRATION_TRANSMISSION(DWORD eventIndex)
{
	switch (eventIndex)
	{
	case RxIndex:
		// ignore all received data
		WSAResetEvent(m_eventArray[RxEvent]);
		break;

	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		m_WLStatus = BAILOUT;
		break;

	case TIMEOUT:
		m_WLStatus = BAILOUT;
		break;

	case TickIndex:
		m_WLStatus = BAILOUT;
		break;
	default:
		m_WLStatus = BAILOUT;
		break;
	}
}

void CWLNet::Net_WAITFOR_LE_MASTER_PEER_REGISTRATION_TX(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_TxOverlapped,
			&m_dwByteSent,
			TRUE,
			&m_dwTxFlags);

		WSAResetEvent(m_eventArray[TxEvent]);
		if (TRUE != rc){
			//LE_MASTER_PEER_REGISTRATION_REQUEST Transmission failed.
			m_WLStatus = STARTING; //Go back to STARTING.
			//strSysLog = _T("0x90(LE_MASTER_PEER_REGISTRATION_REQUEST)发送失败，重新发送！)");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
			break;                   //Will retry at TIMEOUT.
		}

		m_WLStatus = WAITFOR_LE_MASTER_PEER_REGISTRATION_RESPONSE;
		//strSysLog = _T("等待0x91(LE_MASTER_PEER_REGISTRATION_RESPONSE)");
		//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		break;

	case RxIndex:
		WSAResetEvent(m_eventArray[TxEvent]); //Clear any simultaineous Tx Event. 
		// transfer to Net_WAITFOR_LE_MASTER_PEER_REGISTRATION_RESPONSE
		m_WLStatus = WAITFOR_LE_MASTER_PEER_REGISTRATION_RESPONSE;

		m_dwMasterRegisterTimer = GetTickCount();
		break;

	case TIMEOUT:
		if ((GetTickCount() - m_dwMasterRegisterTimer) > 100000UL)  // 100 seconds
		{
			m_WLStatus = STARTING;
			//strSysLog = _T("注册超时，重新注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;
	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_LE_MASTER_PEER_REGISTRATION_RESPONSE(DWORD eveintIndex)
{
	int    rc;
	int    ExpectedPeers = 0xffffff;
	//CString  strSysLog;
	switch (eveintIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);
		WSAResetEvent(m_eventArray[RxEvent]);
		if (FALSE == rc)
		{
			//reception failed
			rc = WSAGetLastError();
		}
		else
		{
			char Opcode = m_CurrentRecvBuffer.buf[0];
			//received something
			if (LE_MASTER_PEER_REGISTRATION_RESPONSE == Opcode)
			{
				T_LE_PROTOCOL_91_LCP networkDataLcp = { 0 };
				T_LE_PROTOCOL_91 networkData = { 0 };
				/*解包*/
				if (LCP == CONFIG_RECORD_TYPE)
				{
					networkDataLcp.length = (u_short)m_dwByteRecevied;
					Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(m_CurrentRecvBuffer.buf, networkDataLcp);
				}
				else
				{
					networkData.length = (u_short)m_dwByteRecevied;
					Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(m_CurrentRecvBuffer.buf, networkData);
				}
				/*获取关键数据*/
				if (LCP == CONFIG_RECORD_TYPE)
				{
					m_ulMasterPeerID = networkDataLcp.peerID;
					m_ucMasterMode = networkDataLcp.peerMode;
					m_uMasterServices = networkDataLcp.peerServices;
					ExpectedPeers = networkDataLcp.numPeers;
				}
				else if ((CPC == CONFIG_RECORD_TYPE || IPSC == CONFIG_RECORD_TYPE))
				{
					m_ulMasterPeerID = networkData.peerID;
					m_ucMasterMode = networkData.peerMode;
					m_uMasterServices = networkData.peerServices;
					ExpectedPeers = networkData.numPeers;
				}

				/*判断是否需要请求map*/
				if (0 == ExpectedPeers)
				{
					/************************************************************************/
					/*改动理由:devspec_link_establishment_0103.pdf line 919
					/************************************************************************/
					m_WLStatus = ALIVE; //Advance state.
					m_dwMasterKeepAliveTime = GetTickCount() - 16000;
					Net_MaintainKeepAlive();

					/*对主中继进行wireline注册*/
					clearPeers();
					m_pPeers.clear();
					CIPSCPeer *peer = new CIPSCPeer(this, m_masterAddress, m_masterPort);
					peer->setRemote3rdParty(false);
					peer->SetPeerID(m_ulMasterPeerID);
					peer->setLogPtr(m_report);
					m_pPeers.push_back(peer);
					//print info of peer
					for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
					{
						(*i)->printInfo();
					}
					//cycle WL register
					for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
					{
						(*i)->HandlePacket(WL_REGISTRATION_REQUEST_LOCAL, NULL, m_masterAddress, m_masterPort, FALSE);
					}

				}
				//Need to get map.
				else
				{
					if (LCP == CONFIG_RECORD_TYPE)
					{
						T_LE_PROTOCOL_92_LCP networkData = { 0 };
						networkData.acceptedLinkProtocolVersion = LCP_CURRENTLPVERSION;
						networkData.mapType = MAP_TYPE;
						networkData.oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
						networkData.Opcode = LE_NOTIFICATION_MAP_REQUEST;
						networkData.peerID = CONFIG_LOCAL_PEER_ID;
						m_SendControlBuffer.buf = m_ControlProto;
						m_SendControlBuffer.len = Build_LE_NOTIFICATION_MAP_REQUEST(m_SendControlBuffer.buf, &networkData);
					}
					else
					{
						T_LE_PROTOCOL_92 networkData = { 0 };
						networkData.Opcode = LE_NOTIFICATION_MAP_REQUEST;
						networkData.peerID = CONFIG_LOCAL_PEER_ID;
						m_SendControlBuffer.buf = m_ControlProto;
						m_SendControlBuffer.len = Build_LE_NOTIFICATION_MAP_REQUEST(m_SendControlBuffer.buf, &networkData);
					}
					m_dwMasterMapBroadcastTimer = GetTickCount();
					rc = SendToLE(&m_masterAddr);
					switch (rc)
					{
					case 0:
					case WSA_IO_PENDING:
						m_WLStatus = WAITFOR_MAP_REQUEST_TX;
						break;
					default:
						break;
					}
				}
			}
		}
		/*继续收取*/
		rc = IssueReadRequest();
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;

	case TIMEOUT:
		if ((GetTickCount() - m_dwMasterRegisterTimer) > 100000UL)  // 100 seconds
		{
			m_WLStatus = STARTING;
			//strSysLog = _T("注册超时，重新注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;

	default:
		break;
	}
}

void CWLNet::Net_MaintainKeepAlive()
{
	//CString strSysLog;
	long dif = GetTickCount() - m_dwMasterKeepAliveTime;
	if (dif > CONFIG_MASTER_HEART_TIME)
	{
		if (m_dwRecvMasterKeepAliveTime == m_dwMasterKeepAliveTime)
		{
			//sprintf_s(m_reportMsg, "MASTER心跳包正常");
			//sendLogToWindow();
		}
		else
		{
			sprintf_s(m_reportMsg, "MASTER心跳包异常");
			sendLogToWindow();
		}
		m_dwRecvMasterKeepAliveTime = 0;
		m_dwMasterKeepAliveTime = GetTickCount();

		/*主动发送LE_MASTER_KEEP_ALIVE_REQUEST*/
		if (LCP == CONFIG_RECORD_TYPE)
		{
			T_LE_PROTOCOL_96_LCP networkData = { 0 };
			networkData.currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
			networkData.leadingChannelID = LEADING_CHANNEL_ID;
			networkData.oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
			networkData.Opcode = LE_MASTER_KEEP_ALIVE_REQUEST;
			networkData.peerID = CONFIG_LOCAL_PEER_ID;
			networkData.peerMode = LCP_MODE;
			networkData.peerServices = LCP_SERVICES;
			m_SendControlBuffer.buf = m_ControlProto;
			m_SendControlBuffer.len = Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(m_SendControlBuffer.buf, &networkData);
		}
		else
		{
			T_LE_PROTOCOL_96 networkData = { 0 };
			networkData.Opcode = LE_MASTER_KEEP_ALIVE_REQUEST;
			networkData.peerID = CONFIG_LOCAL_PEER_ID;
			if (CPC == CONFIG_RECORD_TYPE)
			{
				networkData.currentLinkProtocolVersion = CPC_CURRENTLPVERSION;
				networkData.oldestLinkProtocolVersion = CPC_OLDESTPVERSION;
				networkData.peerMode = CPC_MODE;
				networkData.peerServices = CPC_SERVICES;
			}
			else
			{
				networkData.currentLinkProtocolVersion = IPSC_CURRENTLPVERSION;
				networkData.oldestLinkProtocolVersion = IPSC_OLDESTPVERSION;
				networkData.peerMode = IPSC_MODE;
				networkData.peerServices = IPSC_SERVICES;
			}
			m_SendControlBuffer.buf = m_ControlProto;
			m_SendControlBuffer.len = Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(m_SendControlBuffer.buf, &networkData);
		}
		//InitControlBuffer(m_dwMyPeerID);
		//m_ControlProto[0] = LE_MASTER_KEEP_ALIVE_REQUEST;
		//if (LCP == m_dwRecType)
		//{
		//	m_SendControlBuffer.len = LE_MASTER_KEEP_ALIVE_REQUEST_LCP_L;
		//}
		//else
		//{
		//	m_SendControlBuffer.len = LE_MASTER_KEEP_ALIVE_REQUEST_L;

		//}
		SendToLE(&m_masterAddr);

	}
}

void CWLNet::Net_MaintainAlive(DWORD eventIndex)
{
	/*进行调度组检查*/
	checkDefaultGroup();
	int    rc;
	/*进行语音记录检查*/
	Process_WL_BURST_CALL(WL_BURST_CHECK_TIMEOUT, NULL);

	switch (eventIndex)
	{
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);

		WSAResetEvent(m_eventArray[RxEvent]);

		if (FALSE == rc)
		{
			rc = WSAGetLastError(); //For debugging if nothing else.
			rc = IssueReadRequest();
		}
		else
		{
			//unsigned long tempPeerId = *((DWORD*)(&m_CurrentRecvBuffer.buf[1]));
			//tempPeerId = ntohl(tempPeerId);
			switch (*(m_CurrentRecvBuffer.buf))
			{
			case IPSC_GRP_VOICE_CALL:
			case IPSC_PVT_VOICE_CALL:
			{
										//ProcessCall(*(m_CurrentRecvBuffer.buf));
										sprintf_s(m_reportMsg, "now use wireline don't use p2p");
										sendLogToWindow();
			}
				break;
			case LE_MASTER_KEEP_ALIVE_RESPONSE:
				m_dwMasterKeepAliveTime = GetTickCount();
				m_dwRecvMasterKeepAliveTime = m_dwMasterKeepAliveTime;
				break;
			case LE_PEER_REGISTRATION_REQUEST:
			{
												 T_LE_PROTOCOL_94 networkData = { 0 };
												 networkData.length = (u_short)m_dwByteRecevied;
												 Unpack_LE_PEER_REGISTRATION_REQUEST(m_CurrentRecvBuffer.buf, networkData);
												 CIPSCPeer* peer = GetPeer(networkData.peerID);
												 if (peer)
												 {
													 peer->HandlePacket(LE_PEER_REGISTRATION_REQUEST_REMOTE, &networkData, m_masterAddress, m_masterPort);
												 }
												 break;
			}
			case LE_PEER_KEEP_ALIVE_REQUEST:
			{
											   T_LE_PROTOCOL_98 networkData = { 0 };
											   T_LE_PROTOCOL_98_LCP networkDataLcp = { 0 };
											   CIPSCPeer* peer = NULL;
											   if (LCP == CONFIG_RECORD_TYPE)
											   {
												   networkDataLcp.length = (u_short)m_dwByteRecevied;
												   Unpack_LE_PEER_KEEP_ALIVE_REQUEST(m_CurrentRecvBuffer.buf, networkDataLcp);
												   peer = GetPeer(networkDataLcp.peerID);
											   }
											   else
											   {
												   networkData.length = (u_short)m_dwByteRecevied;
												   Unpack_LE_PEER_KEEP_ALIVE_REQUEST(m_CurrentRecvBuffer.buf, networkData);
												   peer = GetPeer(networkData.peerID);
											   }
											   if (peer)
											   {
												   peer->HandlePacket(LE_PEER_KEEP_ALIVE_REQUEST_REMOTE, &networkData, m_masterAddress, m_masterPort);
											   }
											   break;
			}
				break;
			case LE_PEER_KEEP_ALIVE_RESPONSE:
			{
												T_LE_PROTOCOL_99 networkData = { 0 };
												T_LE_PROTOCOL_99_LCP networkDataLcp = { 0 };
												CIPSCPeer* peer = NULL;
												if (LCP == CONFIG_RECORD_TYPE)
												{
													networkDataLcp.length = (u_short)m_dwByteRecevied;
													Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(m_CurrentRecvBuffer.buf, networkDataLcp);
													peer = GetPeer(networkDataLcp.peerID);
												}
												else
												{
													networkData.length = (u_short)m_dwByteRecevied;
													Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(m_CurrentRecvBuffer.buf, networkData);
													peer = GetPeer(networkData.peerID);
												}
												if (peer)
												{
													peer->HandlePacket(LE_PEER_KEEP_ALIVE_RESPONSE_REMOTE, &networkData, m_masterAddress, m_masterPort);
												}
												break;
			}
			case LE_PEER_REGISTRATION_RESPONSE:
			{
												  T_LE_PROTOCOL_95 networkData = { 0 };
												  networkData.length = (u_short)m_dwByteRecevied;
												  Unpack_LE_PEER_REGISTRATION_RESPONSE(m_CurrentRecvBuffer.buf, networkData);
												  CIPSCPeer* peer = GetPeer(networkData.peerID);
												  if (peer)
												  {
													  peer->HandlePacket(LE_PEER_REGISTRATION_RESPONSE_REMOTE, &networkData, m_masterAddress, m_masterPort);
												  }
												  break;
			}
				/*收到wireline相关信息*/
			case LE_CALL_CONTROL_INTERFACE:
			{
											  T_WL_PROTOCOL_01 networkData01 = { 0 };
											  T_WL_PROTOCOL_02 networkData02 = { 0 };
											  T_WL_PROTOCOL_03 networkData03 = { 0 };
											  T_WL_PROTOCOL_11 networkData11 = { 0 };
											  T_WL_PROTOCOL_12 networkData12 = { 0 };
											  T_WL_PROTOCOL_13 networkData13 = { 0 };
											  T_WL_PROTOCOL_16 networkData16 = { 0 };
											  //T_WL_PROTOCOL_17 networkData01 = { 0 };
											  T_WL_PROTOCOL_18 networkData18 = { 0 };
											  T_WL_PROTOCOL_19 networkData19 = { 0 };
											  T_WL_PROTOCOL_20 networkData20 = { 0 };
											  T_WL_PROTOCOL_21 networkData21 = { 0 };
											  //T_WL_PROTOCOL_22 networkData01 = { 0 };
											  unsigned char Opcode = m_CurrentRecvBuffer.buf[0];
											  unsigned long peerID = ntohl(*((DWORD*)(&m_CurrentRecvBuffer.buf[1])));

											  /*关键值赋值*/
											  networkData01.Opcode = Opcode;
											  networkData01.peerID = peerID;
											  networkData01.length = (u_short)m_dwByteRecevied;

											  networkData02.Opcode = Opcode;
											  networkData02.peerID = peerID;
											  networkData02.length = (u_short)m_dwByteRecevied;

											  networkData03.Opcode = Opcode;
											  networkData03.peerID = peerID;
											  networkData03.length = (u_short)m_dwByteRecevied;

											  networkData11.Opcode = Opcode;
											  networkData11.peerID = peerID;
											  networkData11.length = (u_short)m_dwByteRecevied;

											  networkData12.Opcode = Opcode;
											  networkData12.peerID = peerID;
											  networkData12.length = (u_short)m_dwByteRecevied;

											  networkData13.Opcode = Opcode;
											  networkData13.peerID = peerID;
											  networkData13.length = (u_short)m_dwByteRecevied;

											  networkData16.Opcode = Opcode;
											  networkData16.peerID = peerID;
											  networkData16.length = (u_short)m_dwByteRecevied;

											  networkData18.Opcode = Opcode;
											  networkData18.peerID = peerID;
											  networkData18.length = (u_short)m_dwByteRecevied;

											  networkData19.Opcode = Opcode;
											  networkData19.peerID = peerID;
											  networkData19.length = (u_short)m_dwByteRecevied;

											  networkData20.Opcode = Opcode;
											  networkData20.peerID = peerID;
											  networkData20.length = (u_short)m_dwByteRecevied;

											  networkData21.Opcode = Opcode;
											  networkData21.peerID = peerID;
											  networkData21.length = (u_short)m_dwByteRecevied;
											  unsigned char wirelineOpcode = m_CurrentRecvBuffer.buf[5];
											  CIPSCPeer* peer = GetPeer(peerID);
											  /*解包*/
											  switch (wirelineOpcode)
											  {
											  case WL_REGISTRATION_REQUEST:
											  {
																			  /*理论上来说不存在向第三方应用进行WL注册*/
																			  sprintf_s(m_reportMsg, "WL_REGISTRATION_REQUEST no handle");
																			  sendLogToWindow();
											  }
												  break;
											  case WL_REGISTRATION_GENERAL_OPS:
											  {
																				  Unpack_WL_REGISTRATION_GENERAL_OPS(m_CurrentRecvBuffer.buf, networkData03);
																				  if (peer)
																				  {
																					  peer->HandlePacket(WL_REGISTRATION_GENERAL_OPS_REMOTE, &networkData03, m_masterAddress, m_masterPort);
																				  }
											  }
												  break;
											  case WL_REGISTRATION_STATUS:
											  {
																			 Unpack_WL_REGISTRATION_STATUS(m_CurrentRecvBuffer.buf, networkData02);
																			 if (peer)
																			 {
																				 peer->HandlePacket(WL_REGISTRATION_STATUS_REMOTE, &networkData02, m_masterAddress, m_masterPort);
																			 }
											  }
												  break;
											  case WL_CHNL_STATUS:
											  {
																	 Unpack_WL_CHNL_STATUS(m_CurrentRecvBuffer.buf, networkData11);
																	 if (peer)
																	 {
																		 peer->HandlePacket(WL_CHNL_STATUS_REMOTE, &networkData11, m_masterAddress, m_masterPort);
																	 }
											  }
												  break;
											  case WL_CHNL_STATUS_QUERY:
											  {
																		   Unpack_WL_CHNL_STATUS_QUERY(m_CurrentRecvBuffer.buf, networkData12);
																		   if (peer)
																		   {
																			   peer->HandlePacket(WL_CHNL_STATUS_QUERY_REMOTE, &networkData12, m_masterAddress, m_masterPort);
																		   }
											  }
												  break;
											  case WL_VC_CHNL_CTRL_REQUEST:
											  {
																			  /*理论上来说不存在向第三方应用进行通话申请*/
																			  sprintf_s(m_reportMsg, "WL_VC_CHNL_CTRL_REQUEST no handle");
																			  sendLogToWindow();
											  }
												  break;
											  case  WL_VC_CHNL_CTRL_STATUS:
											  {
																			  Unpack_WL_VC_CHNL_CTRL_STATUS(m_CurrentRecvBuffer.buf, networkData16);
																			  if (peer)
																			  {
																				  peer->HandlePacket(WL_VC_CHNL_CTRL_STATUS_REMOTE, &networkData16, m_masterAddress, m_masterPort);
																			  }
											  }
												  break;
											  case WL_VC_VOICE_START:
											  {
																		Unpack_WL_VC_VOICE_START(m_CurrentRecvBuffer.buf, networkData18);
																		//if (peer)
																		//{
																		//	peer->HandlePacket(wirelineOpcode, &networkData18, m_masterAddress, m_masterPort);
																		//}
																		Process_WL_BURST_CALL(WL_VC_VOICE_START, &networkData18);
											  }
												  break;
											  case WL_VC_VOICE_END_BURST:
											  {
																			Unpack_WL_VC_VOICE_END_BURST(m_CurrentRecvBuffer.buf, networkData19);
																			//if (peer)
																			//{
																			//	peer->HandlePacket(wirelineOpcode, &networkData19, m_masterAddress, m_masterPort);
																			//}
																			Process_WL_BURST_CALL(WL_VC_VOICE_END_BURST, &networkData19);
											  }
												  break;
											  case WL_VC_CALL_SESSION_STATUS:
											  {
																				Unpack_WL_VC_CALL_SESSION_STATUS(m_CurrentRecvBuffer.buf, networkData20);
																				//if (peer)
																				//{
																				//	peer->HandlePacket(WL_VC_CALL_SESSION_STATUS_REMOTE, &networkData20, m_masterAddress, m_masterPort);
																				//}
																				Process_WL_BURST_CALL(WL_VC_CALL_SESSION_STATUS, &networkData20);
											  }
												  break;
											  case WL_VC_VOICE_BURST:
											  {
																		Unpack_WL_VC_VOICE_BURST(m_CurrentRecvBuffer.buf, networkData21);
																		//if (peer)
																		//{
																		//	peer->HandlePacket(wirelineOpcode, &networkData21, m_masterAddress, m_masterPort);
																		//}
																		Process_WL_BURST_CALL(WL_VC_VOICE_BURST, &networkData21);
											  }
												  break;
											  default:
												  break;
											  }
			}
				break;
			case LE_NOTIFICATION_MAP_BROADCAST:
			{
												  m_RecvMap = m_dwByteRecevied;
												  /*解包*/
												  T_LE_PROTOCOL_93 networkData = { 0 };
												  T_LE_PROTOCOL_93_LCP networkDataLcp = { 0 };
												  if (LCP == CONFIG_RECORD_TYPE)
												  {
													  networkDataLcp.length = (u_short)m_RecvMap;
													  Unpack_LE_NOTIFICATION_MAP_BROADCAST(m_CurrentRecvBuffer.buf, networkDataLcp);
												  }
												  else
												  {
													  networkData.length = (u_short)m_RecvMap;
													  Unpack_LE_NOTIFICATION_MAP_BROADCAST(m_CurrentRecvBuffer.buf, networkData);
												  }

												  ParseMapBroadcast(&networkData, &networkDataLcp);
			}
				break;
			case LE_MASTER_PEER_REGISTRATION_RESPONSE:
			default:
				break;
			}
			rc = IssueReadRequest();
		} // end ? if (FALSE == rc)
		Net_MaintainKeepAlive();
		//m_RecorderServer.SendSignal();
		break;
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;

	case TickIndex:
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;

	case TIMEOUT:
		WSAResetEvent(m_eventArray[TxEvent]);
		//m_RecorderServer.SendSignal();
		Net_MaintainKeepAlive();
		//PeerTimeout();
		//ProcessLCPCall(0, TRUE);
		break;
	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_MAP_REQUEST_TX(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_TxOverlapped,
			&m_dwByteSent,
			TRUE,
			&m_dwTxFlags);
		WSAResetEvent(m_eventArray[TxEvent]);
		if (TRUE != rc)
		{
			//LE_NOTIFICATION_MAP_REQUEST Transmission failed.
			m_WLStatus = STARTING;
			//Go back to STARTING.
			//strSysLog = _T("0x92(LE_NOTIFICATION_MAP_REQUEST)发送失败，重新开始注册");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
			break;
		}
		m_WLStatus = WAITFOR_LE_NOTIFICATION_MAP_BROADCAST;
		//strSysLog = _T("等待0x93(LE_NOTIFICATION_MAP_BROADCAST)");
		//m_dwMasterMapBroadcastTimer = GetTickCount();
		break;

	case RxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		// transfer to Net_WAITFOR_LE_NOTIFICATION_MAP_BROADCAST
		m_WLStatus = WAITFOR_LE_NOTIFICATION_MAP_BROADCAST;
		//strSysLog = _T("等待0x93(LE_NOTIFICATION_MAP_BROADCAST)");
		//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		break;
	case TIMEOUT:
		if ((GetTickCount() - m_dwMasterMapBroadcastTimer) > 120000UL)
		{
			m_WLStatus = STARTING;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;
	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_LE_NOTIFICATION_MAP_BROADCAST(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_RecvMap,
			TRUE,
			&m_dwRxFlags);
		WSAResetEvent(m_eventArray[RxEvent]);
		if (FALSE == rc)
		{
			//Reception failed
			rc = WSAGetLastError(); //For debugging if nothing else.
			//Restart listener using same buffer.
			rc = IssueReadRequest();
		}
		else
		{
			char Opcode = m_CurrentRecvBuffer.buf[0];
			if (LE_NOTIFICATION_MAP_BROADCAST == Opcode)
			{
				rc = IssueReadRequest();
				m_dwMasterKeepAliveTime = GetTickCount();
				m_dwRecvMasterKeepAliveTime = m_dwMasterKeepAliveTime;

				/*授权相关*/

				/*程序开始正常工作*/
				m_WLStatus = ALIVE;
				WCHAR  str[8];
				swprintf_s(str, __TEXT("%d"), 1);
				WritePrivateProfileString(SYS_SECTION, CONNECT_RESULT, str, m_strSettingFilePath);

				/*解包*/
				T_LE_PROTOCOL_93 networkData = { 0 };
				T_LE_PROTOCOL_93_LCP networkDataLcp = { 0 };
				if (LCP == CONFIG_RECORD_TYPE)
				{
					networkDataLcp.length = (u_short)m_RecvMap;
					Unpack_LE_NOTIFICATION_MAP_BROADCAST(m_CurrentRecvBuffer.buf, networkDataLcp);
				}
				else
				{
					networkData.length = (u_short)m_RecvMap;
					Unpack_LE_NOTIFICATION_MAP_BROADCAST(m_CurrentRecvBuffer.buf, networkData);
				}

				ParseMapBroadcast(&networkData, &networkDataLcp);

			}
			else
			{
				rc = IssueReadRequest();
			}
		}
		break;

	case TickIndex: // quit
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			// send deregister request to master node
			Net_DeRegisterLE();
		}
		break;

	case  TIMEOUT:
		if ((GetTickCount() - m_dwMasterMapBroadcastTimer) > 120000UL)
		{
			m_WLStatus = STARTING;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;
	default:
		break;
	}
}

//void CWLNet::ParseMapBroadcast(T_LE_PROTOCOL_93* p, T_LE_PROTOCOL_93_LCP* pLcp)
//{
//	u_long	remotePeerID;
//	u_long  remotePeerAddr;
//	u_short	remotePeerPort;
//	u_short PeerMode;
//	char   ChannelID;
//
//	if (LCP == m_dwRecType)
//	{
//		if (0x01 == m_CurrentRecvBuffer.buf[5])
//		{
//			m_pPeers.clear();
//			m_PeerCount = (m_RecvMap - 12) / LE_NOTIFICATION_MAP_BROADCAST_ENTRY;
//			for (int i = 0; (i < m_PeerCount); i++)
//			{
//
//				//remotePeerID
//				remotePeerID = m_CurrentRecvBuffer.buf[i * 13 + 9] & 0xFF;
//				remotePeerID = (remotePeerID << 8) + (m_CurrentRecvBuffer.buf[i * 13 + 10] & 0xFF);
//				remotePeerID = (remotePeerID << 8) + (m_CurrentRecvBuffer.buf[i * 13 + 11] & 0xFF);
//
//				//remotePeerAddr
//				remotePeerAddr = m_CurrentRecvBuffer.buf[i * 13 + 12] & 0xFF;
//				remotePeerAddr = (remotePeerAddr << 8) + (m_CurrentRecvBuffer.buf[i * 13 + 13] & 0xFF);
//				remotePeerAddr = (remotePeerAddr << 8) + (m_CurrentRecvBuffer.buf[i * 13 + 14] & 0xFF);
//				remotePeerAddr = (remotePeerAddr << 8) + (m_CurrentRecvBuffer.buf[i * 13 + 15] & 0xFF);
//
//				//remotePeerPort
//				remotePeerPort = m_CurrentRecvBuffer.buf[i * 13 + 16] & 0xFF;
//				remotePeerPort = (remotePeerPort << 8) + (m_CurrentRecvBuffer.buf[i * 13 + 17] & 0xFF);
//
//				//PeerMode
//				PeerMode = m_CurrentRecvBuffer.buf[i * 13 + 18] & 0xFF;
//				PeerMode = (remotePeerPort << 8) + (m_CurrentRecvBuffer.buf[i * 13 + 19] & 0xFF);
//
//				//ChannelID
//				ChannelID = m_CurrentRecvBuffer.buf[i * 13 + 20] & 0xFF;
//				swprintf_s(m_addr, __TEXT("%d.%d.%d.%d"), (remotePeerAddr >> 24) & 0xFF, (remotePeerAddr >> 16) & 0xFF, (remotePeerAddr >> 8) & 0xFF, remotePeerAddr & 0xFF);
//				swprintf_s(m_port, __TEXT("%d"), remotePeerPort);
//				swprintf_s(m_peerID, __TEXT("%d"), remotePeerID);
//				swprintf_s(m_NumberStr, __TEXT("%03d"), i + 1);
//
//				WriteMapFile();
//				if (!FindLocalIP(m_addr))
//				{
//					CIPSCPeer *peer = new CIPSCPeer(this, m_addr, m_port);
//
//					peer->SetPeerID(remotePeerID);
//					peer->setLogPtr(m_report);
//					m_pPeers.push_back(peer);
//				}
//			}
//
//			//get master ip from setting,then add to peer
//			CIPSCPeer *peer = new CIPSCPeer(this, m_masterAddress, m_masterPort);
//			peer->setRemote3rdParty(false);
//			peer->SetPeerID(m_ulMasterPeerID);
//			peer->setLogPtr(m_report);
//			m_pPeers.push_back(peer);
//
//			sprintf_s(m_reportMsg, "MAP peers:");
//			sendLogToWindow();
//
//			//print info of peer
//			for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
//			{
//				(*i)->printInfo();
//			}
//
//			//cycle WL register
//			for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
//			{
//				(*i)->HandlePacket(WL_REGISTRATION_REQUEST, NULL, 0, m_masterAddress, m_masterPort, FALSE);
//			}
//		}
//
//	}
//	else
//	{
//		m_pPeers.clear();
//		m_PeerCount = (m_RecvMap - 7) / LE_NOTIFICATION_MAP_BROADCAST_ENTRY;
//		for (int i = 0; (i < MAXPEERSSUPPORTED) && (i < m_PeerCount); i++)
//		{
//			//remotePeerID
//			remotePeerID = m_CurrentRecvBuffer.buf[i * 11 + 7] & 0xFF;
//			remotePeerID = (remotePeerID << 8) + (m_CurrentRecvBuffer.buf[i * 11 + 8] & 0xFF);
//			remotePeerID = (remotePeerID << 8) + (m_CurrentRecvBuffer.buf[i * 11 + 9] & 0xFF);
//			remotePeerID = (remotePeerID << 8) + (m_CurrentRecvBuffer.buf[i * 11 + 10] & 0xFF);
//
//			//remotePeerAddr
//			remotePeerAddr = m_CurrentRecvBuffer.buf[i * 11 + 11] & 0xFF;
//			remotePeerAddr = (remotePeerAddr << 8) + (m_CurrentRecvBuffer.buf[i * 11 + 12] & 0xFF);
//			remotePeerAddr = (remotePeerAddr << 8) + (m_CurrentRecvBuffer.buf[i * 11 + 13] & 0xFF);
//			remotePeerAddr = (remotePeerAddr << 8) + (m_CurrentRecvBuffer.buf[i * 11 + 14] & 0xFF);
//
//			//remotePeerPort
//			remotePeerPort = m_CurrentRecvBuffer.buf[i * 11 + 15] & 0xFF;
//			remotePeerPort = (remotePeerPort << 8) + (m_CurrentRecvBuffer.buf[i * 11 + 16] & 0xFF);
//
//			//PeerMode
//			PeerMode = m_CurrentRecvBuffer.buf[i * 11 + 17] & 0xFF;
//
//			swprintf_s(m_addr, __TEXT("%d.%d.%d.%d"), (remotePeerAddr >> 24) & 0xFF, (remotePeerAddr >> 16) & 0xFF, (remotePeerAddr >> 8) & 0xFF, remotePeerAddr & 0xFF);
//			swprintf_s(m_port, __TEXT("%d"), remotePeerPort);
//			swprintf_s(m_peerID, __TEXT("%d"), remotePeerID);
//
//			swprintf_s(m_NumberStr, __TEXT("%03d"), i + 1);
//
//			WriteMapFile();
//
//			if (!FindLocalIP(m_addr))
//			{
//				CIPSCPeer *peer = new CIPSCPeer(this, m_addr, m_port);
//
//				peer->SetPeerID(remotePeerID);
//				peer->setLogPtr(m_report);
//				m_pPeers.push_back(peer);
//			}
//		}
//
//
//		//get master ip from setting,then add to peer
//		CIPSCPeer *peer = new CIPSCPeer(this, m_masterAddress, m_masterPort);
//		peer->setRemote3rdParty(false);
//		peer->SetPeerID(m_ulMasterPeerID);
//		peer->setLogPtr(m_report);
//		m_pPeers.push_back(peer);
//
//		sprintf_s(m_reportMsg, "MAP peers:");
//		sendLogToWindow();
//
//		//print info of peer
//		for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
//		{
//			(*i)->printInfo();
//		}
//
//		//cycle WL register
//		for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
//		{
//			(*i)->HandlePacket(WL_REGISTRATION_REQUEST, NULL, 0, m_masterAddress, m_masterPort);
//		}
//	}
//	WCHAR str[8] = { 0 };
//	swprintf_s(str, __TEXT("%d"), m_PeerCount);
//	WritePrivateProfileString(SYS_SECTION, PEER_COUNT, str, m_strSettingFilePath);
//}
void CWLNet::ParseMapBroadcast(T_LE_PROTOCOL_93* p, T_LE_PROTOCOL_93_LCP* pLcp)
{
	if (LCP == CONFIG_RECORD_TYPE)
	{
		int mapNums = pLcp->mapNums;
		m_PeerCount = mapNums;
		clearPeers();
		m_pPeers.clear();
		if (mapNums > 0)
		{
			for (int i = 0; i < mapNums; i++)
			{
				swprintf_s(m_addr, __TEXT("%d.%d.%d.%d"), (pLcp->mapPayload.wideMapPeers[i].remoteIPAddr >> 24) & 0xFF, (pLcp->mapPayload.wideMapPeers[i].remoteIPAddr >> 16) & 0xFF, (pLcp->mapPayload.wideMapPeers[i].remoteIPAddr >> 8) & 0xFF, pLcp->mapPayload.wideMapPeers[i].remoteIPAddr & 0xFF);
				swprintf_s(m_port, __TEXT("%d"), pLcp->mapPayload.wideMapPeers[i].remotePort);
				swprintf_s(m_peerID, __TEXT("%d"), pLcp->mapPayload.wideMapPeers[i].remotePeerID);
				swprintf_s(m_NumberStr, __TEXT("%03d"), i + 1);

				WriteMapFile();
				if (!FindLocalIP(m_addr))
				{
					CIPSCPeer *peer = new CIPSCPeer(this, m_addr, m_port);
					peer->SetPeerID(pLcp->mapPayload.wideMapPeers[i].remotePeerID);
					peer->setLogPtr(m_report);
					m_pPeers.push_back(peer);
				}
			}
			//get master ip from setting,then add to peer
			CIPSCPeer *peer = new CIPSCPeer(this, m_masterAddress, m_masterPort);
			peer->setRemote3rdParty(false);
			peer->SetPeerID(m_ulMasterPeerID);
			peer->setLogPtr(m_report);
			m_pPeers.push_back(peer);

			sprintf_s(m_reportMsg, "MAP peers:");
			sendLogToWindow();
			//print info of peer
			for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
			{
				(*i)->printInfo();
			}
			//cycle WL register
			for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
			{
				(*i)->HandlePacket(WL_REGISTRATION_REQUEST_LOCAL, NULL, m_masterAddress, m_masterPort, FALSE);
			}
		}
	}
	else
	{
		clearPeers();
		m_pPeers.clear();
		int mapNums = p->mapNums;
		m_PeerCount = mapNums;
		if (mapNums > 0)
		{
			for (int i = 0; i < mapNums; i++)
			{
				swprintf_s(m_addr, __TEXT("%d.%d.%d.%d"), (p->mapPeers[i].remoteIPAddr >> 24) & 0xFF, (p->mapPeers[i].remoteIPAddr >> 16) & 0xFF, (p->mapPeers[i].remoteIPAddr >> 8) & 0xFF, p->mapPeers[i].remoteIPAddr & 0xFF);
				swprintf_s(m_port, __TEXT("%d"), p->mapPeers[i].remotePort);
				swprintf_s(m_peerID, __TEXT("%d"), p->mapPeers[i].remotePeerID);
				swprintf_s(m_NumberStr, __TEXT("%03d"), i + 1);

				WriteMapFile();

				if (!FindLocalIP(m_addr))
				{
					CIPSCPeer *peer = new CIPSCPeer(this, m_addr, m_port);

					peer->SetPeerID(p->mapPeers[i].remotePeerID);
					peer->setLogPtr(m_report);
					m_pPeers.push_back(peer);
				}
			}
			//get master ip from setting,then add to peer
			CIPSCPeer *peer = new CIPSCPeer(this, m_masterAddress, m_masterPort);
			peer->setRemote3rdParty(false);
			peer->SetPeerID(m_ulMasterPeerID);
			peer->setLogPtr(m_report);
			m_pPeers.push_back(peer);

			sprintf_s(m_reportMsg, "MAP peers:");
			sendLogToWindow();
			//print info of peer
			for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
			{
				(*i)->printInfo();
			}
			//cycle WL register
			for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
			{
				(*i)->HandlePacket(WL_REGISTRATION_REQUEST_LOCAL, NULL, m_masterAddress, m_masterPort);
			}
		}
	}
	WCHAR str[8] = { 0 };
	swprintf_s(str, __TEXT("%d"), m_PeerCount);
	WritePrivateProfileString(SYS_SECTION, PEER_COUNT, str, m_strSettingFilePath);
}

// void CWLNet::AMBE2FrameToAMBE3000Frame(char* pFrame)
// {
// 
// }

// DWORD CWLNet::BuildWLRegistrationRequestPacket(LPBYTE pPacket, DWORD peerId, DWORD pudId)
// {
// 	DWORD size = 0;
// 
// 	pPacket[0] = WL_PROTOCOL;								// class
// 	*((DWORD*)&pPacket[1]) = htonl(peerId & 0x00ffffff);			// peer id
// 	pPacket[5] = WL_REGISTRATION_REQUEST;							// wireline opcode
// 	pPacket[6] = BOTH_SLOT1_SLOT2;									// registration slot number
// 	*((DWORD*)&pPacket[7]) = htonl(pudId);							// registration pud id
// 	*((WORD*)&pPacket[11]) = htons(m_wRegistrationId);				// registration id
// 	pPacket[13] = NOT_MONITOR_CHANNEL_STATUS;						// Wireline Channel Status Flag
// 	pPacket[14] = 4;												// 4 registration entry
// 	size = 15;
// 
// 	RegistrationEntry allIndivdualCall;
// 	allIndivdualCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
// 	allIndivdualCall.AddressType = AllIndividualCall;
// 	memcpy(&pPacket[size], &allIndivdualCall, sizeof(RegistrationEntry));
// 	size += sizeof(RegistrationEntry);
// 
// 	RegistrationEntry allTalkGroupCall;
// 	allTalkGroupCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
// 	allTalkGroupCall.AddressType = AllTalkGroupCall;
// 	memcpy(&pPacket[size], &allTalkGroupCall, sizeof(RegistrationEntry));
// 	size += sizeof(allTalkGroupCall);
// 
// 	RegistrationEntry allWideTalkGroupCall;
// 	allWideTalkGroupCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
// 	allWideTalkGroupCall.AddressType = AllWideTalkGroupCall;
// 	memcpy(&pPacket[size], &allWideTalkGroupCall, sizeof(RegistrationEntry));
// 	size += sizeof(allWideTalkGroupCall);
// 
// 	RegistrationEntry allLocalTalkGroupCall;
// 	allLocalTalkGroupCall.VoiceAttributes = REGISTERED_VOICE_SERVICE;
// 	allLocalTalkGroupCall.AddressType = AllLocalTalkGroupCall;
// 	memcpy(&pPacket[size], &allLocalTalkGroupCall, sizeof(RegistrationEntry));
// 	size += sizeof(allLocalTalkGroupCall);
// 
// 	return size;
// }

// BOOL CWLNet::FindPeer(u_long peerAddr)
// {
// 	if (0 == peerAddr)
// 	{
// 		return TRUE;
// 	}
// 
// 	for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
// 	{
// 		if ((*i)->GetAddress() == peerAddr)
// 		{
// 			return TRUE;
// 		}
// 	}
// 
// 	return FALSE;
// }

CIPSCPeer* CWLNet::GetPeer(u_long peerId)
{
	for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
	{
		u_long tempPeerId = (*i)->GetPeerID();
		if (tempPeerId == peerId)
		{
			return *i;
		}
	}

	return NULL;
}

//void CWLNet::PeerTimeout()
//{
//	for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
//	{
//		/************************************************************************/
//		/* 改动理由:MAST PEER 并不需要进行PEER 注册所以不需要纳入比较                                                                     */
//		/************************************************************************/
//		if (m_ulMasterPeerID != (*i)->GetPeerID())
//		{
//			(*i)->HandlePeerTimeout();
//		}
//	}
//}

//void CWLNet::ProcessCall(DWORD dwCallType, BOOL isTimeCheckout)
//{
//	/*将启用WL_LINE P2P暂时取消*/
//	return;
//
//	//如果当前dongle未打开
//	if (!g_dongle_open)
//	{
//		return;
//	}
//
//	//语音结束包超时处理
//	if (isTimeCheckout)
//	{
//		tCallParams nullValue = { 0 };
//		WriteVoiceFrame(nullValue, 0, isTimeCheckout);
//		return;
//	}
//
//	tCallParams thisCall;
//	//int CallIndex;
//	bool IsNewCallEvent;
//
//	IsNewCallEvent = FALSE;
//
//
//	thisCall.All[0] = (m_CurrentRecvBuffer.buf)[0];  //CallOpcode
//	thisCall.All[8] = (m_CurrentRecvBuffer.buf)[4];  //CallOriginatingPeerID
//	thisCall.All[9] = (m_CurrentRecvBuffer.buf)[3];  // "
//	thisCall.All[10] = (m_CurrentRecvBuffer.buf)[2];  // "
//	thisCall.All[11] = (m_CurrentRecvBuffer.buf)[1];  // "
//	thisCall.All[1] = (m_CurrentRecvBuffer.buf)[5];  //CallSequenceNumber
//	thisCall.All[16] = (m_CurrentRecvBuffer.buf)[8];  //CallSrcID
//	thisCall.All[17] = (m_CurrentRecvBuffer.buf)[7];  // "
//	thisCall.All[18] = (m_CurrentRecvBuffer.buf)[6];  // "
//	thisCall.All[19] = 0;  // "
//	thisCall.All[20] = (m_CurrentRecvBuffer.buf)[11]; //CallTgtID
//	thisCall.All[21] = (m_CurrentRecvBuffer.buf)[10]; // "
//	thisCall.All[22] = (m_CurrentRecvBuffer.buf)[9];  // "
//	thisCall.All[23] = 0;  // "
//	thisCall.All[2] = (m_CurrentRecvBuffer.buf)[12]; //CallPriority
//	thisCall.All[12] = (m_CurrentRecvBuffer.buf)[16]; //CallFloorControlTag
//	thisCall.All[13] = (m_CurrentRecvBuffer.buf)[15]; // "
//	thisCall.All[14] = (m_CurrentRecvBuffer.buf)[14]; // "
//	thisCall.All[15] = (m_CurrentRecvBuffer.buf)[13]; // "
//	thisCall.All[3] = (m_CurrentRecvBuffer.buf)[17]; //CallControlInformation
//	thisCall.All[24] = (m_CurrentRecvBuffer.buf)[25]; //RTPTimeStamp
//	thisCall.All[25] = (m_CurrentRecvBuffer.buf)[24]; // "
//	thisCall.All[26] = (m_CurrentRecvBuffer.buf)[23]; // "
//	thisCall.All[27] = (m_CurrentRecvBuffer.buf)[22]; // "
//	thisCall.All[4] = ((m_CurrentRecvBuffer.buf)[30]) & 0x7F; //RepeaterBurstDataType
//	thisCall.All[5] = (m_CurrentRecvBuffer.buf)[32]; //ESNLIEHB
//
//	//TRACE(_T("RTP 0:0x%x\r\n"), (m_CurrentRecvBuffer.buf)[30]);
//	//TRACE(_T("RTP 1:0x%x\r\n"), (m_CurrentRecvBuffer.buf)[31]);
//	//TRACE(_T("RTP 2:0x%x\r\n"), (m_CurrentRecvBuffer.buf)[32]);
//
//	//TRACE(_T("CALL SRC ID:%d\r\n"), thisCall.fld.CallSrcID);
//	//TRACE(_T("CALL Tgt ID:%d\r\n"), thisCall.fld.CallTgtID);
//	//TRACE(_T("CALL Sequence Number:%d\r\n"), thisCall.fld.CallSequenceNumber);
//
//	if (DATA_TYPE_VOICE == thisCall.fld.RepeaterBurstDataType ||
//		DATA_TYPE_VOICE_TERMINATOR == thisCall.fld.RepeaterBurstDataType)
//	{
//
//		if (!isTargetMeCall(thisCall.fld.CallTgtID))
//		{
//			return;
//		}
//
//		if (IPSC_PVT_VOICE_CALL == dwCallType)
//		{
//			WriteVoiceFrame(thisCall, IndividualCall);
//		}
//		else if (IPSC_GRP_VOICE_CALL == dwCallType)
//		{
//			WriteVoiceFrame(thisCall, GroupCall);
//		}
//
//	}
//}

//void CWLNet::Process_WL_BURST_CALL(char wirelineOpCode, bool isCheckTimeOut)
//{
//
//	//如果当前dongle未打开
//	if (!g_dongle_open)
//	{
//		return;
//	}
//
//	int nCallId, nPeerId, nSrcId, nTgtId;
//	char callType = m_CurrentRecvBuffer.buf[11];
//
//	int id, src, tgt, callid;
//
//	memcpy(&id, &m_CurrentRecvBuffer.buf[1], 4);
//	memcpy(&src, &m_CurrentRecvBuffer.buf[12], 4);
//	memcpy(&tgt, &m_CurrentRecvBuffer.buf[16], 4);
//	memcpy(&callid, &m_CurrentRecvBuffer.buf[7], 4);
//
//	nPeerId = ntohl(id);
//	nSrcId = ntohl(src);
//	nTgtId = ntohl(tgt);
//	nCallId = ntohl(callid);
//
//
//	if (!isTargetMeCall(nTgtId))
//	{
//		return;
//	}
//
//	if (WL_VC_VOICE_START == wirelineOpCode)
//	{
//		CRecordFile* rFile = new CRecordFile();
//		rFile->originalPeerId = nPeerId;
//		rFile->srcId = nSrcId;
//		rFile->tagetId = nTgtId;
//		rFile->sequenceNumber = nCallId;
//		rFile->callType = callType;
//		rFile->prevTimestamp = GetTickCount();
//
//		requireVoiceReocrdsLock();
//		m_voiceReocrds.push_back(rFile);
//		releaseVoiceReocrdsLock();
//
//		//g_dongle->m_bPrepareDecode = TRUE;
//		//TRACE(_T("Voice start\r\n"));
//		sprintf_s(m_reportMsg, "Voice start");
//		sendLogToWindow();
//
//		//转换dongle模式
//		if (!g_pDongle->changeAMBEToPCM())
//		{
//			//转换失败
//		}
//		else
//		{
//			//转换成功
//		}
//	}
//	else if (
//		WL_VC_VOICE_END_BURST == wirelineOpCode
//		|| WL_VC_CALL_SESSION_STATUS == wirelineOpCode
//		|| isCheckTimeOut
//		//|| isTimeout
//		)
//	{
//		//TRACE(_T("Voice end\r\n"));
//		for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
//		{
//			if ((*i)->srcId == nSrcId &&
//				(*i)->tagetId == nTgtId &&
//				(*i)->sequenceNumber == nCallId &&
//				!isCheckTimeOut)
//			{
//				//m_pEventLoger->OnNewVoiceRecord((LPBYTE)(*i)->buffer, (*i)->lenght, (*i)->srcId, (*i)->tagetId, (*i)->callType, 1);
//				//sprintf_s(m_reportMsg, "dwSize:%d,srcId:%d,tgtId:%d,type:%d,rectype:%d", (*i)->lenght, (*i)->srcId, (*i)->tagetId, (*i)->callType, 1);
//				//sendLogToWindow();
//
//				requireVoiceReocrdsLock();
//				delete (*i);
//				m_voiceReocrds.erase(i);
//				releaseVoiceReocrdsLock();
//
//				sprintf_s(m_reportMsg, "Voice end");
//				sendLogToWindow();
//
//				return;
//			}
//
//			if (isCheckTimeOut)
//			{
//				//add timeout deal
//				DWORD cur = GetTickCount();
//				DWORD prev = (*i)->prevTimestamp;
//				DWORD rlt = cur - prev;
//				if (rlt > VOICE_END_TIMEOUT)
//				{
//					//m_pEventLoger->OnNewVoiceRecord((LPBYTE)(*i)->buffer, (*i)->lenght, (*i)->srcId, (*i)->tagetId, (*i)->callType, 1);
//					//sprintf_s(m_reportMsg, "dwSize:%d,srcId:%d,tgtId:%d,type:%d,rectype:%d", (*i)->lenght, (*i)->srcId, (*i)->tagetId, (*i)->callType, 1);
//					//sendLogToWindow();
//
//					requireVoiceReocrdsLock();
//					delete (*i);
//					m_voiceReocrds.erase(i);
//					releaseVoiceReocrdsLock();
//
//					sprintf_s(m_reportMsg, "Voice end");
//					sendLogToWindow();
//
//					return;
//				}
//			}
//
//		}
//	}
//	else if (WL_VC_VOICE_BURST == wirelineOpCode)
//	{
//		char voiceFrame[7];
//
//		unsigned char flagRecord = 0x00;//初始化flag
//		unsigned char haveRecord = 0x01;//存在记录
//		unsigned char haveNotRecord = 0x02;//不存在记录
//
//		for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
//		{
//			if ((*i)->srcId == nSrcId &&
//				(*i)->tagetId == nTgtId &&
//				(*i)->sequenceNumber == nCallId)
//			{
//				flagRecord = 0x01;
//
//				voiceFrame[0] = (m_CurrentRecvBuffer.buf)[45];
//				voiceFrame[1] = (m_CurrentRecvBuffer.buf)[46];
//				voiceFrame[2] = (m_CurrentRecvBuffer.buf)[47];
//				voiceFrame[3] = (m_CurrentRecvBuffer.buf)[48];
//				voiceFrame[4] = (m_CurrentRecvBuffer.buf)[49];
//				voiceFrame[5] = (m_CurrentRecvBuffer.buf)[50];
//				voiceFrame[6] = ((m_CurrentRecvBuffer.buf)[51]) & 0x80;
//
//				//AMBE2FrameToAMBE3000Frame(voiceFrame);
//
//				(*i)->WriteVoiceFrame(voiceFrame);
//
//				//TRACE("Bad voice frame? %s\r\n", (((m_CurrentRecvBuffer.buf)[39]) & 0x40) > 0 ? _T("Yes") : _T("No"));
//
//
//				voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[51]) << 2) | ((((m_CurrentRecvBuffer.buf)[52]) >> 6) & 0x03);
//				voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[52]) << 2) | ((((m_CurrentRecvBuffer.buf)[53]) >> 6) & 0x03);
//				voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[53]) << 2) | ((((m_CurrentRecvBuffer.buf)[54]) >> 6) & 0x03);
//				voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[54]) << 2) | ((((m_CurrentRecvBuffer.buf)[55]) >> 6) & 0x03);
//				voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[55]) << 2) | ((((m_CurrentRecvBuffer.buf)[56]) >> 6) & 0x03);
//				voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[56]) << 2) | ((((m_CurrentRecvBuffer.buf)[57]) >> 6) & 0x03);
//				voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[57]) << 2) & 0x80;
//				//TRACE("Bad voice frame? %s\r\n", ((((m_CurrentRecvBuffer.buf)[45]) << 2) & 0x40) > 0 ? _T("Yes") : _T("No"));
//
//				//				AMBE2FrameToAMBE3000Frame(voiceFrame);
//				(*i)->WriteVoiceFrame(voiceFrame);
//
//				voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[57]) << 4) | ((((m_CurrentRecvBuffer.buf)[58]) >> 4) & 0x0F);
//				voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[58]) << 4) | ((((m_CurrentRecvBuffer.buf)[59]) >> 4) & 0x0F);
//				voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[59]) << 4) | ((((m_CurrentRecvBuffer.buf)[60]) >> 4) & 0x0F);
//				voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[60]) << 4) | ((((m_CurrentRecvBuffer.buf)[61]) >> 4) & 0x0F);
//				voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[61]) << 4) | ((((m_CurrentRecvBuffer.buf)[62]) >> 4) & 0x0F);
//				voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[62]) << 4) | ((((m_CurrentRecvBuffer.buf)[63]) >> 4) & 0x0F);
//				voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[63]) << 4) & 0x80;
//				//TRACE("Bad voice frame? %s\r\n", ((((m_CurrentRecvBuffer.buf)[51]) << 4) & 0x40) > 0 ? _T("Yes") : _T("No"));
//				//				AMBE2FrameToAMBE3000Frame(voiceFrame);
//				(*i)->WriteVoiceFrame(voiceFrame);
//				break;
//			}
//			else
//			{
//				flagRecord = 0x02;
//			}
//		}
//
//		if (flagRecord & haveNotRecord)
//		{
//			CRecordFile* rFile = new CRecordFile();
//			rFile->originalPeerId = nPeerId;
//			rFile->srcId = nSrcId;
//			rFile->tagetId = nTgtId;
//			rFile->sequenceNumber = nCallId;
//			rFile->callType = callType;
//			rFile->prevTimestamp = GetTickCount();
//
//			requireVoiceReocrdsLock();
//			m_voiceReocrds.push_back(rFile);
//			releaseVoiceReocrdsLock();
//
//
//			voiceFrame[0] = (m_CurrentRecvBuffer.buf)[45];
//			voiceFrame[1] = (m_CurrentRecvBuffer.buf)[46];
//			voiceFrame[2] = (m_CurrentRecvBuffer.buf)[47];
//			voiceFrame[3] = (m_CurrentRecvBuffer.buf)[48];
//			voiceFrame[4] = (m_CurrentRecvBuffer.buf)[49];
//			voiceFrame[5] = (m_CurrentRecvBuffer.buf)[50];
//			voiceFrame[6] = ((m_CurrentRecvBuffer.buf)[51]) & 0x80;
//
//			//			AMBE2FrameToAMBE3000Frame(voiceFrame);
//
//			rFile->WriteVoiceFrame(voiceFrame);
//
//			//TRACE("Bad voice frame? %s\r\n", (((m_CurrentRecvBuffer.buf)[39]) & 0x40) > 0 ? _T("Yes") : _T("No"));
//
//
//			voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[51]) << 2) | ((((m_CurrentRecvBuffer.buf)[52]) >> 6) & 0x03);
//			voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[52]) << 2) | ((((m_CurrentRecvBuffer.buf)[53]) >> 6) & 0x03);
//			voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[53]) << 2) | ((((m_CurrentRecvBuffer.buf)[54]) >> 6) & 0x03);
//			voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[54]) << 2) | ((((m_CurrentRecvBuffer.buf)[55]) >> 6) & 0x03);
//			voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[55]) << 2) | ((((m_CurrentRecvBuffer.buf)[56]) >> 6) & 0x03);
//			voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[56]) << 2) | ((((m_CurrentRecvBuffer.buf)[57]) >> 6) & 0x03);
//			voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[57]) << 2) & 0x80;
//			//TRACE("Bad voice frame? %s\r\n", ((((m_CurrentRecvBuffer.buf)[45]) << 2) & 0x40) > 0 ? _T("Yes") : _T("No"));
//
//			//			AMBE2FrameToAMBE3000Frame(voiceFrame);
//			rFile->WriteVoiceFrame(voiceFrame);
//
//			voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[57]) << 4) | ((((m_CurrentRecvBuffer.buf)[58]) >> 4) & 0x0F);
//			voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[58]) << 4) | ((((m_CurrentRecvBuffer.buf)[59]) >> 4) & 0x0F);
//			voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[59]) << 4) | ((((m_CurrentRecvBuffer.buf)[60]) >> 4) & 0x0F);
//			voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[60]) << 4) | ((((m_CurrentRecvBuffer.buf)[61]) >> 4) & 0x0F);
//			voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[61]) << 4) | ((((m_CurrentRecvBuffer.buf)[62]) >> 4) & 0x0F);
//			voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[62]) << 4) | ((((m_CurrentRecvBuffer.buf)[63]) >> 4) & 0x0F);
//			voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[63]) << 4) & 0x80;
//			//TRACE("Bad voice frame? %s\r\n", ((((m_CurrentRecvBuffer.buf)[51]) << 4) & 0x40) > 0 ? _T("Yes") : _T("No"));
//			//			AMBE2FrameToAMBE3000Frame(voiceFrame);
//			rFile->WriteVoiceFrame(voiceFrame);
//		}
//	}
//}

void CWLNet::Process_WL_BURST_CALL(char wirelineOpCode, void  *pNetWork)
{
	DWORD callId, srcId, tgtId;
	switch (wirelineOpCode)
	{
		/*check is lost terminal*/
	case WL_BURST_CHECK_TIMEOUT:
	{
								   /*获取当前时间戳*/
								   DWORD currentTimestamp = GetTickCount();
								   /*结束超时的语音*/
								   for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
								   {
									   long diffTimestamp = currentTimestamp - (*i)->prevTimestamp;
									   CRecordFile* p = (CRecordFile*)(*i);
									   if (diffTimestamp > CONFIG_HUNG_TIME && VOICE_STATUS_CALLBACK == (*i)->callStatus)
									   {
										   if (isTargetMeCall(p->tagetId, p->callType))
										   {
											   Send_CARE_CALL_STATUS(p->callType, p->srcId, p->tagetId, END_CALL_NO_PLAY);
										   }
										   m_pEventLoger->OnNewVoiceRecord((LPBYTE)(*i)->buffer, (*i)->lenght, (*i)->srcId, (*i)->tagetId, (*i)->callType, CONFIG_RECORD_TYPE, (*i)->originalPeerId, (*i)->srcSlot, (*i)->srcRssi, (*i)->callStatus, &((*i)->recordTime));
										   requireVoiceReocrdsLock();
										   delete (*i);
										   m_voiceReocrds.erase(i);
										   releaseVoiceReocrdsLock();
										   sprintf_s(m_reportMsg, "call back,then voice end %ld", diffTimestamp);
										   sendLogToWindow();
										   break;
									   }
									   else if (diffTimestamp > VOICE_END_TIMEOUT && VOICE_STATUS_CALLBACK != (*i)->callStatus)
									   {
										   if (isTargetMeCall((*i)->tagetId, (*i)->callType) && !g_dongleIsUsing)
										   {
											   SetCallStatus(CALL_IDLE);
										   }
										   (*i)->callStatus = VOICE_STATUS_END;
										   GetLocalTime(&((*i)->recordTime));

										   if (isTargetMeCall(p->tagetId, p->callType))
										   {
											   Send_CARE_CALL_STATUS(p->callType, p->srcId, p->tagetId, END_CALL_NO_PLAY);
										   }
										   m_pEventLoger->OnNewVoiceRecord((LPBYTE)(*i)->buffer, (*i)->lenght, (*i)->srcId, (*i)->tagetId, (*i)->callType, CONFIG_RECORD_TYPE, (*i)->originalPeerId, (*i)->srcSlot, (*i)->srcRssi,(*i)->callStatus,&((*i)->recordTime));
										   requireVoiceReocrdsLock();
										   delete (*i);
										   m_voiceReocrds.erase(i);
										   releaseVoiceReocrdsLock();
										   sprintf_s(m_reportMsg, "call timeout,then end %ld", diffTimestamp);
										   sendLogToWindow();
										   break;
									   }
								   }
	}
		break;
		/*recive a voice statrt of a call*/
	case WL_VC_VOICE_START:
	{
							  //SetCallStatus(CALL_START);
							  //g_pDongle->changeAMBEToPCM();

							  T_WL_PROTOCOL_18 *p = (T_WL_PROTOCOL_18*)pNetWork;
							  /*建立语音记录*/
							  CRecordFile* rFile = new CRecordFile();
							  rFile->originalPeerId = p->peerID;
							  rFile->srcId = p->sourceID;
							  rFile->tagetId = p->targetID;
							  rFile->callId = p->callID;
							  rFile->callType = p->callType;
							  rFile->prevTimestamp = GetTickCount();
							  rFile->srcSlot = p->slotNumber;
							  rFile->callStatus = VOICE_STATUS_START;

							  if (isTargetMeCall(p->targetID, p->callType))
							  {
								  SetCallStatus(CALL_START);
							  }

							  requireVoiceReocrdsLock();
							  m_voiceReocrds.push_back(rFile);
							  releaseVoiceReocrdsLock();

							  sprintf_s(m_reportMsg, "Voice start");
							  sendLogToWindow();
	}
		break;
		/*recive a voice end of a call*/
	case WL_VC_VOICE_END_BURST:
	{
								  T_WL_PROTOCOL_19 *p = (T_WL_PROTOCOL_19*)pNetWork;
								  callId = p->callID;
								  srcId = p->sourceID;
								  tgtId = p->targetID;
								  /*结束本次语音*/
								  for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
								  {
									  if ((*i)->srcId == srcId &&
										  (*i)->tagetId == tgtId &&
										  (*i)->callId == callId)
									  {
										  GetLocalTime(&((*i)->recordTime));
										  //m_pEventLoger->OnNewVoiceRecord((LPBYTE)(*i)->buffer, (*i)->lenght, (*i)->srcId, (*i)->tagetId, (*i)->callType, g_recordType, (*i)->originalPeerId, (*i)->srcSlot, (*i)->srcRssi, (*i)->callStatus, &((*i)->recordTime));
										  //requireVoiceReocrdsLock();
										  //delete (*i);
										  //m_voiceReocrds.erase(i);
										  //releaseVoiceReocrdsLock();
										  sprintf_s(m_reportMsg, "Voice end");
										  sendLogToWindow();
										  break;
									  }
								  }
	}
		break;
		/*recive a voice data of a call*/
	case WL_VC_VOICE_BURST:
	{

							  //SetCallStatus(CALL_ONGOING);
							  char voiceFrame1[7] = { 0 };
							  char voiceFrame2[7] = { 0 };
							  char voiceFrame3[7] = { 0 };
							  T_WL_PROTOCOL_21 *p = (T_WL_PROTOCOL_21*)pNetWork;
							  callId = p->callID;
							  srcId = p->sourceID;
							  tgtId = p->targetID;
							  bool ishaveRecord = false;

							  if (isTargetMeCall(p->targetID, p->callType))
							  {
								  SetCallStatus(CALL_ONGOING);
							  }

							  char* pAmbePacket = &(p->AMBEVoiceEncodedFrames.data[0]);

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

							  for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
							  {
								  /*存在则记录语音*/
								  if ((*i)->srcId == srcId &&
									  (*i)->tagetId == tgtId &&
									  (*i)->callId == callId)
								  {
									  ishaveRecord = true;
									  (*i)->srcRssi = p->rawRssiValue;
									  (*i)->WriteVoiceFrame(voiceFrame1);
									  (*i)->WriteVoiceFrame(voiceFrame2);
									  (*i)->WriteVoiceFrame(voiceFrame3);
									  break;
								  }
							  }
							  if (!ishaveRecord)
							  {
								  CRecordFile* rFile = new CRecordFile();
								  rFile->originalPeerId = p->peerID;
								  rFile->srcId = p->sourceID;
								  rFile->tagetId = p->targetID;
								  rFile->callId = p->callID;
								  rFile->callType = p->callType;
								  rFile->prevTimestamp = GetTickCount();
								  rFile->srcSlot = p->slotNumber;
								  rFile->srcRssi = p->rawRssiValue;
								  rFile->callStatus = VOICE_STATUS_START;

								  rFile->WriteVoiceFrame(voiceFrame1);
								  rFile->WriteVoiceFrame(voiceFrame2);
								  rFile->WriteVoiceFrame(voiceFrame3);

								  requireVoiceReocrdsLock();
								  m_voiceReocrds.push_back(rFile);
								  releaseVoiceReocrdsLock();

								  sprintf_s(m_reportMsg, "lost header Voice start");
								  sendLogToWindow();
							  }
	}
		break;
		/*recive a call hang up or end*/
	case WL_VC_CALL_SESSION_STATUS:
	{
									  T_WL_PROTOCOL_20* p = (T_WL_PROTOCOL_20*)pNetWork;
									  callId = p->callID;
									  srcId = p->sourceID;
									  tgtId = p->targetID;

									  switch (p->callSessionStatus)
									  {
									  case Call_Session_End:
									  {
															   if (isTargetMeCall(tgtId, p->callType))
															   {
																   SetCallStatus(CALL_IDLE);
																   Send_CARE_CALL_STATUS(p->callType, srcId, tgtId, END_CALL_NO_PLAY);
															   }
															   ///*结束本次通话*/
															   for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
															   {
																   if ((*i)->srcId == srcId &&
																	   (*i)->tagetId == tgtId &&
																	   (*i)->callId == callId)
																   {
																	   (*i)->callStatus = VOICE_STATUS_END;
																	   //GetLocalTime(&((*i)->recordTime));
																	   m_pEventLoger->OnNewVoiceRecord((LPBYTE)(*i)->buffer, (*i)->lenght, (*i)->srcId, (*i)->tagetId, (*i)->callType, CONFIG_RECORD_TYPE, (*i)->originalPeerId, (*i)->srcSlot, (*i)->srcRssi, (*i)->callStatus, &((*i)->recordTime));
																	   requireVoiceReocrdsLock();
																	   delete (*i);
																	   m_voiceReocrds.erase(i);
																	   releaseVoiceReocrdsLock();
																	   sprintf_s(m_reportMsg, "Voice session end");
																	   sendLogToWindow();
																	   break;
																   }
															   }
									  }
										  break;
									  case Call_Session_Call_Hang:
									  {

																	 /*设置当前可通话peer相关属性*/

																	 if (isTargetMeCall(p->targetID, p->callType))
																	 {
																		 SetCallStatus(CALL_HANGUP);
																		 CIPSCPeer *peer = GetPeer(p->peerID);
																		 peer->setUseSlot(p->slotNumber);
																		 setCurrentSendVoicePeer(peer);
																		 g_targetId = p->targetID;
																		 g_targetCallType = p->callType;
																	 }
																	 for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
																	 {
																		 if ((*i)->srcId == srcId &&
																			 (*i)->tagetId == tgtId &&
																			 (*i)->callId == callId)
																		 {
																			 (*i)->callStatus = VOICE_STATUS_CALLBACK;
																			 //GetLocalTime(&((*i)->recordTime));
																			 //m_pEventLoger->OnNewVoiceRecord((LPBYTE)(*i)->buffer, (*i)->lenght, (*i)->srcId, (*i)->tagetId, (*i)->callType, g_recordType, (*i)->originalPeerId, (*i)->srcSlot, (*i)->srcRssi, (*i)->callStatus, &((*i)->recordTime));
																			 //requireVoiceReocrdsLock();
																			 //delete (*i);
																			 //m_voiceReocrds.erase(i);
																			 //releaseVoiceReocrdsLock();
																			 sprintf_s(m_reportMsg, "Voice hang up");
																			 sendLogToWindow();
																			 break;
																		 }
																	 }
									  }
										  break;
									  default:
										  break;
									  }
	}
		break;
	default:
		//do nothing
		break;
	}
}

BOOL CWLNet::WriteVoiceFrame(tCallParams& call, DWORD dwCallType, BOOL isCheckTimeout)
{
	//尾结束包超时处理
	if (isCheckTimeout)
	{

		//TRACE(_T("m_voiceReocrds.size:%d\r\n"), m_voiceReocrds.size());

		DWORD curTime = 0;
		DWORD preTime = 0;
		DWORD rlt = 0;
		for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
		{
			curTime = GetTickCount();
			preTime = (*i)->prevTimestamp;
			rlt = curTime - preTime;
			//TRACE(_T("rlt:%d\r\n"), rlt);
			if (rlt > VOICE_END_TIMEOUT)
			{
				//if (m_pEventLoger)
				//{
				//	int temp = 0;

				//	if (m_dwRecType == IPSC)
				//	{
				//		if (IndividualCall == (*i)->callType)
				//		{
				//			temp = IndividualCall;
				//		}
				//		else if (GroupCall == (*i)->callType)
				//		{
				//			temp = GroupCall;
				//		}
				//	}
				//	else if (m_dwRecType == CPC)
				//	{
				//		if (IndividualCall == (*i)->callType)
				//		{
				//			temp = CPC_PRIVATE_CALL;
				//		}
				//		else if (GroupCall == (*i)->callType)
				//		{
				//			temp = CPC_GROUP_CALL;
				//		}
				//	}

				//	m_pEventLoger->OnNewVoiceRecord((LPBYTE)(*i)->buffer, (*i)->lenght, (*i)->srcId, (*i)->tagetId, temp, 0);

				//}


				requireVoiceReocrdsLock();
				delete (*i);
				m_voiceReocrds.erase(i);
				releaseVoiceReocrdsLock();

				sprintf_s(m_reportMsg, "Voice end");
				sendLogToWindow();
			}
		}
		return TRUE;
	}

	char voiceFrame[7];
	BOOL bFind = FALSE;

	for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
	{
		if ((*i)->srcId == call.fld.CallSrcID &&
			(*i)->tagetId == call.fld.CallTgtID &&
			(*i)->callId == call.fld.CallSequenceNumber)
		{
			voiceFrame[0] = (m_CurrentRecvBuffer.buf)[33];
			voiceFrame[1] = (m_CurrentRecvBuffer.buf)[34];
			voiceFrame[2] = (m_CurrentRecvBuffer.buf)[35];
			voiceFrame[3] = (m_CurrentRecvBuffer.buf)[36];
			voiceFrame[4] = (m_CurrentRecvBuffer.buf)[37];
			voiceFrame[5] = (m_CurrentRecvBuffer.buf)[38];
			voiceFrame[6] = ((m_CurrentRecvBuffer.buf)[39]) & 0x80;

			//			AMBE2FrameToAMBE3000Frame(voiceFrame);

			(*i)->WriteVoiceFrame(voiceFrame);

			//TRACE("Bad voice frame? %s\r\n", (((m_CurrentRecvBuffer.buf)[39]) & 0x40) > 0 ? _T("Yes") : _T("No"));


			voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[39]) << 2) | ((((m_CurrentRecvBuffer.buf)[40]) >> 6) & 0x03);
			voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[40]) << 2) | ((((m_CurrentRecvBuffer.buf)[41]) >> 6) & 0x03);
			voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[41]) << 2) | ((((m_CurrentRecvBuffer.buf)[42]) >> 6) & 0x03);
			voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[42]) << 2) | ((((m_CurrentRecvBuffer.buf)[43]) >> 6) & 0x03);
			voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[43]) << 2) | ((((m_CurrentRecvBuffer.buf)[44]) >> 6) & 0x03);
			voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[44]) << 2) | ((((m_CurrentRecvBuffer.buf)[45]) >> 6) & 0x03);
			voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[45]) << 2) & 0x80;
			//TRACE("Bad voice frame? %s\r\n", ((((m_CurrentRecvBuffer.buf)[45]) << 2) & 0x40) > 0 ? _T("Yes") : _T("No"));

			//			AMBE2FrameToAMBE3000Frame(voiceFrame);
			(*i)->WriteVoiceFrame(voiceFrame);

			voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[45]) << 4) | ((((m_CurrentRecvBuffer.buf)[46]) >> 4) & 0x0F);
			voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[46]) << 4) | ((((m_CurrentRecvBuffer.buf)[47]) >> 4) & 0x0F);
			voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[47]) << 4) | ((((m_CurrentRecvBuffer.buf)[48]) >> 4) & 0x0F);
			voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[48]) << 4) | ((((m_CurrentRecvBuffer.buf)[49]) >> 4) & 0x0F);
			voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[49]) << 4) | ((((m_CurrentRecvBuffer.buf)[50]) >> 4) & 0x0F);
			voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[50]) << 4) | ((((m_CurrentRecvBuffer.buf)[51]) >> 4) & 0x0F);
			voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[51]) << 4) & 0x80;
			//TRACE("Bad voice frame? %s\r\n", ((((m_CurrentRecvBuffer.buf)[51]) << 4) & 0x40) > 0 ? _T("Yes") : _T("No"));
			//			AMBE2FrameToAMBE3000Frame(voiceFrame);
			(*i)->WriteVoiceFrame(voiceFrame);
			//TRACE(_T("burst type:%d\r\n"), call.fld.RepeaterBurstDataType);
			if (DATA_TYPE_VOICE_TERMINATOR == call.fld.RepeaterBurstDataType)
			{
				// it's last frame, write to file and erase this item
				//FILE *f;
				//f = fopen(("e:\\moto.bit"), ("wb+"));
				//fwrite((*i)->buffer, 1, (*i)->lenght, f);
				//fclose(f);
				//if (m_pEventLoger)
				//{
				//	int temp = 0;

				//	if (m_dwRecType == IPSC)
				//	{
				//		if (IndividualCall == (*i)->callType)
				//		{
				//			temp = IndividualCall;
				//		}
				//		else if (GroupCall == (*i)->callType)
				//		{
				//			temp = GroupCall;
				//		}
				//	}
				//	else if (m_dwRecType == CPC)
				//	{
				//		if (IndividualCall == (*i)->callType)
				//		{
				//			temp = CPC_PRIVATE_CALL;
				//		}
				//		else if (GroupCall == (*i)->callType)
				//		{
				//			temp = CPC_GROUP_CALL;
				//		}
				//	}

				//	m_pEventLoger->OnNewVoiceRecord((LPBYTE)(*i)->buffer, (*i)->lenght, (*i)->srcId, (*i)->tagetId, temp, 0);

				//}

				requireVoiceReocrdsLock();
				delete (*i);
				m_voiceReocrds.erase(i);
				releaseVoiceReocrdsLock();

				sprintf_s(m_reportMsg, "Voice end");
				sendLogToWindow();
			}

			bFind = TRUE;
			break;
		}
	}
	if (!bFind)
	{

		sprintf_s(m_reportMsg, "Voice start");
		sendLogToWindow();


		//转换dongle模式
		if (!g_pDongle->changeAMBEToPCM())
		{
			//转换失败
		}
		else
		{
			//转换成功
		}

		CRecordFile* rFile = new CRecordFile();
		voiceFrame[0] = (m_CurrentRecvBuffer.buf)[33];
		voiceFrame[1] = (m_CurrentRecvBuffer.buf)[34];
		voiceFrame[2] = (m_CurrentRecvBuffer.buf)[35];
		voiceFrame[3] = (m_CurrentRecvBuffer.buf)[36];
		voiceFrame[4] = (m_CurrentRecvBuffer.buf)[37];
		voiceFrame[5] = (m_CurrentRecvBuffer.buf)[38];
		voiceFrame[6] = ((m_CurrentRecvBuffer.buf)[39]) & 0x80;

		//		AMBE2FrameToAMBE3000Frame(voiceFrame);
		rFile->WriteVoiceFrame(voiceFrame);


		voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[39]) << 2) | ((((m_CurrentRecvBuffer.buf)[40]) >> 6) & 0x03);
		voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[40]) << 2) | ((((m_CurrentRecvBuffer.buf)[41]) >> 6) & 0x03);
		voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[41]) << 2) | ((((m_CurrentRecvBuffer.buf)[42]) >> 6) & 0x03);
		voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[42]) << 2) | ((((m_CurrentRecvBuffer.buf)[43]) >> 6) & 0x03);
		voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[43]) << 2) | ((((m_CurrentRecvBuffer.buf)[44]) >> 6) & 0x03);
		voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[44]) << 2) | ((((m_CurrentRecvBuffer.buf)[45]) >> 6) & 0x03);
		voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[45]) << 2) & 0x80;

		//		AMBE2FrameToAMBE3000Frame(voiceFrame);
		rFile->WriteVoiceFrame(voiceFrame);

		voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[45]) << 4) | ((((m_CurrentRecvBuffer.buf)[46]) >> 4) & 0x0F);
		voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[46]) << 4) | ((((m_CurrentRecvBuffer.buf)[47]) >> 4) & 0x0F);
		voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[47]) << 4) | ((((m_CurrentRecvBuffer.buf)[48]) >> 4) & 0x0F);
		voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[48]) << 4) | ((((m_CurrentRecvBuffer.buf)[49]) >> 4) & 0x0F);
		voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[49]) << 4) | ((((m_CurrentRecvBuffer.buf)[50]) >> 4) & 0x0F);
		voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[50]) << 4) | ((((m_CurrentRecvBuffer.buf)[51]) >> 4) & 0x0F);
		voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[51]) << 4) & 0x80;

		//		AMBE2FrameToAMBE3000Frame(voiceFrame);
		rFile->WriteVoiceFrame(voiceFrame);

		rFile->originalPeerId = call.fld.CallOriginatingPeerID;
		rFile->srcId = call.fld.CallSrcID;
		rFile->tagetId = call.fld.CallTgtID;
		rFile->callId = call.fld.CallSequenceNumber;
		rFile->callType = (unsigned char)dwCallType;


		sprintf_s(m_reportMsg, "src:%d,tgt:%d", rFile->srcId, rFile->tagetId);
		sendLogToWindow();

		requireVoiceReocrdsLock();
		m_voiceReocrds.push_back(rFile);
		releaseVoiceReocrdsLock();
	}
	return TRUE;
}

void CWLNet::calEnciphe()
{
	unsigned long * lpUnencryptValue = (unsigned long *)(unsigned char *const)un_Auth;
	unsigned long * lpEncryptValue = (unsigned long *)(unsigned char *)en_Auth;
	unsigned long   u_auth_key[4] = { 0x2df96670, 0x53434667, 0x7c6c4973, 0x260f0c7d };

	register  unsigned long delta = 0x7ae6582c;
	register unsigned  long n = 32, sum = 0;


	//大端传输转小端传输
	*lpUnencryptValue = (((*lpUnencryptValue) & 0xff000000) >> 24) | (((*lpUnencryptValue) & 0x00ff0000) >> 8) | (((*lpUnencryptValue) & 0x0000ff00) << 8) | (((*lpUnencryptValue) & 0x000000ff) << 24);
	*(lpUnencryptValue + 1) = ((*(lpUnencryptValue + 1) & 0xff000000) >> 24) | ((*(lpUnencryptValue + 1) & 0x00ff0000) >> 8) | ((*(lpUnencryptValue + 1) & 0x0000ff00) << 8) | ((*(lpUnencryptValue + 1) & 0x000000ff) << 24);

	register unsigned long v0 = lpUnencryptValue[0];
	register unsigned long v1 = lpUnencryptValue[1];

	//加密计算
	while (n-- > 0)
	{

		sum += delta;
		v0 += ((v1 << 4) + u_auth_key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + u_auth_key[1]);
		v1 += ((v0 << 4) + u_auth_key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + u_auth_key[3]);
	}
	lpEncryptValue[0] = v0;
	lpEncryptValue[1] = v1;

	//小端传输转大端传输
	*lpEncryptValue = (((*lpEncryptValue) & 0xff000000) >> 24) | (((*lpEncryptValue) & 0x00ff0000) >> 8) | (((*lpEncryptValue) & 0x0000ff00) << 8) | (((*lpEncryptValue) & 0x000000ff) << 24);
	*(lpEncryptValue + 1) = (((*(lpEncryptValue + 1)) & 0xff000000) >> 24) | (((*(lpEncryptValue + 1)) & 0x00ff0000) >> 8) | (((*(lpEncryptValue + 1)) & 0x0000ff00) << 8) | (((*(lpEncryptValue + 1)) & 0x000000ff) << 24);

	//加密计算的结果

	memcpy(en_Auth, lpEncryptValue, 8);
}

void CWLNet::Net_XNL_CONNECT(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);
		WSAResetEvent(m_eventArray[RxEvent]);

		if (FALSE == rc)
		{
			rc = WSAGetLastError();
			rc = IssueReadRequest();
		}
		else
		{
			rc = IssueReadRequest();

			m_ControlProto[0] = LE_XNL;
			m_ControlProto[5] = 0x00;
			m_ControlProto[6] = 0x0C;
			m_ControlProto[7] = LE_XNL_DEVICE_MASTER_QUERY_1;
			m_ControlProto[8] = LE_XNL_DEVICE_MASTER_QUERY_2;
			m_ControlProto[9] = 0x00;
			m_ControlProto[10] = 0x00;
			m_ControlProto[11] = 0X00;
			m_ControlProto[12] = 0X00;
			m_ControlProto[13] = 0x00;
			m_ControlProto[14] = 0x00;
			m_ControlProto[15] = 0x00;
			m_ControlProto[16] = 0x00;
			m_ControlProto[17] = 0x00;
			m_ControlProto[18] = 0x00;


			m_SendControlBuffer.len = LE_XNL_DEVICE_MASTER_QUERY_L;
			m_dwXnlMasterStatusBrdcstTimer = GetTickCount();
			rc = SendToLE(&m_masterAddr);
			switch (rc)
			{
			case 0:               //Tx completed successfully immediately.
			case WSA_IO_PENDING:  //Completion is pending
				m_WLStatus = WAITFOR_XNL_DEVICE_MASTER_QUERY_TX;
				//strSysLog = _T("发送0x70(XNL_DEVICE_MASTER_QUERY)");
				//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
				break;
			default:
				break;

			}
			break;

	case TickIndex: // quit
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			// send deregister request to master node
			Net_DeRegisterLE();
		}
		break;

	case  TIMEOUT:
		if ((GetTickCount() - m_dwMasterMapBroadcastTimer) > 100000UL)
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;
	default:
		break;
		}
	}
}

void CWLNet::Net_XNL_DEVICE_MASTER_QUERY_TX(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_TxOverlapped,
			&m_dwByteSent,
			TRUE,
			&m_dwTxFlags);
		WSAResetEvent(m_eventArray[TxEvent]);
		if (TRUE != rc){ //XNL_DEVICE_MASTER_QUERY Transmission failed.

			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("发送0x70(XNL_DEVICE_MASTER_QUERY)失败，重新注册");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
			break;
		}
		m_dwXnlMasterStatusBrdcstTimer = GetTickCount();

		m_WLStatus = WAITFOR_XNL_MASTER_STATUS_BROADCAST;

		break;

	case RxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		// transfer to XNL_DEVICE_MASTER_QUERY
		m_WLStatus = WAITFOR_XNL_MASTER_STATUS_BROADCAST;
		//strSysLog = _T("等待0x70(XNL_MASTER_STATUS_BROADCAST)");
		//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		break;

	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlMasterStatusBrdcstTimer) > 100000UL)
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}

		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;
	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_XNL_MASTER_STATUS_BROADCAST(DWORD eventIndex)
{
	int rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);

		WSAResetEvent(m_eventArray[RxEvent]);
		if (FALSE == rc) {//reception failed
			rc = WSAGetLastError(); //For debugging if nothing else.
			//Restart listener using same buffer.
			rc = IssueReadRequest();
			break;
		}
		else{
			rc = IssueReadRequest();
			if (LE_XNL == (m_CurrentRecvBuffer.buf[0]) && (0X02 == m_CurrentRecvBuffer.buf[8]))
			{
				memcpy(masterAddr, m_CurrentRecvBuffer.buf + 13, 2);
				m_ControlProto[0] = LE_XNL;
				m_ControlProto[5] = 0x00;
				m_ControlProto[6] = 0x0C;
				m_ControlProto[7] = LE_XNL_DEVICE_AUTH_KEY_REQUEST_1;
				m_ControlProto[8] = LE_XNL_DEVICE_AUTH_KEY_REQUEST_2;
				m_ControlProto[9] = 0x00;
				m_ControlProto[10] = 0x00;
				m_ControlProto[11] = masterAddr[0];
				m_ControlProto[12] = masterAddr[1];
				m_ControlProto[13] = 0x00;
				m_ControlProto[14] = 0x00;
				m_ControlProto[15] = 0x00;
				m_ControlProto[16] = 0x00;
				m_ControlProto[17] = 0x00;
				m_ControlProto[18] = 0x00;

				WSAResetEvent(m_eventArray[TxEvent]); //Shouldn't be needed.
				m_SendControlBuffer.len = 19;
				m_dwXnlDeviceAuthTimer = GetTickCount();
				rc = SendToLE(&m_masterAddr);
				switch (rc)
				{
				case 0:               //Tx completed successfully immediately.
				case WSA_IO_PENDING:  //Completion is pending
					m_WLStatus = WAITFOR_XNL_DEVICE_AUTH_KEY_REQUEST_TX;
					//strSysLog = _T("发送0x70(XNL_DEVICE_AUTH_KEY_REQUEST)");
					//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
					break;
				default:              //stay in this state and eventually retry.
					break;
				}
			}
			else
			{
				rc = IssueReadRequest();
			}
			// if ((LE_MASTER_PEER_REGISTRATION_RESPONSE
		} // if (FALSE == rc)
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;

	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlMasterStatusBrdcstTimer) > 100000UL)  // 100 seconds
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;

	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_XNL_DEVICE_AUTH_KEY_REQUEST_TX(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_TxOverlapped,
			&m_dwByteSent,
			TRUE,
			&m_dwTxFlags);

		WSAResetEvent(m_eventArray[TxEvent]);
		if (TRUE != rc){
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("发送0x70(XNL_DEVICE_AUTH_KEY_REQUEST)失败，重新注册");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
			break;
		}
		m_dwXnlDeviceAuthTimer = GetTickCount();
		m_WLStatus = WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY;
		//strSysLog = _T("等待0x70(XNL_DEVICE_AUTH_KEY_REPLY)");
		//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		break;

	case RxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		m_WLStatus = WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY;
		//	strSysLog = _T("等待0x70(XNL_DEVICE_AUTH_KEY_REPLY)");
		//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		break;
	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlDeviceAuthTimer) > 100000UL)  // 100 seconds
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;
	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY(DWORD eventIndex)
{
	int rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);

		WSAResetEvent(m_eventArray[RxEvent]);
		if (FALSE == rc)
		{
			rc = WSAGetLastError();
			rc = IssueReadRequest();
			break;
		}
		else{
			if (m_CurrentRecvBuffer.buf[0] == LE_XNL && m_CurrentRecvBuffer.buf[8] == 0x05)
			{
				rc = IssueReadRequest();
				memcpy(tempXNLAddr, m_CurrentRecvBuffer.buf + 19, 2);
				memcpy(un_Auth, m_CurrentRecvBuffer.buf + 21, 8);
				calEnciphe();

				m_ControlProto[0] = LE_XNL;
				m_ControlProto[5] = 0x00;
				m_ControlProto[6] = 0x18;
				m_ControlProto[7] = LE_XNL_DEVICE_CONNECT_REQUEST_1;
				m_ControlProto[8] = LE_XNL_DEVICE_CONNECT_REQUEST_2;
				m_ControlProto[9] = 0x00;
				m_ControlProto[10] = 0x00;
				m_ControlProto[11] = masterAddr[0];
				m_ControlProto[12] = masterAddr[1];
				m_ControlProto[13] = tempXNLAddr[0];
				m_ControlProto[14] = tempXNLAddr[1];
				m_ControlProto[15] = 0x00;
				m_ControlProto[16] = 0x00;
				m_ControlProto[17] = 0x00;
				m_ControlProto[18] = 0x0C;
				m_ControlProto[19] = 0x00;
				m_ControlProto[20] = 0x00;
				m_ControlProto[21] = 0x0A;
				m_ControlProto[22] = 0x01;
				memcpy(m_ControlProto + 23, en_Auth, 8);

				WSAResetEvent(m_eventArray[TxEvent]);
				m_SendControlBuffer.len = LE_XNL_DEVICE_CONNECT_REQUEST_L;
				m_dwXnlConnectTimer = GetTickCount();
				rc = SendToLE(&m_masterAddr);
				switch (rc)
				{
				case 0:               //Tx completed successfully immediately.
				case WSA_IO_PENDING:  //Completion is pending
					m_WLStatus = WAITFOR_XNL_DEVICE_CONNECT_REQUEST_TX;
					//strSysLog = _T("发送0x70(XNL_DEVICE_CONNECT_REQUEST)");
					//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
					break;
				default:              //stay in this state and eventually retry.
					break;
				}
			}
			else
			{
				rc = IssueReadRequest();
			}

			// if ((LE_MASTER_PEER_REGISTRATION_RESPONSE
		} // if (FALSE == rc)
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;

	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlDeviceAuthTimer) > 100000UL)  // 100 seconds
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;

	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_XNL_DEVICE_CONNECT_REQUEST_TX(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_TxOverlapped,
			&m_dwByteSent,
			TRUE,
			&m_dwTxFlags);

		WSAResetEvent(m_eventArray[TxEvent]);
		if (TRUE != rc){
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("发送0x70(XNL_DEVICE_CONNECT_REQUEST)失败，重新注册");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
			break;
		}
		m_dwXnlConnectTimer = GetTickCount();
		m_WLStatus = WAITFOR_XNL_DEVICE_CONNECT_REPLY;
		//strSysLog = _T("等待0x70(XNL_DEVICE_CONNECT_REPLY)");
		//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		break;

	case RxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		m_WLStatus = WAITFOR_XNL_DEVICE_CONNECT_REPLY;

		break;

	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlConnectTimer) > 100000UL)  // 100 seconds
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;
	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_XNL_DEVICE_CONNECT_REPLY(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);
		WSAResetEvent(m_eventArray[RxEvent]);

		if (FALSE == rc) {//Reception failed
			rc = WSAGetLastError(); //For debugging if nothing else.

			//Restart listener using same buffer.
			rc = IssueReadRequest();
		}
		else
		{
			if (LE_XNL == *(m_CurrentRecvBuffer.buf) && (m_CurrentRecvBuffer.buf[8] == 0x07)
				&& (m_CurrentRecvBuffer.buf[19] == 0x01)){//received LE_XNL_CONNECT_REPLY

				rc = IssueReadRequest();
				m_dwXnlDeviceSysmapBrdcstTimer = GetTickCount();
				m_WLStatus = WAITFOR_XNL_DEVICE_SYSMAP_BROADCAST; //Advance state.
				//strSysLog = _T("等待0x70(XNL_DEVICE_SYSMAP_BROADCAST)");
				//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
				memcpy(xnlAddr, m_CurrentRecvBuffer.buf + 21, 2);
				memcpy(logicalAddr, m_CurrentRecvBuffer.buf + 23, 8);
			}
			else
			{
				rc = IssueReadRequest();
			}
		}
		break;

	case TickIndex: // quit
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			// send deregister request to master node
			Net_DeRegisterLE();
		}
		break;
	case  TIMEOUT:
		if ((GetTickCount() - m_dwXnlConnectTimer) > 100000UL)
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;
	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_XNL_DEVICE_SYSMAP_BROADCAST(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);

		WSAResetEvent(m_eventArray[RxEvent]);

		if (FALSE == rc) {//Reception failed
			rc = WSAGetLastError(); //For debugging if nothing else.

			//Restart listener using same buffer.
			rc = IssueReadRequest();
		}
		else
		{
			if ((LE_XNL
				== *(m_CurrentRecvBuffer.buf))
				&& (m_CurrentRecvBuffer.buf[8]
				== 0x0b)){
				rc = IssueReadRequest();
				memcpy(transationID, m_CurrentRecvBuffer.buf + 15, 2);
				flags = m_CurrentRecvBuffer.buf[10];
				WSAResetEvent(m_eventArray[TxEvent]); //Shouldn't be needed.
				m_ControlProto[0] = LE_XNL;
				m_ControlProto[5] = 0x00;
				m_ControlProto[6] = 0x0C;
				m_ControlProto[7] = 0x00;
				m_ControlProto[8] = 0x0C;
				m_ControlProto[9] = 0x01;
				m_ControlProto[10] = flags;
				m_ControlProto[11] = masterAddr[0];
				m_ControlProto[12] = masterAddr[1];
				m_ControlProto[13] = xnlAddr[0];
				m_ControlProto[14] = xnlAddr[1];
				m_ControlProto[15] = transationID[0];
				m_ControlProto[16] = transationID[1];
				m_ControlProto[17] = 0x00;
				m_ControlProto[18] = 0x00;
				m_SendControlBuffer.len = LE_XNL_DATA_MSG_ACK_L;
				m_dwXnlDeviceSysmapBrdcstTimer = GetTickCount();
				rc = SendToLE(&m_masterAddr);
				switch (rc)
				{
				case 0:               //Tx completed successfully immediately.
				case WSA_IO_PENDING:
					//Completion is pending
					m_WLStatus = WAITFOR_XNL_DATA_MSG_DEVICE_INIT_1;
					//strSysLog = _T("发送0x70(XNL_DATA_MSG_DEVICE_INIT_1)");
					//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
					break;
				default:              //stay in this state and eventually retry.
					break;
				}
			}
			else //Reception was not the expected LE_MASTER_PEER_REGISTRATION_RESPONSE
			{
				rc = IssueReadRequest();
			} // if ((LE_MASTER_PEER_REGISTRATION_RESPONSE
		} // if (FALSE == rc)
		break;

	case TickIndex: // quit
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			// send deregister request to master node
			//Net_DeRegisterLE();
		}
		break;

	case  TIMEOUT:
		if ((GetTickCount() - m_dwXnlDeviceSysmapBrdcstTimer) > 100000UL)
		{
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
			m_WLStatus = XNL_CONNECT;
		}
		break;
	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_1(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);

		WSAResetEvent(m_eventArray[RxEvent]);
		if (FALSE == rc) {//reception failed
			rc = WSAGetLastError(); //For debugging if nothing else.
			//Restart listener using same buffer.
			rc = IssueReadRequest();
			break;
		}
		else{//received something
			if ((LE_XNL == *(m_CurrentRecvBuffer.buf)) && (m_CurrentRecvBuffer.buf[8] == 0x0b)){
				rc = IssueReadRequest();
				memcpy(transationID, m_CurrentRecvBuffer.buf + 15, 2);
				flags = m_CurrentRecvBuffer.buf[10];
				WSAResetEvent(m_eventArray[TxEvent]); //Shouldn't be needed.
				m_ControlProto[0] = 0x70;
				m_ControlProto[5] = 0x00;
				m_ControlProto[6] = 0x0C;
				m_ControlProto[7] = 0x00;
				m_ControlProto[8] = 0x0C;
				m_ControlProto[9] = 0x01;
				m_ControlProto[10] = flags;
				m_ControlProto[11] = masterAddr[0];
				m_ControlProto[12] = masterAddr[1];
				m_ControlProto[13] = xnlAddr[0];
				m_ControlProto[14] = xnlAddr[1];
				m_ControlProto[15] = transationID[0];
				m_ControlProto[16] = transationID[1];
				m_ControlProto[17] = 0x00;
				m_ControlProto[18] = 0x00;
				m_SendControlBuffer.len = LE_XNL_DATA_MSG_ACK_L;
				m_dwXnlDeviceSysmapBrdcstTimer = GetTickCount();
				rc = SendToLE(&m_masterAddr);
				switch (rc)
				{
				case 0:               //Tx completed successfully immediately.
				case WSA_IO_PENDING:
					//Completion is pending
					m_WLStatus = WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2_TX;
					//strSysLog = _T("发送0x70(XNL_DATA_MSG_DEVICE_INIT_2)");
					//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
					break;
				default:              //stay in this state and eventually retry.
					break;
				}

			}
			else //Reception was not the expected LE_MASTER_PEER_REGISTRATION_RESPONSE
			{
				rc = IssueReadRequest();
			} // if ((LE_MASTER_PEER_REGISTRATION_RESPONSE
		} // if (FALSE == rc)
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;

	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlDeviceSysmapBrdcstTimer) > 100000UL)  // 100 seconds
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;

	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2_TX(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_TxOverlapped,
			&m_dwByteSent,
			TRUE,
			&m_dwTxFlags);
		WSAResetEvent(m_eventArray[TxEvent]);
		if (TRUE != rc){
			m_WLStatus = XNL_CONNECT;
			break;
		}
		m_dwXnlDeviceSysmapBrdcstTimer = GetTickCount();
		m_WLStatus = WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2;
		break;

	case RxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		m_WLStatus = WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2;
		break;

	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlDeviceSysmapBrdcstTimer) > 100000UL)  // 100 seconds
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;
	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_2(DWORD eventIndex)
{
	int rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);

		WSAResetEvent(m_eventArray[RxEvent]);
		if (FALSE == rc)
		{
			rc = WSAGetLastError();
			rc = IssueReadRequest();
			break;
		}
		else{
			rc = IssueReadRequest();
			if ((LE_XNL == m_CurrentRecvBuffer.buf[0]) && (m_CurrentRecvBuffer.buf[8] == 0x0b)){

				memcpy(transationID, m_CurrentRecvBuffer.buf + 15, 2);
				WSAResetEvent(m_eventArray[TxEvent]); //Shouldn't be needed.
				m_ControlProto[0] = 0x70;
				m_ControlProto[5] = 0x00;
				m_ControlProto[6] = 0x17;
				m_ControlProto[7] = 0x00;
				m_ControlProto[8] = 0x0B;
				m_ControlProto[9] = 0x01;
				m_ControlProto[10] = ++flags;
				m_ControlProto[11] = masterAddr[0];
				m_ControlProto[12] = masterAddr[1];
				m_ControlProto[13] = xnlAddr[0];
				m_ControlProto[14] = xnlAddr[1];
				m_ControlProto[15] = transationID[0];
				m_ControlProto[16] = transationID[1];
				m_ControlProto[17] = 0x00;
				m_ControlProto[18] = 0x0B;
				m_ControlProto[19] = (unsigned char)0xB4;
				m_ControlProto[20] = 0x00;
				m_ControlProto[21] = 0x00;
				m_ControlProto[22] = 0x00;
				m_ControlProto[23] = 0x00;
				m_ControlProto[24] = 0x02;
				m_ControlProto[25] = 0x00;
				m_ControlProto[26] = 0x0A;
				m_ControlProto[27] = 0x00;
				m_ControlProto[28] = 0x00;
				m_ControlProto[29] = 0x00;
				m_SendControlBuffer.len = LE_XNL_XCMP_DEVICE_INIT_L;
				m_dwXnlDeviceInitTimer = GetTickCount();
				rc = SendToLE(&m_masterAddr);
				switch (rc)
				{
				case 0:               //Tx completed successfully immediately.
				case WSA_IO_PENDING:
					//Completion is pending

					m_WLStatus = WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3_TX;
					//strSysLog = _T("发送0x70(XNL_DATA_MSG_DEVICE_INIT_3)");
					//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
					break;
				default:              //stay in this state and eventually retry.
					break;
				}
			}
			else //Reception was not the expected LE_MASTER_PEER_REGISTRATION_RESPONSE
			{
				rc = IssueReadRequest();
			} // if ((LE_MASTER_PEER_REGISTRATION_RESPONSE
		} // if (FALSE == rc)
		break;
	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;
	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlDeviceSysmapBrdcstTimer) > 100000UL)  // 100 seconds
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;

	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3_TX(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_TxOverlapped,
			&m_dwByteSent,
			TRUE,
			&m_dwTxFlags);

		WSAResetEvent(m_eventArray[TxEvent]);
		if (TRUE != rc){
			m_WLStatus = XNL_CONNECT;
			break;
		}
		m_dwXnlDeviceInitTimer = GetTickCount();
		m_WLStatus = WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3;
		break;
	case RxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		m_WLStatus = WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3;
		break;
	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlDeviceInitTimer) > 100000UL)  // 100 seconds
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;
	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;
	default:
		break;
	}
}

void CWLNet::Net_WAITFOR_XNL_DATA_MSG_DEVICE_INIT_3(DWORD eventIndex)
{
	int rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);

		WSAResetEvent(m_eventArray[RxEvent]);
		if (FALSE == rc)
		{
			rc = WSAGetLastError();
			rc = IssueReadRequest();
			break;
		}
		else{
			rc = IssueReadRequest();
			if ((LE_XNL == *(m_CurrentRecvBuffer.buf)) && (m_CurrentRecvBuffer.buf[8] == 0x0b)){
				memcpy(transationID, m_CurrentRecvBuffer.buf + 15, 2);
				flags = m_CurrentRecvBuffer.buf[10];
				WSAResetEvent(m_eventArray[TxEvent]); //Shouldn't be needed.
				m_ControlProto[0] = 0x70;
				m_ControlProto[5] = 0x00;
				m_ControlProto[6] = 0x0C;
				m_ControlProto[7] = 0x00;
				m_ControlProto[8] = 0x0C;
				m_ControlProto[9] = 0x01;
				m_ControlProto[10] = flags;
				m_ControlProto[11] = masterAddr[0];
				m_ControlProto[12] = masterAddr[1];
				m_ControlProto[13] = xnlAddr[0];
				m_ControlProto[14] = xnlAddr[1];
				m_ControlProto[15] = transationID[0];
				m_ControlProto[16] = transationID[1];
				m_ControlProto[17] = 0x00;
				m_ControlProto[18] = 0x00;
				m_SendControlBuffer.len = LE_XNL_DATA_MSG_ACK_L;
				m_dwXnlReadSerialTimer = GetTickCount();
				rc = SendToLE(&m_masterAddr);
				switch (rc)
				{
				case 0:               //Tx completed successfully immediately.
				case WSA_IO_PENDING:
					//Completion is pending
					m_WLStatus = WAITFOR_XNL_XCMP_READ_SERIAL;
					//strSysLog = _T("发送0x70(WAITFOR_XNL_XCMP_READ_SERIAL)");
					//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
					break;
				default:              //stay in this state and eventually retry.
					break;
				}
			}
			else //Reception was not the expected LE_MASTER_PEER_REGISTRATION_RESPONSE
			{
				rc = IssueReadRequest();
			} // if ((LE_MASTER_PEER_REGISTRATION_RESPONSE
		} // if (FALSE == rc)
		break;
	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;

	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlDeviceInitTimer) > 100000UL)  // 100 seconds
		{
			m_WLStatus = XNL_CONNECT;
			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
		}
		break;

	default:
		break;
	}
}

void CWLNet::Net_XNL_XCMP_READ_SERIAL(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);

		WSAResetEvent(m_eventArray[RxEvent]);
		if (FALSE == rc) {//reception failed
			rc = WSAGetLastError(); //For debugging if nothing else.
			//Restart listener using same buffer.
			rc = IssueReadRequest();
			break;
		}
		else{//received something

			rc = IssueReadRequest();
			memcpy(transationID, m_CurrentRecvBuffer.buf + 15, 2);
			flags = m_CurrentRecvBuffer.buf[10];
			WSAResetEvent(m_eventArray[TxEvent]); //Shouldn't be needed.

			m_ControlProto[0] = LE_XNL;
			m_ControlProto[5] = 0x00;
			m_ControlProto[6] = 0x0f;
			m_ControlProto[7] = LE_XNL_DATA_MSG_1;
			m_ControlProto[8] = LE_XNL_DATA_MSG_2;
			m_ControlProto[9] = 0x01;
			m_ControlProto[10] = ++flags;
			m_ControlProto[11] = masterAddr[0];
			m_ControlProto[12] = masterAddr[1];
			m_ControlProto[13] = xnlAddr[0];
			m_ControlProto[14] = xnlAddr[1];
			m_ControlProto[15] = transationID[0];
			m_ControlProto[16] = transationID[1];
			m_ControlProto[17] = 0x00;
			m_ControlProto[18] = 0x03;
			m_ControlProto[19] = 0x00;
			m_ControlProto[20] = 0x0E;
			m_ControlProto[21] = 0x0B;
			m_SendControlBuffer.len = LE_XNL_XCMP_READ_SERIAL_L;
			m_dwXnlReadSerialTimer = GetTickCount();
			rc = SendToLE(&m_masterAddr);
			switch (rc)
			{
			case 0:               //Tx completed successfully immediately.
			case WSA_IO_PENDING:
				//Completion is pending
				m_WLStatus = WAITFOR_XNL_XCMP_READ_SERIAL_RESULT;
				//strSysLog = _T("......获取master序列号......");
				//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
				break;
			default:              //stay in this state and eventually retry.
				break;
			}

		} // if (FALSE == rc)
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;

	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlReadSerialTimer) > 100000UL)  // 100 seconds
		{

			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
			m_WLStatus = XNL_CONNECT;
		}
		break;

	default:
		break;
	}
}

/************************************************************************/
/* 处理收到的数据
/************************************************************************/
void CWLNet::Net_WAITFOR_XNL_XCMP_READ_SERIAL_RESULT(DWORD eventIndex)
{
	int    rc;
	//CString strSysLog;
	switch (eventIndex)
	{
	case TxIndex:
		WSAResetEvent(m_eventArray[TxEvent]);
		break;
	case RxIndex:
		rc = WSAGetOverlappedResult(m_socket,
			&m_RxOverlapped,
			&m_dwByteRecevied,
			TRUE,
			&m_dwRxFlags);

		WSAResetEvent(m_eventArray[RxEvent]);
		if (FALSE == rc) {//reception failed
			rc = WSAGetLastError(); //For debugging if nothing else.
			//Restart listener using same buffer.
			rc = IssueReadRequest();
			break;
		}
		else{//received something
			rc = IssueReadRequest();
			if ((m_CurrentRecvBuffer.buf[0] == LE_XNL) && (m_CurrentRecvBuffer.buf[8] == 0X0B) && (m_CurrentRecvBuffer.buf[20] == 0X0E))
			{
				memcpy(m_MasterSerial, m_CurrentRecvBuffer.buf + 23, 11);


				//bool result = GetLic(m_MasterSerial);
				int licenseResult;
				WCHAR str[16] = { 0 };
				//if (!result)
				//{
				//	PostMessage(hwnd, WM_LICENSE, 0, (LPARAM)m_MasterSerial);

				//	strSysLog = _T("授权失败！");

				//	licenseResult = 0;
				//	str.Format(_T("%d"), licenseResult);
				//	CString m_connResult;
				//	m_connResult = _T("与主站点连接失败");
				//	::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)m_connResult.AllocSysString());
				//	WritePrivateProfileString(SYS_SECTION, CONNECT_RESULT, str, m_strSettingFilePath);
				//	isLicense = false;
				//}
				//else
				//{
				//strSysLog = _T("授权成功！");
				sprintf_s(m_reportMsg, "授权成功");
				sendLogToWindow();
				licenseResult = 1;
				swprintf_s(str, L"%d", licenseResult);
				//str.Format(_T("%d"), licenseResult);
				//CString m_connResult;
				//m_connResult= _T("与主站点连接成功");
				//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)m_connResult.AllocSysString());
				WritePrivateProfileString(SYS_SECTION, CONNECT_RESULT, str, m_strSettingFilePath);
				isLicense = true;
				//}
				//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
				//WritePrivateProfileString(SYS_SECTION, LICENSE_RESULT, str, m_strSettingFilePath);

				memcpy(transationID, m_CurrentRecvBuffer.buf + 15, 2);
				flags = m_CurrentRecvBuffer.buf[10];
				WSAResetEvent(m_eventArray[TxEvent]); //Shouldn't be needed.
				m_ControlProto[0] = 0x70;
				m_ControlProto[5] = 0x00;
				m_ControlProto[6] = 0x0C;
				m_ControlProto[7] = 0x00;
				m_ControlProto[8] = 0x0C;
				m_ControlProto[9] = 0x01;
				m_ControlProto[10] = flags;
				m_ControlProto[11] = masterAddr[0];
				m_ControlProto[12] = masterAddr[1];
				m_ControlProto[13] = xnlAddr[0];
				m_ControlProto[14] = xnlAddr[1];
				m_ControlProto[15] = transationID[0];
				m_ControlProto[16] = transationID[1];
				m_ControlProto[17] = 0x00;
				m_ControlProto[18] = 0x00;
				m_SendControlBuffer.len = LE_XNL_DATA_MSG_ACK_L;
				m_dwXnlReadSerialTimer = GetTickCount();
				rc = SendToLE(&m_masterAddr);
				switch (rc)
				{
				case 0:               //Tx completed successfully immediately.
				case WSA_IO_PENDING:
					//Completion is pending
					m_WLStatus = ALIVE;
					break;
				default:              //stay in this state and eventually retry.
					break;
				}
			}
			else
			{
				rc = IssueReadRequest();
			}

		} // if (FALSE == rc)
		break;

	case TickIndex:
		WSAResetEvent(m_eventArray[TickEvent]);
		if (m_bExit)
		{
			Net_DeRegisterLE();
		}
		break;

	case TIMEOUT:
		if ((GetTickCount() - m_dwXnlReadSerialTimer) > 100000UL)  // 100 seconds
		{

			//strSysLog = _T("注册超时，重新开始注册！");
			//::PostMessage(hwnd, WM_SYSLOG, 0, (LPARAM)strSysLog.AllocSysString());
			m_WLStatus = WAITFOR_XNL_XCMP_READ_SERIAL;
		}
		break;

	default:
		break;
	}
}

/************************************************************************/
/* 将peer map写入配置文件
/************************************************************************/
void CWLNet::WriteMapFile()
{
	//wcscat_s()
	wchar_t temp[256] = { 0 };
	wcscat_s(temp, PEER_SECTION);
	wcscat_s(temp, m_NumberStr);

	WritePrivateProfileString(temp, PEER_ADDR, m_addr, m_strSettingFilePath);
	WritePrivateProfileString(temp, PEER_PORT, m_port, m_strSettingFilePath);
	WritePrivateProfileString(temp, PEER_ID, m_peerID, m_strSettingFilePath);
}

/************************************************************************/
/*把当前与master的连接状态置为初始状态
/************************************************************************/
void CWLNet::RestartLE()
{
	m_WLStatus = STARTING;
}

/************************************************************************/
/* 获取本机的所有IP地址，并返回获得的IP地址数目
/************************************************************************/
DWORD CWLNet::GetIpList(void * iplist)
{
#ifdef WIN32
	WSADATA dat;
	WSAStartup(MAKEWORD(2, 2), &dat);
#endif
	//char ip[255][200];
	char host_name[255];
	//获取本地主机名称
	if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR) {
		return 0;
	}
	//printf("Host name is: %s\n", host_name);
	//从主机名数据库中得到对应的“主机”
	struct hostent *phe = gethostbyname(host_name);
	if (phe == 0)
	{
		return 0;
	}
	DWORD nIpCount = 0;
	//循环得出本地机器所有IP地址
	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		memcpy((char*)iplist + 15 * i, inet_ntoa(addr), 15);
		nIpCount++;
	}
	return nIpCount;
#ifdef WIN32
	WSACleanup();
#endif // WIN32

	return 0;
}

/************************************************************************/
/*判断地址是否存在于本机地址中
/************************************************************************/
bool CWLNet::FindLocalIP(WCHAR* strAddr)
{
	char iplist[255][15];
	DWORD ipcount = GetIpList(iplist);

	for (DWORD i = 0; i < ipcount; i++)
	{

		std::wstring temp = g_tool.ANSIToUnicode(iplist[i]);

		if (0 == wcscmp(temp.c_str(), strAddr))
		{
			return true;
		}
	}
	return false;
}

void CWLNet::SetLogPtr(PLogReport value)
{
	m_report = value;
	m_pEventLoger->SetLogPtr(m_report);
}

void CWLNet::sendLogToWindow()
{
	//SYSTEMTIME now = { 0 };
	//GetLocalTime(&now);
	//printf_s("%04u-%02u-%02u %02u:%02u:%02u %03u %s\n", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds, m_reportMsg);
	if (NULL != m_report)
	{
		m_report(m_reportMsg);
	}
}


//DWORD prev = 0;
void CWLNet::NetTx(bool Start)
{
	bool last = false;

	if (Start)
	{
		//初始化各个标识位
		//m_TxBurstType = VOICEHEADER0;
		m_TxSubCount = 0;

		////初始化数据
		//m_pCurrentBuildSendvoice = new IPSCVoiceTemplate;
		//memset(m_pCurrentBuildSendvoice, 0, sizeof(IPSCVoiceTemplate));

		////初始化New Call参数
		//InitialCallRecord();

		/************************************************************************/
		/* WL部分初始化                                                                     */
		/************************************************************************/
		/*初始化关键参数*/
		initCallParam();
		/*建立第一个Burst*/
		m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
		m_pSendVoicePackage = new SendVoicePackage;
		T_WL_PROTOCOL_21  networkData = { 0 };
		Build_T_WL_PROTOCOL_21(networkData, true);
		m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_BURST(m_pVoice, &networkData, false);
		m_pSendVoicePackage->pPackageData = m_pVoice;
		requireReadySendVoicesLock();
		m_sendVoices.push_back(m_pSendVoicePackage);
		releaseReadySendVoicesLock();
		/*建立缓冲*/
		for (int i = 0; i < (SEND_360MS_TIMES * 6) - 1; i++)
		{
			if (m_burstType == (char)BURST_T)
			{
				m_burstType = BURST_A;
			}
			m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
			m_pSendVoicePackage = new SendVoicePackage;
			T_WL_PROTOCOL_21  networkData = { 0 };
			Build_T_WL_PROTOCOL_21(networkData, false);
			networkData.RTPInformationField.MPT = BURST_START_RTP_MPT;
			m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_BURST(m_pVoice, &networkData, false);
			m_pSendVoicePackage->pPackageData = m_pVoice;
			requireReadySendVoicesLock();
			m_sendVoices.push_back(m_pSendVoicePackage);
			releaseReadySendVoicesLock();
		}
		m_pSendVoicePackage = new SendVoicePackage;
		m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
	}
	else
	{
		//{
		//	//Depending on what was sent LAST time:
		//	switch (m_TxBurstType)
		//	{
		//	case VOICEBURST_A:
		//		//m_TxBurstType += 1;
		//		m_TxBurstType = VOICEBURST_B;
		//		break;
		//	case VOICEBURST_B:
		//		m_TxBurstType = VOICEBURST_C;
		//		break;
		//	case VOICEBURST_C:
		//		m_TxBurstType = VOICEBURST_D;
		//		break;
		//	case VOICEBURST_D:
		//		m_TxBurstType = VOICEBURST_E;
		//		break;
		//	case VOICEBURST_E:
		//		//松开了PTT
		//		if (!g_bPTT)
		//		{
		//			printf_s("松开了PTT\r\n");
		//			last = TRUE;
		//		}
		//		m_TxBurstType = VOICEBURST_F;
		//		break;
		//	case VOICEBURST_F:
		//		//当前仍然在通话中
		//		if (g_bPTT)
		//		{    //Start transmitting next super frame.
		//			m_TxBurstType = VOICEBURST_A;
		//		}
		//		//本次通话结束
		//		else
		//		{
		//			//Last super frame has been transmitted.
		//			m_TxBurstType = VOICETERMINATOR;
		//			//g_bTX = FALSE;
		//		}
		//		break;
		//	case VOICEHEADER1:
		//		m_TxBurstType = VOICEHEADER2;
		//		break;
		//	case VOICEHEADER2:
		//		m_TxBurstType = VOICEBURST_A;
		//		break;
		//	case VOICEHEADER0:
		//		m_TxBurstType = VOICEHEADER1;
		//		break;
		//		//Shouldn't get here, but if does, do nothing.
		//	case VOICETERMINATOR:
		//	case PREAMBLECBSK:
		//	default:
		//		return;
		//	}
		//}

		////填充音频数据
		//FillIPSCFormat(m_pCurrentBuildSendvoice, m_TxBurstType, last);



		//push the data which build success to list 
		requireReadySendVoicesLock();
		//m_readySendVoices.push_back(m_pCurrentBuildSendvoice);
		m_sendVoices.push_back(m_pSendVoicePackage);
		releaseReadySendVoicesLock();

		//init the next data which will build
		//m_pCurrentBuildSendvoice = NULL;
		//m_pCurrentBuildSendvoice = new IPSCVoiceTemplate;
		//memset(m_pCurrentBuildSendvoice, 0, sizeof(IPSCVoiceTemplate));

		m_pSendVoicePackage = new SendVoicePackage;
		m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
		/*需要定时间隔发送音频数据*/
		if (!m_bIsSending)
		{
			SetCallStatus(CALL_ONGOING);
			NetWorker_SendCallByWL();
			timeSetEvent(SEND_VOICE_INTERVAL, 1, OneMilliSecondProc, (DWORD)this, TIME_PERIODIC);
			m_bIsSending = true;
		}
	}
}



// void CWLNet::InitialCallRecord()
// {
// 	int tmp_peer_id;
// 	int tmp_src_ID;
// 	int tmp_tgt_ID;
// 	unsigned __int16 tmp_crc;
// 	unsigned __int16 reorder_bits[8];
// 
// 	//set everything to 0
// 	memset(&myCallRecord, 0, sizeof(CallRecord));
// 
// 	// *************** Start to fill in the IPSC header ******************
// 	// set the Opcode, assume it is always a group call
// 	myCallRecord.CommonField.fld.CallOpcode = IPSC_GRP_VOICE_CALL;
// 
// 	//Set the peer ID
// 	tmp_peer_id = GetMyPeerID();
// 	myCallRecord.CommonField.fld.CallOriginatingPeerID[0] = tmp_peer_id >> 24;
// 	myCallRecord.CommonField.fld.CallOriginatingPeerID[1] = (tmp_peer_id >> 16) & 0x000000FF;
// 	myCallRecord.CommonField.fld.CallOriginatingPeerID[2] = (tmp_peer_id >> 8) & 0x000000FF;
// 	myCallRecord.CommonField.fld.CallOriginatingPeerID[3] = tmp_peer_id & 0x000000FF;
// 
// 	// the Call sequence number is initialized at the application initialization phase
// 
// 	// Increment the Call Sequence Number
// 	if (m_callSequenceNumber < MAXCALLSEQUENCE)
// 	{
// 		m_callSequenceNumber++;
// 	}
// 	else
// 	{
// 		m_callSequenceNumber = 0x00;
// 	}
// 
// 	myCallRecord.CommonField.fld.CallSequenceNumber = m_callSequenceNumber;
// 
// 
// 	tmp_src_ID = GetMyRadioID();
// 	//	myCallRecord.CommonField.fld.srcID[0] = tmp_src_ID >> 24;
// 	//	myCallRecord.CommonField.fld.srcID[1] = (tmp_src_ID >> 16) & 0x0000FF00;
// 	//	myCallRecord.CommonField.fld.srcID[2] = (tmp_src_ID >> 8) & 0x000000FF;
// 
// 	myCallRecord.CommonField.fld.srcID[0] = (tmp_src_ID >> 16) & 0x000000FF;
// 	myCallRecord.CommonField.fld.srcID[1] = (tmp_src_ID >> 8) & 0x000000FF;
// 	myCallRecord.CommonField.fld.srcID[2] = (tmp_src_ID)& 0x000000FF;
// 
// 	// set it to 0 now, we are not implementing floor control yet
// 	myCallRecord.CommonField.fld.CallFloorControlTag[0] = 0;
// 	myCallRecord.CommonField.fld.CallFloorControlTag[1] = 0;
// 	myCallRecord.CommonField.fld.CallFloorControlTag[2] = 0;
// 	myCallRecord.CommonField.fld.CallFloorControlTag[3] = 0;
// 
// 
// 	tmp_tgt_ID = GetSelectedTalkgroup();
// 	myCallRecord.CommonField.fld.tgtID[0] = (tmp_tgt_ID >> 16) & 0x000000FF;
// 	myCallRecord.CommonField.fld.tgtID[1] = (tmp_tgt_ID >> 8) & 0x000000FF;
// 	myCallRecord.CommonField.fld.tgtID[2] = (tmp_tgt_ID)& 0x000000FF;
// 
// 	myCallRecord.CommonField.fld.CallPriority = IPSC_HEADER_PRIORITY_VOICE;
// 
// 	// assume it is always non-secure call
// 	myCallRecord.CommonField.fld.CallControlInformation = 0;  // by default,it is set to non-secure, non-last packet,and the the slot number of 1 
// 
// 	if (GetTxSlot() == 1)
// 	{
// 		myCallRecord.CommonField.fld.CallControlInformation |= IPSC_HEADER_CALL_CONTROL_SLOT2;
// 	}
// 
// 	//*************** Start to fill in the RTP header ******************
// 
// 	// set the first byte of the RTP header
// 	myCallRecord.CommonField.fld.RTP_VPXCC = RTP_VPXCC_VALUE;
// 
// 	// set the second byte of the RTP header
// 	myCallRecord.CommonField.fld.RTP_MPT = RTP_NON_LAST_PAYLOAD_TYPE;
// 
// 
// 	myCallRecord.CommonField.fld.RTPSequence[0] = 0;             //Host order   10
// 	myCallRecord.CommonField.fld.RTPSequence[1] = 0;
// 
// 	myCallRecord.CommonField.fld.RTPTimeStamp[0] = 0;  //Host order
// 	myCallRecord.CommonField.fld.RTPTimeStamp[1] = 0;
// 	myCallRecord.CommonField.fld.RTPTimeStamp[2] = 0;
// 	myCallRecord.CommonField.fld.RTPTimeStamp[3] = 0;
// 
// 	myCallRecord.CommonField.fld.RTPSSRC[0] = 0;  //Always 0
// 	myCallRecord.CommonField.fld.RTPSSRC[1] = 0;
// 	myCallRecord.CommonField.fld.RTPSSRC[2] = 0;
// 	myCallRecord.CommonField.fld.RTPSSRC[3] = 0;
// 
// 	//*************  Start to fill in the RTP Payload header ******************
// 	// init the RepeaterBurstDataType, the value will be filled based on the burst type
// 	myCallRecord.CommonField.fld.RepeaterBurstDataType = 0;
// 
// 	// ************  initialize the timestampe
// 	myCallRecord.SampleTimeStamp = 100;			// just assign an non-zero number to begin with
// 
// 	// ************  create the LC msg
// 	//	assume the call is non-emergency, non privacy, group call, non Open Voice Call Mode, and priority level of lowest (0)
// 	myCallRecord.LCField.fld.PRFLCO = 0;			// set the field to 0, so that the bit 7 (PF) and bit 6(reserved) are set to 0
// 	myCallRecord.LCField.fld.PRFLCO = myCallRecord.LCField.fld.PRFLCO | (const unsigned __int8)(LC_STANDARD_FID >> 2);			// set the FID to standard one
// 	myCallRecord.LCField.fld.ServiceOptions = 0;
// 	memcpy(myCallRecord.LCField.fld.TargetAddress, myCallRecord.CommonField.fld.tgtID, 3);
// 	memcpy(myCallRecord.LCField.fld.SourceAddress, myCallRecord.CommonField.fld.srcID, 3);
// 
// 	// calculate the 5-bit embedded LC CRC
// 	tmp_crc = embeddedLCCheckSumGen(myCallRecord.LCField.All);
// 
// 	//tmp_crc = tmp_crc << 11;				//Change the right justified data to left justified
// 	myCallRecord.EmbeddedLCCRC = tmp_crc;   //the most significant 5 bits contain the crc
// 
// 	// calculate the EmLC hard bits for burst B-E, the output is un-interleaved hardbits
// 	//SQZ start ?????? 2/25/2010
// 	memset(reorder_bits, 0, 8);
// 	//SQZ end ?????? 2/25/2010
// 	FECEncoding(&myCallRecord, reorder_bits);
// 
// 	// interleave the 16-byte integer into 8-byte order
// 	Interleave(reorder_bits, &myCallRecord);
// 	return;
// }

unsigned __int32 CWLNet::GetMyPeerID(void)
{
	return CONFIG_LOCAL_PEER_ID;
}

unsigned __int32 CWLNet::GetMyRadioID(void)
{
	return m_dwMyRadioID;
}

//unsigned __int32 CWLNet::GetSelectedTalkgroup(void)
//{
//	return m_dwMyRadioGroup;
//}

// int CWLNet::GetTxSlot(void)
// {
// 	return m_TxSlot;
// }

unsigned __int16 CWLNet::embeddedLCCheckSumGen(unsigned __int8 * inputPtr)
{
	unsigned __int8 temp;
	unsigned __int16 mask = 0x00FF;
	unsigned __int16 generatedCrc = 0;
	unsigned __int16 i = 0;

	for (i = 0; i<9; ++i)
	{
		temp = inputPtr[i];
		generatedCrc += temp;
	}

	while (generatedCrc>31)					//Total result of temporary mod with value 31
	{
		generatedCrc -= 31;
	}

	return generatedCrc;
}

void CWLNet::FECEncoding(CallRecord * myCallRecordInput, unsigned __int16 *reorder_bits)
{
	unsigned __int16 tmp;
	int num_of_ones = 0;
	unsigned __int16 tmp_parity = 0;
	unsigned __int16 mask = 0x8000; //start with the most significant bit
	int i;


	unsigned __int16 hamming_bits[7]; // the least 5 significant bits store the hamming code

	// reorder the 72-bit LC and 5-bit checksum into the following format
	// b71 - b64,    b63 - b61 0 0 0 0 0
	// b60 - b53,    b52 - b50 0 0 0 0 0
	// b49 - b42, b41 - b40 c4 0 0 0 0 0
	// b39 - b32, b31 - b30 c3 0 0 0 0 0
	// b29 - b22, b21 - b20 c2 0 0 0 0 0
	// b19 - b12, b11 - b10 c1 0 0 0 0 0
	// b9  - b2,  b1  - b0  c0 0 0 0 0 0

	///////////////////////////////////
	reorder_bits[0] = myCallRecordInput->LCField.All[0]; //fill in b71 - b64
	reorder_bits[0] <<= 8; // move the most significant bits
	reorder_bits[0] |= myCallRecordInput->LCField.All[1] & 0xE0; // fill in b63 -b61 

	//////////////////////////////////
	reorder_bits[1] = myCallRecordInput->LCField.All[1] && 0x1F; //fill in b60 - b56 at the least significant 5-bit
	reorder_bits[1] <<= 11; // move them to the most significant bits
	tmp = myCallRecordInput->LCField.All[2] & 0xFC;
	tmp <<= 3;
	reorder_bits[1] |= tmp; // fill in b55 - b50 

	//////////////////////////////////
	tmp = myCallRecordInput->LCField.All[2] & 0x03;
	tmp <<= 14;
	reorder_bits[2] = tmp; //fill in b49 - b48 and move them to the most significant bit

	tmp = myCallRecordInput->LCField.All[3];
	tmp <<= 6; // fill in b47 - b40
	reorder_bits[2] |= tmp;

	tmp = myCallRecordInput->EmbeddedLCCRC & 0x0010; // get c4
	tmp <<= 1;
	reorder_bits[2] |= tmp; //fill in c4 bit

	//////////////////////////////////
	reorder_bits[3] = myCallRecordInput->LCField.All[4];
	reorder_bits[3] <<= 8; //fil in b39 - b32

	tmp = myCallRecordInput->LCField.All[5] & 0xC0;
	reorder_bits[3] |= tmp; // fill in b31 -b30

	tmp = myCallRecordInput->EmbeddedLCCRC & 0x0008; // get c3
	tmp <<= 2;
	reorder_bits[3] |= tmp; // fill in c3

	//////////////////////////////////
	reorder_bits[4] = myCallRecordInput->LCField.All[5] & 0x3F;
	reorder_bits[4] <<= 10; // fill in b29 - b24

	tmp = myCallRecordInput->LCField.All[6] & 0xF0;
	tmp <<= 2;
	reorder_bits[4] |= tmp; //fill in b23 - b20
	tmp = myCallRecordInput->EmbeddedLCCRC & 0x0004; // get c2
	tmp <<= 3;
	reorder_bits[4] |= tmp; // fill in c2

	//////////////////////////////////
	reorder_bits[5] = myCallRecordInput->LCField.All[6] & 0x0F;
	reorder_bits[5] <<= 12; // fill in b19 -b16
	tmp = myCallRecordInput->LCField.All[7] & 0xFC;
	tmp <<= 4;
	reorder_bits[5] |= tmp; // fill in bb15 -b10

	tmp = myCallRecordInput->EmbeddedLCCRC & 0x0002; // get c1
	tmp <<= 4;
	reorder_bits[5] |= tmp; // fill in c1

	//////////////////////////////////
	reorder_bits[6] = myCallRecordInput->LCField.All[7] & 0x03;
	reorder_bits[6] <<= 14;  // fill in b9 -b8

	tmp = myCallRecordInput->LCField.All[8];
	tmp <<= 6;
	reorder_bits[6] |= tmp; //fill in b7 -b0

	tmp = myCallRecordInput->EmbeddedLCCRC & 0x0001; // get c0
	tmp <<= 5;
	reorder_bits[6] |= tmp; // fill in c0

	//generate hamming code for each row
	for (i = 0; i < 7; i++)
	{
		//generate hamming code for each row
		hamming_bits[i] = hamming(reorder_bits[i]);

		//append the hamming code
		reorder_bits[i] |= (hamming_bits[i] & 0x1F);
	}

	//calculate the parity check bits in reorder_bits[7]
	//calculate the parity check bits in reorder_bits[7]
	//SQZ start ?????? 2/25/2010
	tmp_parity = reorder_bits[0];
	for (i = 1; i < 7; i++)
	{
		tmp_parity ^= reorder_bits[i];
	}
	reorder_bits[7] = tmp_parity;
}

unsigned __int16 CWLNet::hamming(unsigned __int16 inputRow)
{
	int i;
	unsigned __int16 mask = 0x8000;
	unsigned __int16 result = 0;

	for (i = 0; i < 11; i++)
	{
		if ((inputRow & mask) != 0)
		{
			// only the non-zero row will contribute for the final result. and XOR is equavlent to the bit addition operation
			result ^= HammingMatrixGenerator[i]; // the matrix only contain the last 5-bit of the full MatrixGenerator for easy calculation
		}
		mask >>= 1;
	}

	// the last 5 bit of inputRow are 0, and the result is initialized as 0, the twice XOR operations againist 0 restore the original value
	result ^= inputRow;
	return result;  // the least 5 significant bits are the hamming code
}

void CWLNet::Interleave(unsigned __int16 *reorder_bits, CallRecord * myCallRecordInput)
{
	int row = 0;
	int col = 0;
	unsigned __int16 row_mask = 0x8000;
	unsigned __int8 byte_mask = 0x80;

	// initialize the hard bit array to 0 in myCallRecord
	memset(myCallRecordInput->EmbeddedHardBitLC.All, 0, 16);

	for (col = 0; col < 16; col++)
	{
		byte_mask = 0x80;
		for (row = 0; row < 8; row++)
		{
			if ((reorder_bits[row] & row_mask) != 0)
			{
				// set this bit to 1
				myCallRecordInput->EmbeddedHardBitLC.All[col] |= byte_mask;
			}
			byte_mask >>= 1;
		}
		row_mask >>= 1;
	}
}

//void CWLNet::FillIPSCFormat(IPSCVoiceTemplate* pBuffer, IPSCBurstType burstType, bool lastBurst)
//{
//	// check if this is the last packet
//	if (lastBurst == TRUE)
//	{
//		myCallRecord.CommonField.fld.CallControlInformation |= IPSC_HEADER_CALL_CONTROL_LAST_PACKET;
//
//		// set the second byte of the RTP header mean this is last pack
//		myCallRecord.CommonField.fld.RTP_MPT = RTP_LAST_PAYLOAD_TYPE;
//	}
//
//	//check if the RTP sequence number and Timestamp have to be reset
//	//	if (restartSequence == TRUE)
//	//	{
//	//		myCallRecord.CommonField.fld.RTPSequence[0] = 0;             //Host order   10
//	//		myCallRecord.CommonField.fld.RTPSequence[1] = 0;
//
//	//		myCallRecord.SampleTimeStamp = 100;			// just assign an non-zero number to begin with
//
//	//	}
//	//	else
//	//	{
//	// increment the RTP Sequence number;
//	myCallRecord.CommonField.fld.RTPSequence[1]++;
//	if (myCallRecord.CommonField.fld.RTPSequence[1] == 0)
//	{
//		myCallRecord.CommonField.fld.RTPSequence[0]++;
//	}
//
//	// increment the timestamp
//	myCallRecord.SampleTimeStamp += RTP_TIMESTAMP_INTERVAL;    // each packet will contain 60ms audio frame.
//
//	//	}
//
//	myCallRecord.CommonField.fld.RTPTimeStamp[0] = (unsigned __int8)(myCallRecord.SampleTimeStamp >> 24) & 0x000000FF;  //Host order
//	myCallRecord.CommonField.fld.RTPTimeStamp[1] = (unsigned __int8)(myCallRecord.SampleTimeStamp >> 16) & 0x000000FF;
//	myCallRecord.CommonField.fld.RTPTimeStamp[2] = (unsigned __int8)(myCallRecord.SampleTimeStamp >> 8) & 0x000000FF;
//	myCallRecord.CommonField.fld.RTPTimeStamp[3] = (unsigned __int8)myCallRecord.SampleTimeStamp & 0x000000FF;
//
//
//
//	// fill in the common header parts first which includes the IPSC, RTP headers
//	memcpy(pBuffer, &myCallRecord.CommonField.fld, 35);
//
//	// fill in the RepeaterBurstType based on the input
//	switch (burstType){
//	case VOICEBURST_A:
//		FillBurstA(pBuffer);
//		break;
//	case VOICEBURST_B:
//		FillBurstBCDF(pBuffer, burstType);
//		break;
//	case VOICEBURST_C:
//		FillBurstBCDF(pBuffer, burstType);
//		break;
//	case VOICEBURST_D:
//		FillBurstBCDF(pBuffer, burstType);
//		break;
//	case VOICEBURST_E:
//		FillBurstE(pBuffer);
//		break;
//	case VOICEBURST_F:
//		FillBurstBCDF(pBuffer, burstType);
//		break;
//	case VOICEHEADER1:
//	case VOICEHEADER2:
//	case VOICETERMINATOR:
//		FillVoiceHeaderTerminator(pBuffer, burstType);
//		break;
//	case PREAMBLECBSK:
//	default:
//		break;
//	}
//}

//The RTP payload format for Burst A is defined as below.All three voice frames in Burst
//A are sent in one RTP message.
// void CWLNet::FillBurstA(IPSCVoiceTemplate* pBuffer)
// {
// 	//the actual length of the burst A in the buffer
// 	pBuffer->BurstA.fld.Length = SIZEOFVOICEBURSTA;
// 
// 	// set the slot number to 0 first
// 	pBuffer->BurstA.fld.RepeaterBurstDataType = 0x00;
// 
// 	if (GetTxSlot() == 1)
// 	{
// 		//if slot number 2 is used, set bit 7 to 1
// 		pBuffer->BurstA.fld.RepeaterBurstDataType |= 0x80;
// 	}
// 	pBuffer->BurstA.fld.RepeaterBurstDataType |= DATA_TYPE_VOICE;
// 
// 	//fill in lengthinbytes
// 	pBuffer->BurstA.fld.LengthInBytes = RTP_PAYLOAD_BURST_A_LEN_IN_BYTES;
// 
// 	//fill in control field for voice
// 	//Embedded-LC Parity, Sync,  NULL LC, 72 Bit EMB LC, Ignore Sig Bits,  EMB,  Emb_LC Hard Bits, Bad Voice Burst
// 
// 	pBuffer->BurstA.fld.ESNEIEHB = 0; // reset all the bits to 0, in most case 0 means success
// 	pBuffer->BurstA.fld.ESNEIEHB |= RTP_PAYLOAD_VOICE_CONTROL_FIELD_SYNC_DETECTED; // SET SYNC bit
// 
// 	//the Voice[19] bytes will be filled by the other function calls
// }


//Voice Bursts B, C and D have the same RTP packet format.All three voice frames for
//each burst are sent in one RTP message with 7 bits decoded EMB and 32 bits of raw
//LC.
//All three voice frames in Burst F are sent in one RTP message with 7 bits decoded EMB
//and 43 crypto parameter bits.
//void CWLNet::FillBurstBCDF(IPSCVoiceTemplate* pBuffer, IPSCBurstType burstType)
//{
//	int i;
//
//	//the actual length of the burst B,C,D,F in the buffer
//	pBuffer->BurstBCDF.fld.Length = SIZEOFVOICEBURSTBCDF;
//
//	// set the slot number to 0 first
//	pBuffer->BurstBCDF.fld.RepeaterBurstDataType = 0x00;
//
//	if (GetTxSlot() == 1)
//	{
//		//if slot number 2 is used, set bit 7 to 1
//		pBuffer->BurstBCDF.fld.RepeaterBurstDataType |= 0x80;
//	}
//	pBuffer->BurstBCDF.fld.RepeaterBurstDataType |= DATA_TYPE_VOICE;
//
//	//fill in lengthinbytes
//	pBuffer->BurstBCDF.fld.LengthInBytes = RTP_PAYLOAD_BURST_BCDF_LEN_IN_BYTES;
//
//	//fill in control field for voice
//	//Embedded-LC Parity, Sync,  NULL LC, 72 Bit EMB LC, Ignore Sig Bits,  EMB,  Emb_LC Hard Bits, Bad Voice Burst
//
//	pBuffer->BurstBCDF.fld.ESNEIEHB = 0; // reset all the bits to 0, in most case 0 means success, the 7-bit EMB field will be set by the repeater
//	pBuffer->BurstBCDF.fld.ESNEIEHB |= RTP_PAYLOAD_VOICE_CONTROL_FIELD_EMB_LC_HARDBITS_PRESENT; // SET EMB LC hard bit present
//	//csz013 may 10
//	pBuffer->BurstBCDF.fld.ESNEIEHB |= RTP_PAYLOAD_VOICE_CONTROL_FIELD_EMB_PRESENT; // set the EMB field present, the actually emb field is set to 0
//
//
//	//the Voice[19] bytes will be filled by the other function calls
//
//	//fill the EMB hard bits
//	if (burstType == VOICEBURST_B)
//	{
//		for (i = 0; i < 4; i++)
//		{
//			pBuffer->BurstBCDF.fld.HardBits[i] = myCallRecord.EmbeddedHardBitLC.fld.BurstB[i];
//		}
//	}
//
//	if (burstType == VOICEBURST_C)
//	{
//		for (i = 0; i < 4; i++)
//		{
//			pBuffer->BurstBCDF.fld.HardBits[i] = myCallRecord.EmbeddedHardBitLC.fld.BurstC[i];
//		}
//	}
//
//	if (burstType == VOICEBURST_D)
//	{
//		for (i = 0; i < 4; i++)
//		{
//			pBuffer->BurstBCDF.fld.HardBits[i] = myCallRecord.EmbeddedHardBitLC.fld.BurstD[i];
//		}
//	}
//
//	if (burstType == VOICEBURST_F)
//	{
//		for (i = 0; i < 4; i++)
//		{
//			pBuffer->BurstBCDF.fld.HardBits[i] = 0;
//		}
//	}
//
//	pBuffer->BurstBCDF.fld.EMB7 = 0xA0;
//
//	//the EMB field does not have to be filled
//
//	return;
//}

//All three voice frames in Burst E are sent in one RTP message with 7 bits decoded
//EMB, 32 bits of raw LC and 72 bits of decoded LC.The payload length is 35 bytes.
// void CWLNet::FillBurstE(IPSCVoiceTemplate* pBuffer)
// {
// 	int i;
// 
// 	//the actual length of the burst E in the buffer
// 	pBuffer->BurstE.fld.Length = SIZEOFVOICEBURSTE;
// 
// 	// set the slot number to 0 first
// 	pBuffer->BurstE.fld.RepeaterBurstDataType = 0x00;
// 
// 	if (GetTxSlot() == 1)
// 	{
// 		//if slot number 2 is used, set bit 7 to 1
// 		pBuffer->BurstE.fld.RepeaterBurstDataType |= 0x80;
// 	}
// 	pBuffer->BurstE.fld.RepeaterBurstDataType |= DATA_TYPE_VOICE;
// 
// 	//fill in lengthinbytes
// 	pBuffer->BurstBCDF.fld.LengthInBytes = RTP_PAYLOAD_BURST_E_LEN_IN_BYTES;
// 
// 	//fill in control field for voice
// 	//Embedded-LC Parity, Sync,  NULL LC, 72 Bit EMB LC, Ignore Sig Bits,  EMB,  Emb_LC Hard Bits, Bad Voice Burst
// 
// 	pBuffer->BurstE.fld.ESNEIEHB = 0; // reset all the bits to 0, in most case 0 means success, the 7-bit EMB field will be set by the repeater
// 	pBuffer->BurstE.fld.ESNEIEHB |= RTP_PAYLOAD_VOICE_CONTROL_FIELD_EMB_LC_HARDBITS_PRESENT; // SET EMB LC hard bit present
// 	pBuffer->BurstE.fld.ESNEIEHB |= RTP_PAYLOAD_VOICE_CONTROL_FIELD_72_EMB_LC_PRESENT; // set 72 bit EMB LC present
// 
// 	////test code
// 	//pBuffer->BurstE.fld.ESNEIEHB |= RTP_PAYLOAD_VOICE_CONTROL_FIELD_EMB_PRESENT; // set the EMB field present, the actually emb field is set to 0
// 
// 
// 	//the Voice[19] bytes will be filled by the other function calls
// 
// 	//fill the EMB hard bits
// 	for (i = 0; i < 4; i++)
// 	{
// 		pBuffer->BurstE.fld.HardBits[i] = myCallRecord.EmbeddedHardBitLC.fld.BurstE[i];
// 	}
// 
// 	//fill the 72 bit LC
// 	for (i = 0; i < 9; i++)
// 	{
// 		pBuffer->BurstE.fld.LC_copy[i] = myCallRecord.LCField.All[i];
// 	}
// 
// 	pBuffer->BurstE.fld.EMB7 = 0xA0;
// 
// 	//the EMB field does not have to be filled
// 
// 	return;
// }

//The Voice Header is used to initiate a voice call.And the Voice Terminator is used to
// terminate a voice call.Both the Voice Header and Voice Terminator are transmitted as a
// data burst and have the same RTP payload format shown as below.The field of
// RepeaterBurstDataType in the RTP packet differentiates the Voice Header and the Voice
// Terminator.
// void CWLNet::FillVoiceHeaderTerminator(IPSCVoiceTemplate* pBuffer, IPSCBurstType burstType)
// {
// 	pBuffer->Control.fld.Length = SIZEOFVOICEHEADER; //the actual length of the voice header and terminator in the buffer
// 
// 	if (burstType == VOICEHEADER1 || burstType == VOICEHEADER2)
// 	{
// 		pBuffer->Control.fld.RepeaterBurstDataType = DATA_TYPE_VOICE_HEADER;
// 	}
// 	else if (burstType == VOICETERMINATOR)
// 	{
// 		pBuffer->Control.fld.RepeaterBurstDataType = DATA_TYPE_VOICE_TERMINATOR;
// 	}
// 
// 	// set the slot number to 0 first, set the RSSI status to above threshold, set the LC, RS and CRC parity to 0 (success)
// 	pBuffer->Control.fld.RepeaterBurstDataStatus = 0x00;
// 
// 	if (GetTxSlot() == 1)
// 	{
// 		//if slot number 2 is used, set bit 7 to 1
// 		pBuffer->Control.fld.RepeaterBurstDataStatus |= 0x80;
// 	}
// 
// 	pBuffer->Control.fld.LengthInWords[0] = 0x00;
// 	pBuffer->Control.fld.LengthInWords[1] = RTP_PAYLOAD_VOICE_HEADER_OR_TERMINATOR_LENGTH_IN_WORD_NO_RSSI;
// 
// 	//The RSSI is set to not present (0) at bit 7, Burst Source at bit -4 is set to present (1) so that the repeater will calculate the CRC for the LC
// 	pBuffer->Control.fld.RepeterBurstEmbSigBits[0] = RTP_PAYLOADD_EMBSIGBIT_FIELD_BURST_SOURCE_REPEATER;
// 
// 	////test code
// 	//pBuffer->Control.fld.RepeterBurstEmbSigBits[0] = (unsigned __int8)0x80;
// 
// 	//The Sync hard bits at bit 6 is set to no error (0), slot type at bit 3 is set to present (1), sync at bit 1 and 0 are set to DATA sync (%10)
// 	pBuffer->Control.fld.RepeterBurstEmbSigBits[1] = RTP_PAYLOADD_EMBSIGBIT_FIELD_SLOT_TYPE_PRESENT | RTP_PAYLOADD_EMBSIGBIT_FIELD_DATA_SYNC_DETECTED;
// 
// 
// 	//fill in RepeaterBurstDataSize in bits
// 	pBuffer->Control.fld.RepeterBurstDataSize[0] = 0x00;
// 	pBuffer->Control.fld.RepeterBurstDataSize[1] = (unsigned __int8)VOICEHEADER_REPEATER_BURST_DATA_SIZE;
// 
// 
// 	//fill in the LC bytes
// 
// 	pBuffer->Control.fld.LC_PFFLCO = myCallRecord.LCField.fld.PRFLCO;
// 	pBuffer->Control.fld.LC_FID = myCallRecord.LCField.fld.FID;
// 	pBuffer->Control.fld.LC_ServiceOptions = myCallRecord.LCField.fld.ServiceOptions;
// 	pBuffer->Control.fld.LC_tgtID[0] = myCallRecord.LCField.fld.TargetAddress[0];
// 	pBuffer->Control.fld.LC_tgtID[1] = myCallRecord.LCField.fld.TargetAddress[1];
// 	pBuffer->Control.fld.LC_tgtID[2] = myCallRecord.LCField.fld.TargetAddress[2];
// 
// 	pBuffer->Control.fld.LC_srcID[0] = myCallRecord.LCField.fld.SourceAddress[0];
// 	pBuffer->Control.fld.LC_srcID[1] = myCallRecord.LCField.fld.SourceAddress[1];
// 	pBuffer->Control.fld.LC_srcID[2] = myCallRecord.LCField.fld.SourceAddress[2];
// 
// 	// the repeater will do the CRC calculation, therefore we can just fill in 0 here
// 	pBuffer->Control.fld.LC_CRC[0] = 0;
// 	pBuffer->Control.fld.LC_CRC[1] = 0;
// 	pBuffer->Control.fld.LC_CRC[2] = 0;
// 
// 	pBuffer->Control.fld.reserved1 = 0;
// 
// 	//fill in slot byte at the least significant 4 bits. The repeater will fill in the color code
// 	if (burstType == VOICEHEADER1 || burstType == VOICEHEADER2)
// 	{
// 		pBuffer->Control.fld.SlotType = SLOT_TYPE_VOICE_HEADER;
// 	}
// 	else if (burstType == VOICETERMINATOR)
// 	{
// 		pBuffer->Control.fld.SlotType = SLOT_TYPE_VOICE_TERMINATOR;
// 	}
// 
// 	return;
// }

void CWLNet::requireReadySendVoicesLock()
{
	m_readySendVoicesLock.lock();
}

void CWLNet::releaseReadySendVoicesLock()
{
	m_readySendVoicesLock.unlock();
}

void CWLNet::NetStuffTxVoice(unsigned char* pVoiceBytes)
{
	/*将当前AMBE数据写入对应的语音记录*/
	for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
	{
		/*存在则记录语音*/
		if ((*i)->srcId == CONFIG_LOCAL_RADIO_ID &&
			(*i)->tagetId == g_targetId &&
			(*i)->callId == g_callId)
		{
			(*i)->WriteVoiceFrame((char*)pVoiceBytes, 7, true);
			break;
		}
	}

	switch (m_TxSubCount)
	{
	case 0:
	{
			  m_pVoice[45] = *(pVoiceBytes);
			  m_pVoice[46] = *(pVoiceBytes + 1);
			  m_pVoice[47] = *(pVoiceBytes + 2);
			  m_pVoice[48] = *(pVoiceBytes + 3);
			  m_pVoice[49] = *(pVoiceBytes + 4);
			  m_pVoice[50] = *(pVoiceBytes + 5);
			  m_pVoice[51] = (*(pVoiceBytes + 6)) & 0x80;

			  m_TxSubCount = 1;
	}
		break;
	case 1:  //Note: Check that right shift shifts in zeros.
	{

				 m_pVoice[51] |= (*(pVoiceBytes)) >> 2;
				 m_pVoice[52] = ((*(pVoiceBytes)) << 6) | ((*(pVoiceBytes + 1)) >> 2);
				 m_pVoice[53] = ((*(pVoiceBytes + 1)) << 6) | ((*(pVoiceBytes + 2)) >> 2);
				 m_pVoice[54] = ((*(pVoiceBytes + 2)) << 6) | ((*(pVoiceBytes + 3)) >> 2);
				 m_pVoice[55] = ((*(pVoiceBytes + 3)) << 6) | ((*(pVoiceBytes + 4)) >> 2);
				 m_pVoice[56] = ((*(pVoiceBytes + 4)) << 6) | ((*(pVoiceBytes + 5)) >> 2);
				 m_pVoice[57] = (((*(pVoiceBytes + 5)) << 6) | ((*(pVoiceBytes + 6)) >> 2)) & 0xE0;

				 m_TxSubCount = 2;
	}
		break;
	case 2:
	{
			  m_pVoice[57] |= (*(pVoiceBytes)) >> 4;
			  m_pVoice[58] = ((*(pVoiceBytes)) << 4) | ((*(pVoiceBytes + 1)) >> 4);
			  m_pVoice[59] = ((*(pVoiceBytes + 1)) << 4) | ((*(pVoiceBytes + 2)) >> 4);
			  m_pVoice[60] = ((*(pVoiceBytes + 2)) << 4) | ((*(pVoiceBytes + 3)) >> 4);
			  m_pVoice[61] = ((*(pVoiceBytes + 3)) << 4) | ((*(pVoiceBytes + 4)) >> 4);
			  m_pVoice[62] = ((*(pVoiceBytes + 4)) << 4) | ((*(pVoiceBytes + 5)) >> 4);
			  m_pVoice[63] = (((*(pVoiceBytes + 5)) << 4) | ((*(pVoiceBytes + 6)) >> 4)) & 0xF8;

			  T_WL_PROTOCOL_21 networkData = { 0 };
			  Build_T_WL_PROTOCOL_21(networkData, false);
			  m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_BURST(m_pVoice, &networkData, false);
			  m_pSendVoicePackage->pPackageData = m_pVoice;
			  m_TxSubCount = 0;
			  NetTx(FALSE);
	}
		break;
	}
}

void CWLNet::FILL_AMBE_FRAME(char* pVoiceBytes, char* pSendVoice, int txSubCount)
{
	/*将当前AMBE数据写入对应的语音记录*/
	for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
	{
		/*存在则记录语音*/
		if ((*i)->srcId == CONFIG_LOCAL_RADIO_ID &&
			(*i)->tagetId == g_targetId &&
			(*i)->callId == g_callId)
		{
			(*i)->WriteVoiceFrame((char*)pVoiceBytes, 7, true);
			break;
		}
	}
	switch (txSubCount)
	{
	case 0:
		pSendVoice[0] = *(pVoiceBytes);
		pSendVoice[1] = *(pVoiceBytes + 1);
		pSendVoice[2] = *(pVoiceBytes + 2);
		pSendVoice[3] = *(pVoiceBytes + 3);
		pSendVoice[4] = *(pVoiceBytes + 4);
		pSendVoice[5] = *(pVoiceBytes + 5);
		pSendVoice[6] = (*(pVoiceBytes + 6)) & 0x80;
		break;
	case 1:  //Note: Check that right shift shifts in zeros.
		pSendVoice[6] |= ((*(pVoiceBytes)) >> 2 & 0x3f);//此处位移补1
		pSendVoice[7] = ((*(pVoiceBytes)) << 6 & 0xc0) | ((*(pVoiceBytes + 1)) >> 2 & 0x3f);
		pSendVoice[8] = ((*(pVoiceBytes + 1)) << 6 & 0xc0) | ((*(pVoiceBytes + 2)) >> 2 & 0x3f);
		pSendVoice[9] = ((*(pVoiceBytes + 2)) << 6 & 0xc0) | ((*(pVoiceBytes + 3)) >> 2 & 0x3f);
		pSendVoice[10] = ((*(pVoiceBytes + 3)) << 6 & 0xc0) | ((*(pVoiceBytes + 4)) >> 2 & 0x3f);
		pSendVoice[11] = ((*(pVoiceBytes + 4)) << 6 & 0xc0) | ((*(pVoiceBytes + 5)) >> 2 & 0x3f);
		pSendVoice[12] = (((*(pVoiceBytes + 5)) << 6 & 0xc0) | ((*(pVoiceBytes + 6)) >> 2 & 0x3f)) & 0xE0;
		break;
	case 2:
		pSendVoice[12] |= ((*(pVoiceBytes)) >> 4 & 0x0f);
		pSendVoice[13] = ((*(pVoiceBytes)) << 4 & 0xf0) | ((*(pVoiceBytes + 1)) >> 4 & 0x0f);
		pSendVoice[14] = ((*(pVoiceBytes + 1)) << 4 & 0xf0) | ((*(pVoiceBytes + 2)) >> 4 & 0x0f);
		pSendVoice[15] = ((*(pVoiceBytes + 2)) << 4 & 0xf0) | ((*(pVoiceBytes + 3)) >> 4 & 0x0f);
		pSendVoice[16] = ((*(pVoiceBytes + 3)) << 4 & 0xf0) | ((*(pVoiceBytes + 4)) >> 4 & 0x0f);
		pSendVoice[17] = ((*(pVoiceBytes + 4)) << 4 & 0xf0) | ((*(pVoiceBytes + 5)) >> 4 & 0x0f);
		pSendVoice[18] = (((*(pVoiceBytes + 5)) << 4 & 0xf0) | ((*(pVoiceBytes + 6)) >> 4 & 0x0f)) & 0xF8;
		break;
	}
}


//DWORD prev = 0;
//DWORD current = 0;



void CWLNet::NetWorker_TxIfCall(void)
{
	int rc = 0;

	if (m_readySendVoices.size() > 0)
	{
		requireReadySendVoicesLock();
		IPSCVoiceTemplate* temp = m_readySendVoices.front();
		releaseReadySendVoicesLock();

		m_BytesSent = 0;
		m_SendBurstBuffer.len = temp->Control.fld.Length;
		m_SendBurstBuffer.buf = &(temp->RawChar[4]);

		struct sockaddr_in m_peerAddr = { 0 };


		//prev = current;
		//对当前在线的每个peer发送音频数据
		for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
		{
			m_peerAddr = (*i)->GetPeerAddressOfSockaddrin();
			rc = WSASendTo(m_socket, &m_SendBurstBuffer, 1, &m_BytesSent, 0, (const struct sockaddr *)&m_peerAddr, 16, &m_TxOverlapped, NULL);
			if (0 != rc && WSA_IO_PENDING != rc)
			{
				//发送语音数据包失败
				char* tempAddr = inet_ntoa(m_peerAddr.sin_addr);
				printf_s("WSASendTo %s error,error no:%d\r\n", tempAddr, rc);
				return;
			}
		}
		//current = GetTickCount();

		//printf_s("rlt:%lu\r\n", (current - prev));

		requireReadySendVoicesLock();
		m_readySendVoices.pop_front();
		releaseReadySendVoicesLock();

		//释放内存
		if (NULL != temp)
		{
			delete temp;
			temp = NULL;
		}

	}
	else
	{
		//int i = 0;
		sprintf_s(m_reportMsg, "error:m_readySendVoices size is zero");
		sendLogToWindow();
	}
}

bool CWLNet::isTargetMeCall(unsigned int tagetId, unsigned char callType)
{
	//sprintf_s(m_reportMsg, "recive a call to %u,local raidoId is %u,local radioGroup is %u", tgtId, m_dwMyRadioID, m_dwMyRadioGroup);
	//sendLogToWindow();

	//return true;

	return (callType == GROUPCALL_TYPE && tagetId == CONFIG_DEFAULT_GROUP) || (callType == PRIVATE_CALL && tagetId == CONFIG_LOCAL_RADIO_ID) || (callType == ALL_CALL) || (callType == GROUPCALL_TYPE && tagetId == g_targetId);
}

void CWLNet::requireVoiceReocrdsLock()
{
	m_mutextVoicRecords.lock();
}

void CWLNet::releaseVoiceReocrdsLock()
{
	m_mutextVoicRecords.unlock();
}

void PASCAL CWLNet::OneMilliSecondProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{

	CWLNet* p = (CWLNet *)dwUser;

	if (NULL == p)
	{
		return;
	}
	switch (p->m_cuurentSendType)
	{
	case SEND_TYPE_FILE:
	{
						   if (!g_bPTT
							   && p->m_sendVoices.size() <= 0)
						   {
							   /*关闭timer*/
							   timeKillEvent(wTimerID);
							   p->m_bIsSending = false;

							   sprintf_s(p->m_reportMsg, "SendVoices send out end");
							   p->sendLogToWindow();
						   }
						   else
						   {
							   p->NetWorker_SendCallByWL();
						   }
	}
		break;
	case SEND_TYPE_MIC:
	{
						  /*用户松开了PTT并且已发送完毕待发送数据*/
						  if (!g_bPTT
							  && p->m_readySendVoices.size() <= 0)
						  {
							  /*关闭timer*/
							  timeKillEvent(wTimerID);
							  /*重置flag*/
							  p->m_bIsSending = false;

							  sprintf_s(p->m_reportMsg, "m_readySendVoices send out end");
							  p->sendLogToWindow();
						  }
						  /*发送数据*/
						  else
						  {
							  p->NetWorker_TxIfCall();
							  //p->NetWorker_SendFile();
						  }
	}
		break;
	default:
		break;
	}
}

// void CWLNet::Build_LE_MASTER_PEER_REGISTRATION_REQUEST()
// {
// 	DWORD temp = m_dwMyPeerID;
// 
// 	/*PEER_ID*/
// 	//m_ControlProto[4] = (char)(temp & 0x000000FF);
// 	//temp = temp >> 8;
// 	//m_ControlProto[3] = (char)(temp & 0x000000FF);
// 	//temp = temp >> 8;
// 	//m_ControlProto[2] = (char)(temp & 0x000000FF);
// 	//temp = temp >> 8;
// 	//m_ControlProto[1] = (char)(temp & 0x000000FF);
// 	*((DWORD*)(&m_ControlProto[1])) = ntohl(temp);
// 
// 	if (m_dwRecType == IPSC)
// 	{
// 		/*PEER_MODE*/
// 		m_ControlProto[5] = IPSC_PEERMODE_DEFAULT;
// 
// 		/*PEER_SERVICE*/
// 		m_ControlProto[6] = IPSC_PEERSERVICES_DEFAULT_4;
// 		m_ControlProto[7] = IPSC_PEERSERVICES_DEFAULT_3;
// 		m_ControlProto[8] = IPSC_PEERSERVICES_DEFAULT_2;
// 		m_ControlProto[9] = IPSC_PEERSERVICES_DEFAULT_1;
// 
// 		/*CURRENT_LINK_PROTOCOL_VERSION*/
// 		m_ControlProto[10] = IPSC_CURRENTLPVERSION_DEFAULT_2;
// 		m_ControlProto[11] = IPSC_CURRENTLPVERSION_DEFAULT_1;
// 		m_ControlProto[12] = IPSC_OLDESTLPVERSION_DEFAULT_2;
// 		m_ControlProto[13] = IPSC_OLDESTLPVERSION_DEFAULT_1;
// 	}
// 	else if (m_dwRecType == CPC)
// 	{
// 		/*PEER_MODE*/
// 		m_ControlProto[5] = CPC_PEERMODE_DEFAULT;
// 
// 		/*PEER_SERVICE*/
// 		m_ControlProto[6] = CPC_PEERSERVICES_DEFAULT_4;
// 		m_ControlProto[7] = CPC_PEERSERVICES_DEFAULT_3;
// 		m_ControlProto[8] = CPC_PEERSERVICES_DEFAULT_2;
// 		m_ControlProto[9] = CPC_PEERSERVICES_DEFAULT_1;
// 
// 		/*CURRENT_LINK_PROTOCOL_VERSION*/
// 		m_ControlProto[10] = CPC_CURRENTVERSION_DEFAULT_2;
// 		m_ControlProto[11] = CPC_CURRENTVERSION_DEFAULT_1;
// 		m_ControlProto[12] = CPC_OLDESTVERSION_DEFAULT_2;
// 		m_ControlProto[13] = CPC_OLDESTVERSION_DEFAULT_1;
// 	}
// 	else if (m_dwRecType == LCP)
// 	{
// 		/*peerMode*/
// 		m_ControlProto[5] = LCP_PEERMODE_DEFAULT_1;
// 		m_ControlProto[6] = LCP_PEERMODE_DEFAULT_2;
// 
// 		/*peerServices*/
// 		m_ControlProto[7] = LCP_PEERSERVICES_DEFAULT_1;
// 		m_ControlProto[8] = LCP_PEERSERVICES_DEFAULT_2;
// 		m_ControlProto[9] = LCP_PEERSERVICES_DEFAULT_3;
// 		m_ControlProto[10] = LCP_PEERSERVICES_DEFAULT_4;
// 
// 		/*leadingChannelID*/
// 		m_ControlProto[11] = 0x00;  // leadingChannelID
// 
// 		/*currentLinkProtocolVersion*/
// 		m_ControlProto[12] = LCP_CURRENTVERSION_DEFAULT_2;
// 		m_ControlProto[13] = LCP_CURRENTVERSION_DEFAULT_1;
// 
// 		/*oldestLinkProtocolVersion*/
// 		m_ControlProto[14] = LCP_OLDESTVERSION_DEFAULT_2;
// 		m_ControlProto[15] = LCP_OLDESTVERSION_DEFAULT_1;
// 	}
// 
// 
// 	m_SendControlBuffer.buf = &m_ControlProto[0];
// }

DWORD CWLNet::Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90* pData)
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

DWORD CWLNet::Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90_LCP* pData)
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

int CWLNet::initCallParam()
{
	m_cuurentSendType = SEND_TYPE_FILE;
	m_burstType = BURST_A;
	//m_isRequestNewCall = false;
	m_SequenceNumber = 1;
	m_Timestamp = 0;
	SetCallStatus(CALL_START);
	//memset(&m_currentCallInfo, 0, sizeof(CallInfo));
	clearSendVoices();
	return 0;
}

int CWLNet::SendFile(unsigned int length, char* pData)
{
	if (length > MAX_RECORD_BUFFER_SIZE)
	{
		sprintf_s(m_reportMsg, "this voice file is too large");
		sendLogToWindow();
		return 1;
	}
	bool requestCallSuccess = false;
	m_retryRequestCallCount = REQUEST_CALL_REPEAT_FREQUENCY;
	initCallParam();
	long lengthAmbe = length;
	char* pAmbeData = pData;

	m_pCurrentSendVoicePeer = GetPeer(m_ulMasterPeerID);//192.168.2.121：50000
	if (IPSC == CONFIG_RECORD_TYPE)
	{
		if (m_pCurrentSendVoicePeer)
		{
			/*填充第一帧数据*/
			m_pSendVoicePackage = new SendVoicePackage;
			m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
			T_WL_PROTOCOL_21 networkData = { 0 };
			Build_T_WL_PROTOCOL_21(networkData, true);
			for (int txSubCount = 0; txSubCount < 3; txSubCount++)
			{
				if (pAmbeData)
				{
					if (lengthAmbe >= 7)
					{
						FILL_AMBE_FRAME(pAmbeData, &m_pVoice[45], txSubCount);
						lengthAmbe -= 7;
						pAmbeData += 7;
					}
					else
					{
						char temp[7] = { 0 };
						if (lengthAmbe)
						{
							memcpy(temp, pAmbeData, lengthAmbe);
						}
						FILL_AMBE_FRAME(temp, &m_pVoice[45], txSubCount);
						lengthAmbe = 0;
						pAmbeData = NULL;
					}
				}
			}
			m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_BURST(m_pVoice, &networkData, false);
			m_pSendVoicePackage->pPackageData = m_pVoice;
			requireReadySendVoicesLock();
			m_sendVoices.push_back(m_pSendVoicePackage);
			releaseReadySendVoicesLock();
			/*填充剩余的帧数据*/
			while (lengthAmbe > 0
				&& pAmbeData)
			{
				m_pSendVoicePackage = new SendVoicePackage;
				m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
				T_WL_PROTOCOL_21 networkData = { 0 };
				Build_T_WL_PROTOCOL_21(networkData, false);
				for (int txSubCount = 0; txSubCount < 3; txSubCount++)
				{
					if (pAmbeData)
					{
						if (lengthAmbe >= 7)
						{
							FILL_AMBE_FRAME(pAmbeData, &m_pVoice[45], txSubCount);
							lengthAmbe -= 7;
							pAmbeData += 7;
						}
						else
						{
							char temp[7] = { 0 };
							if (lengthAmbe)
							{
								memcpy(temp, pAmbeData, lengthAmbe);
							}
							FILL_AMBE_FRAME(temp, &m_pVoice[45], txSubCount);
							lengthAmbe = 0;
							pAmbeData = NULL;
						}
					}
				}
				m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_BURST(m_pVoice, &networkData, false);
				m_pSendVoicePackage->pPackageData = m_pVoice;
				requireReadySendVoicesLock();
				m_sendVoices.push_back(m_pSendVoicePackage);
				releaseReadySendVoicesLock();
			}

			/*填充超级帧*/
			while (m_burstType != (char)BURST_T)
			{
				m_pSendVoicePackage = new SendVoicePackage;
				m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
				T_WL_PROTOCOL_21 networkData = { 0 };
				Build_T_WL_PROTOCOL_21(networkData, false);
				m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_BURST(m_pVoice, &networkData, false);
				m_pSendVoicePackage->pPackageData = m_pVoice;
				requireReadySendVoicesLock();
				m_sendVoices.push_back(m_pSendVoicePackage);
				releaseReadySendVoicesLock();
			}
			/*填充结束标识*/
			m_pSendVoicePackage = new SendVoicePackage;
			m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
			T_WL_PROTOCOL_19 networkData_19 = { 0 };
			Build_T_WL_PROTOCOL_19(networkData_19);
			m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_END_BURST(m_pVoice, &networkData_19);
			m_pSendVoicePackage->pPackageData = m_pVoice;
			requireReadySendVoicesLock();
			m_sendVoices.push_back(m_pSendVoicePackage);
			releaseReadySendVoicesLock();

			_SlotNumber registSlotNumber = SLOT1;
			while (m_retryRequestCallCount)
			{
				/*申请通话*/
				requestNewCallEvent();
				m_pCurrentSendVoicePeer->HandlePacket(WL_VC_CHNL_CTRL_REQUEST_LOCAL, &registSlotNumber, 0, 0, 0);
				WaitForSingleObject(m_wlInitNewCallEvent, INFINITE);
				if (g_callRequstDeclineReasonCodeInfo.BhaveGet && 0x00 == g_callRequstDeclineReasonCodeInfo.Value)
				{
					/*开始发送文件数据*/
					SetCallStatus(CALL_ONGOING);
					NetWorker_SendCallByWL();
					timeSetEvent(SEND_VOICE_INTERVAL, 1, OneMilliSecondProc, (DWORD)this, TIME_PERIODIC);
					m_bIsSending = true;
					requestCallSuccess = true;
					break;
				}
				if (g_callRequstDeclineReasonCodeInfo.NewCallRetry)
				{
					m_retryRequestCallCount--;
					if (m_retryRequestCallCount == 0 && registSlotNumber == SLOT1)
					{
						m_retryRequestCallCount = REQUEST_CALL_REPEAT_FREQUENCY;
						registSlotNumber = SLOT2;
					}
				}
				else
				{
					sprintf_s(m_reportMsg, "%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
					sendLogToWindow();
					break;
				}
			}
			if (!requestCallSuccess)
			{
				SetCallStatus(CALL_IDLE);
				clearSendVoices();
				sprintf_s(m_reportMsg, "new call failure:%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
				sendLogToWindow();
			}
		}
	}
	else
	{
		if (m_pSitePeer)
		{
			/*填充第一帧数据*/
			m_pSendVoicePackage = new SendVoicePackage;
			m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
			T_WL_PROTOCOL_21 networkData = { 0 };
			Build_T_WL_PROTOCOL_21(networkData, true);
			for (int txSubCount = 0; txSubCount < 3; txSubCount++)
			{
				if (pAmbeData)
				{
					if (lengthAmbe >= 7)
					{
						FILL_AMBE_FRAME(pAmbeData, &m_pVoice[45], txSubCount);
						lengthAmbe -= 7;
						pAmbeData += 7;
					}
					else
					{
						char temp[7] = { 0 };
						if (lengthAmbe)
						{
							memcpy(temp, pAmbeData, lengthAmbe);
						}
						FILL_AMBE_FRAME(temp, &m_pVoice[45], txSubCount);
						lengthAmbe = 0;
						pAmbeData = NULL;
					}
				}
			}
			m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_BURST(m_pVoice, &networkData, false);
			m_pSendVoicePackage->pPackageData = m_pVoice;
			requireReadySendVoicesLock();
			m_sendVoices.push_back(m_pSendVoicePackage);
			releaseReadySendVoicesLock();
			/*填充剩余的帧数据*/
			while (lengthAmbe > 0
				&& pAmbeData)
			{
				m_pSendVoicePackage = new SendVoicePackage;
				m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
				T_WL_PROTOCOL_21 networkData = { 0 };
				Build_T_WL_PROTOCOL_21(networkData, false);
				for (int txSubCount = 0; txSubCount < 3; txSubCount++)
				{
					if (pAmbeData)
					{
						if (lengthAmbe >= 7)
						{
							FILL_AMBE_FRAME(pAmbeData, &m_pVoice[45], txSubCount);
							lengthAmbe -= 7;
							pAmbeData += 7;
						}
						else
						{
							char temp[7] = { 0 };
							if (lengthAmbe)
							{
								memcpy(temp, pAmbeData, lengthAmbe);
							}
							FILL_AMBE_FRAME(temp, &m_pVoice[45], txSubCount);
							lengthAmbe = 0;
							pAmbeData = NULL;
						}
					}
				}
				m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_BURST(m_pVoice, &networkData, false);
				m_pSendVoicePackage->pPackageData = m_pVoice;
				requireReadySendVoicesLock();
				m_sendVoices.push_back(m_pSendVoicePackage);
				releaseReadySendVoicesLock();
			}

			/*填充超级帧*/
			while (m_burstType != (char)BURST_T)
			{
				m_pSendVoicePackage = new SendVoicePackage;
				m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
				T_WL_PROTOCOL_21 networkData = { 0 };
				Build_T_WL_PROTOCOL_21(networkData, false);
				m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_BURST(m_pVoice, &networkData, false);
				m_pSendVoicePackage->pPackageData = m_pVoice;
				requireReadySendVoicesLock();
				m_sendVoices.push_back(m_pSendVoicePackage);
				releaseReadySendVoicesLock();
			}
			/*填充结束标识*/
			m_pSendVoicePackage = new SendVoicePackage;
			m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
			T_WL_PROTOCOL_19 networkData_19 = { 0 };
			Build_T_WL_PROTOCOL_19(networkData_19);
			m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_END_BURST(m_pVoice, &networkData_19);
			m_pSendVoicePackage->pPackageData = m_pVoice;
			requireReadySendVoicesLock();
			m_sendVoices.push_back(m_pSendVoicePackage);
			releaseReadySendVoicesLock();

			while (m_retryRequestCallCount)
			{
				/*申请通话*/
				requestNewCallEvent();
				m_pSitePeer->HandlePacket(WL_VC_CHNL_CTRL_REQUEST_LOCAL, NULL, 0, 0, 0);
				WaitForSingleObject(m_wlInitNewCallEvent, REQUEST_CALL_OUT_TIMER);
				if (g_callRequstDeclineReasonCodeInfo.BhaveGet && 0x00 == g_callRequstDeclineReasonCodeInfo.Value)
				{
					/*开始发送文件数据*/
					SetCallStatus(CALL_ONGOING);
					NetWorker_SendCallByWL();
					timeSetEvent(SEND_VOICE_INTERVAL, 1, OneMilliSecondProc, (DWORD)this, TIME_PERIODIC);
					m_bIsSending = true;
					requestCallSuccess = true;
					break;
				}
				if (g_callRequstDeclineReasonCodeInfo.NewCallRetry)
				{
					m_retryRequestCallCount--;
				}
				else
				{
					sprintf_s(m_reportMsg, "%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
					sendLogToWindow();
					break;
				}
			}
			if (!requestCallSuccess)
			{
				SetCallStatus(CALL_IDLE);
				clearSendVoices();
				sprintf_s(m_reportMsg, "new call failure:%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
				sendLogToWindow();
			}
		}
	}
	if (requestCallSuccess)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void CWLNet::requestNewCallEvent()
{
	ResetEvent(m_wlInitNewCallEvent);
}

void CWLNet::releaseNewCallEvent()
{
	SetEvent(m_wlInitNewCallEvent);
}

void CWLNet::NetWorker_SendCallByWL(void)
{
	/*更新时间戳*/
	for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
	{
		/*存在则记录语音*/
		if ((*i)->srcId == CONFIG_LOCAL_RADIO_ID &&
			(*i)->tagetId == g_targetId &&
			(*i)->callId == g_callId)
		{
			(*i)->prevTimestamp = GetTickCount();
			break;
		}
	}
	/*每60ms发送一次数据*/
	int rc = 0;
	if (m_sendVoices.size() > 0)
	{
		requireReadySendVoicesLock();
		SendVoicePackage* temp = m_sendVoices.front();
		m_BytesSent = 0;
		m_SendBurstBuffer.len = temp->sPackageLenth;
		m_SendBurstBuffer.buf = temp->pPackageData;
		struct sockaddr_in m_peerAddr = { 0 };
		if (m_pCurrentSendVoicePeer)
		{
			m_peerAddr = m_pCurrentSendVoicePeer->GetPeerAddressOfSockaddrin();
			rc = WSASendTo(m_socket, &m_SendBurstBuffer, 1, &m_BytesSent, 0, (const struct sockaddr *)&m_peerAddr, 16, &m_TxOverlapped, NULL);
			if (0 != rc && WSA_IO_PENDING != rc)
			{
				//发送语音数据包失败
				char* tempAddr = inet_ntoa(m_peerAddr.sin_addr);
				printf_s("WSASendTo %s error,error no:%d\r\n", tempAddr, rc);
				return;
			}
		}
		m_sendVoices.pop_front();

		//释放内存
		if (NULL != temp)
		{
			if (temp->pPackageData)
			{
				/*当前语音信息为结束标识，启动HangTimer*/
				if (temp->pPackageData[5] == WL_VC_VOICE_END_BURST)
				{
					for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
					{
						/*存在则记录语音*/
						if ((*i)->srcId == CONFIG_LOCAL_RADIO_ID &&
							(*i)->tagetId == g_targetId &&
							(*i)->callId == g_callId)
						{
							GetLocalTime(&((*i)->recordTime));
							break;
						}
					}
					timeSetEvent(CONFIG_HUNG_TIME, 1, HangTimerCallProc, (DWORD)this, TIME_ONESHOT);
				}
				free(temp->pPackageData);
				temp->pPackageData = NULL;
			}
			delete temp;
			temp = NULL;
		}
		releaseReadySendVoicesLock();
	}
	else
	{
		sprintf_s(m_reportMsg, "error:sendVoices size is zero");
		sendLogToWindow();
	}
}

//short CWLNet::Build_WL_VC_VOICE_BURST(char* pAmbeData, long& lengthAmbe, bool isfirst)
//{
//	/*Opcode*/
//	m_pVoice[0] = LE_CALL_CONTROL_INTERFACE;
//	/*peerId*/
//	*((DWORD*)&m_pVoice[1]) = htonl(g_localPeerId);
//	/*wirelineOpcode*/
//	m_pVoice[5] = WL_VC_VOICE_BURST;
//	/*slotNumber*/
//	if (m_pCurrentSendVoicePeer)
//	{
//		m_pVoice[6] = m_pCurrentSendVoicePeer->getUseSlot();
//	}
//	else
//	{
//		m_pVoice[6] = NULL_SLOT;
//	}
//	//m_pVoice[6] = SLOT1;
//	
//	/*call id*/
//	*((DWORD*)&m_pVoice[7]) = htonl(g_callId);
//	//*((DWORD*)&m_pVoice[7]) = htonl(1);
//	/*callType*/
//	m_pVoice[11] = Group_Voice_Call;
//	/*source ID*/
//	*((DWORD*)&m_pVoice[12]) = htonl(g_localRadioId);
//	/*target ID*/
//	*((DWORD*)&m_pVoice[16]) = htonl(g_localGroup);
//	/*callAttributes*/
//	m_pVoice[20] = 0x00;//clear call
//	/*RTP Information Field*/
//	m_pVoice[22] = (char)0x80;
//	if (isfirst)
//	{
//		m_pVoice[23] = (char)0xdd;
//	}
//	else
//	{
//		m_pVoice[23] = (char)0x5d;
//	}
//	*((WORD*)&m_pVoice[24]) = htons(m_SequenceNumber);
//	m_SequenceNumber += 1;
//	*((DWORD*)&m_pVoice[26]) = htonl(m_Timestamp);
//	m_Timestamp += 480;
//	m_pVoice[30] = 0x00;
//	m_pVoice[31] = 0x00;
//	m_pVoice[32] = 0x00;
//	m_pVoice[33] = 0x00;
//	/*burstType*/
//	m_pVoice[34] = m_burstType;
//	m_burstType++;
//	/*MFID*/
//	m_pVoice[36] = 0x00;
//	/*service option*/
//	m_pVoice[37] = 0x00;//clear call
//	/*algorithmID*/
//	m_pVoice[38] = 0x00;
//	/*Key ID*/
//	m_pVoice[39] = 0x00;
//	/*privacy IV*/
//	m_pVoice[40] = 0x00;
//	m_pVoice[41] = 0x00;
//	m_pVoice[42] = 0x00;
//	m_pVoice[43] = 0x00;
//	/*AMBE Frame*/
//
//	for (int count = 0; count < 3; count++)
//	{
//		if (pAmbeData)
//		{
//			if (lengthAmbe >= 7)
//			{
//				FILL_AMBE_FRAME(pAmbeData, &m_pVoice[45], count);
//				lengthAmbe -= 7;
//				pAmbeData += 7;
//			}
//			else
//			{
//				char temp[7] = { 0 };
//				if (lengthAmbe)
//				{
//					memcpy(temp, pAmbeData, lengthAmbe);
//				}
//				FILL_AMBE_FRAME(temp, &m_pVoice[45], count);
//				lengthAmbe = 0;
//				pAmbeData = NULL;
//			}
//		}
//	}
//
//#pragma region 测试语音代码
//
//	//switch (index)
//	//{
//	//case 1:
//	//{
//	//		  pVoice[44] = (char)0x00;
//	//		  pVoice[45] = (char)0xf8;
//	//		  pVoice[46] = (char)0x01;
//	//		  pVoice[47] = (char)0xa9;
//	//		  pVoice[48] = (char)0x9f;
//
//	//		  pVoice[49] = (char)0x8c;
//	//		  pVoice[50] = (char)0xe0;
//	//		  pVoice[51] = (char)0xbe;
//	//		  pVoice[52] = (char)0x00;
//	//		  pVoice[53] = (char)0x6a;
//
//	//		  pVoice[54] = (char)0x67;
//	//		  pVoice[55] = (char)0xe3;
//	//		  pVoice[56] = (char)0x38;
//	//		  pVoice[57] = (char)0x2f;
//	//		  pVoice[58] = (char)0x80;
//
//	//		  pVoice[59] = (char)0x1a;
//	//		  pVoice[60] = (char)0x99;
//	//		  pVoice[61] = (char)0xf8;
//	//		  pVoice[62] = (char)0xce;
//	//		  pVoice[63] = (char)0x08;
//	//}
//	//	break;
//	//case 2:
//	//{
//	//		  pVoice[44] = (char)0x00;
//	//		  pVoice[45] = (char)0xf8;
//	//		  pVoice[46] = (char)0x01;
//	//		  pVoice[47] = (char)0xa9;
//	//		  pVoice[48] = (char)0x9f;
//
//	//		  pVoice[49] = (char)0x8c;
//	//		  pVoice[50] = (char)0xe0;
//	//		  pVoice[51] = (char)0xbe;
//	//		  pVoice[52] = (char)0x00;
//	//		  pVoice[53] = (char)0x6a;
//
//	//		  pVoice[54] = (char)0x67;
//	//		  pVoice[55] = (char)0xe3;
//	//		  pVoice[56] = (char)0x38;
//	//		  pVoice[57] = (char)0x2f;
//	//		  pVoice[58] = (char)0x80;
//
//	//		  pVoice[59] = (char)0x1a;
//	//		  pVoice[60] = (char)0x99;
//	//		  pVoice[61] = (char)0xf8;
//	//		  pVoice[62] = (char)0xce;
//	//		  pVoice[63] = (char)0x08;
//	//}
//	//	break;
//	//case 3:
//	//{
//	//		  pVoice[44] = (char)0x00;
//	//		  pVoice[45] = (char)0xf8;
//	//		  pVoice[46] = (char)0x01;
//	//		  pVoice[47] = (char)0xa9;
//	//		  pVoice[48] = (char)0x9f;
//
//	//		  pVoice[49] = (char)0x8c;
//	//		  pVoice[50] = (char)0xe0;
//	//		  pVoice[51] = (char)0xbe;
//	//		  pVoice[52] = (char)0x00;
//	//		  pVoice[53] = (char)0x6a;
//
//	//		  pVoice[54] = (char)0x67;
//	//		  pVoice[55] = (char)0xe3;
//	//		  pVoice[56] = (char)0x38;
//	//		  pVoice[57] = (char)0x29;
//	//		  pVoice[58] = (char)0x80;
//
//	//		  pVoice[59] = (char)0x2b;
//	//		  pVoice[60] = (char)0x94;
//	//		  pVoice[61] = (char)0xfa;
//	//		  pVoice[62] = (char)0x4d;
//	//		  pVoice[63] = (char)0x38;
//	//}
//	//	break;
//	//case 4:
//	//{
//	//		  pVoice[44] = (char)0x00;
//	//		  pVoice[45] = (char)0xac;
//	//		  pVoice[46] = (char)0x71;
//	//		  pVoice[47] = (char)0xda;
//	//		  pVoice[48] = (char)0x54;
//
//	//		  pVoice[49] = (char)0x47;
//	//		  pVoice[50] = (char)0xe8;
//	//		  pVoice[51] = (char)0x8a;
//	//		  pVoice[52] = (char)0xcb;
//	//		  pVoice[53] = (char)0xbc;
//
//	//		  pVoice[54] = (char)0x20;
//	//		  pVoice[55] = (char)0x67;
//	//		  pVoice[56] = (char)0x69;
//	//		  pVoice[57] = (char)0xa9;
//	//		  pVoice[58] = (char)0x81;
//
//	//		  pVoice[59] = (char)0x09;
//	//		  pVoice[60] = (char)0x23;
//	//		  pVoice[61] = (char)0x14;
//	//		  pVoice[62] = (char)0x3d;
//	//		  pVoice[63] = (char)0x18;
//	//}
//	//	break;
//	//case 5:
//	//{
//	//		  pVoice[44] = (char)0x00;
//	//		  pVoice[45] = (char)0x98;
//	//		  pVoice[46] = (char)0x10;
//	//		  pVoice[47] = (char)0x76;
//	//		  pVoice[48] = (char)0x18;
//
//	//		  pVoice[49] = (char)0x23;
//	//		  pVoice[50] = (char)0x8b;
//	//		  pVoice[51] = (char)0x26;
//	//		  pVoice[52] = (char)0x07;
//	//		  pVoice[53] = (char)0x1a;
//
//	//		  pVoice[54] = (char)0x47;
//	//		  pVoice[55] = (char)0x68;
//	//		  pVoice[56] = (char)0xb4;
//	//		  pVoice[57] = (char)0xe9;
//	//		  pVoice[58] = (char)0x81;
//
//	//		  pVoice[59] = (char)0x9d;
//	//		  pVoice[60] = (char)0x6a;
//	//		  pVoice[61] = (char)0xfc;
//	//		  pVoice[62] = (char)0x28;
//	//		  pVoice[63] = (char)0xc8;
//	//}
//	//	break;
//	//case 6:
//	//{
//	//		  pVoice[44] = (char)0x00;
//	//		  pVoice[45] = (char)0x98;
//	//		  pVoice[46] = (char)0x2f;
//	//		  pVoice[47] = (char)0xc3;
//	//		  pVoice[48] = (char)0x99;
//
//	//		  pVoice[49] = (char)0xa3;
//	//		  pVoice[50] = (char)0x2a;
//	//		  pVoice[51] = (char)0x26;
//	//		  pVoice[52] = (char)0x0b;
//	//		  pVoice[53] = (char)0xab;
//
//	//		  pVoice[54] = (char)0xad;
//	//		  pVoice[55] = (char)0xba;
//	//		  pVoice[56] = (char)0xc7;
//	//		  pVoice[57] = (char)0x02;
//	//		  pVoice[58] = (char)0xc9;
//
//	//		  pVoice[59] = (char)0xe3;
//	//		  pVoice[60] = (char)0x72;
//	//		  pVoice[61] = (char)0xa8;
//	//		  pVoice[62] = (char)0xa0;
//	//		  pVoice[63] = (char)0x30;
//	//}
//	//	break;
//	//case 7:
//	//{
//	//		  pVoice[44] = (char)0x00;
//	//		  pVoice[45] = (char)0x90;
//	//		  pVoice[46] = (char)0xce;
//	//		  pVoice[47] = (char)0xac;
//	//		  pVoice[48] = (char)0xdc;
//
//	//		  pVoice[49] = (char)0xfc;
//	//		  pVoice[50] = (char)0x04;
//	//		  pVoice[51] = (char)0x24;
//	//		  pVoice[52] = (char)0x36;
//	//		  pVoice[53] = (char)0xae;
//
//	//		  pVoice[54] = (char)0xd3;
//	//		  pVoice[55] = (char)0xfb;
//	//		  pVoice[56] = (char)0x48;
//	//		  pVoice[57] = (char)0xa9;
//	//		  pVoice[58] = (char)0x0f;
//
//	//		  pVoice[59] = (char)0xfc;
//	//		  pVoice[60] = (char)0xf4;
//	//		  pVoice[61] = (char)0xec;
//	//		  pVoice[62] = (char)0x48;
//	//		  pVoice[63] = (char)0x38;
//	//}
//	//	break;
//	//case 8:
//	//{
//	//		  pVoice[44] = (char)0x00;
//	//		  pVoice[45] = (char)0x90;
//	//		  pVoice[46] = (char)0xee;
//	//		  pVoice[47] = (char)0xf8;
//	//		  pVoice[48] = (char)0x92;
//
//	//		  pVoice[49] = (char)0x6b;
//	//		  pVoice[50] = (char)0x04;
//	//		  pVoice[51] = (char)0xa4;
//	//		  pVoice[52] = (char)0x3f;
//	//		  pVoice[53] = (char)0x9b;
//
//	//		  pVoice[54] = (char)0xeb;
//	//		  pVoice[55] = (char)0xb0;
//	//		  pVoice[56] = (char)0x94;
//	//		  pVoice[57] = (char)0x69;
//	//		  pVoice[58] = (char)0x0e;
//
//	//		  pVoice[59] = (char)0xe6;
//	//		  pVoice[60] = (char)0x95;
//	//		  pVoice[61] = (char)0x0e;
//	//		  pVoice[62] = (char)0xa9;
//	//		  pVoice[63] = (char)0x68;
//	//}
//	//	break;
//	//case 9:
//	//{
//	//		  pVoice[44] = (char)0x00;
//	//		  pVoice[45] = (char)0x90;
//	//		  pVoice[46] = (char)0xfe;
//	//		  pVoice[47] = (char)0x68;
//	//		  pVoice[48] = (char)0x8e;
//
//	//		  pVoice[49] = (char)0xe3;
//	//		  pVoice[50] = (char)0x90;
//	//		  pVoice[51] = (char)0x24;
//	//		  pVoice[52] = (char)0x3b;
//	//		  pVoice[53] = (char)0xea;
//
//	//		  pVoice[54] = (char)0xdb;
//	//		  pVoice[55] = (char)0xe3;
//	//		  pVoice[56] = (char)0x17;
//	//		  pVoice[57] = (char)0xa9;
//	//		  pVoice[58] = (char)0x0e;
//
//	//		  pVoice[59] = (char)0xe6;
//	//		  pVoice[60] = (char)0xb5;
//	//		  pVoice[61] = (char)0xee;
//	//		  pVoice[62] = (char)0xe7;
//	//		  pVoice[63] = (char)0x10;
//	//}
//	//	break;
//	//case 10:
//	//{
//	//		   pVoice[44] = (char)0x00;
//	//		   pVoice[45] = (char)0xa0;
//	//		   pVoice[46] = (char)0xdc;
//	//		   pVoice[47] = (char)0xfb;
//	//		   pVoice[48] = (char)0xdf;
//
//	//		   pVoice[49] = (char)0x82;
//	//		   pVoice[50] = (char)0x7a;
//	//		   pVoice[51] = (char)0xa8;
//	//		   pVoice[52] = (char)0x2f;
//	//		   pVoice[53] = (char)0x8a;
//
//	//		   pVoice[54] = (char)0x54;
//	//		   pVoice[55] = (char)0x33;
//	//		   pVoice[56] = (char)0x84;
//	//		   pVoice[57] = (char)0x0b;
//	//		   pVoice[58] = (char)0x03;
//
//	//		   pVoice[59] = (char)0xe3;
//	//		   pVoice[60] = (char)0x9b;
//	//		   pVoice[61] = (char)0xec;
//	//		   pVoice[62] = (char)0x15;
//	//		   pVoice[63] = (char)0xd8;
//	//}
//	//	break;
//	//case 11:
//	//{
//	//		   pVoice[44] = (char)0x00;
//	//		   pVoice[45] = (char)0xb6;
//	//		   pVoice[46] = (char)0x4f;
//	//		   pVoice[47] = (char)0xf6;
//	//		   pVoice[48] = (char)0x5b;
//
//	//		   pVoice[49] = (char)0x01;
//	//		   pVoice[50] = (char)0x0a;
//	//		   pVoice[51] = (char)0x2d;
//	//		   pVoice[52] = (char)0x99;
//	//		   pVoice[53] = (char)0x9c;
//
//	//		   pVoice[54] = (char)0x45;
//	//		   pVoice[55] = (char)0xb0;
//	//		   pVoice[56] = (char)0x4b;
//	//		   pVoice[57] = (char)0x2b;
//	//		   pVoice[58] = (char)0x73;
//
//	//		   pVoice[59] = (char)0xde;
//	//		   pVoice[60] = (char)0x08;
//	//		   pVoice[61] = (char)0x7c;
//	//		   pVoice[62] = (char)0x11;
//	//		   pVoice[63] = (char)0x78;
//	//}
//	//	break;
//	//case 12:
//	//{
//	//		   pVoice[44] = (char)0x00;
//	//		   pVoice[45] = (char)0xb8;
//	//		   pVoice[46] = (char)0x2c;
//	//		   pVoice[47] = (char)0x59;
//	//		   pVoice[48] = (char)0x38;
//
//	//		   pVoice[49] = (char)0x81;
//	//		   pVoice[50] = (char)0x41;
//	//		   pVoice[51] = (char)0xb2;
//	//		   pVoice[52] = (char)0xe1;
//	//		   pVoice[53] = (char)0xff;
//
//	//		   pVoice[54] = (char)0x99;
//	//		   pVoice[55] = (char)0xbe;
//	//		   pVoice[56] = (char)0xc5;
//	//		   pVoice[57] = (char)0x98;
//	//		   pVoice[58] = (char)0xc0;
//
//	//		   pVoice[59] = (char)0xe5;
//	//		   pVoice[60] = (char)0xa8;
//	//		   pVoice[61] = (char)0x11;
//	//		   pVoice[62] = (char)0xcd;
//	//		   pVoice[63] = (char)0x98;
//	//}
//	//	break;
//	//default:
//	//	break;
//	//}
//#pragma endregion
//
//	/*RSSI value*/
//	m_pVoice[64] = 0x01;
//	m_pVoice[65] = (char)0xa8;
//	/*version*/
//	m_pVoice[66] = Wireline_Protocol_Version;
//	m_pVoice[67] = Wireline_Protocol_Version;
//
//	CHMAC_SHA1 sha1;
//	unsigned char digest[20];
//	unsigned char kw[40];
//	memset(kw, 0, sizeof(kw));
//	memcpy_s(kw, sizeof(kw), VenderKey, sizeof(VenderKey));
//	sha1.HMAC_SHA1((unsigned char*)m_pVoice, 68, kw, sizeof(kw), digest);
//
//	memcpy_s(m_pVoice + 68, 10, AuthenticId, AUTHENTIC_ID_SIZE);
//	memcpy_s(m_pVoice + 72, 20, digest, 10);
//
//	return 82;
//}

short CWLNet::Build_WL_VC_VOICE_BURST(CHAR* pPacket, T_WL_PROTOCOL_21* pData, bool bFillAmbe)
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
	if (bFillAmbe)
	{
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
	}
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

void CWLNet::clearSendVoices()
{
	requireReadySendVoicesLock();
	while (m_sendVoices.size() > 0)
	{
		SendVoicePackage* p = m_sendVoices.front();
		m_sendVoices.pop_front();
		if (p)
		{
			if (p->pPackageData)
			{
				free(p->pPackageData);
				p->pPackageData = NULL;
			}
			delete p;
			p = NULL;
		}
	}
	releaseReadySendVoicesLock();
}

WORD CWLNet::GetCallStatus()
{
	return m_callStatus;
}

void CWLNet::SetCallStatus(WORD value)
{
	if (value != m_callStatus)
	{
		WORD prev = m_callStatus;
		m_callStatus = value;
		sprintf_s(m_reportMsg, "CALL_STATUS:%u->%u", prev, m_callStatus);
		sendLogToWindow();
	}
}

int CWLNet::callBack()
{
	if (g_targetId != CONFIG_DEFAULT_GROUP && g_targetCallType == GROUPCALL_TYPE)
	{
		m_dwChangeToCurrentTick = GetTickCount();
	}
	bool requestCallSuccess = false;
	m_retryRequestCallCount = REQUEST_CALL_REPEAT_FREQUENCY;
	if (m_pCurrentSendVoicePeer)
	{
		while (m_retryRequestCallCount)
		{
			NetTx(true);
			requestNewCallEvent();
			/*初始化通话*/
			m_pCurrentSendVoicePeer->HandlePacket(WL_VC_CHNL_CTRL_REQUEST_LOCAL, NULL, 0, 0, FALSE, TRUE);
			WaitForSingleObject(m_wlInitNewCallEvent, REQUEST_CALL_OUT_TIMER);
			if (g_callRequstDeclineReasonCodeInfo.BhaveGet && 0x00 == g_callRequstDeclineReasonCodeInfo.Value)
			{
				g_pSound->StartRecord();
				requestCallSuccess = true;
				break;
			}
			else
			{
				if ((IPSC == CONFIG_RECORD_TYPE && g_callRequstDeclineReasonCodeInfo.RetryOfIPSC)
					|| g_callRequstDeclineReasonCodeInfo.HangCallRetry)
				{
					m_retryRequestCallCount--;
				}
				else
				{
					sprintf_s(m_reportMsg, "%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
					sendLogToWindow();
					break;
				}
			}
		}
		if (!requestCallSuccess)
		{
			SetCallStatus(CALL_IDLE);
			sprintf_s(m_reportMsg, "call back failure:%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
			sendLogToWindow();
		}
	}
	else
	{
		sprintf_s(m_reportMsg, "m_pCurrentSendVoicePeer is null");
		sendLogToWindow();
	}
	if (requestCallSuccess)
	{
		return 0;
	}
	else
	{
		Send_CARE_CALL_STATUS(g_targetCallType, CONFIG_LOCAL_RADIO_ID, g_targetId, NEW_CALL_END);
		return 1;
	}
}

int CWLNet::newCall()
{
	if (g_targetId != CONFIG_DEFAULT_GROUP && g_targetCallType == GROUPCALL_TYPE)
	{
		m_dwChangeToCurrentTick = GetTickCount();
	}
	bool requestCallSuccess = false;
	m_retryRequestCallCount = REQUEST_CALL_REPEAT_FREQUENCY;
	if (IPSC == CONFIG_RECORD_TYPE)
	{
		/*获取当前主中继相关信息*/
		m_pCurrentSendVoicePeer = GetPeer(m_ulMasterPeerID);//192.168.2.121：50000
		if (m_pCurrentSendVoicePeer)
		{
			_SlotNumber registerSlot = CONFIG_DEFAULT_SLOT;
			while (m_retryRequestCallCount)
			{
				/*初始化*/
				NetTx(true);
				requestNewCallEvent();
				/*初始化通话*/
				m_pCurrentSendVoicePeer->HandlePacket(WL_VC_CHNL_CTRL_REQUEST_LOCAL, &registerSlot, 0, 0, 0);
				WaitForSingleObject(m_wlInitNewCallEvent, REQUEST_CALL_OUT_TIMER);
				if (g_callRequstDeclineReasonCodeInfo.BhaveGet && 0x00 == g_callRequstDeclineReasonCodeInfo.Value)
				{
					g_pSound->StartRecord();
					requestCallSuccess = true;
					break;
				}
				if (g_callRequstDeclineReasonCodeInfo.RetryOfIPSC)
				{
					m_retryRequestCallCount--;
					if (m_retryRequestCallCount == 0 && registerSlot == CONFIG_DEFAULT_SLOT)
					{
						m_retryRequestCallCount = REQUEST_CALL_REPEAT_FREQUENCY;
						if (SLOT1 == CONFIG_DEFAULT_SLOT)
						{
							registerSlot = SLOT2;
						}
						else if (SLOT2 == CONFIG_DEFAULT_SLOT)
						{
							registerSlot = SLOT1;
						}
						else
						{
							sprintf_s(m_reportMsg, "error default slot");
							sendLogToWindow();
							break;
						}
					}
				}
				else
				{
					sprintf_s(m_reportMsg, "%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
					sendLogToWindow();
					break;
				}
			}
			if (!requestCallSuccess)
			{
				SetCallStatus(CALL_IDLE);
				sprintf_s(m_reportMsg, "new call failure:%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
				sendLogToWindow();
			}
		}
	}
	else if (CPC == CONFIG_RECORD_TYPE)
	{

		if (m_pSitePeer)
		{
			while (m_retryRequestCallCount)
			{
				/*初始化*/
				NetTx(true);
				requestNewCallEvent();
				/*初始化通话*/
				m_pSitePeer->HandlePacket(WL_VC_CHNL_CTRL_REQUEST_LOCAL, NULL, 0, 0, 0);
				WaitForSingleObject(m_wlInitNewCallEvent, REQUEST_CALL_OUT_TIMER);
				if (g_callRequstDeclineReasonCodeInfo.BhaveGet && g_callRequstDeclineReasonCodeInfo.Value == 0x00)
				{
					g_pSound->StartRecord();
					requestCallSuccess = true;
					break;
				}
				if (g_callRequstDeclineReasonCodeInfo.NewCallRetry)
				{

					m_retryRequestCallCount--;
				}
				else
				{
					sprintf_s(m_reportMsg, "%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
					sendLogToWindow();
					break;
				}
			}
			if (!requestCallSuccess)
			{
				SetCallStatus(CALL_IDLE);
				sprintf_s(m_reportMsg, "new call failure:%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
				sendLogToWindow();
			}
		}
	}
	else if (LCP == CONFIG_RECORD_TYPE)
	{
		if (m_pSitePeer)
		{
			while (m_retryRequestCallCount)
			{
				/*初始化*/
				NetTx(true);
				requestNewCallEvent();
				/*初始化通话*/
				m_pSitePeer->HandlePacket(WL_VC_CHNL_CTRL_REQUEST_LOCAL, NULL, 0, 0, 0);
				WaitForSingleObject(m_wlInitNewCallEvent, REQUEST_CALL_OUT_TIMER);
				if (g_callRequstDeclineReasonCodeInfo.BhaveGet && g_callRequstDeclineReasonCodeInfo.Value == 0x00)
				{
					g_pSound->StartRecord();
					requestCallSuccess = true;
					break;
				}
				if (g_callRequstDeclineReasonCodeInfo.NewCallRetry)
				{

					m_retryRequestCallCount--;
				}
				else
				{
					sprintf_s(m_reportMsg, "%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
					sendLogToWindow();
					break;
				}
			}
			if (!requestCallSuccess)
			{
				SetCallStatus(CALL_IDLE);
				sprintf_s(m_reportMsg, "new call failure:%s", g_callRequstDeclineReasonCodeInfo.ReasonCode);
				sendLogToWindow();
			}
		}
	}
	if (requestCallSuccess)
	{
		return 0;
	}
	else
	{
		Send_CARE_CALL_STATUS(g_targetCallType, CONFIG_LOCAL_RADIO_ID, g_targetId, NEW_CALL_END);
		return 1;
	}
}

// short CWLNet::Build_WL_VC_VOICE_END_BURST()
// {
// 	/*Opcode*/
// 	m_pVoice[0] = LE_CALL_CONTROL_INTERFACE;
// 	/*peerId*/
// 	*((DWORD*)&m_pVoice[1]) = htonl(g_localPeerId);
// 	/*wirelineOpcode*/
// 	m_pVoice[5] = WL_VC_VOICE_END_BURST;
// 	/*slotNumber*/
// 	m_pVoice[6] = m_pCurrentSendVoicePeer->getUseSlot();
// 	//m_pVoice[6] = SLOT1;
// 	/*call id*/
// 	*((DWORD*)&m_pVoice[7]) = htonl(g_callId);
// 	//*((DWORD*)&m_pVoice[7]) = htonl(1);
// 	/*callType*/
// 	m_pVoice[11] = Group_Voice_Call;
// 	/*source ID*/
// 	*((DWORD*)&m_pVoice[12]) = htonl(g_localRadioId);
// 	/*target ID*/
// 	*((DWORD*)&m_pVoice[16]) = htonl(g_localGroup);
// 	/*RTP Information Field*/
// 	m_pVoice[20] = (char)0x80;
// 	m_pVoice[21] = (char)0x5e;
// 
// 	*((WORD*)&m_pVoice[22]) = htons(m_SequenceNumber);
// 	m_SequenceNumber += 1;
// 	*((DWORD*)&m_pVoice[24]) = htonl(m_Timestamp);
// 	m_Timestamp += 480;
// 	m_pVoice[28] = 0x00;
// 	m_pVoice[29] = 0x00;
// 	m_pVoice[30] = 0x00;
// 	m_pVoice[31] = 0x00;
// 	/*burstType*/
// 	m_pVoice[32] = m_burstType;
// 	m_burstType++;
// 	/*MFID*/
// 	m_pVoice[34] = 0x00;
// 	/*service option*/
// 	m_pVoice[35] = 0x00;//clear call
// 	/*version*/
// 	m_pVoice[36] = Wireline_Protocol_Version;
// 	m_pVoice[37] = Wireline_Protocol_Version;
// 
// 	CHMAC_SHA1 sha1;
// 	unsigned char digest[20];
// 	unsigned char kw[40];
// 	memset(kw, 0, sizeof(kw));
// 	memcpy_s(kw, sizeof(kw), VenderKey, sizeof(VenderKey));
// 	sha1.HMAC_SHA1((unsigned char*)m_pVoice, 38, kw, sizeof(kw), digest);
// 
// 	memcpy_s(m_pVoice + 38, 10, AuthenticId, AUTHENTIC_ID_SIZE);
// 	memcpy_s(m_pVoice + 42, 20, digest, 10);
// 
// 	return 52;
// }

short CWLNet::Build_WL_VC_VOICE_END_BURST(CHAR* pPacket, T_WL_PROTOCOL_19* pData)
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

void CWLNet::CorrectingBuffer(DWORD callId)
{
	_SlotNumber slot = m_pCurrentSendVoicePeer->getUseSlot();
	/*核对语音记录信息*/
	for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
	{
		/*存在则记录语音*/
		if ((*i)->srcId == CONFIG_LOCAL_RADIO_ID &&
			(*i)->callStatus == VOICE_STATUS_START)
		{
			(*i)->callId = callId;
			(*i)->srcSlot = slot;
			//callType = (*i)->callType;
			break;
		}
	}
	g_callId = callId;

	requireReadySendVoicesLock();
	for (auto i = m_sendVoices.begin(); i != m_sendVoices.end(); i++)
	{
		SendVoicePackage* p = *i;
		short size = p->sPackageLenth - 14;
		p->pPackageData[6] = slot;
		*((DWORD*)(&(p->pPackageData[7]))) = htonl(g_callId);
		getWirelineAuthentication(p->pPackageData, size);
	}
	releaseReadySendVoicesLock();

	Send_CARE_CALL_STATUS(g_targetCallType, CONFIG_LOCAL_RADIO_ID, g_targetId,NEW_CALL_START);
}

void CWLNet::requestRecordEndEvent()
{
	ResetEvent(m_endRecordEvent);
}

void CWLNet::releaseRecordEndEvent()
{
	SetEvent(m_endRecordEvent);
}

void CWLNet::waitRecordEnd()
{
	WaitForSingleObject(m_endRecordEvent, INFINITE);
	/*填充超级帧*/
	while (m_burstType != (char)BURST_T)
	{
		long tempLength = 0;
		m_pSendVoicePackage = new SendVoicePackage;
		m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
		T_WL_PROTOCOL_21 networkData = { 0 };
		Build_T_WL_PROTOCOL_21(networkData, false);
		m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_BURST(m_pVoice, &networkData, false);
		m_pSendVoicePackage->pPackageData = m_pVoice;
		requireReadySendVoicesLock();
		m_sendVoices.push_back(m_pSendVoicePackage);
		releaseReadySendVoicesLock();
	}
	//sprintf_s(m_reportMsg, "填充超级帧");
	//sendLogToWindow();
	/*填充结束标识*/
	m_pSendVoicePackage = new SendVoicePackage;
	m_pVoice = (char*)calloc(MAX_PACKET_SIZE, sizeof(char));
	T_WL_PROTOCOL_19 networkData = { 0 };
	Build_T_WL_PROTOCOL_19(networkData);
	m_pSendVoicePackage->sPackageLenth = Build_WL_VC_VOICE_END_BURST(m_pVoice, &networkData);
	m_pSendVoicePackage->pPackageData = m_pVoice;
	requireReadySendVoicesLock();
	m_sendVoices.push_back(m_pSendVoicePackage);
	releaseReadySendVoicesLock();
	g_bPTT = FALSE;
	//sprintf_s(m_reportMsg, "填充结束标识");
	//sendLogToWindow();
}

void CWLNet::setCurrentSendVoicePeer(CIPSCPeer* value)
{
	m_pCurrentSendVoicePeer = value;
}

void PASCAL CWLNet::HangTimerCallProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	CWLNet* p = (CWLNet *)dwUser;
	if (p)
	{
		p->HangTimerCallCheck();
	}
}

void CWLNet::HangTimerCallCheck()
{
	if (!(GetCallStatus() == CALL_HANGUP || GetCallStatus() == CALL_IDLE || g_dongleIsUsing))
	{
		SetCallStatus(CALL_HANGUP);
		SetCallStatus(CALL_IDLE);
	}
}

void CWLNet::setSitePeer(CIPSCPeer* value)
{
	m_pSitePeer = value;
}

void CWLNet::getWirelineAuthentication(char* pPacket, short &size)
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

void CWLNet::Build_T_WL_PROTOCOL_19(T_WL_PROTOCOL_19& networkData)
{
	networkData.burstType = m_burstType;
	networkData.callID = g_callId;
	networkData.callType = g_targetCallType;
	networkData.currentLinkProtocolVersion = Wireline_Protocol_Version;
	networkData.MFID = VALUE_MFID;
	networkData.oldestLinkProtocolVersion = Wireline_Protocol_Version;
	networkData.Opcode = WL_PROTOCOL;
	networkData.peerID = CONFIG_LOCAL_PEER_ID;
	networkData.RTPInformationField.header = BURST_RTP_HEADER;
	networkData.RTPInformationField.MPT = BURST_END_RTP_MPT;
	networkData.RTPInformationField.SequenceNumber = m_SequenceNumber;
	networkData.RTPInformationField.SSRC = BURST_RTP_SSRC;
	networkData.RTPInformationField.Timestamp = m_Timestamp;
	networkData.serviceOption = BURST_SERVICEOPTION;
	if (m_pCurrentSendVoicePeer)
	{
		networkData.slotNumber = m_pCurrentSendVoicePeer->getUseSlot();
	}
	else
	{
		networkData.slotNumber = NULL_SLOT;
	}
	networkData.sourceID = CONFIG_LOCAL_RADIO_ID;
	networkData.targetID = g_targetId;
	networkData.wirelineOpcode = WL_VC_VOICE_END_BURST;
}

void CWLNet::Build_T_WL_PROTOCOL_21(T_WL_PROTOCOL_21& networkData, bool bStart)
{
	networkData.algorithmID = ALGORITHMID;
	if (m_burstType == (char)BURST_T)
	{
		m_burstType = BURST_A;
	}
	networkData.burstType = m_burstType;
	m_burstType++;
	networkData.callAttributes = CALL_ATTRIBUTES;
	networkData.callID = g_callId;
	networkData.callType = g_targetCallType;
	networkData.currentLinkProtocolVersion = Wireline_Protocol_Version;
	networkData.IV = VALUE_IV;
	networkData.keyID = KEY_ID;
	networkData.MFID = VALUE_MFID;
	networkData.oldestLinkProtocolVersion = Wireline_Protocol_Version;
	networkData.Opcode = WL_PROTOCOL;
	networkData.peerID = CONFIG_LOCAL_PEER_ID;
	networkData.rawRssiValue = VALUE_RSSI;
	networkData.RTPInformationField.header = BURST_RTP_HEADER;
	networkData.sourceID = CONFIG_LOCAL_RADIO_ID;
	networkData.targetID = g_targetId;
	if (m_pCurrentSendVoicePeer)
	{
		networkData.slotNumber = m_pCurrentSendVoicePeer->getUseSlot();
	}
	else
	{
		networkData.slotNumber = NULL_SLOT;
	}
	if (bStart)
	{
		networkData.RTPInformationField.MPT = BURST_START_RTP_MPT;
		/*生成语音记录相关信息*/
		CRecordFile* rFile = new CRecordFile();
		rFile->originalPeerId = networkData.peerID;
		rFile->srcId = networkData.sourceID;
		rFile->tagetId = networkData.targetID;
		rFile->callId = networkData.callID;
		rFile->callType = networkData.callType;
		rFile->prevTimestamp = GetTickCount();
		rFile->srcSlot = networkData.slotNumber;
		rFile->callStatus = VOICE_STATUS_START;
		requireVoiceReocrdsLock();
		m_voiceReocrds.push_back(rFile);
		releaseVoiceReocrdsLock();
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

DWORD CWLNet::Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96* pData)
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

DWORD CWLNet::Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96_LCP* pData)
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

DWORD CWLNet::Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92* pData)
{
	DWORD size;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	size = 5;
	return size;
}

DWORD CWLNet::Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92_LCP* pData)
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

void CWLNet::Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91& networkData)
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

void CWLNet::Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91_LCP& networkData)
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

void CWLNet::Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93& networkData)
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
		networkData.mapPeers[index].remotePeerID = ntohl(*((DWORD*)(&pData[7 + (index * 11)])));
		networkData.mapPeers[index].remoteIPAddr = ntohl(*((DWORD*)(&pData[11 + (index * 11)])));
		networkData.mapPeers[index].remotePort = ntohs(*((WORD*)(&pData[15 + (index * 11)])));
		networkData.mapPeers[index].peerMode = pData[17 + (index * 11)];
		dataLength -= 11;
		index++;
	}
	/*mapNums;*/
	networkData.mapNums = index;

}

void CWLNet::Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93_LCP& networkData)
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
			networkData.mapPayload.wideMapPeers[index].remotePeerID = ntohl(*((DWORD*)(&pData[8 + (index * 13)])));
			networkData.mapPayload.wideMapPeers[index].remoteIPAddr = ntohl(*((DWORD*)(&pData[12 + (index * 13)])));
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

void CWLNet::Unpack_LE_PEER_REGISTRATION_REQUEST(char* pData, T_LE_PROTOCOL_94& networkData)
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

void CWLNet::Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98& networkData)
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

void CWLNet::Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98_LCP& networkData)
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

void CWLNet::Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99& networkData)
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

void CWLNet::Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99_LCP& networkData)
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

void CWLNet::Unpack_LE_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_95& networkData)
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

void CWLNet::Unpack_WL_REGISTRATION_GENERAL_OPS(char* pData, T_WL_PROTOCOL_03& networkData)
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

void CWLNet::Unpack_WL_REGISTRATION_STATUS(char* pData, T_WL_PROTOCOL_02& networkData)
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

void CWLNet::Unpack_WL_CHNL_STATUS(char* pData, T_WL_PROTOCOL_11& networkData)
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

void CWLNet::Unpack_WL_CHNL_STATUS_QUERY(char* pData, T_WL_PROTOCOL_12& networkData)
{
	/*slotNumber*/
	networkData.slotNumber = pData[6];
	/*Current / Accepted Wireline Protocol Version*/
	networkData.currentLinkProtocolVersion = pData[7];
	/*Oldest Wireline Protocol Version*/
	networkData.oldestLinkProtocolVersion = pData[8];
}

void CWLNet::Unpack_WL_VC_CHNL_CTRL_STATUS(char* pData, T_WL_PROTOCOL_16& networkData)
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

void CWLNet::Unpack_WL_VC_VOICE_START(char* pData, T_WL_PROTOCOL_18& networkData)
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

void CWLNet::Unpack_WL_VC_VOICE_END_BURST(char* pData, T_WL_PROTOCOL_19& networkData)
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

void CWLNet::Unpack_WL_VC_CALL_SESSION_STATUS(char* pData, T_WL_PROTOCOL_20& networkData)
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

void CWLNet::Unpack_WL_VC_VOICE_BURST(char* pData, T_WL_PROTOCOL_21& networkData)
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

void CWLNet::clearPeers()
{
	for (auto i = m_pPeers.begin(); i != m_pPeers.end(); i++)
	{
		(*i)->destroy();
		delete (*i);
	}
}

int CWLNet::checkDefaultGroup()
{
	if (g_targetId != CONFIG_DEFAULT_GROUP && g_targetCallType == GROUPCALL_TYPE)
	{
		long dif = GetTickCount() - m_dwChangeToCurrentTick;
		if (dif > GO_BACK_DEFAULT_GROUP_TIME)
		{
			g_targetId = CONFIG_DEFAULT_GROUP;
		}
	}
	return 0;
}



int CWLNet::setPlayCallOfCare(unsigned char calltype, unsigned long srcId, unsigned long targetId)
{
	//int type = atoi(pCallType);
	unsigned long src = srcId;
	unsigned long tgt = targetId;
	switch (calltype)
	{
	case GROUPCALL_TYPE:
	{
						   if (tgt != CONFIG_DEFAULT_GROUP)
						   {
							   g_targetId = tgt;
							   g_bIsHaveCurrentGroupCall = true;
							   g_bIsHaveDefaultGroupCall = false;
							   g_bIsHaveAllCall = false;
							   g_bIsHavePrivateCall = false;
						   }
						   else
						   {
							   g_bIsHaveCurrentGroupCall = false;
							   g_bIsHaveDefaultGroupCall = true;
							   g_bIsHaveAllCall = false;
							   g_bIsHavePrivateCall = false;
						   }
	}
		break;
	case PRIVATE_CALL:
	{
						 g_bIsHaveCurrentGroupCall = false;
						 g_bIsHaveDefaultGroupCall = false;
						 g_bIsHaveAllCall = false;
						 g_bIsHavePrivateCall = true;
	}
		break;
	case ALL_CALL:
	{
					 g_bIsHaveCurrentGroupCall = false;
					 g_bIsHaveDefaultGroupCall = false;
					 g_bIsHaveAllCall = true;
					 g_bIsHavePrivateCall = false;
	}
		break;
	default:
	{
			   return 1;
	}
		break;
	}
	return 0;
}

int CWLNet::thereIsCallOfCare(CRecordFile *pCallRecord)
{
	Send_CARE_CALL_STATUS(pCallRecord->callType, pCallRecord->srcId, pCallRecord->tagetId, HAVE_CALL_NO_PLAY);
	return 0;
}

int CWLNet::Send_CARE_CALL_STATUS(unsigned char callType, unsigned long srcId, unsigned long tgtId, int status)
{
	/*将参数打包成json格式*/
	std::map<std::string, std::string> args;
	char temp[128] = { 0 };
	sprintf_s(temp, "%u", callType);
	args["callType"] = temp;
	sprintf_s(temp, "%lu", srcId);
	args["srcId"] = temp;
	sprintf_s(temp, "%lu", tgtId);
	args["tgtId"] = temp;
	sprintf_s(temp, "%d", status);
	args["status"] = temp;
	args["module"] = "wl";
	std::string strRequest = CRpcJsonParser::buildCall("Send_CARE_CALL_STATUS",++g_sn,args);
	sprintf_s(m_reportMsg, "%s", strRequest.c_str());
	sendLogToWindow();
	/*发送到Client*/
	for (auto i = g_onLineClients.begin(); i != g_onLineClients.end();i++)
	{
		TcpClient* p = *i;
		try
		{
			p->sendResponse(strRequest.c_str(), strRequest.size());
		}
		catch (...)
		{
			sprintf_s(m_reportMsg, "Send_CARE_CALL_STATUS fail, socket:%lu", p->s);
			sendLogToWindow();
		}
	}
	return 0;
}

void CWLNet::setWlStatus(WLStatus value)
{
	m_WLStatus = value;
}

WLStatus CWLNet::getWlStatus()
{
	return m_WLStatus;
}

CRecordFile* CWLNet::getCurrentPlayInfo()
{
	return m_pPlayCall;
}

void CWLNet::setCurrentPlayInfo(CRecordFile *value)
{
	m_pPlayCall = value;
}

//bool CWLNet::getIsFirstBurstA()
//{
//	return m_isFirstBurstA;
//}
//
//void CWLNet::setIsFirstBurstA(bool value)
//{
//	m_isFirstBurstA = value;
//}
