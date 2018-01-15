#include "stdafx.h"
#include "P2pNet.h"
#include "NSRecordFile.h"
#include <process.h>
CP2PNet::CP2PNet(NSManager* pManager)
:  m_isWork(false)
, m_isTimeoutWork(false)
, m_isAmbeDataWork(false)
, m_isGetSerialWork(false)
, m_isCheckRecords(false)
, m_p2pDataMutex(INITLOCKER())
, m_p2pDataLink(NULL)
, m_p2pTimeoutDataMutex(INITLOCKER())
, m_peersMutex(INITLOCKER())
//, m_serialMutex(INITLOCKER())
, m_p2pTimeoutDataLink(NULL)
//, m_serialLink(NULL)
, m_peersLink(NULL)
, m_ambeMutex(INITLOCKER())
, m_ambeLink(NULL)
, m_recordMutex(INITLOCKER())
, m_recordLink(NULL)
, m_le_status_enum(STARTING)
, m_sendMutex(INITLOCKER())
//, m_xnl_status_enum(WAITLE)
//, m_desAddress(0)
//, m_sourceAddress(0)
//, m_trascationId(0)
, m_pSerial(new NSSerial())
, m_manager(pManager)
{
	m_workThread = NULL;
	m_timeoutThread = NULL;
	m_ambeDataThread = NULL;
	m_getSerailThread = NULL;
	m_recordsThread = NULL;
	m_pXQTTNet = NULL;
	m_log = NSLog::instance();
	netInit();
	g_repeater_net_mode = P2P;
	memset(&m_localIpList, 0, sizeof(local_ip_list_t));
	/*获取当前ip list*/
	GetIpList(&m_localIpList);
	
}
CP2PNet::~CP2PNet()
{
	
	//netDeinit();
	if (m_pSerial) delete m_pSerial;
	m_pSerial = NULL;
	UnInit();
	ClearWorkDataLink();
	ClearTimeoutDataLink();
	ClearPeersLink();
	ClearAmbeLink();
	ClearRecordLink();
	DELETELOCKER(m_p2pDataMutex);
	DELETELOCKER(m_p2pTimeoutDataMutex);
	DELETELOCKER(m_peersMutex);
	DELETELOCKER(m_ambeMutex);
	DELETELOCKER(m_recordMutex);
}
void CP2PNet::ClearWorkDataLink()
{
	TRYLOCK(m_p2pDataMutex);
	pLinkItem it = popFront(&m_p2pDataLink);
	while (it)
	{
		if (NULL != it->data)
		{
			delete (work_item_t*)it->data;
			it->data = NULL;
		}
		freeList(it);
		it = popFront(&m_p2pDataLink);
	}
	RELEASELOCK(m_p2pDataMutex);
}
void CP2PNet::ClearTimeoutDataLink()
{
	TRYLOCK(m_p2pTimeoutDataMutex);
	pLinkItem it = popFront(&m_p2pTimeoutDataLink);
	while (it)
	{
		if (NULL != it->data)
		{
			delete (work_item_t*)it->data;
			it->data = NULL;
		}
		freeList(it);
		it = popFront(&m_p2pTimeoutDataLink);
	}
	RELEASELOCK(m_p2pTimeoutDataMutex);
}
//void CP2PNet::ClearSerialLink()
//{
//	TRYLOCK(m_serialMutex);
//	pLinkItem it = popFront(&m_serialLink);
//	while (it)
//	{
//		if (NULL != it->data)
//		{
//			delete (work_item_t *)it->data;
//			it->data = NULL;
//		}
//		freeList(it);
//		it = popFront(&m_serialLink);
//	}
//	RELEASELOCK(m_serialMutex);
//}
void CP2PNet::ClearPeersLink()
{
	TRYLOCK(m_peersMutex);
	pLinkItem item = popFront(&m_peersLink);
	while (item)
	{
		if (NULL != item->data)
		{
			delete (CP2PPeer*)item->data;
			item->data = NULL;
		}
		freeList(item);
		item = popFront(&m_peersLink);
	}
	m_peersLink = NULL;
	RELEASELOCK(m_peersMutex);
}
void CP2PNet::ClearAmbeLink()
{
	TRYLOCK(m_ambeMutex);
	pLinkItem item = popFront(&m_ambeLink);
	while (item)
	{
		if (NULL != item->data)
		{
			delete (work_item_t*)item->data;
			item->data = NULL;
		}
		freeList(item);
		item = popFront(&m_ambeLink);
	}
	m_ambeLink = NULL;
	RELEASELOCK(m_ambeMutex);
}
void CP2PNet::ClearRecordLink()
{
	NSRecordFile* p = NULL;
	TRYLOCK(m_recordMutex);
	pLinkItem item = popFront(&m_recordLink);
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
		item = popFront(&m_recordLink);
	}
	m_recordLink = NULL;
	RELEASELOCK(m_recordMutex);
}
int CP2PNet::StartNet(StartNetParam* p)
{
	if (p != NULL)
	{
		m_netParam = *p;
		if (NULL == m_pXQTTNet)
		{
			m_pXQTTNet = connectServerUdp(m_netParam.master_ip, m_netParam.master_port, m_netParam.local_ip, m_netParam.local_port, &m_masterSocket);
		}
		if (NULL == m_pXQTTNet)
		{
			m_log->AddLog("connectServer failed!");
			return -1;
		}
		/*获取序列号*/
		StartSerialParam param = { 0 };
		param.param = this;
		param.pMasterXqttnet = m_pXQTTNet;
		param.pNetParam = &m_netParam;
		param.pSerialCallBack = &SetSerialNumberCallback;
		param.pSockaddrMaster = &m_masterSocket;
		m_pSerial->Start(&param);
		/*设置回调*/
		m_pXQTTNet->param = this;
		m_pXQTTNet->_netOnRecv = &OnRecv;
		m_pXQTTNet->_netOnDisconn = &OnDisconn;
		m_pXQTTNet->_netOnError = &OnError;
		//m_pXQTTNet->_netOnSendComplete = &OnSendComplete;
		/*初始化*/
		Init();
		/* 发送90注册包*/
		SetLeStatus(STARTING);
		if (STARTING == GetLeStatus())
		{
			work_item_t* pItem = new work_item_t;
			memset(pItem, 0, sizeof(work_item_t));
			SEND_LE_MASTER_PEER_REGISTRATION_REQUEST(pItem);
			AddWorkItem(pItem);
		}
	}
	else
	{
		return -1;
	}
	return 0;
}
void CP2PNet::GetSerial(char* &pSerial,int &length)
{
	length = 0;
	if (NULL == pSerial)
	{
		return;
	}
	/*if (GET_SERIAL_SUCCESS != GetXnlStatus())
	{
		return;
	}*/
	length = sizeof(m_serialNumber);
	memcpy(pSerial, m_serialNumber, length);
}
void CP2PNet::Init()
{
	/*创建 m_p2pDataLink*/
	//if (NULL == m_p2pDataLink)
	//{
	//	m_p2pDataLink = createLinkList();
	//}
	/*创建work 线程*/
	if (NULL == m_workThread)
	{
		unsigned int m_workThreadId = 1;
		m_workThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			WorkThreadProc,
			this,
			CREATE_SUSPENDED,
			&m_workThreadId
			);
		if (NULL == m_workThread)
		{
			return;
		}
	}
	/*创建timeout 线程*/
	if (NULL == m_timeoutThread)
	{
		unsigned int m_timeoutThreadId = 2;
		m_timeoutThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			TimeoutThreadProc,
			this,
			CREATE_SUSPENDED,
			&m_timeoutThreadId
			);
		if (NULL == m_timeoutThread)
		{
			return;
		}
	}
	
	/* 创建 ambe 线程*/
	if (NULL == m_ambeDataThread)
	{
		unsigned int m_ambedataThreadId = 3;
		m_ambeDataThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			AmbeDataThreadProc,
			this,
			CREATE_SUSPENDED,
			&m_ambedataThreadId
			);
		if (NULL == m_ambeDataThread)
		{
			return;
		}
	}
	/*创建 getSerial 线程*/
	/*if (NULL == m_getSerailThread)
	{
		unsigned int m_getserialThreadId = 4;
		m_getSerailThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			GetSerialThreadProc,
			this,
			CREATE_SUSPENDED,
			&m_getserialThreadId
			);
		if (NULL == m_getSerailThread)
		{
			return;
		}
	}*/
	/*创建check records 线程*/
	if (NULL == m_recordsThread)
	{
		unsigned int m_checkRecordsThreadId = 5;
		m_recordsThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			CheckRecordsThreadProc,
			this,
			CREATE_SUSPENDED,
			&m_checkRecordsThreadId
			);
		if (NULL == m_recordsThread)
		{
			return;
		}
	}
	/*启动所有线程*/
	m_isWork = true;
	m_isTimeoutWork = true;
	m_isAmbeDataWork = true;
	m_isGetSerialWork = true;
	m_isCheckRecords = true;
	ResumeThread(m_workThread);
	ResumeThread(m_timeoutThread);
	ResumeThread(m_ambeDataThread);
	ResumeThread(m_getSerailThread);
	ResumeThread(m_recordsThread);
}
void CP2PNet::UnInit()
{
	/* 释放回调*/
	if (m_pXQTTNet)
	{
		m_pXQTTNet->param = NULL;
		m_pXQTTNet->_netOnRecv = NULL;
		m_pXQTTNet->_netOnDisconn = NULL;
		m_pXQTTNet->_netOnError = NULL;
		//m_pXQTTNet->_netOnSendComplete = NULL;
	}
	/*停止线程*/
	m_isWork = false;
	m_isTimeoutWork = false;
	m_isAmbeDataWork = false;
	m_isGetSerialWork = false;
	m_isCheckRecords = false;
	if (NULL != m_workThread)
	{
		WaitForSingleObject(m_workThread, MILLISECONDS);
		CloseHandle(m_workThread);
		m_workThread = NULL;
	}
	if (NULL != m_timeoutThread)
	{
		WaitForSingleObject(m_timeoutThread, MILLISECONDS);
		CloseHandle(m_timeoutThread);
		m_timeoutThread = NULL;
	}
	if (NULL != m_ambeDataThread)
	{
		WaitForSingleObject(m_ambeDataThread, MILLISECONDS);
		CloseHandle(m_ambeDataThread);
		m_ambeDataThread = NULL;
	}
	/*if (NULL != m_getSerailThread)
	{
		WaitForSingleObject(m_getSerailThread, MILLISECONDS);
		CloseHandle(m_getSerailThread);
		m_getSerailThread = NULL;
	}*/
	if (NULL != m_recordsThread)
	{
		WaitForSingleObject(m_recordsThread,MILLISECONDS);
		CloseHandle(m_recordsThread);
		m_recordsThread = NULL;
	}
	/*关闭socket*/
	if (m_pXQTTNet)
	{
		//disConnect(m_pXQTTNet);
		//WSACleanup();
	}
	
}
unsigned int __stdcall CP2PNet::WorkThreadProc(void* pParam)
{
	CP2PNet *p = (CP2PNet*)pParam;
	if (p)
	{
		p->WorkThread();
	}
	return 0;
}
void CP2PNet::WorkThread()
{
	m_log->AddLog("work thread start!");
	item_oprate_enum OpreateFlag = Oprate_Del;
	while (m_isWork)
	{
		work_item_t* currentItem = PopWorkItem();
		if (NULL == currentItem)
		{
			Sleep(SLEEP_WORK_THREAD);
			continue;
		}
		OpreateFlag = Oprate_Del;
		work_item_type_enum type = currentItem->type;
		switch (type)
		{
			case Send:
				SendDataToMaster(currentItem);
				break;
			case Recive:
			{
				char opcode = currentItem->data.recive_data.protocol.le.PROTOCOL_90.Opcode;
				switch (m_le_status_enum)
				{
					case STARTING:  //发送90
					{
						if (LE_MASTER_PEER_REGISTRATION_RESPONSE == opcode)
						{
							Handle_Le_Status_Starting(currentItem, OpreateFlag);
						}
						break;
					}
				case WAITFOR_LE_NOTIFICATION_MAP_BROADCAST:  //收到93
				{
					if (LE_NOTIFICATION_MAP_BROADCAST == opcode)
					{
						Handle_Le_Status_Wait_Map(currentItem, OpreateFlag);
					}
					break;
				}
				case ALIVE:
					Handle_Le_Status_Alive(opcode, currentItem, OpreateFlag);
					break;
				default:
					break;
				}
				if (Oprate_Del == OpreateFlag)
				{
					DeleteWorkItem(currentItem);
				}
				else if (Oprate_Add == OpreateFlag)
				{
					AddTimeoutItem(currentItem);
				}
				
			}
		default:
			break;
		}

	}
	m_log->AddLog("work thread end");
}
unsigned int __stdcall CP2PNet::TimeoutThreadProc(void* pParam)
{
	CP2PNet *p = (CP2PNet*)pParam;
	if (p)
	{
		p->TimeoutThread();
	}
	return 0;
}
void CP2PNet::TimeoutThread()
{
	m_log->AddLog("time out thread start");
	item_oprate_enum OpreateFlag = Oprate_Del;
	while (m_isTimeoutWork)
	{
		work_item_t* currentItem = PopTimeoutItem();
		if (NULL == currentItem)
		{
			Sleep(SLEEP_TIMEOUT_THREAD);
			continue;
		}
		work_item_type_enum type = currentItem->type;
		OpreateFlag = Oprate_Del;
		switch (type)
		{
		case Send:
			{
				char opcode = currentItem->data.send_data.protocol.le.PROTOCOL_90.Opcode;
				switch (GetLeStatus())
				{
				case STARTING:   //0x90
					if (LE_MASTER_PEER_REGISTRATION_REQUEST == opcode)
					{
						Handle_Le_Status_Starting_Timeout(currentItem, OpreateFlag, Send);
					}
					break;
				case WAITFOR_LE_NOTIFICATION_MAP_BROADCAST:  //0x92
					if (LE_NOTIFICATION_MAP_REQUEST == opcode)
					{
						Handle_Le_Status_Wait_Map_Timeout(currentItem, OpreateFlag, Send);
					}
					break;
				case ALIVE:
				{
					Handle_Le_Status_Alive_Timeout(opcode, currentItem, OpreateFlag, Send);
				}
					break;
				default:
					break;
				}
			}
			break;
		case Recive:
			char opcode = currentItem->data.send_data.protocol.le.PROTOCOL_90.Opcode;
			switch (GetLeStatus())
			{
			case STARTING:   //0x91
				if (LE_MASTER_PEER_REGISTRATION_RESPONSE == opcode)
				{
					Handle_Le_Status_Starting_Timeout(currentItem, OpreateFlag, Recive);
				}
				break;
			case WAITFOR_LE_NOTIFICATION_MAP_BROADCAST:  //0x92
				if (LE_NOTIFICATION_MAP_BROADCAST == opcode)
				{
					Handle_Le_Status_Wait_Map_Timeout(currentItem, OpreateFlag, Recive);
				}
				break;
			case ALIVE:
			{
				Handle_Le_Status_Alive_Timeout(opcode, currentItem, OpreateFlag, Recive);
			}
				break;
			default:
				break;
			}
			break;
		}
		if (Oprate_Del == OpreateFlag)
		{
			DeleteWorkItem(currentItem);
		}
		Sleep(SLEEP_TIMEOUT_THREAD);
	}
	m_log->AddLog("time out thread end");
}
unsigned int __stdcall CP2PNet::AmbeDataThreadProc(void* pParam)
{
	CP2PNet *p = (CP2PNet*)pParam;
	if (p)
	{
		p->AmbeDataThread();
	}
	return 0;
}
void CP2PNet::AmbeDataThread()
{
	m_log->AddLog("ambe thread start!");
	item_oprate_enum OpreateFlag = Oprate_Del;
	NSRecordFile* m_recordFile = NULL;     
	find_record_condition_t condition = { 0 };
	bool bFind = true;
	unsigned __int8  CallSequenceNumber;                      
	unsigned __int8  CallPriority;                            
	unsigned __int8  CallControlInformation;                   // 3
	unsigned __int8  RepeaterBurstDataType;                    // 4
	unsigned __int8  ESNLIEHB;                                 // 5
	unsigned __int16 RTPSequenceNumber;     //Host Order       // 6
	unsigned __int32 CallOriginatingPeerID; //Host Order       // 8
	unsigned __int32 CallFloorControlTag;   //Host Order       //12
	unsigned __int32 CallSrcID;             //Host Order 0x3210//16
	unsigned __int32 CallTgtID;             //Host Order 0x3210//20
	unsigned __int32 RTPTimeStamp;          //Host order 0x4321//24
	unsigned char src_slot;
	unsigned char call_type;
	while (m_isAmbeDataWork)
	{
		work_item_t* currentItem = PopAmbeItem();
		if (NULL == currentItem)
		{
			Sleep(SLEEP_AMBE_THREAD);
			continue;
		}
		OpreateFlag = Oprate_Del;
		moto_protocol_p2p_t m_moto_protocol_p2p_t = currentItem->data.recive_data.protocol.p2p;
		P2P_CALL_HEADER call_header = m_moto_protocol_p2p_t.P2PCall.CallHeader;
		AMBE_VOICE_ENCODED_FRAMES m_ambeVoice = m_moto_protocol_p2p_t.P2PCall.AmbeVoiceEncodedFrames;
		CallSequenceNumber = call_header.CallSequenceNumber;
		CallPriority =call_header.CallPriority;
		CallControlInformation =call_header.CallControlInformation;                  
		RepeaterBurstDataType =call_header.RepeaterBurstDataType;                    
		ESNLIEHB = call_header.ESNLIEHB;                                 
		RTPSequenceNumber=call_header.RTPSequenceNumber;     
		CallOriginatingPeerID =call_header.CallOriginatingPeerID; 
		CallFloorControlTag =call_header.CallFloorControlTag;   
		CallSrcID =call_header.CallSrcID;            
		CallTgtID =call_header.CallTgtID;             
		RTPTimeStamp =call_header.RTPTimeStamp;
		src_slot = (CallControlInformation & 0x20) >> 5;
		call_type = m_moto_protocol_p2p_t.P2PCall.CallType;
		//FillThisCall(call_header);
		char voiceFrame[21] = { 0 };
		char RepeaterBurstDataType = m_moto_protocol_p2p_t.P2PCall.CallHeader.RepeaterBurstDataType;
		switch (RepeaterBurstDataType)
		{
		case DATA_TYPE_VOICE_HEADER:   //语音头（语音初始化）
		{
										  
			condition.call_id = CallSequenceNumber;
			condition.src_radio = CallSrcID;
			condition.target_radio = CallTgtID;
			m_recordFile = FindOrAddRecordsItem(&condition, bFind);
			if (!bFind)
			{
				//m_recordFile = new CRecordFile(m_manager);
				m_recordFile->target_radio = CallTgtID;
				m_recordFile->src_radio = CallSrcID;
				m_recordFile->src_peer_id = CallOriginatingPeerID;
				m_recordFile->call_id = CallSequenceNumber;
				m_recordFile->src_slot = src_slot;
				m_recordFile->call_type = call_type;
				//AddRecordItem(m_recordFile);
			}
			
		}
			break;
		case DATA_TYPE_VOICE:
			Handle_P2P_Call(voiceFrame, m_ambeVoice.data);
			condition.call_id = CallSequenceNumber;
			condition.src_radio = CallSrcID;
			condition.target_radio = CallTgtID;
			m_recordFile = FindOrAddRecordsItem(&condition, bFind);
			if (!bFind)
			{
				m_recordFile->target_radio = CallTgtID;
				m_recordFile->src_radio = CallSrcID;
				m_recordFile->src_peer_id = CallOriginatingPeerID;
				m_recordFile->call_id = CallSequenceNumber;
				m_recordFile->src_slot = src_slot;
				m_recordFile->call_type = call_type;
			}
			m_recordFile->setCallStatus(VOICE_BURST);
			m_recordFile->WriteVoiceFrame(voiceFrame, 21);
			break;
		case DATA_TYPE_VOICE_TERMINATOR:
			
			Handle_P2P_Call(voiceFrame, m_ambeVoice.data);
			condition.call_id = CallSequenceNumber;
			condition.src_radio = CallSrcID;
			condition.target_radio = CallTgtID;
			m_recordFile = FindRecordsItem(&condition);
			if (m_recordFile)
			{
				m_recordFile->setCallStatus(VOICE_END_BURST);
				m_recordFile->WriteVoiceFrame(voiceFrame, 21);
				/*写入数据库*/
				m_recordFile->WriteToDb();
				/*从处理容器中移除*/
				RemoveRecordsItem(m_recordFile);
				/*删除此记录*/
				delete m_recordFile;
				m_recordFile = NULL;
			}
			break;
		default:
			break;
		}
		if (Oprate_Del == OpreateFlag)
		{
			delete currentItem;
			currentItem = NULL;
		}
	}
	m_log->AddLog("ambe thread end!");
}
unsigned int __stdcall CP2PNet::CheckRecordsThreadProc(void* pParam)
{
	CP2PNet *p = (CP2PNet*)pParam;
	if (p)
	{
		p->CheckRecordsThread();
	}
	return 0;
}
void CP2PNet::CheckRecordsThread()
{
	m_log->AddLog("CheckRecordsThread Start");
	NSRecordFile* record = NULL;
	pLinkItem curItem = NULL;
	while (m_isCheckRecords)
	{
		TRYLOCK(m_recordMutex);
		curItem = m_recordLink;
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
					removeItem(&m_recordLink, record);
					curItem->pNext = NULL;
					freeList(curItem);
					curItem = m_recordLink;
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
				removeItem(&m_recordLink, record);
				curItem->pNext = NULL;
				freeList(curItem);
				curItem = m_recordLink;
			}
		}
		RELEASELOCK(m_recordMutex);
		Sleep(SLEEP_CHECK_AMBE_THREAD);
	}
	m_log->AddLog("CheckRecordsThread End");
}
//unsigned int __stdcall CP2PNet::GetSerialThreadProc(void* pParam)
//{
//	CP2PNet *p = (CP2PNet*)pParam;
//	if (p)
//	{
//		p->GetSerialThread();
//	}
//	return 0;
//}
//void CP2PNet::GetSerialThread()
//{
//	m_log->AddLog("get serial Thread start");
//	while (m_getSerailThread)
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
//			{
//				unsigned short opcode = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_MASTER_QUERY.Opcode;
//				switch (GetXnlStatus())   
//				{
//				case WAITFOR_XNL_MASTER_STATUS_BROADCAST:  //等待接收0x0002
//					if (LE_XNL_MASTER_STATUS_BRDCST == opcode)
//					{
//						work_item_t* pItem = new work_item_t;
//						memset(pItem, 0, sizeof(work_item_t));
//						pItem->data.send_data.protocol.xcmp.Payload.DEVICE_AUTH_KEY_REQUEST.DesAddress = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_SYSMAP_BRDCST.SourceAddress;
//						SEND_LE_XCMP_XNL_DEVICE_AUTH_KEY_REQUEST(pItem);
//						AddXnlItem(pItem);
//						SetXnlStatus(WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY);   //等待接收 0x0005
//						break;
//					}
//					
//				case WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY:
//					if (LE_XNL_DEVICE_AUTH_KEY_REPLY == opcode)
//					{
//						work_item_t* pItem = new work_item_t;
//						memset(pItem, 0, sizeof(work_item_t));
//						pItem->data.send_data.protocol.xcmp.Payload.DEVICE_CONN_REQUEST.SourceAddress = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_AUTH_KEY_REPLY.TemporaryXnlAddress;
//						unsigned char * unAuth = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_AUTH_KEY_REPLY.UnencryptedAuthenticationValue;
//						unsigned char*  enAuth = pItem->data.recive_data.protocol.xcmp.Payload.DEVICE_CONN_REQUEST.EncryptedAuthenticationValue;
//						EncryptAuthenticationValue(unAuth, enAuth);
//						SEND_LE_XCMP_XNL_DEVICE_CONN_REQUEST(pItem);
//						AddXnlItem(pItem);
//						SetXnlStatus(WAITFOR_XNL_DEVICE_CONNECT_REPLY);   //等待接收 0x0007
//						break;
//					}
//				
//				case WAITFOR_XNL_DEVICE_CONNECT_REPLY:
//					if (LE_XNL_DEVICE_CONN_REPLY == opcode)
//					{
//						if (LE_XNL_XCMP_CONNECT_SUCCESS == currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_CONN_REPLY.ResultCode)
//						{
//							work_item_t* pItem = new work_item_t;
//							memset(pItem, 0, sizeof(work_item_t));
//							pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.TrascationId = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;
//							pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.XnlFlags = ++(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags);
//							SEND_LE_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(pItem);
//							AddXnlItem(pItem);
//							SetXnlStatus(WAITFOR_XNL_XCMP_READ_SERIAL_RESULT);   //等待接收序列号
//						}
//						else
//						{
//							SetXnlStatus(XNL_CONNECT);// 连接失败后重新开始连接
//						}	
//					}
//				case WAITFOR_XNL_XCMP_READ_SERIAL_RESULT:
//					if (LE_XNL_DATA_MSG == opcode)
//					{
//						work_item_t* pItem = new work_item_t;
//						memset(pItem, 0, sizeof(work_item_t));
//						pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.XnlFlags = ++(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags);
//						pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.TrascationId = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;
//						SEND_LE_XCMP_XNL_DATA_MSG_ACK(pItem);
//						AddXnlItem(pItem);
//						//若取到序列号，此处直接跳出该线程；若没有取得序列号，则继续发送获取序列号
//						unsigned short payloadOpcode =  currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.Opcode;
//						char result = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.Result;
//						if (LE_XNL_XCMP_RADIO_STATUS_REPLY == payloadOpcode && LE_XNL_XCMP_READ_SERIAL_SUCCESS == result)
//						{
//							//memcpy(m_serialNumber, &(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.ProductSerialNumber[0]), sizeof(m_serialNumber));
//							unsigned char* pSerial = &(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.ProductSerialNumber[0]);
//							SetSerialNumber(pSerial);
//							SetXnlStatus(GET_SERIAL_SUCCESS);
//						}
//						else
//						{
//							SetXnlStatus(WAITFOR_XNL_XCMP_READ_SERIAL);
//						}
//						   
//					}
//					break;
//				case GET_SERIAL_SUCCESS:
//					break;
//				default:
//					if (LE_XNL_DATA_MSG == opcode)    //接收xnl_data_msg 后，回复xnl_data_msg_ack
//					{
//						work_item_t* pItem = new work_item_t;
//						memset(pItem, 0, sizeof(work_item_t));
//						pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.XnlFlags = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags;
//						pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.TrascationId = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;
//						SEND_LE_XCMP_XNL_DATA_MSG_ACK(pItem);
//						AddXnlItem(pItem);
//					}
//					break;
//				}
//			}
//			break;
//		default:
//			break;
//		}
//	}
//	m_log->AddLog("get serial Thread end");
//}
void CP2PNet::Handle_Le_Status_Starting(work_item_t* currentItem, item_oprate_enum &OpreateFlag)
{
	int ExpectedPeers = 0;
	T_LE_PROTOCOL_91* pProtocol = &currentItem->data.recive_data.protocol.le.PROTOCOL_91;
	ExpectedPeers = pProtocol->numPeers;
	m_netParam.matser_peer_id = pProtocol->peerID;
	if (0 == ExpectedPeers)
	{
		SetLeStatus(ALIVE);
		/*发送主中继心跳包*/
		work_item_t* temp = new work_item_t;
		memset(temp, 0, sizeof(work_item_t));
		SEND_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(temp);
		SendDataToMaster(temp);
	}
	else
	{
		/*组建0x92并发送*/
		work_item_t* temp = new work_item_t;
		memset(temp, 0, sizeof(work_item_t));
		SEND_LE_NOTIFICATION_MAP_REQUEST(temp);
		SendDataToMaster(temp);
		SetLeStatus(WAITFOR_LE_NOTIFICATION_MAP_BROADCAST);
	}
	m_pSerial->SetXnlStatus(XNL_CONNECT);
	OpreateFlag = Oprate_Add;
}
void CP2PNet::Handle_Le_Status_Starting_Timeout(work_item_t* currentItem, item_oprate_enum &OpreateFlag , work_item_type_enum value)
{
	if (NULL == currentItem)
	{
		return;
	}
	switch (value)
	{
	case Send:
		if (!HandleRetryAndTimingSend(currentItem, OpreateFlag))
		{
			//DeleteWorkItem(currentItem);
		}
		break;
	case Recive:
		/*删除0x90*/
		FindTimeOutItemAndDelete(m_netParam.matser_peer_id, LE_MASTER_PEER_REGISTRATION_REQUEST);
		//DeleteWorkItem(currentItem);
		break;
	default:
		//DeleteWorkItem(currentItem);
		break;
	}
}
void CP2PNet::Handle_Le_Status_Wait_Map(work_item_t* currentItem, item_oprate_enum &OpreateFlag)
{
	SetLeStatus(ALIVE);
	/*发送主中继心跳包*/
	work_item_t * temp = new work_item_t();
	memset(temp, 0, sizeof(work_item_t));
	SEND_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(temp);
	AddWorkItem(temp);
	/*解析map表*/
	T_LE_PROTOCOL_93* pProtocol = NULL;
	pProtocol = &currentItem->data.recive_data.protocol.le.PROTOCOL_93;
	ParseMapBroadcast(pProtocol);
	OpreateFlag = Oprate_Add;
}
void CP2PNet::Handle_Le_Status_Wait_Map_Timeout(work_item_t* currentItem, item_oprate_enum &OpreateFlag, work_item_type_enum value)
{
	if (NULL == currentItem)
	{
		return;
	}
	switch (value)
	{
	case Send:
		if (!HandleRetryAndTimingSend(currentItem, OpreateFlag))
		{
			/*0x92尝试次数完毕,重新发送0x90*/
			SetLeStatus(STARTING);
			work_item_t* p = new work_item_t;
			memset(p, 0, sizeof(work_item_t));
			SEND_LE_MASTER_PEER_REGISTRATION_REQUEST(p);
			AddWorkItem(p);
			//DeleteWorkItem(currentItem);
		}
		break;
	case Recive:
		/*删除0x92*/
		FindTimeOutItemAndDelete(m_netParam.matser_peer_id, LE_NOTIFICATION_MAP_REQUEST);
		//DeleteWorkItem(currentItem);
		break;
	default:
		//DeleteWorkItem(currentItem);
		break;
	}
}
void CP2PNet::Handle_Le_Status_Alive(const char Opcode, work_item_t* currentItem, item_oprate_enum &OpreateFlag)
{
	OpreateFlag = Oprate_Add;
	work_mode_enum workMode = m_netParam.work_mode;
	find_peer_condition_t condition = { 0 };
	CP2PPeer* pPeer = NULL;
	switch (Opcode)
	{
	case P2P_GRP_VOICE_CALL:
	case P2P_PVT_VOICE_CALL:
	case P2P_GRP_DATA_CALL:
	case P2P_PVT_DATA_CALL:   //p2p语音
	{
		/*交给ambe线程处理*/
		OpreateFlag = Oprate_Other;  
		AddAmbeItem(currentItem);
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
		pProtocol = &currentItem->data.recive_data.protocol.le.PROTOCOL_94;
		condition.peer_id = pProtocol->peerID;
		pPeer = FindPeersItem(&condition);   //判断该peer是否在map中，如果在，则回复注册，如果不存在，则不用做任何操作
		if (pPeer)
		{
			pPeer->Handle_NetPack(LE_PEER_REGISTRATION_REQUEST);
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
	case LE_NOTIFICATION_MAP_BROADCAST:
	{
			/*Map表通知*/
			T_LE_PROTOCOL_93* pProtocol = NULL;
			T_LE_PROTOCOL_93_LCP* pProtocolLcp = NULL;
			pProtocol = &currentItem->data.recive_data.protocol.le.PROTOCOL_93;
			/*解析map表*/
			ParseMapBroadcast(pProtocol);
	}
		break;
	default:
		OpreateFlag = Oprate_Del;
		break;
	}
}
void CP2PNet::Handle_Le_Status_Alive_Timeout(const char Opcode, work_item_t* currentItem, item_oprate_enum &OpreateFlag, work_item_type_enum value)
{
	if (NULL == currentItem)
	{
		return;
	}
	switch (value)
	{
	case Send:
	{
		switch (Opcode)
		{
		case  LE_MASTER_KEEP_ALIVE_REQUEST:    //0x96
		{
			if (!HandleRetryAndTimingSend(currentItem, OpreateFlag))
			{
				/*0x96尝试次数完毕,重新发送0x90*/
				SetLeStatus(STARTING);
				work_item_t* p = new work_item_t;
				memset(p, 0, sizeof(work_item_t));
				SEND_LE_MASTER_PEER_REGISTRATION_REQUEST(p);
				AddWorkItem(p);
				//DeleteWorkItem(currentItem);
			}
		}
			break;
		case LE_PEER_REGISTRATION_REQUEST:   //0x94
		{
			HandleRetryAndTimingSend(currentItem, OpreateFlag);
		}
			break;
		case LE_PEER_REGISTRATION_RESPONSE:
			//DeleteWorkItem(currentItem);
			break;
		case LE_PEER_KEEP_ALIVE_REQUEST:   //0x98    
		{
			if (!HandleRetryAndTimingSend(currentItem, OpreateFlag))
			{
				CP2PPeer* peer = (CP2PPeer*)currentItem->data.send_data.pFrom;
				if (peer)
				{
					peer->LE_PEER_REGISTRATION();
				}
				//DeleteWorkItem(currentItem);
			}
		}
			break;
		case LE_PEER_KEEP_ALIVE_RESPONSE:   //0x99
			//DeleteWorkItem(currentItem);
			break;
		default:
			//DeleteWorkItem(currentItem);
			break;
		}
	}
		break;
	case Recive:
	{
		find_peer_condition_t condition = { 0 };
		CP2PPeer* peer = NULL;
		switch (Opcode)
		{
		case LE_PEER_REGISTRATION_RESPONSE:    //0x95
		{
			T_LE_PROTOCOL_95* protocol = NULL;
			protocol = &currentItem->data.recive_data.protocol.le.PROTOCOL_95;
			/*删除0x94*/
			FindTimeOutItemAndDelete(protocol->peerID, LE_PEER_REGISTRATION_REQUEST);
			/*定时发送0x98*/
			condition.peer_id = protocol->peerID;
			peer = FindPeersItem(&condition);
			if (peer)
			{
				work_item_t* p = new work_item_t;
				memset(p, 0, sizeof(work_item_t));
				peer->SEND_LE_PEER_KEEP_ALIVE_REQUEST(p, g_timing_alive_time_peer);
				AddTimeoutItem(p);
			}
			//DeleteWorkItem(currentItem); 
		}
			break;
		case LE_MASTER_KEEP_ALIVE_RESPONSE:  //0x97
		{
			/*删除0x96*/
			FindTimeOutItemAndDelete(m_netParam.matser_peer_id, LE_MASTER_KEEP_ALIVE_REQUEST);
			/*定时发送0x96*/
			work_item_t* p = new work_item_t;
			memset(p, 0, sizeof(work_item_t));
			SEND_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(p, g_timing_alive_time_master);
			AddTimeoutItem(p);
			//DeleteWorkItem(currentItem);
		}
			break;
		case LE_PEER_KEEP_ALIVE_REQUEST:    //0x98
		{
			T_LE_PROTOCOL_98* protocol = NULL;
			bool is3rd = false;
			protocol = &currentItem->data.recive_data.protocol.le.PROTOCOL_98;
			if (protocol->peerServices & 0x00002000) is3rd = true;
			/*删除0x98*/
			//FindTimeOutItemAndDelete(protocol->peerID, LE_PEER_KEEP_ALIVE_REQUEST);
			condition.peer_id = protocol->peerID;
			peer = FindPeersItem(&condition);
			if (peer)
			{
				/*发送0x99*/
				work_item_t* p = new work_item_t;
				memset(p, 0, sizeof(work_item_t));
				peer->SEND_LE_PEER_KEEP_ALIVE_RESPONSE(p);
				AddWorkItem(p);
				/*发送0x98*/
				p = new work_item_t;
				memset(p, 0, sizeof(work_item_t));
				peer->SEND_LE_PEER_KEEP_ALIVE_REQUEST(p);
				AddWorkItem(p);
				//peer->setRemote3rdParty(is3rd);
				//peer->Handle_LE_PEER_KEEP_ALIVE_REQUEST_Recive();
			}		
			//DeleteWorkItem(currentItem);
		}
			break;
		case LE_PEER_KEEP_ALIVE_RESPONSE:  //0x99
		{
			T_LE_PROTOCOL_99* protocol = NULL;							
			protocol = &currentItem->data.recive_data.protocol.le.PROTOCOL_99;
			/*删除0x98*/
			FindTimeOutItemAndDelete(protocol->peerID, LE_PEER_KEEP_ALIVE_REQUEST);
			condition.peer_id = protocol->peerID;
			peer = FindPeersItem(&condition);
			if (peer)
			{
				/*发送0x98*/
				work_item_t* p = new work_item_t;
				memset(p, 0, sizeof(work_item_t));
				peer->SEND_LE_PEER_KEEP_ALIVE_REQUEST(p, g_timing_alive_time_peer);
				AddTimeoutItem(p);
			}
			//DeleteWorkItem(currentItem);
											
		}
			break;
		default:
			//DeleteWorkItem(currentItem);
			break;
		}
	}
		break;
	default:
		//DeleteWorkItem(currentItem);
		break;
	}
	
}
bool CP2PNet::HandleRetryAndTimingSend(work_item_t* curItem, item_oprate_enum &OpreateFlag)
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
			AddTimeoutItem(curItem);
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
			AddTimeoutItem(curItem);
		}
	}
	return rlt;
}
work_item_t* CP2PNet::PopWorkItem()
{
	work_item_t* p = NULL;
	TRYLOCK(m_p2pDataMutex);
	pLinkItem it = popFront(&m_p2pDataLink);
	if (NULL != it)
	{
		if (NULL != it->data)
		{
			p = (work_item_t*)it->data;
		}
		freeList(it);
	}
	RELEASELOCK(m_p2pDataMutex);
	return p;
}
work_item_t* CP2PNet::PopTimeoutItem()
{
	work_item_t* p = NULL;
	TRYLOCK(m_p2pTimeoutDataMutex);
	pLinkItem it = popFront(&m_p2pTimeoutDataLink);
	if (NULL != it)
	{
		if (NULL != it->data)
		{
			p = (work_item_t*)it->data;
		}
		freeList(it);
	}
	RELEASELOCK(m_p2pTimeoutDataMutex);
	return p;
}
work_item_t* CP2PNet::PopAmbeItem()
{
	work_item_t* p = NULL;
	TRYLOCK(m_ambeMutex);
	pLinkItem it = popFront(&m_ambeLink);
	if (NULL != it)
	{
		if (NULL != it->data)
		{
			p = (work_item_t*)it->data;
		}
		freeList(it);
	}
	RELEASELOCK(m_ambeMutex);
	return p;
}
//work_item_t* CP2PNet::PopSerialItem()
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
void CP2PNet::OnRecv(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len)
{
	CP2PNet *p = (CP2PNet*)pNet->param;
	if (p)
	{
		p->onRecv(pNet, pNetClient,pData,len);
	}
}
void CP2PNet::OnDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode)
{
	CP2PNet *p = (CP2PNet*)pNet->param;
	if (p)
	{
		p->onDisconn(pNet, pNetClient, errCode);
	}
}
void CP2PNet::OnError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode)
{
	CP2PNet *p = (CP2PNet *)pNet->param;
	if (p)
	{
		p->OnError(pNet, pNetClient, errCode);
	}
}
void CP2PNet::OnSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient)
{
	CP2PNet*p = (CP2PNet*)pNet->param;
	if (p)
	{
		p->onSendComplete(pNet, pNetClient);
	}
}

void CP2PNet::onRecv(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len)
{
	m_log->AddLog("onRecv");
	if (NULL == pData)
	{
		return;
	}
	unsigned char m_callType = 0;
	switch (pData[0])
	{
	case LE_MASTER_PEER_REGISTRATION_REQUEST:
		break;
	case LE_MASTER_PEER_REGISTRATION_RESPONSE:
		if (STARTING == GetLeStatus())
		{
			work_item_t* temp = new work_item_t;
			memset(temp, 0, sizeof(work_item_t));
			RECV_LE_MASTER_PEER_REGISTRATION_RESPONSE(temp,pData);
			AddWorkItem(temp);
			break;
		}
	case LE_NOTIFICATION_MAP_REQUEST:
		break;
	case LE_NOTIFICATION_MAP_BROADCAST:
		if (WAITFOR_LE_NOTIFICATION_MAP_BROADCAST == GetLeStatus())
		{
			work_item_t* temp = new work_item_t;
			memset(temp, 0, sizeof(work_item_t));
			temp->data.recive_data.protocol.le.PROTOCOL_93.length = len;
			RECV_LE_NOTIFICATION_MAP_BROADCAST(temp, pData);
			AddWorkItem(temp);
			break;
		}
		break;
	case LE_PEER_REGISTRATION_REQUEST:
		if (ALIVE == GetLeStatus())
		{
			work_item_t* temp = new work_item_t;
			memset(temp, 0, sizeof(work_item_t));
			temp->data.recive_data.protocol.le.PROTOCOL_94.length = len;
			RECV_LE_PEER_REGISTRATION_REQUEST(temp, pData);
			AddWorkItem(temp);
			break;
		}
		break;
	case LE_PEER_REGISTRATION_RESPONSE:
		if (ALIVE == GetLeStatus())
		{
			work_item_t* temp = new work_item_t;
			memset(temp, 0, sizeof(work_item_t));
			temp->data.recive_data.protocol.le.PROTOCOL_95.length = len;
			RECV_LE_PEER_REGISTRATION_RESPONSE(temp, pData);
			AddWorkItem(temp);
			break;
		}
		break;
	case LE_MASTER_KEEP_ALIVE_REQUEST:
		/*if (ALIVE == GetLeStatus())
		{
			work_item_t* temp = new work_item_t;
			memset(temp, 0, sizeof(work_item_t));
			temp->data.recive_data.protocol.le.PROTOCOL_97.length = len;
			RECV_LE_MASTER_KEEP_ALIVE_RESPONSE(temp, pData);
			AddWorkItem(temp);
			break;
		}*/
		break;
	case LE_MASTER_KEEP_ALIVE_RESPONSE:
		if (ALIVE == GetLeStatus())
		{
			work_item_t* temp = new work_item_t;
			memset(temp, 0, sizeof(work_item_t));
			temp->data.recive_data.protocol.le.PROTOCOL_97.length = len;
			RECV_LE_MASTER_KEEP_ALIVE_RESPONSE(temp, pData);
			AddWorkItem(temp);
			break;
		}
		break;
	case LE_PEER_KEEP_ALIVE_REQUEST:
		if (ALIVE == GetLeStatus())
		{
			work_item_t* temp = new work_item_t;
			memset(temp, 0, sizeof(work_item_t));
			temp->data.recive_data.protocol.le.PROTOCOL_98.length = len;
			RECV_LE_PEER_KEEP_ALIVE_REQUEST(temp, pData);
			AddWorkItem(temp);
			break;
		}
		break;
	case LE_PEER_KEEP_ALIVE_RESPONSE:
		if (ALIVE == GetLeStatus())
		{
			work_item_t* temp = new work_item_t;
			memset(temp, 0, sizeof(work_item_t));
			temp->data.recive_data.protocol.le.PROTOCOL_99.length = len;
			RECV_LE_PEER_KEEP_ALIVE_RESPONSE(temp, pData);
			AddWorkItem(temp);
			break;
		}
		break;
	case LE_DEREGISTRATION_REQUEST:
			break;
	case LE_DEREGISTRATION_RESPONSE:
		break;
	case P2P_GRP_VOICE_CALL:
		m_callType = GroupCall;
	case P2P_PVT_VOICE_CALL:
		m_callType = IndividualCall;
	case P2P_GRP_DATA_CALL:
		m_callType = GroupCall;
	case P2P_PVT_DATA_CALL:
		m_callType = IndividualCall;
	{
		work_item_t* temp = new work_item_t;
		memset(temp, 0, sizeof(work_item_t));
		RECV_VOICE_CALL(temp, pData,m_callType);
		AddWorkItem(temp);
	}
		break;
	case LE_XNL:
		//if (LE_XNL == GetXnlStatus())
		{
			work_item_t* temp = new work_item_t;
			memset(temp, 0, sizeof(work_item_t));
			m_pSerial->RECV_LE_XCMP_XNL(temp, pData);
			m_pSerial->AddXnlItem(temp);
		}
		break;
	default:
		break;
	}
}
void CP2PNet::onDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode)
{
	m_log->AddLog("onDisconn");
	UnInit();
}
void CP2PNet::onError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode)
{
	m_log->AddLog("onError");
	UnInit();
}
void CP2PNet::onSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient)
{
	m_log->AddLog("onSendComplete");
}
void CP2PNet::SEND_LE_MASTER_PEER_REGISTRATION_REQUEST(work_item_t* w)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = &m_masterSocket;
	w->data.send_data.timeout_try = TIMEOUT_TRY_COUNT;   
	if (IPSC == m_netParam.work_mode)
	{

	}
	else if (CPC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_90 networkData = { 0 };
		networkData.Opcode = LE_MASTER_PEER_REGISTRATION_REQUEST;
		networkData.peerID = m_netParam.local_peer_id;
		networkData.currentLinkProtocolVersion = P2P_CPC_CURRENTLPVERSION;
		networkData.oldestLinkProtocolVersion = P2P_CPC_OLDESTPVERSION;
		networkData.peerMode = P2P_CPC_MODE;
		networkData.peerServices = P2P_CPC_SERVICES;
		w->data.send_data.protocol.le.PROTOCOL_90 = networkData;
		w->data.send_data.net_lenth = Build_LE_MASTER_PEER_REGISTRATION_REQUEST(w->data.send_data.net_data, &networkData); 
		
	}
}
void CP2PNet::RECV_LE_MASTER_PEER_REGISTRATION_RESPONSE(work_item_t * w, const char* pData)
{
	if (NULL == pData)
	{
		return;
	}
	w->type = Recive;
	Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE((char*)pData, w->data.recive_data.protocol.le.PROTOCOL_91);
}
void CP2PNet::SEND_LE_NOTIFICATION_MAP_REQUEST(work_item_t* w)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = &m_masterSocket;
	w->data.send_data.timeout_try = 0;   
	T_LE_PROTOCOL_92 networkData = {0};
	networkData.Opcode = LE_NOTIFICATION_MAP_REQUEST;
	networkData.peerID = m_netParam.local_peer_id;
	w->data.send_data.protocol.le.PROTOCOL_92 = networkData;
	w->data.send_data.net_lenth = Build_LE_NOTIFICATION_MAP_REQUEST(w->data.send_data.net_data, &networkData);
}
void CP2PNet::RECV_LE_NOTIFICATION_MAP_BROADCAST(work_item_t * w, const char* pData)
{
	if (NULL == pData)
	{
		return;
	}
	w->type = Recive;
	Unpack_LE_NOTIFICATION_MAP_BROADCAST((char*)pData, w->data.recive_data.protocol.le.PROTOCOL_93);
}
void CP2PNet::RECV_LE_PEER_REGISTRATION_REQUEST(work_item_t * w, const char* pData)
{
	if (NULL == pData)
	{
		return;
	}
	w->type = Recive;
	Unpack_LE_PEER_REGISTRATION_REQUEST((char*)pData, &(w->data.recive_data.protocol.le.PROTOCOL_94));
}
void CP2PNet::RECV_LE_PEER_REGISTRATION_RESPONSE(work_item_t * w, const char* pData)
{
	if (NULL == pData)
	{
		return;
	}
	w->type = Recive;
	Unpack_LE_PEER_REGISTRATION_RESPONSE((char*)pData, &(w->data.recive_data.protocol.le.PROTOCOL_95));
}
void CP2PNet::RECV_LE_MASTER_KEEP_ALIVE_RESPONSE(work_item_t * w, const char* pData)
{
	if (NULL == pData)
	{
		return;
	}
	w->type = Recive;
	Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE((char*)pData, &(w->data.recive_data.protocol.le.PROTOCOL_97));
}
void CP2PNet::RECV_LE_PEER_KEEP_ALIVE_REQUEST(work_item_t * w, const char* pData)
{
	if (NULL == pData)
	{
		return;
	}
	w->type = Recive;
	Unpack_LE_PEER_KEEP_ALIVE_REQUEST((char*)pData, &(w->data.recive_data.protocol.le.PROTOCOL_98));
}
void CP2PNet::RECV_LE_PEER_KEEP_ALIVE_RESPONSE(work_item_t * w, const char* pData)
{
	if (NULL == pData)
	{
		return;
	}
	w->type = Recive;
	Unpack_LE_PEER_KEEP_ALIVE_RESPONSE((char*)pData, &(w->data.recive_data.protocol.le.PROTOCOL_99));
}
void CP2PNet::RECV_VOICE_CALL(work_item_t *w, const char * pData,unsigned char callType)
{
	if (NULL == pData)
	{
		return;
	}
	w->type = Recive;
	Unpack_VOICE_CALL((char*)pData, &(w->data.recive_data.protocol.p2p.P2PCall),callType);
}
void CP2PNet::SEND_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(work_item_t* w, unsigned long timing)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = &m_masterSocket;
	w->data.send_data.timeout_try = TIMEOUT_TRY_COUNT;
	if (0 != timing)
	{
		w->data.send_data.timing = GetTickCount() + timing;
	}
	if (IPSC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_96 networkData = {0};
		networkData.Opcode = LE_MASTER_KEEP_ALIVE_REQUEST;
		networkData.peerID = m_netParam.local_peer_id;
		networkData.currentLinkProtocolVersion = P2P_IPSC_CURRENTLPVERSION;
		networkData.oldestLinkProtocolVersion = P2P_IPSC_OLDESTPVERSION;
		networkData.peerMode = P2P_IPSC_MODE;
		networkData.peerServices = P2P_IPSC_SERVICES;
		w->data.send_data.protocol.le.PROTOCOL_96 = networkData;
		w->data.send_data.net_lenth = Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(w->data.send_data.net_data, &networkData);
	}
	else if (CPC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_96 networkData = {0};
		networkData.Opcode = LE_MASTER_KEEP_ALIVE_REQUEST;
		networkData.peerID = m_netParam.local_peer_id;
		networkData.currentLinkProtocolVersion = P2P_CPC_CURRENTLPVERSION;
		networkData.oldestLinkProtocolVersion = P2P_CPC_OLDESTPVERSION;
		networkData.peerMode = P2P_CPC_MODE;
		networkData.peerServices = P2P_CPC_SERVICES;
		w->data.send_data.protocol.le.PROTOCOL_96 = networkData;
		w->data.send_data.net_lenth = Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(w->data.send_data.net_data, &networkData);
	}
}
//void CP2PNet::SEND_LE_XCMP_XNL_DEVICE_MASTER_QUERY(work_item_t* w)
//{
//	if (NULL == w)
//	{
//		return;
//	}
//	w->type = Send;
//	w->data.send_data.send_to = &m_masterSocket;
//	w->data.send_data.timeout_try = 0;
//	T_XNL_DEVICE_MASTER_QUERY networkData = {0};
//	networkData.Opcode = LE_XNL_DEVICE_MASTER_QUERY;
//	networkData.ProtocolId = LE_XNL_PROTOCOL_ID;
//	networkData.XnlFlags = LE_XNL_FLAGS;
//	networkData.SourceAddress = m_sourceAddress;
//	networkData.DesAddress = m_desAddress;
//	networkData.TrascationId = m_trascationId;
//	networkData.PayloadLength = 0;
//	char buf[BUFSIZ] = { 0 };
//	int len = Build_XCMP_XNL_DEVICE_MASTER_QUERY(buf, &networkData);
//	moto_protocol_xcmp_xnl_t m_moto_protocol_xcmp_xnl_t = { 0 };
//	m_moto_protocol_xcmp_xnl_t.Opcode = LE_XNL;
//	m_moto_protocol_xcmp_xnl_t.PeerId = m_netParam.local_peer_id;
//	m_moto_protocol_xcmp_xnl_t.lenth = sizeof(m_moto_protocol_xcmp_xnl_t.Payload.DEVICE_MASTER_QUERY);
//	w->data.send_data.protocol.xcmp = m_moto_protocol_xcmp_xnl_t;
//	w->data.send_data.net_lenth = Build_LE_XCMP_XNL(w->data.send_data.net_data, &m_moto_protocol_xcmp_xnl_t, buf, len);
//}
//void CP2PNet::SEND_LE_XCMP_XNL_DEVICE_AUTH_KEY_REQUEST(work_item_t* w)
//{
//	if (NULL == w)
//	{
//		return;
//	}
//	w->type = Send;
//	w->data.send_data.send_to = &m_masterSocket;
//	w->data.send_data.timeout_try = 0;
//	T_XNL_DEVICE_AUTH_KEY_REQUEST networkData = { 0 };
//	networkData.Opcode = LE_XNL_DEVICE_AUTH_KEY_REQUEST;
//	networkData.ProtocolId = LE_XNL_PROTOCOL_ID;
//	networkData.XnlFlags = LE_XNL_FLAGS;
//	networkData.SourceAddress = m_sourceAddress;
//	networkData.DesAddress = m_desAddress;
//	networkData.TrascationId = m_trascationId;
//	networkData.PayloadLength = 0;
//	char buf[BUFSIZ] = { 0 };
//	int len = Build_XCMP_XNL_DEVICE_AUTH_KEY_REQUEST(buf, &networkData);
//	moto_protocol_xcmp_xnl_t m_moto_protocol_xcmp_xnl_t = { 0 };
//	m_moto_protocol_xcmp_xnl_t.Opcode = LE_XNL;
//	m_moto_protocol_xcmp_xnl_t.PeerId = m_netParam.local_peer_id;
//	m_moto_protocol_xcmp_xnl_t.lenth = sizeof(m_moto_protocol_xcmp_xnl_t.Payload.DEVICE_AUTH_KEY_REQUEST);
//	w->data.send_data.protocol.xcmp = m_moto_protocol_xcmp_xnl_t;
//	w->data.send_data.net_lenth = Build_LE_XCMP_XNL(w->data.send_data.net_data, &m_moto_protocol_xcmp_xnl_t, buf, len);
//}
//void CP2PNet::SEND_LE_XCMP_XNL_DEVICE_CONN_REQUEST(work_item_t* w)
//{
//	if (NULL == w)
//	{
//		return;
//	}
//	w->type = Send;
//	w->data.send_data.send_to = &m_masterSocket;
//	w->data.send_data.timeout_try = 0;
//	T_XNL_DEVICE_CONN_REQUEST networkData = { 0 };
//	networkData.Opcode = LE_XNL_DEVICE_CONN_REQUEST;
//	networkData.ProtocolId = LE_XNL_PROTOCOL_ID;
//	networkData.XnlFlags = LE_XNL_FLAGS;
//	networkData.DesAddress = m_desAddress;
//	networkData.SourceAddress = w->data.send_data.protocol.xcmp.Payload.DEVICE_CONN_REQUEST.SourceAddress;
//	networkData.TrascationId = m_trascationId;
//	networkData.PayloadLength = 0X000C;
//	networkData.PreferredXnlAddress = m_sourceAddress;   //m_source 0x00
//	networkData.DeviceType = LE_XNL_DEVICE_TYPE;
//	networkData.AuthenticationIndex = LE_XNL_AUTHENTICATION_INDEX;
//	memcpy(networkData.EncryptedAuthenticationValue, &(w->data.send_data.protocol.xcmp.Payload.DEVICE_CONN_REQUEST.EncryptedAuthenticationValue), 8);
//	char buf[BUFSIZ] = { 0 };
//	int len = Build_XCMP_XNL_DEVICE_CONN_REQUEST(buf, &networkData);
//	moto_protocol_xcmp_xnl_t m_moto_protocol_xcmp_xnl_t = { 0 };
//	m_moto_protocol_xcmp_xnl_t.Opcode = LE_XNL;
//	m_moto_protocol_xcmp_xnl_t.PeerId = m_netParam.local_peer_id;
//	m_moto_protocol_xcmp_xnl_t.lenth = sizeof(m_moto_protocol_xcmp_xnl_t.Payload.DEVICE_CONN_REQUEST);
//	w->data.send_data.protocol.xcmp = m_moto_protocol_xcmp_xnl_t;
//	w->data.send_data.net_lenth = Build_LE_XCMP_XNL(w->data.send_data.net_data, &m_moto_protocol_xcmp_xnl_t, buf, len);
//}
//void CP2PNet::SEND_LE_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(work_item_t* w)
//{
//	if (NULL == w)
//	{
//		return;
//	}
//	w->type = Send;
//	w->data.send_data.send_to = &m_masterSocket;
//	w->data.send_data.timeout_try = 0;
//	T_XNL_DATA_MSG networkData = { 0 };
//	networkData.Opcode = LE_XNL_DATA_MSG;
//	networkData.ProtocolId = LE_XNL_PROTOCOL_ID+1;
//	networkData.XnlFlags = w->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags;
//	networkData.DesAddress = m_desAddress;
//	networkData.SourceAddress = m_sourceAddress;
//	networkData.TrascationId = w->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;
//	networkData.PayloadLength = 0X0003;
//	networkData.Payload.RadioStatus.RadioStatusRequest.Opcode = LE_XNL_XCMP_RADIO_STATUS;
//	networkData.Payload.RadioStatus.RadioStatusRequest.Condition = LE_XNL_XCMP_RADIO_STATUS_CONDITION_SERIAL;
//	char buf[BUFSIZ] = { 0 };
//	int len = Build_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(buf, &networkData);
//	moto_protocol_xcmp_xnl_t m_moto_protocol_xcmp_xnl_t = { 0 };
//	m_moto_protocol_xcmp_xnl_t.Opcode = LE_XNL;
//	m_moto_protocol_xcmp_xnl_t.PeerId = m_netParam.local_peer_id;
//	m_moto_protocol_xcmp_xnl_t.lenth = 15 /*sizeof(m_moto_protocol_xcmp_xnl_t.Payload.XNL_DATA_MSG)*/;
//	w->data.send_data.protocol.xcmp = m_moto_protocol_xcmp_xnl_t;
//	w->data.send_data.net_lenth = Build_LE_XCMP_XNL(w->data.send_data.net_data, &m_moto_protocol_xcmp_xnl_t, buf, len);
//}
//void CP2PNet::SEND_LE_XCMP_XNL_DATA_MSG_DEVICE_INIT_STATUS(work_item_t* w)
//{
//	if (NULL == w)
//	{
//		return;
//	}
//	w->type = Send;
//	w->data.send_data.send_to = &m_masterSocket;
//	w->data.send_data.timeout_try = 0;
//	T_XNL_DATA_MSG networkData = { 0 };
//	networkData.Opcode = LE_XNL_DATA_MSG;
//	networkData.ProtocolId = LE_XNL_PROTOCOL_ID + 1;
//	networkData.XnlFlags = w->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags;;
//	networkData.DesAddress = m_desAddress;
//	networkData.SourceAddress = m_sourceAddress;
//	networkData.TrascationId = w->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;;
//	networkData.PayloadLength = 0x000b;
//	networkData.Payload.DeviceInit.Opcode = LE_XNL_XCMP_DEVICE_INIT;
//	networkData.Payload.DeviceInit.Version = LE_XNL_XCMP_VERSION;
//	unsigned char temp[3] = { 0x0a, 0x00, 0x00 };
//	memcpy(networkData.Payload.DeviceInit.DeviceInitType, temp, 3);
//	char buf[BUFSIZ] = { 0 };
//	int len = Build_XCMP_XNL_DATA_MSG_DEVICE_INIT_STATUS(buf, &networkData);
//	moto_protocol_xcmp_xnl_t m_moto_protocol_xcmp_xnl_t = { 0 };
//	m_moto_protocol_xcmp_xnl_t.Opcode = LE_XNL;
//	m_moto_protocol_xcmp_xnl_t.PeerId = m_selfPeerId;
//	m_moto_protocol_xcmp_xnl_t.lenth = 23/*sizeof(m_moto_protocol_xcmp_xnl_t.Payload.XNL_DATA_MSG.Payload.DeviceInit)*/;
//	w->data.send_data.net_lenth = Build_LE_XCMP_XNL(w->data.send_data.net_data, &m_moto_protocol_xcmp_xnl_t, buf, len);
//}
//void CP2PNet::SEND_LE_XCMP_XNL_DATA_MSG_ACK(work_item_t* w)
//{
//	if (NULL == w)
//	{
//		return;
//	}
//	w->type = Send;
//	w->data.send_data.send_to = &m_masterSocket;
//	w->data.send_data.timeout_try = 0;
//	T_XNL_DATA_MSG_ACK networkData = { 0 };
//	networkData.Opcode = LE_XNL_DATA_MSG_ACK;
//	networkData.ProtocolId = LE_XNL_PROTOCOL_ID;
//	//networkData.XnlFlags = LE_XNL_FLAGS;
//	networkData.DesAddress = m_desAddress;
//	networkData.SourceAddress = m_sourceAddress;
//	networkData.TrascationId = w->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.TrascationId;
//	networkData.PayloadLength = 0x0000;
//	char buf[BUFSIZ] = { 0 };
//	int len = Build_XCMP_XNL_DATA_MSG_ACK(buf, &networkData);
//	moto_protocol_xcmp_xnl_t m_moto_protocol_xcmp_xnl_t = { 0 };
//	m_moto_protocol_xcmp_xnl_t.Opcode = LE_XNL;
//	m_moto_protocol_xcmp_xnl_t.PeerId = m_netParam.local_peer_id;
//	m_moto_protocol_xcmp_xnl_t.lenth = sizeof(m_moto_protocol_xcmp_xnl_t.Payload.XNL_DATA_MSG);
//	w->data.send_data.net_lenth = Build_LE_XCMP_XNL(w->data.send_data.net_data, &m_moto_protocol_xcmp_xnl_t, buf, len);
//}
//void CP2PNet::RECV_LE_XCMP_XNL(work_item_t * w, const char* pData)
//{
//	if (NULL == pData)
//	{
//		return;
//	}
//	w->type = Recive;
//	switch (ntohs(*(WORD*)(&pData[7])))
//	{
//	case LE_XNL_MASTER_STATUS_BRDCST:
//		Unpack_XCMP_XNL_MASTER_STATUS_BRDCST((char*)pData,&(w->data.recive_data.protocol.xcmp));
//		break;
//	case LE_XNL_DEVICE_AUTH_KEY_REPLY:
//		Unpack_XCMP_XNL_DEVICE_AUTH_KEY_REPLY((char*)pData, &(w->data.recive_data.protocol.xcmp));
//		break;
//	case LE_XNL_DEVICE_CONN_REPLY:
//		Unpack_XCMP_XNL_DEVICE_CONN_REPLY((char*)pData, &(w->data.recive_data.protocol.xcmp));
//		break;
//	case LE_XNL_DEVICE_SYSMAP_BRDCST:
//		break;
//	case LE_XNL_DATA_MSG:
//		Unpack_XCMP_XNL_DATA_MSG((char*)pData, &(w->data.recive_data.protocol.xcmp));
//		break;
//	default:
//		break;
//	}
//}
DWORD CP2PNet::Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90* pData)
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
DWORD CP2PNet::Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92* pData)
{
	DWORD size;
	/*Opcode*/
	pPacket[0] = pData->Opcode;
	/*peerID*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->peerID);
	size = 5;
	return size;
}
DWORD CP2PNet::Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96* pData)
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
//DWORD CP2PNet::Build_LE_XCMP_XNL(CHAR* pPacket, moto_protocol_xcmp_xnl_t* pData, CHAR* pXnlPacket,int length)
//{
//	/*unsigned char Opcode;
//	unsigned long PeerId;
//	unsigned short lenth;
//	T_XNL          Payload;*/
//	/*opcode*/
//	pPacket[0] = pData->Opcode;
//	/* peer id*/
//	*((DWORD*)(&pPacket[1])) = htonl(pData->PeerId);
//	/*length*/
//	*((WORD*)(&pPacket[5])) = htons(pData->lenth);
//	/*payload*/
//	memcpy(&pPacket[7],pXnlPacket,length);
//	return length + 7;
//
//}
//DWORD CP2PNet::Build_XCMP_XNL_DEVICE_MASTER_QUERY(CHAR* pPacket, T_XNL_DEVICE_MASTER_QUERY* pData)
//{
//	//unsigned short Opcode;
//	//unsigned char  ProtocolId;
//	//unsigned char  XnlFlags;
//	//unsigned short DesAddress;
//	//unsigned short SourceAddress;
//	//unsigned short TrascationId;
//	//unsigned short PayloadLength;
//	DWORD size;
//	/*Opcode*/
//	*((WORD*)(&pPacket[0])) = htons(pData->Opcode);
//	/*ProtocolId*/
//	pPacket[2] = pData->ProtocolId;
//	/*XnlFlags*/
//	pPacket[3] = pData->XnlFlags;
//	/*DesAddress*/
//	*((WORD*)(&pPacket[4])) = htons(pData->DesAddress);
//	/*SourceAddress*/
//	*((WORD*)(&pPacket[6])) = htons(pData->SourceAddress);
//	/*TrascationId*/
//	*((WORD*)(&pPacket[8])) = htons(pData->TrascationId);
//	/*PayloadLength*/
//	*((WORD*)(&pPacket[10])) = htons(pData->PayloadLength);
//	size = 12;
//	return size;
//}
//DWORD CP2PNet::Build_XCMP_XNL_DEVICE_AUTH_KEY_REQUEST(CHAR* pPacket, T_XNL_DEVICE_AUTH_KEY_REQUEST* pData)
//{
//	DWORD size;
//	/*Opcode*/
//	*((WORD*)(&pPacket[0])) = htons(pData->Opcode);
//	/*ProtocolId*/
//	pPacket[2] = pData->ProtocolId;
//	/*XnlFlags*/
//	pPacket[3] = pData->XnlFlags;
//	/*DesAddress*/
//	*((WORD*)(&pPacket[4])) = htons(pData->DesAddress);
//	/*SourceAddress*/
//	*((WORD*)(&pPacket[6])) = htons(pData->SourceAddress);
//	/*TrascationId*/
//	*((WORD*)(&pPacket[8])) = htons(pData->TrascationId);
//	/*PayloadLength*/
//	*((WORD*)(&pPacket[10])) = htons(pData->PayloadLength);
//	size = 12;
//	return size;
//}
//DWORD CP2PNet::Build_XCMP_XNL_DEVICE_CONN_REQUEST(CHAR* pPacket, T_XNL_DEVICE_CONN_REQUEST* pData)
//{
//	DWORD size;
//	/*Opcode*/
//	*((WORD*)(&pPacket[0])) = htons(pData->Opcode);
//	/*ProtocolId*/
//	pPacket[2] = pData->ProtocolId;
//	/*XnlFlags*/
//	pPacket[3] = pData->XnlFlags;
//	/*DesAddress*/
//	*((WORD*)(&pPacket[4])) = htons(pData->DesAddress);
//	/*SourceAddress*/
//	*((WORD*)(&pPacket[6])) = htons(pData->SourceAddress);
//	/*TrascationId*/
//	*((WORD*)(&pPacket[8])) = htons(pData->TrascationId);
//	/*PayloadLength*/
//	*((WORD*)(&pPacket[10])) = htons(pData->PayloadLength);
//	/*PreferredXnlAddress*/
//	*((WORD*)(&pPacket[12])) = htons(pData->PreferredXnlAddress);
//	pPacket[14] = pData->DeviceType;
//	pPacket[15] = pData->AuthenticationIndex;
//	memcpy(&pPacket[16], pData->EncryptedAuthenticationValue, 8);
//	size = 24;
//	return size;
//}
//DWORD CP2PNet::Build_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(CHAR* pPacket, T_XNL_DATA_MSG* pData)
//{
//	DWORD size;
//	/*Opcode*/
//	*((WORD*)(&pPacket[0])) = htons(pData->Opcode);
//	/*ProtocolId*/
//	pPacket[2] = pData->ProtocolId;
//	/*XnlFlags*/
//	pPacket[3] = pData->XnlFlags;
//	/*DesAddress*/
//	*((WORD*)(&pPacket[4])) = htons(pData->DesAddress);
//	/*SourceAddress*/
//	*((WORD*)(&pPacket[6])) = htons(pData->SourceAddress);
//	/*TrascationId*/
//	*((WORD*)(&pPacket[8])) = htons(pData->TrascationId);
//	/*PayloadLength*/
//	*((WORD*)(&pPacket[10])) = htons(pData->PayloadLength);
//	/*RadioStatus Opcode*/
//	*((WORD*)(&pPacket[12])) = htons(pData->Payload.RadioStatus.RadioStatusRequest.Opcode);
//	pPacket[14] = pData->Payload.RadioStatus.RadioStatusRequest.Condition;
//	size = 15;
//	return size;
//}
////DWORD CP2PNet::Build_XCMP_XNL_DATA_MSG_DEVICE_INIT_STATUS(CHAR* pPacket, T_XNL_DATA_MSG* pData)
////{
////	DWORD size;
////	/*Opcode*/
////	*((WORD*)(&pPacket[0])) = htons(pData->Opcode);
////	/*ProtocolId*/
////	pPacket[2] = pData->ProtocolId;
////	/*XnlFlags*/
////	pPacket[3] = pData->XnlFlags;
////	/*DesAddress*/
////	*((WORD*)(&pPacket[4])) = htons(pData->DesAddress);
////	/*SourceAddress*/
////	*((WORD*)(&pPacket[6])) = htons(pData->SourceAddress);
////	/*TrascationId*/
////	*((WORD*)(&pPacket[8])) = htons(pData->TrascationId);
////	/*PayloadLength*/
////	*((WORD*)(&pPacket[10])) = htons(pData->PayloadLength);
////	/*RadioStatus Opcode*/
////	*((WORD*)(&pPacket[12])) = htons(pData->Payload.DeviceInit.Opcode);
////	*((DWORD*)(&pPacket[14])) = htonl(pData->Payload.DeviceInit.Version);
////	memcpy(&pPacket[18], &pData->Payload.DeviceInit.DeviceInitType, sizeof(pData->Payload.DeviceInit.DeviceInitType));
////	size = 22;
////	return size;
////}
//DWORD CP2PNet::Build_XCMP_XNL_DATA_MSG_ACK(CHAR* pPacket, T_XNL_DATA_MSG_ACK* pData)
//{
//	DWORD size;
//	/*Opcode*/
//	*((WORD*)(&pPacket[0])) = htons(pData->Opcode);
//	/*ProtocolId*/
//	pPacket[2] = pData->ProtocolId;
//	/*XnlFlags*/
//	pPacket[3] = pData->XnlFlags;
//	/*DesAddress*/
//	*((WORD*)(&pPacket[4])) = htons(pData->DesAddress);
//	/*SourceAddress*/
//	*((WORD*)(&pPacket[6])) = htons(pData->SourceAddress);
//	/*TrascationId*/
//	*((WORD*)(&pPacket[8])) = htons(pData->TrascationId);
//	/*PayloadLength*/
//	*((WORD*)(&pPacket[10])) = htons(pData->PayloadLength);
//	size = 12;
//	return size;
//}
void CP2PNet::Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91& networkData)
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
void CP2PNet::Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93& networkData)
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
void CP2PNet::Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98& networkData)
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
//void CP2PNet::Unpack_XCMP_XNL_MASTER_STATUS_BRDCST(char* pData, moto_protocol_xcmp_xnl_t * networkData)
//{
//	/*opcode*/
//	networkData->Opcode = pData[0];
//	/* peer id*/
//	networkData->PeerId = ntohl(*((DWORD*)(&pData[1])));
//	/*length */
//	networkData->lenth = ntohs(*(WORD*)(&pData[5]));
//	/*xnl*/
//	/*opcode*/
//	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.Opcode= ntohs(*(WORD*)(&pData[7]));
//	/*protocol id */
//	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.ProtocolId = pData[9];
//	/*xnl flags*/
//	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.XnlFlags = pData[10];
//	/* Destination Address */
//	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.DesAddress = ntohs(*(WORD*)(&pData[11]));
//	/*SourceAddress   ------  Master’s XNL address*/  
//	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.SourceAddress = ntohs(*(WORD*)(&pData[13]));
//	m_desAddress = networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.SourceAddress;
//	/*TrascationId*/
//	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.TrascationId = ntohs(*(WORD*)(&pData[15]));
//	/* payload length*/
//	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.PayloadLength = ntohs(*(WORD*)(&pData[17]));
//	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.MinorXNLVersionNumber = ntohs(*(WORD*)(&pData[19]));
//	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.MajorXNVersioNumber = ntohs(*(WORD*)(&pData[21]));
//	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.MasterLogicalIdentifier = ntohs(*(WORD*)(&pData[23]));
//	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.DataMessageSent = pData[24];
//}
//void CP2PNet::Unpack_XCMP_XNL_DEVICE_AUTH_KEY_REPLY(char* pData, moto_protocol_xcmp_xnl_t* networkData)
//{
//	/*opcode*/
//	networkData->Opcode = pData[0];
//	/* peer id*/
//	networkData->PeerId = ntohl(*((DWORD*)(&pData[1])));
//	/*length */
//	networkData->lenth = ntohs(*(WORD*)(&pData[5]));
//	/*xnl*/
//	/*opcode*/
//	networkData->Payload.DEVICE_AUTH_KEY_REPLY.Opcode = ntohs(*(WORD*)(&pData[7]));
//	/*protocol id */
//	networkData->Payload.DEVICE_AUTH_KEY_REPLY.ProtocolId = pData[9];
//	/*xnl flags*/
//	networkData->Payload.DEVICE_AUTH_KEY_REPLY.XnlFlags = pData[10];
//	/* Destination Address */
//	networkData->Payload.DEVICE_AUTH_KEY_REPLY.DesAddress = ntohs(*(WORD*)(&pData[11]));
//	/*SourceAddress   ------  Master’s XNL address*/
//	networkData->Payload.DEVICE_AUTH_KEY_REPLY.SourceAddress = ntohs(*(WORD*)(&pData[13]));
//	/*TrascationId*/
//	networkData->Payload.DEVICE_AUTH_KEY_REPLY.TrascationId = ntohs(*(WORD*)(&pData[15]));
//	/* payload length*/
//	networkData->Payload.DEVICE_AUTH_KEY_REPLY.PayloadLength = ntohs(*(WORD*)(&pData[17]));
//	/*Temporary XNl Address*/
//	networkData->Payload.DEVICE_AUTH_KEY_REPLY.TemporaryXnlAddress = ntohs(*(WORD*)(&pData[19]));
//	/*UnencryptedAuthenticationValue*/
//	memcpy(networkData->Payload.DEVICE_AUTH_KEY_REPLY.UnencryptedAuthenticationValue,&pData[21],8);
//}
//void CP2PNet::Unpack_XCMP_XNL_DEVICE_CONN_REPLY(char* pData, moto_protocol_xcmp_xnl_t* networkData)
//{
//	/*opcode*/
//	networkData->Opcode = pData[0];
//	/* peer id*/
//	networkData->PeerId = ntohl(*((DWORD*)(&pData[1])));
//	/*length */
//	networkData->lenth = ntohs(*(WORD*)(&pData[5]));
//	/*xnl*/
//	/*opcode*/
//	networkData->Payload.DEVICE_CONN_REPLY.Opcode = ntohs(*(WORD*)(&pData[7]));
//	/*protocol id */
//	networkData->Payload.DEVICE_CONN_REPLY.ProtocolId = pData[9];
//	/*xnl flags*/
//	networkData->Payload.DEVICE_CONN_REPLY.XnlFlags = pData[10];
//	/* Destination Address */
//	networkData->Payload.DEVICE_CONN_REPLY.DesAddress = ntohs(*(WORD*)(&pData[11]));
//	/*SourceAddress   ------  Master’s XNL address*/
//	networkData->Payload.DEVICE_CONN_REPLY.SourceAddress = ntohs(*(WORD*)(&pData[13]));
//	/*TrascationId*/
//	networkData->Payload.DEVICE_CONN_REPLY.TrascationId = ntohs(*(WORD*)(&pData[15]));
//	/* payload length*/
//	networkData->Payload.DEVICE_CONN_REPLY.PayloadLength = ntohs(*(WORD*)(&pData[17]));
//	networkData->Payload.DEVICE_CONN_REPLY.ResultCode = pData[19];
//	networkData->Payload.DEVICE_CONN_REPLY.TrascationIdBase = pData[20];
//	m_sourceAddress = ntohs(*(WORD*)(&pData[21]));
//	networkData->Payload.DEVICE_CONN_REPLY.XnlAddress = ntohs(*(WORD*)(&pData[22]));
//	networkData->Payload.DEVICE_CONN_REPLY.LogicalAddress = ntohs(*(WORD*)(&pData[23]));
//	memcpy(networkData->Payload.DEVICE_CONN_REPLY.EncryptedAuthenticationValue, &pData[24], 8);
//}
//void CP2PNet::Unpack_XCMP_XNL_DATA_MSG(char* pData, moto_protocol_xcmp_xnl_t* networkData)
//{
//	/*opcode*/
//	networkData->Opcode = pData[0];
//	/* peer id*/
//	networkData->PeerId = ntohl(*((DWORD*)(&pData[1])));
//	/*length */
//	networkData->lenth = ntohs(*(WORD*)(&pData[5]));
//	/*xnl*/
//	/*opcode*/
//	networkData->Payload.XNL_DATA_MSG.Opcode = ntohs(*(WORD*)(&pData[7]));
//	/*protocol id */
//	networkData->Payload.XNL_DATA_MSG.ProtocolId = pData[9];
//	/*xnl flags*/
//	networkData->Payload.XNL_DATA_MSG.XnlFlags = pData[10];
//	/* Destination Address */
//	networkData->Payload.XNL_DATA_MSG.DesAddress = ntohs(*(WORD*)(&pData[11]));
//	/*SourceAddress   ------  Master’s XNL address*/
//	networkData->Payload.XNL_DATA_MSG.SourceAddress = ntohs(*(WORD*)(&pData[13]));
//	/*TrascationId*/
//	m_trascationId = ntohs(*(WORD*)(&pData[15]));
//	networkData->Payload.XNL_DATA_MSG.TrascationId = ntohs(*(WORD*)(&pData[15]));
//	/* payload length*/
//	networkData->Payload.XNL_DATA_MSG.PayloadLength = ntohs(*(WORD*)(&pData[17]));
//	if (LE_XNL_XCMP_RADIO_STATUS_REPLY == ntohs(*(WORD*)(&pData[19]))) // reply 得到序列号  
//	{
//		networkData->Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.Opcode = LE_XNL_XCMP_RADIO_STATUS_REPLY;
//		if (0 == (networkData->Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.Result = pData[21]))
//		{
//			memcpy (networkData->Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.ProductSerialNumber,&pData[23],10);
//		}
//	}
//	else if (LE_XNL_XCMP_RADIO_STATUS_BRDCST == ntohs(*(WORD*)(&pData[19])))  //broadcast 得到序列号
//	{
//		memcpy(networkData->Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.ProductSerialNumber, &pData[22], 10);
//	}
//}
void CP2PNet::Unpack_LE_PEER_REGISTRATION_REQUEST(char* pData, T_LE_PROTOCOL_94* networkData)
{
	/*Opcode*/
	networkData->Opcode = pData[0];
	/*peerID*/
	networkData->peerID = ntohl(*((DWORD*)(&pData[1])));
	/*currentLinkProtocolVersion*/
	networkData->currentLinkProtocolVersion = ntohs(*((WORD*)(&pData[5])));
	/*oldestLinkProtocolVersion*/
	networkData->oldestLinkProtocolVersion = ntohs(*((WORD*)(&pData[7])));
}
void CP2PNet::Unpack_LE_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_95* networkData)
{
	/*Opcode*/
	networkData->Opcode = pData[0];
	/*peerID*/
	networkData->peerID = ntohl(*((DWORD*)(&pData[1])));
	/*currentLinkProtocolVersion*/
	networkData->currentLinkProtocolVersion = ntohs(*((WORD*)(&pData[5])));
	/*oldestLinkProtocolVersion*/
	networkData->oldestLinkProtocolVersion = ntohs(*((WORD*)(&pData[7])));
}
void CP2PNet::Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_97* networkData)
{
	int index = 0;
	//unsigned char Opcode;
	networkData->Opcode = pData[index];
	index += (sizeof(unsigned char));
	//unsigned long peerID;
	networkData->peerID = ntohl(*(unsigned long*)(&pData[index]));
	index += (sizeof(unsigned long));
	//unsigned char peerMode;
	networkData->peerMode = pData[index];
	index += (sizeof(unsigned char));
	//unsigned long peerServices;
	networkData->peerServices = ntohl(*(unsigned long*)(&pData[index]));
	index += (sizeof(unsigned long));
	//unsigned short acceptedLinkProtocolVersion;
	networkData->acceptedLinkProtocolVersion = ntohs(*(unsigned short*)(&pData[index]));
	index += (sizeof(unsigned short));
	//unsigned short oldestLinkProtocolVersion;
	networkData->oldestLinkProtocolVersion = ntohs(*(unsigned short*)(&pData[index]));
	index += (sizeof(unsigned short));
}
void CP2PNet::Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98* networkData)
{
	/*Opcode*/
	networkData->Opcode = pData[0];
	/*peerID*/
	networkData->peerID = ntohl(*((DWORD*)(&pData[1])));
	/*peerMode*/
	networkData->peerMode = pData[5];
	/*peerServices*/
	networkData->peerServices = ntohl(*((DWORD*)(&pData[6])));
}
void CP2PNet::Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99* networkData)
{
	/*Opcode*/
	networkData->Opcode = pData[0];
	/*peerID*/
	networkData->peerID = ntohl(*((DWORD*)(&pData[1])));
	/*peerMode*/
	networkData->peerMode =pData[5];
	/*peerServices*/
	networkData->peerServices = ntohl(*((DWORD*)(&pData[6])));
}
void CP2PNet::Unpack_VOICE_CALL(char* pData, T_P2P_CALL* networkData, unsigned char callType)
{
	/*opcode*/
	networkData->CallHeader.CallOpcode = pData[0];
	/*peerID*/
	networkData->CallHeader.CallOriginatingPeerID = ntohl(*((DWORD*)(&pData[1])));
	/*call seq number*/
	networkData->CallHeader.CallSequenceNumber = pData[5];
	/*src id*/
	//memcpy(&networkData->CallHeader.CallSrcID, &pData[6],3);
	networkData->CallHeader.CallSrcID = ntohl(*((DWORD*)(&pData[6]))); /*call src id 是24个字节*/
	networkData->CallHeader.CallSrcID = networkData->CallHeader.CallSrcID >> 8;
	/*tgt id*/;
	memcpy(&networkData->CallHeader.CallTgtID, &pData[9], 3);
	networkData->CallHeader.CallTgtID = ntohl(*((DWORD*)(&pData[9])));
	networkData->CallHeader.CallTgtID = networkData->CallHeader.CallTgtID >> 8;
	/*CallPriority*/
	networkData->CallHeader.CallPriority = pData[12];
	/*FlloorControlTag */
	networkData->CallHeader.CallFloorControlTag = ntohl(*((DWORD*)(&pData[13])));
	/*CallControlInformation*/
	networkData->CallHeader.CallControlInformation = pData[17];
	/*RTPTimeStamp*/
	networkData->CallHeader.RTPTimeStamp  = ntohl(*((DWORD*)(&pData[22])));
	/*RepeaterBurstDataType*/
	networkData->CallHeader.RepeaterBurstDataType = pData[30] & 0x7f;
	/*ESNLIEHB*/
	networkData->CallHeader.ESNLIEHB = pData[32];
	/*AmbeVoiceEncodedFrames*/
	networkData->AmbeVoiceEncodedFrames = *((AMBE_VOICE_ENCODED_FRAMES*)(&pData[32]));
	/*call type*/
	networkData->CallType = callType;
}
le_status_enum CP2PNet::GetLeStatus()
{
	return m_le_status_enum;
}
void CP2PNet::SetLeStatus(le_status_enum value)
{
	if (m_le_status_enum != value)
	{
		char temp[64] = { 0 };
		sprintf_s(temp, "=====Le Status From %d To %d=====", m_le_status_enum, value);
		m_log->AddLog(temp);
		m_le_status_enum = value;
	}
}
//xnl_status_enum CP2PNet::GetXnlStatus()
//{
//	return m_xnl_status_enum;
//}
//void CP2PNet::SetXnlStatus(xnl_status_enum value)
//{
//	if (m_xnl_status_enum != value)
//	{
//		char temp[64] = { 0 };
//		sprintf_s(temp, "=====Serial Status From %d To %d=====", m_xnl_status_enum, value);
//		m_log->AddLog(temp);
//		m_xnl_status_enum = value;
//	}
//}
void CP2PNet::AddWorkItem(work_item_t* p)
{
	TRYLOCK(m_p2pDataMutex);
	appendData(&m_p2pDataLink, p);
	RELEASELOCK(m_p2pDataMutex);
}
//void CP2PNet::AddXnlItem(work_item_t* p)
//{
//	TRYLOCK(m_serialMutex);
//	appendData(&m_serialLink,p);
//	RELEASELOCK(m_serialMutex);
//}
void CP2PNet::SendDataToMaster(work_item_t *w, unsigned long timeout/* = TIMEOUT_LE*/)
{
	send_data_t* pSend = &w->data.send_data;
	/*发送成功，写入timeout item;发送失败，写入work item*/
	//if (sendDataUdp(m_pXQTTNet, pSend->net_data, pSend->net_lenth, (SOCKADDR_IN*)pSend->send_to, sizeof(SOCKADDR_IN)))
	if (sendNetDataBase(pSend->net_data,pSend->net_lenth,pSend->send_to))
	{
		if (0 != timeout)
		{
			pSend->timeout_send = GetTickCount() + timeout;
		}
		AddTimeoutItem(w);
		m_log->AddLog("send data to master !");
	}
	else
	{
		AddWorkItem(w);
	}
}
void CP2PNet::SendXnlToMaster(work_item_t *w, unsigned long timeout/* = TIMEOUT_LE*/)
{
	send_data_t* pSend = &w->data.send_data;
	if (sendDataUdp(m_pXQTTNet, pSend->net_data, pSend->net_lenth, (SOCKADDR_IN*)pSend->send_to, sizeof(SOCKADDR_IN)))
	{
		if (0 != timeout)
		{
			pSend->timeout_send = GetTickCount() + timeout;
		}
		m_log->AddLog("send xnl to master !");
	}
}
void CP2PNet::AddTimeoutItem(work_item_t* p)
{
	TRYLOCK(m_p2pTimeoutDataMutex);
	appendData(&m_p2pTimeoutDataLink, p);
	RELEASELOCK(m_p2pTimeoutDataMutex);
}
void CP2PNet::AddAmbeItem(work_item_t* p)
{
	TRYLOCK(m_ambeMutex);
	appendData(&m_ambeLink,p);
	RELEASELOCK(m_ambeMutex);
}
void CP2PNet::AddRecordItem(NSRecordFile* p)
{
	TRYLOCK(m_recordMutex);
	appendData(&m_recordLink, p);
	RELEASELOCK(m_recordMutex);
}
void CP2PNet::ParseMapBroadcast(T_LE_PROTOCOL_93* p)
{
	p2p_peer_build_param_t param;
	find_peer_condition_t condition;
	pLinkList newPeers = NULL;
	/*增加主中继到新map*/
	param.addr = inet_addr(m_netParam.master_ip);
	param.peerId = m_netParam.matser_peer_id;;
	param.pNet = this;
	param.pNetParam = &m_netParam;
	param.port = m_netParam.master_port;
	condition.peer_id = m_netParam.matser_peer_id;
	CP2PPeer* peer = FindPeersItemAndRemove(&condition);
	if (NULL == peer)
	{
		peer = new CP2PPeer(&param);

	}
	appendData(&newPeers, peer);
	int mapNums = p->mapNums;
	for (int i = 0; i < mapNums; i++)
	{
		map_peer_info_t* pMapInfo = &p->mapPeers[i];
		param.addr = htonl( pMapInfo->remoteIPAddr);
		param.peerId = pMapInfo->remotePeerID;
		param.pNet = this;
		param.pNetParam = &m_netParam;
		param.port = pMapInfo->remotePort;
		if (!FindLocalIP(param.addr))
		{
			condition.peer_id = param.peerId;
			CP2PPeer* peer = FindPeersItemAndRemove(&condition);
			if (NULL == peer)
			{
				peer = new CP2PPeer(&param);
				peer->LE_PEER_REGISTRATION();
			}
			appendData(&newPeers, peer);
		}
	}
	/*更新map表*/
	ClearPeersLink();
	TRYLOCK(m_peersMutex);
	m_peersLink = newPeers;
	RELEASELOCK(m_peersMutex);
}
bool CP2PNet::FindLocalIP(unsigned long addr)
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
void CP2PNet::GetIpList(void * p)
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
CP2PPeer* CP2PNet::FindPeersItemAndRemove(find_peer_condition_t* pCondition)
{
	CP2PPeer* p = NULL;
	TRYLOCK(m_peersMutex);
	pLinkItem item = findItem(m_peersLink, pCondition, &FuncFindPeer);
	if (NULL != item)
	{
		if (NULL != item->data)
		{
			p = (CP2PPeer*)item->data;
		}
		removeItem(&m_peersLink, p);
		item->pNext = NULL;
		freeList(item);
	}
	RELEASELOCK(m_peersMutex);
	return p;
}
void CP2PNet::FindTimeOutItemAndDelete(unsigned long peerId, const char Opcode)
{
	find_item_condition_t condition = { 0 };
	condition.peerId = peerId;
	condition.Opcode = Opcode;
	work_item_t* p = NULL;
	TRYLOCK(m_p2pTimeoutDataMutex);
	pLinkItem item = findItem(m_p2pTimeoutDataLink, &condition, &FuncFindItem);
	while (item)
	{
		p = (work_item_t*)item->data;
		if (p)
		{
#ifdef _DEBUG
			char temp[128] = { 0 };
			unsigned char Opcode = '\0';
			if (Recive == p->type)
			{
				Opcode = p->data.recive_data.protocol.le.PROTOCOL_90.Opcode;
				sprintf_s(temp, "Rely Success TimeOut Recive Opcode:0x%02x Delete", Opcode);
			}
			else
			{
				Opcode = p->data.send_data.protocol.le.PROTOCOL_90.Opcode;
				sprintf_s(temp, "Rely Success TimeOut Send Opcode:0x%02x Delete", Opcode);
			}
			m_log->AddLog(temp);
#endif // _DEBUG
			removeItem(&m_p2pTimeoutDataLink, p);
			delete p;
			p = NULL;
		}
		item->pNext = NULL;
		freeList(item);
		item = findItem(m_p2pTimeoutDataLink, &condition, &FuncFindItem);
	}
	RELEASELOCK(m_p2pTimeoutDataMutex);
}
bool CP2PNet::FuncFindItem(const void* pValue, const void* pCondition)
{
	bool rlt = false;
	work_item_t* p = (work_item_t*)pValue;
	unsigned char Opcode;
	unsigned long peerId;
	bool bNeedCompilePeerId = true;
	if (Recive == p->type)
	{
		Opcode = p->data.recive_data.protocol.le.PROTOCOL_90.Opcode;
		peerId = p->data.recive_data.protocol.le.PROTOCOL_90.peerID;
	}
	else
	{
		Opcode = p->data.send_data.protocol.le.PROTOCOL_90.Opcode;
		CP2PPeer* peer = (CP2PPeer*)p->data.send_data.pFrom;
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
	if (p)
	{
		find_item_condition_t* condition = (find_item_condition_t*)pCondition;
		if (condition)
		{
			if (Opcode == condition->Opcode)
			{
				if (0x00 != condition->wlOpcode)
				{
					
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
	}
	return rlt;
}
bool CP2PNet::FuncFindPeer(const void* pValue, const void* pCondition)
{
	bool rlt = false;
	CP2PPeer* p = (CP2PPeer*)pValue;
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
CP2PPeer* CP2PNet::FindPeersItem(find_peer_condition_t* pCondition)
{
	CP2PPeer* p = NULL;
	TRYLOCK(m_peersMutex);
	pLinkItem item = findItem(m_peersLink, pCondition, &FuncFindPeer);
	if (NULL != item)
	{
		if (NULL != item->data)
		{
			p = (CP2PPeer*)item->data;
		}
	}
	RELEASELOCK(m_peersMutex);
	return p;
}
void CP2PNet::SetSerialNumber(unsigned char* pSerial)
{
	memcpy(m_serialNumber, pSerial, sizeof(m_serialNumber));
}
void CP2PNet::SetSerialNumberCallback(unsigned char* pData, void* pArguments)
{
	CP2PNet* p = (CP2PNet*)pArguments;
	if (p)
	{
		p->SetSerialNumber(pData);
	}
}
void CP2PNet::EncryptAuthenticationValue(unsigned char* un_Auth, unsigned char* en_Auth)
{
	//unsigned char un_Auth[8] = {0x6e,0xa0,0x7b,0xd5,0x36,0x9a,0x0f,0x3e};
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
		//sprintf_s(m_reportMsg, "8");
		//sendLogToWindow();
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
void CP2PNet::DeleteWorkItem(work_item_t *w)
{
	if (NULL == w)
	{
		return;
	}
	delete w;
	w = NULL;
}
bool CP2PNet::WriteVoiceFrame(moto_protocol_p2p_t& call, DWORD dwCallType, BOOL isCheckTimeout)
{

	////尾结束包超时处理
	//if (isCheckTimeout)
	//{

	//	//TRACE(_T("m_voiceReocrds.size:%d\r\n"), m_voiceReocrds.size());

	//	DWORD curTime = 0;
	//	DWORD preTime = 0;
	//	DWORD rlt = 0;
	//	for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
	//	{
	//		curTime = GetTickCount();
	//		preTime = (*i)->prevTimestamp;
	//		rlt = curTime - preTime;
	//		//TRACE(_T("rlt:%d\r\n"), rlt);
	//		if (rlt > VOICE_END_TIMEOUT)
	//		{
	//			if (m_pEventLoger)
	//			{
	//				int temp = 0;

	//				if (m_dwRecType == IPSC)
	//				{
	//					if (IndividualCall == (*i)->callType)
	//					{
	//						temp = IndividualCall;
	//					}
	//					else if (GroupCall == (*i)->callType)
	//					{
	//						temp = GroupCall;
	//					}
	//				}
	//				else if (m_dwRecType == CPC)
	//				{
	//					if (IndividualCall == (*i)->callType)
	//					{
	//						temp = CPC_PRIVATE_CALL;
	//					}
	//					else if (GroupCall == (*i)->callType)
	//					{
	//						temp = CPC_GROUP_CALL;
	//					}
	//				}

	//				m_pEventLoger->OnNewVoiceRecord((LPBYTE)(*i)->buffer, (*i)->lenght, (*i)->srcId, (*i)->tagetId, temp, 0, (*i)->originalPeerId, (*i)->srcSlot, (*i)->srcRssi);
	//				TRACE(_T("VOICE_END_TIMEOUT,srcSlot:%d,srcRssi:%d\r\n"), (*i)->srcSlot, (*i)->srcRssi);
	//			}

	//			delete (*i);
	//			m_voiceReocrds.erase(i);
	//		}
	//	}
	//	return TRUE;
	//}

	//char voiceFrame[7];
	//BOOL bFind = FALSE;

	//for (auto i = m_voiceReocrds.begin(); i != m_voiceReocrds.end(); i++)
	//{
	//	if ((*i)->srcId == call.fld.CallSrcID &&
	//		(*i)->tagetId == call.fld.CallTgtID &&
	//		(*i)->sequenceNumber == call.fld.CallSequenceNumber)
	//	{
	//		voiceFrame[0] = (m_CurrentRecvBuffer.buf)[33];
	//		voiceFrame[1] = (m_CurrentRecvBuffer.buf)[34];
	//		voiceFrame[2] = (m_CurrentRecvBuffer.buf)[35];
	//		voiceFrame[3] = (m_CurrentRecvBuffer.buf)[36];
	//		voiceFrame[4] = (m_CurrentRecvBuffer.buf)[37];
	//		voiceFrame[5] = (m_CurrentRecvBuffer.buf)[38];
	//		voiceFrame[6] = ((m_CurrentRecvBuffer.buf)[39]) & 0x80;

	//		AMBE2FrameToAMBE3000Frame(voiceFrame);

	//		(*i)->WriteVoiceFrame(voiceFrame);

	//		//TRACE("Bad voice frame? %s\r\n", (((m_CurrentRecvBuffer.buf)[39]) & 0x40) > 0 ? _T("Yes") : _T("No"));


	//		voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[39]) << 2) | ((((m_CurrentRecvBuffer.buf)[40]) >> 6) & 0x03);
	//		voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[40]) << 2) | ((((m_CurrentRecvBuffer.buf)[41]) >> 6) & 0x03);
	//		voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[41]) << 2) | ((((m_CurrentRecvBuffer.buf)[42]) >> 6) & 0x03);
	//		voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[42]) << 2) | ((((m_CurrentRecvBuffer.buf)[43]) >> 6) & 0x03);
	//		voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[43]) << 2) | ((((m_CurrentRecvBuffer.buf)[44]) >> 6) & 0x03);
	//		voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[44]) << 2) | ((((m_CurrentRecvBuffer.buf)[45]) >> 6) & 0x03);
	//		voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[45]) << 2) & 0x80;
	//		//TRACE("Bad voice frame? %s\r\n", ((((m_CurrentRecvBuffer.buf)[45]) << 2) & 0x40) > 0 ? _T("Yes") : _T("No"));

	//		AMBE2FrameToAMBE3000Frame(voiceFrame);
	//		(*i)->WriteVoiceFrame(voiceFrame);

	//		voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[45]) << 4) | ((((m_CurrentRecvBuffer.buf)[46]) >> 4) & 0x0F);
	//		voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[46]) << 4) | ((((m_CurrentRecvBuffer.buf)[47]) >> 4) & 0x0F);
	//		voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[47]) << 4) | ((((m_CurrentRecvBuffer.buf)[48]) >> 4) & 0x0F);
	//		voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[48]) << 4) | ((((m_CurrentRecvBuffer.buf)[49]) >> 4) & 0x0F);
	//		voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[49]) << 4) | ((((m_CurrentRecvBuffer.buf)[50]) >> 4) & 0x0F);
	//		voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[50]) << 4) | ((((m_CurrentRecvBuffer.buf)[51]) >> 4) & 0x0F);
	//		voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[51]) << 4) & 0x80;
	//		//TRACE("Bad voice frame? %s\r\n", ((((m_CurrentRecvBuffer.buf)[51]) << 4) & 0x40) > 0 ? _T("Yes") : _T("No"));
	//		AMBE2FrameToAMBE3000Frame(voiceFrame);
	//		(*i)->WriteVoiceFrame(voiceFrame);
	//		//TRACE(_T("burst type:%d\r\n"), call.fld.RepeaterBurstDataType);
	//		if (DATA_TYPE_VOICE_TERMINATOR == call.fld.RepeaterBurstDataType)
	//		{
	//			// it's last frame, write to file and erase this item
	//			//FILE *f;
	//			//f = fopen(("e:\\moto.bit"), ("wb+"));
	//			//fwrite((*i)->buffer, 1, (*i)->lenght, f);
	//			//fclose(f);
	//			if (m_pEventLoger)
	//			{
	//				int temp = 0;

	//				if (m_dwRecType == IPSC)
	//				{
	//					if (IndividualCall == (*i)->callType)
	//					{
	//						temp = IndividualCall;
	//					}
	//					else if (GroupCall == (*i)->callType)
	//					{
	//						temp = GroupCall;
	//					}
	//				}
	//				else if (m_dwRecType == CPC)
	//				{
	//					if (IndividualCall == (*i)->callType)
	//					{
	//						temp = CPC_PRIVATE_CALL;
	//					}
	//					else if (GroupCall == (*i)->callType)
	//					{
	//						temp = CPC_GROUP_CALL;
	//					}
	//				}

	//				/*IPSC 模式下,RSSI为0表示高于阈值，1标识低于阈值*/
	//				(*i)->srcRssi = (m_CurrentRecvBuffer.buf[31]) & 0x40 >> 6;
	//				if (0 == (*i)->srcRssi)
	//				{
	//					/*正常信号强度*/
	//					(*i)->srcRssi = 1;
	//				}
	//				else
	//				{
	//					/*非正常信号强度*/
	//					(*i)->srcRssi = 0;
	//				}
	//				m_pEventLoger->OnNewVoiceRecord((LPBYTE)(*i)->buffer, (*i)->lenght, (*i)->srcId, (*i)->tagetId, temp, 0, (*i)->originalPeerId, (*i)->srcSlot, (*i)->srcRssi);
	//				TRACE(_T("DATA_TYPE_VOICE_TERMINATOR,srcSlot:%d,srcRssi:%d\r\n"), (*i)->srcSlot, (*i)->srcRssi);
	//			}

	//			delete (*i);
	//			m_voiceReocrds.erase(i);
	//		}

	//		bFind = TRUE;
	//		break;
	//	}
	//}
	//if (!bFind)
	//{
	//	CRecordFile* rFile = new CRecordFile();
	//	voiceFrame[0] = (m_CurrentRecvBuffer.buf)[33];
	//	voiceFrame[1] = (m_CurrentRecvBuffer.buf)[34];
	//	voiceFrame[2] = (m_CurrentRecvBuffer.buf)[35];
	//	voiceFrame[3] = (m_CurrentRecvBuffer.buf)[36];
	//	voiceFrame[4] = (m_CurrentRecvBuffer.buf)[37];
	//	voiceFrame[5] = (m_CurrentRecvBuffer.buf)[38];
	//	voiceFrame[6] = ((m_CurrentRecvBuffer.buf)[39]) & 0x80;

	//	AMBE2FrameToAMBE3000Frame(voiceFrame);
	//	rFile->WriteVoiceFrame(voiceFrame);


	//	voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[39]) << 2) | ((((m_CurrentRecvBuffer.buf)[40]) >> 6) & 0x03);
	//	voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[40]) << 2) | ((((m_CurrentRecvBuffer.buf)[41]) >> 6) & 0x03);
	//	voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[41]) << 2) | ((((m_CurrentRecvBuffer.buf)[42]) >> 6) & 0x03);
	//	voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[42]) << 2) | ((((m_CurrentRecvBuffer.buf)[43]) >> 6) & 0x03);
	//	voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[43]) << 2) | ((((m_CurrentRecvBuffer.buf)[44]) >> 6) & 0x03);
	//	voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[44]) << 2) | ((((m_CurrentRecvBuffer.buf)[45]) >> 6) & 0x03);
	//	voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[45]) << 2) & 0x80;

	//	AMBE2FrameToAMBE3000Frame(voiceFrame);
	//	rFile->WriteVoiceFrame(voiceFrame);

	//	voiceFrame[0] = (((m_CurrentRecvBuffer.buf)[45]) << 4) | ((((m_CurrentRecvBuffer.buf)[46]) >> 4) & 0x0F);
	//	voiceFrame[1] = (((m_CurrentRecvBuffer.buf)[46]) << 4) | ((((m_CurrentRecvBuffer.buf)[47]) >> 4) & 0x0F);
	//	voiceFrame[2] = (((m_CurrentRecvBuffer.buf)[47]) << 4) | ((((m_CurrentRecvBuffer.buf)[48]) >> 4) & 0x0F);
	//	voiceFrame[3] = (((m_CurrentRecvBuffer.buf)[48]) << 4) | ((((m_CurrentRecvBuffer.buf)[49]) >> 4) & 0x0F);
	//	voiceFrame[4] = (((m_CurrentRecvBuffer.buf)[49]) << 4) | ((((m_CurrentRecvBuffer.buf)[50]) >> 4) & 0x0F);
	//	voiceFrame[5] = (((m_CurrentRecvBuffer.buf)[50]) << 4) | ((((m_CurrentRecvBuffer.buf)[51]) >> 4) & 0x0F);
	//	voiceFrame[6] = (((m_CurrentRecvBuffer.buf)[51]) << 4) & 0x80;

	//	AMBE2FrameToAMBE3000Frame(voiceFrame);
	//	rFile->WriteVoiceFrame(voiceFrame);

	//	rFile->originalPeerId = call.fld.CallOriginatingPeerID;
	//	rFile->srcId = call.fld.CallSrcID;
	//	rFile->tagetId = call.fld.CallTgtID;
	//	rFile->sequenceNumber = call.fld.CallSequenceNumber;
	//	rFile->callType = dwCallType;
	//	rFile->srcRssi = -1;
	//	rFile->srcSlot = (call.fld.CallControlInformation & 0x20) >> 5;
	//	/*IPSC模式下0表示时隙1，1表示时隙2*/
	//	rFile->srcSlot++;

	//	m_voiceReocrds.push_back(rFile);
	//}
	return TRUE;
}
moto_protocol_p2p_t CP2PNet::FillThisCall(P2P_CALL_HEADER call_header)
{
	moto_protocol_p2p_t m_moto_protocol_p2p_t;
	m_moto_protocol_p2p_t.All[0] = call_header.CallOpcode;
	m_moto_protocol_p2p_t.All[1] = call_header.CallSequenceNumber;
	m_moto_protocol_p2p_t.All[2] = call_header.CallPriority;
	m_moto_protocol_p2p_t.All[3] = call_header.CallControlInformation;
	m_moto_protocol_p2p_t.All[4] = call_header.RepeaterBurstDataType;
	m_moto_protocol_p2p_t.All[5] = call_header.ESNLIEHB;
	*((DWORD*)(&m_moto_protocol_p2p_t.All[8])) = call_header.CallOriginatingPeerID;
	*((DWORD*)(&m_moto_protocol_p2p_t.All[12])) = call_header.CallFloorControlTag;
	*((DWORD*)(&m_moto_protocol_p2p_t.All[16])) = call_header.CallSrcID;
	*((DWORD*)(&m_moto_protocol_p2p_t.All[20])) = call_header.CallTgtID;
	*((DWORD*)(&m_moto_protocol_p2p_t.All[24])) = call_header.RTPTimeStamp;
	return m_moto_protocol_p2p_t;
}
void CP2PNet::Handle_P2P_Call(char *dst, const char* pAmbePacket)
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
NSRecordFile* CP2PNet::FindOrAddRecordsItem(find_record_condition_t* condition, bool &bFind)
{
	bFind = true;
	NSRecordFile* p = FindRecordsItem(condition);
	if (NULL == p)
	{
		bFind = false;
		p = new NSRecordFile(m_manager, NULL);
		AddRecordItem(p);
	}
	return p;
}
NSRecordFile* CP2PNet::FindRecordsItem(find_record_condition_t* condition)
{
	NSRecordFile* p = NULL;
	TRYLOCK(m_recordMutex);
	pLinkItem item = findItem(m_recordLink, condition, &FuncFindRecord);
	if (NULL != item)
	{
		if (NULL != item->data)
		{
			p = (NSRecordFile*)item->data;
		}
	}
	RELEASELOCK(m_recordMutex);
	return p;
}
bool CP2PNet::FuncFindRecord(const void* pValue, const void* pCondition)
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
void CP2PNet::RemoveRecordsItem(NSRecordFile* p)
{
	TRYLOCK(m_recordMutex);
	pLinkItem item = removeItem(&m_recordLink, p);
	if (item)
	{
		item->pNext = NULL;
		freeList(item);
		item = NULL;
	}
	RELEASELOCK(m_recordMutex);
}

int CP2PNet::sendNetDataBase(const char* pData, int len, void* send_to)
{
	int rlt = 0;
	TRYLOCK(m_sendMutex);
	rlt = sendDataUdp(m_pXQTTNet, pData, len, (SOCKADDR_IN*)send_to, sizeof(SOCKADDR_IN));
	RELEASELOCK(m_sendMutex);
	return rlt;
}

void CP2PNet::HandleAmbeData(void* pData, unsigned long length)
{
	/*do nothing in p2p*/
}
