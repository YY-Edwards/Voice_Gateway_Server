#include "stdafx.h"
#include "../include/NSSerial.h"
#include "../include/NSLog.h"
#include <process.h>
#include "../include/NSNetBase.h"
//#include "Manager.h"

NSSerial::NSSerial()
:m_bThreadWork(false)
, m_getSerailThread(NULL)
, m_pLog(NSLog::instance())
, m_serialMutex(INITLOCKER())
, m_pMasterXqttnet(NULL)
, m_xnl_status_enum(WAITLE)
, m_pSerialCallBack(NULL)
, m_serialLink(NULL)
, m_desAddress(0)
, m_sourceAddress(0)
, m_trascationId(0)
{

}

NSSerial::~NSSerial()
{
	m_pSerialCallBack = NULL;
	m_bThreadWork = false;
	/*停止获取序列号线程*/
	if (NULL != m_getSerailThread)
	{
		WaitForSingleObject(m_getSerailThread, 1000);
		CloseHandle(m_getSerailThread);
		m_getSerailThread = NULL;
	}
	ClearSerialLink();
	DELETELOCKER(m_serialMutex);
}

int NSSerial::Start(StartSerialParam *param)
{
	if (NULL == param)
	{
		return -1;
	}
	m_pSerialCallBack = param->pSerialCallBack;
	m_pSockaddrMaster = param->pSockaddrMaster;
	m_pMasterXqttnet = param->pMasterXqttnet;
	m_netParam = *param->pNetParam;
	m_pCallParam = (NSNetBase*)param->param;
	if (m_bThreadWork)
	{
		return 0;
	}
	unsigned int pThreadId = 0;
	/*获取序列号线程*/
	m_getSerailThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		GetSerialThreadProc,
		this,
		CREATE_SUSPENDED,
		&pThreadId
		);
	if (NULL == m_getSerailThread)
	{
		m_pLog->AddLog(Ns_Log_Error, "GetSerialThreadProc create fail");
		return -1;
	}
	m_bThreadWork = true;
	ResumeThread(m_getSerailThread);
	return 0;
}

unsigned int __stdcall NSSerial::GetSerialThreadProc(void* pArguments)
{
	NSSerial* p = (NSSerial*)pArguments;
	if (p)
	{

		p->GetSerialThread();
	}
	return 0;
}

void NSSerial::GetSerialThread()
{
	NSLog* m_log = m_pLog;
	m_log->AddLog(Ns_Log_Info, "get serial Thread start");
	while (m_bThreadWork)
	{
		work_item_t* currentItem = PopSerialItem();
		if (NULL == currentItem)
		{
			if (XNL_CONNECT == GetXnlStatus() || WAITFOR_XNL_MASTER_STATUS_BROADCAST == GetXnlStatus()
				|| WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY == GetXnlStatus() || WAITFOR_XNL_DEVICE_CONNECT_REPLY == GetXnlStatus())
			{
				work_item_t* pItem = new work_item_t;
				memset(pItem, 0, sizeof(work_item_t));
				SEND_LE_XCMP_XNL_DEVICE_MASTER_QUERY(pItem);
				AddXnlItem(pItem);
				SetXnlStatus(WAITFOR_XNL_MASTER_STATUS_BROADCAST);   //等待接收 0x0002
			}
			if (WAITFOR_XNL_XCMP_READ_SERIAL_RESULT == GetXnlStatus())
			{
				work_item_t* pItem = new work_item_t;
				memset(pItem, 0, sizeof(work_item_t));
				SEND_LE_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(pItem);
				AddXnlItem(pItem);
				SetXnlStatus(WAITFOR_XNL_XCMP_READ_SERIAL_RESULT);   //等待接收序列号
			}
			Sleep(SLEEP_GETSERIAL_THREAD);
			continue;
		}
		work_item_type_enum type = currentItem->type;
		switch (type)
		{
		case Send:
			SendXnlToMaster(currentItem);
			Sleep(SLEEP_GETSERIAL_THREAD);
			break;
		case Recive:
		{
					   unsigned short opcode = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_MASTER_QUERY.Opcode;
					   switch (GetXnlStatus())
					   {
					   case WAITFOR_XNL_MASTER_STATUS_BROADCAST:  //等待接收0x0002
						   if (LE_XNL_MASTER_STATUS_BRDCST == opcode)
						   {
							   work_item_t* pItem = new work_item_t;
							   memset(pItem, 0, sizeof(work_item_t));
							   pItem->data.send_data.protocol.xcmp.Payload.DEVICE_AUTH_KEY_REQUEST.DesAddress = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_SYSMAP_BRDCST.SourceAddress;
							   SEND_LE_XCMP_XNL_DEVICE_AUTH_KEY_REQUEST(pItem);
							   AddXnlItem(pItem);
							   SetXnlStatus(WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY);   //等待接收 0x0005
							   break;
						   }

					   case WAITFOR_XNL_DEVICE_AUTH_KEY_REPLY:
						   if (LE_XNL_DEVICE_AUTH_KEY_REPLY == opcode)
						   {
							   work_item_t* pItem = new work_item_t;
							   memset(pItem, 0, sizeof(work_item_t));
							   pItem->data.send_data.protocol.xcmp.Payload.DEVICE_CONN_REQUEST.SourceAddress = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_AUTH_KEY_REPLY.TemporaryXnlAddress;
							   unsigned char * unAuth = currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_AUTH_KEY_REPLY.UnencryptedAuthenticationValue;
							   unsigned char*  enAuth = pItem->data.recive_data.protocol.xcmp.Payload.DEVICE_CONN_REQUEST.EncryptedAuthenticationValue;
							   EncryptAuthenticationValue(unAuth, enAuth);
							   SEND_LE_XCMP_XNL_DEVICE_CONN_REQUEST(pItem);
							   AddXnlItem(pItem);
							   SetXnlStatus(WAITFOR_XNL_DEVICE_CONNECT_REPLY);   //等待接收 0x0007
							   break;
						   }

					   case WAITFOR_XNL_DEVICE_CONNECT_REPLY:
						   if (LE_XNL_DEVICE_CONN_REPLY == opcode)
						   {
							   if (LE_XNL_XCMP_CONNECT_SUCCESS == currentItem->data.recive_data.protocol.xcmp.Payload.DEVICE_CONN_REPLY.ResultCode)
							   {
								   work_item_t* pItem = new work_item_t;
								   memset(pItem, 0, sizeof(work_item_t));
								   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.TrascationId = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;
								   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.XnlFlags = ++(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags);
								   SEND_LE_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(pItem);
								   AddXnlItem(pItem);
								   SetXnlStatus(WAITFOR_XNL_XCMP_READ_SERIAL_RESULT);   //等待接收序列号
							   }
							   else
							   {
								   SetXnlStatus(XNL_CONNECT);// 连接失败后重新开始连接
							   }
						   }
					   case WAITFOR_XNL_XCMP_READ_SERIAL_RESULT:
						   if (LE_XNL_DATA_MSG == opcode)
						   {
							   work_item_t* pItem = new work_item_t;
							   memset(pItem, 0, sizeof(work_item_t));
							   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.XnlFlags = ++(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags);
							   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.TrascationId = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;
							   SEND_LE_XCMP_XNL_DATA_MSG_ACK(pItem);
							   AddXnlItem(pItem);
							   //若取到序列号，此处直接跳出该线程；若没有取得序列号，则继续发送获取序列号
							   unsigned short payloadOpcode = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.Opcode;
							   char result = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.Result;
							   if (LE_XNL_XCMP_RADIO_STATUS_REPLY == payloadOpcode && LE_XNL_XCMP_READ_SERIAL_SUCCESS == result)
							   {
								   //memcpy(m_serialNumber, &(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.ProductSerialNumber[0]), sizeof(m_serialNumber));
								   unsigned char* pSerial = &(currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.ProductSerialNumber[0]);
								   SetSerialNumber(pSerial);
								   SetXnlStatus(GET_SERIAL_SUCCESS);
							   }
							   else
							   {
								   SetXnlStatus(WAITFOR_XNL_XCMP_READ_SERIAL_RESULT);
							   }

						   }
						   break;
					   case GET_SERIAL_SUCCESS:
						   break;
					   default:
						   if (LE_XNL_DATA_MSG == opcode)    //接收xnl_data_msg 后，回复xnl_data_msg_ack
						   {
							   work_item_t* pItem = new work_item_t;
							   memset(pItem, 0, sizeof(work_item_t));
							   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.XnlFlags = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags;
							   pItem->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.TrascationId = currentItem->data.recive_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;
							   SEND_LE_XCMP_XNL_DATA_MSG_ACK(pItem);
							   AddXnlItem(pItem);
						   }
						   break;
					   }
					   if (NULL != currentItem)
					   {
						   delete currentItem;
						   currentItem = NULL;
					   }
		}
			break;
		default:
			break;
		}
	}
	m_log->AddLog(Ns_Log_Info, "get serial Thread end");
}

work_item_t * NSSerial::PopSerialItem()
{
	work_item_t* p = NULL;
	TRYLOCK(m_serialMutex);
	pLinkItem it = popFront(&m_serialLink);
	if (NULL != it)
	{
		if (NULL != it->data)
		{
			p = (work_item_t*)it->data;
		}
		freeList(it);
	}
	RELEASELOCK(m_serialMutex);
	return p;
}

void NSSerial::AddXnlItem(work_item_t* p)
{
	TRYLOCK(m_serialMutex);
	appendData(&m_serialLink, p);
	RELEASELOCK(m_serialMutex);
}

void NSSerial::SetSerialNumber(unsigned char* pSerial)
{
	memcpy(m_serialNumber, pSerial, sizeof(m_serialNumber));
	(*m_pSerialCallBack)(m_serialNumber, m_pCallParam);
	char temp[128] = { 0 };
	int index = 0;
	for each (unsigned char var in m_serialNumber)
	{
		temp[index] = var;
		index++;
	}
	strcpy(&temp[index], " Serial Number");
	m_pLog->AddLog(Ns_Log_Info, temp);
}

void NSSerial::SEND_LE_XCMP_XNL_DEVICE_MASTER_QUERY(work_item_t* w)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = m_pSockaddrMaster;
	w->data.send_data.timeout_try = 0;
	T_XNL_DEVICE_MASTER_QUERY networkData = { 0 };
	networkData.Opcode = LE_XNL_DEVICE_MASTER_QUERY;
	networkData.ProtocolId = LE_XNL_PROTOCOL_ID;
	networkData.XnlFlags = LE_XNL_FLAGS;
	networkData.SourceAddress = m_sourceAddress;
	networkData.DesAddress = m_desAddress;
	networkData.TrascationId = m_trascationId;
	networkData.PayloadLength = 0;
	char buf[BUFSIZ] = { 0 };
	int len = Build_XCMP_XNL_DEVICE_MASTER_QUERY(buf, &networkData);
	moto_protocol_xcmp_xnl_t m_moto_protocol_xcmp_xnl_t = { 0 };
	m_moto_protocol_xcmp_xnl_t.Opcode = LE_XNL;
	m_moto_protocol_xcmp_xnl_t.PeerId = m_netParam.local_peer_id;
	m_moto_protocol_xcmp_xnl_t.lenth = sizeof(m_moto_protocol_xcmp_xnl_t.Payload.DEVICE_MASTER_QUERY);
	w->data.send_data.protocol.xcmp = m_moto_protocol_xcmp_xnl_t;
	w->data.send_data.net_lenth = Build_LE_XCMP_XNL(w->data.send_data.net_data, &m_moto_protocol_xcmp_xnl_t, buf, len);
}

void NSSerial::SEND_LE_XCMP_XNL_DEVICE_AUTH_KEY_REQUEST(work_item_t* w)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = m_pSockaddrMaster;
	w->data.send_data.timeout_try = 0;
	T_XNL_DEVICE_AUTH_KEY_REQUEST networkData = { 0 };
	networkData.Opcode = LE_XNL_DEVICE_AUTH_KEY_REQUEST;
	networkData.ProtocolId = LE_XNL_PROTOCOL_ID;
	networkData.XnlFlags = LE_XNL_FLAGS;
	networkData.SourceAddress = m_sourceAddress;
	networkData.DesAddress = m_desAddress;
	networkData.TrascationId = m_trascationId;
	networkData.PayloadLength = 0;
	char buf[BUFSIZ] = { 0 };
	int len = Build_XCMP_XNL_DEVICE_AUTH_KEY_REQUEST(buf, &networkData);
	moto_protocol_xcmp_xnl_t m_moto_protocol_xcmp_xnl_t = { 0 };
	m_moto_protocol_xcmp_xnl_t.Opcode = LE_XNL;
	m_moto_protocol_xcmp_xnl_t.PeerId = m_netParam.local_peer_id;
	m_moto_protocol_xcmp_xnl_t.lenth = sizeof(m_moto_protocol_xcmp_xnl_t.Payload.DEVICE_AUTH_KEY_REQUEST);
	w->data.send_data.protocol.xcmp = m_moto_protocol_xcmp_xnl_t;
	w->data.send_data.net_lenth = Build_LE_XCMP_XNL(w->data.send_data.net_data, &m_moto_protocol_xcmp_xnl_t, buf, len);
}

void NSSerial::SEND_LE_XCMP_XNL_DEVICE_CONN_REQUEST(work_item_t* w)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = m_pSockaddrMaster;
	w->data.send_data.timeout_try = 0;
	T_XNL_DEVICE_CONN_REQUEST networkData = { 0 };
	networkData.Opcode = LE_XNL_DEVICE_CONN_REQUEST;
	networkData.ProtocolId = LE_XNL_PROTOCOL_ID;
	networkData.XnlFlags = LE_XNL_FLAGS;
	networkData.DesAddress = m_desAddress;
	networkData.SourceAddress = w->data.send_data.protocol.xcmp.Payload.DEVICE_CONN_REQUEST.SourceAddress;
	networkData.TrascationId = m_trascationId;
	networkData.PayloadLength = 0X000C;
	networkData.PreferredXnlAddress = m_sourceAddress;   //m_source 0x00
	networkData.DeviceType = LE_XNL_DEVICE_TYPE;
	networkData.AuthenticationIndex = LE_XNL_AUTHENTICATION_INDEX;
	memcpy(networkData.EncryptedAuthenticationValue, &(w->data.send_data.protocol.xcmp.Payload.DEVICE_CONN_REQUEST.EncryptedAuthenticationValue), 8);
	char buf[BUFSIZ] = { 0 };
	int len = Build_XCMP_XNL_DEVICE_CONN_REQUEST(buf, &networkData);
	moto_protocol_xcmp_xnl_t m_moto_protocol_xcmp_xnl_t = { 0 };
	m_moto_protocol_xcmp_xnl_t.Opcode = LE_XNL;
	m_moto_protocol_xcmp_xnl_t.PeerId = m_netParam.local_peer_id;
	m_moto_protocol_xcmp_xnl_t.lenth = sizeof(m_moto_protocol_xcmp_xnl_t.Payload.DEVICE_CONN_REQUEST);
	w->data.send_data.protocol.xcmp = m_moto_protocol_xcmp_xnl_t;
	w->data.send_data.net_lenth = Build_LE_XCMP_XNL(w->data.send_data.net_data, &m_moto_protocol_xcmp_xnl_t, buf, len);
}

void NSSerial::SEND_LE_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(work_item_t* w)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = m_pSockaddrMaster;
	w->data.send_data.timeout_try = 0;
	T_XNL_DATA_MSG networkData = { 0 };
	networkData.Opcode = LE_XNL_DATA_MSG;
	networkData.ProtocolId = LE_XNL_PROTOCOL_ID + 1;
	networkData.XnlFlags = w->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG.XnlFlags;
	networkData.DesAddress = m_desAddress;
	networkData.SourceAddress = m_sourceAddress;
	networkData.TrascationId = w->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG.TrascationId;
	networkData.PayloadLength = 0X0003;
	networkData.Payload.RadioStatus.RadioStatusRequest.Opcode = LE_XNL_XCMP_RADIO_STATUS;
	networkData.Payload.RadioStatus.RadioStatusRequest.Condition = LE_XNL_XCMP_RADIO_STATUS_CONDITION_SERIAL;
	char buf[BUFSIZ] = { 0 };
	int len = Build_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(buf, &networkData);
	moto_protocol_xcmp_xnl_t m_moto_protocol_xcmp_xnl_t = { 0 };
	m_moto_protocol_xcmp_xnl_t.Opcode = LE_XNL;
	m_moto_protocol_xcmp_xnl_t.PeerId = m_netParam.local_peer_id;
	m_moto_protocol_xcmp_xnl_t.lenth = 15 /*sizeof(m_moto_protocol_xcmp_xnl_t.Payload.XNL_DATA_MSG)*/;
	w->data.send_data.protocol.xcmp = m_moto_protocol_xcmp_xnl_t;
	w->data.send_data.net_lenth = Build_LE_XCMP_XNL(w->data.send_data.net_data, &m_moto_protocol_xcmp_xnl_t, buf, len);
}

void NSSerial::SEND_LE_XCMP_XNL_DATA_MSG_ACK(work_item_t* w)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = m_pSockaddrMaster;
	w->data.send_data.timeout_try = 0;
	T_XNL_DATA_MSG_ACK networkData = { 0 };
	networkData.Opcode = LE_XNL_DATA_MSG_ACK;
	networkData.ProtocolId = LE_XNL_PROTOCOL_ID;
	//networkData.XnlFlags = LE_XNL_FLAGS;
	networkData.DesAddress = m_desAddress;
	networkData.SourceAddress = m_sourceAddress;
	networkData.TrascationId = w->data.send_data.protocol.xcmp.Payload.XNL_DATA_MSG_ACK.TrascationId;
	networkData.PayloadLength = 0x0000;
	char buf[BUFSIZ] = { 0 };
	int len = Build_XCMP_XNL_DATA_MSG_ACK(buf, &networkData);
	moto_protocol_xcmp_xnl_t m_moto_protocol_xcmp_xnl_t = { 0 };
	m_moto_protocol_xcmp_xnl_t.Opcode = LE_XNL;
	m_moto_protocol_xcmp_xnl_t.PeerId = m_netParam.local_peer_id;
	m_moto_protocol_xcmp_xnl_t.lenth = sizeof(m_moto_protocol_xcmp_xnl_t.Payload.XNL_DATA_MSG);
	w->data.send_data.protocol.xcmp = m_moto_protocol_xcmp_xnl_t;
	w->data.send_data.net_lenth = Build_LE_XCMP_XNL(w->data.send_data.net_data, &m_moto_protocol_xcmp_xnl_t, buf, len);
}

void NSSerial::EncryptAuthenticationValue(unsigned char* un_Auth, unsigned char* en_Auth)
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

DWORD NSSerial::Build_XCMP_XNL_DEVICE_MASTER_QUERY(CHAR* pPacket, T_XNL_DEVICE_MASTER_QUERY* pData)
{
	//unsigned short Opcode;
	//unsigned char  ProtocolId;
	//unsigned char  XnlFlags;
	//unsigned short DesAddress;
	//unsigned short SourceAddress;
	//unsigned short TrascationId;
	//unsigned short PayloadLength;
	DWORD size;
	/*Opcode*/
	*((WORD*)(&pPacket[0])) = htons(pData->Opcode);
	/*ProtocolId*/
	pPacket[2] = pData->ProtocolId;
	/*XnlFlags*/
	pPacket[3] = pData->XnlFlags;
	/*DesAddress*/
	*((WORD*)(&pPacket[4])) = htons(pData->DesAddress);
	/*SourceAddress*/
	*((WORD*)(&pPacket[6])) = htons(pData->SourceAddress);
	/*TrascationId*/
	*((WORD*)(&pPacket[8])) = htons(pData->TrascationId);
	/*PayloadLength*/
	*((WORD*)(&pPacket[10])) = htons(pData->PayloadLength);
	size = 12;
	return size;
}

DWORD NSSerial::Build_XCMP_XNL_DEVICE_CONN_REQUEST(CHAR* pPacket, T_XNL_DEVICE_CONN_REQUEST* pData)
{
	DWORD size;
	/*Opcode*/
	*((WORD*)(&pPacket[0])) = htons(pData->Opcode);
	/*ProtocolId*/
	pPacket[2] = pData->ProtocolId;
	/*XnlFlags*/
	pPacket[3] = pData->XnlFlags;
	/*DesAddress*/
	*((WORD*)(&pPacket[4])) = htons(pData->DesAddress);
	/*SourceAddress*/
	*((WORD*)(&pPacket[6])) = htons(pData->SourceAddress);
	/*TrascationId*/
	*((WORD*)(&pPacket[8])) = htons(pData->TrascationId);
	/*PayloadLength*/
	*((WORD*)(&pPacket[10])) = htons(pData->PayloadLength);
	/*PreferredXnlAddress*/
	*((WORD*)(&pPacket[12])) = htons(pData->PreferredXnlAddress);
	pPacket[14] = pData->DeviceType;
	pPacket[15] = pData->AuthenticationIndex;
	memcpy(&pPacket[16], pData->EncryptedAuthenticationValue, 8);
	size = 24;
	return size;
}

DWORD NSSerial::Build_XCMP_XNL_DEVICE_AUTH_KEY_REQUEST(CHAR* pPacket, T_XNL_DEVICE_AUTH_KEY_REQUEST* pData)
{
	DWORD size;
	/*Opcode*/
	*((WORD*)(&pPacket[0])) = htons(pData->Opcode);
	/*ProtocolId*/
	pPacket[2] = pData->ProtocolId;
	/*XnlFlags*/
	pPacket[3] = pData->XnlFlags;
	/*DesAddress*/
	*((WORD*)(&pPacket[4])) = htons(pData->DesAddress);
	/*SourceAddress*/
	*((WORD*)(&pPacket[6])) = htons(pData->SourceAddress);
	/*TrascationId*/
	*((WORD*)(&pPacket[8])) = htons(pData->TrascationId);
	/*PayloadLength*/
	*((WORD*)(&pPacket[10])) = htons(pData->PayloadLength);
	size = 12;
	return size;
}

DWORD NSSerial::Build_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(CHAR* pPacket, T_XNL_DATA_MSG* pData)
{
	DWORD size;
	/*Opcode*/
	*((WORD*)(&pPacket[0])) = htons(pData->Opcode);
	/*ProtocolId*/
	pPacket[2] = pData->ProtocolId;
	/*XnlFlags*/
	pPacket[3] = pData->XnlFlags;
	/*DesAddress*/
	*((WORD*)(&pPacket[4])) = htons(pData->DesAddress);
	/*SourceAddress*/
	*((WORD*)(&pPacket[6])) = htons(pData->SourceAddress);
	/*TrascationId*/
	*((WORD*)(&pPacket[8])) = htons(pData->TrascationId);
	/*PayloadLength*/
	*((WORD*)(&pPacket[10])) = htons(pData->PayloadLength);
	/*RadioStatus Opcode*/
	*((WORD*)(&pPacket[12])) = htons(pData->Payload.RadioStatus.RadioStatusRequest.Opcode);
	pPacket[14] = pData->Payload.RadioStatus.RadioStatusRequest.Condition;
	size = 15;
	return size;
}

DWORD NSSerial::Build_XCMP_XNL_DATA_MSG_ACK(CHAR* pPacket, T_XNL_DATA_MSG_ACK* pData)
{
	DWORD size;
	/*Opcode*/
	*((WORD*)(&pPacket[0])) = htons(pData->Opcode);
	/*ProtocolId*/
	pPacket[2] = pData->ProtocolId;
	/*XnlFlags*/
	pPacket[3] = pData->XnlFlags;
	/*DesAddress*/
	*((WORD*)(&pPacket[4])) = htons(pData->DesAddress);
	/*SourceAddress*/
	*((WORD*)(&pPacket[6])) = htons(pData->SourceAddress);
	/*TrascationId*/
	*((WORD*)(&pPacket[8])) = htons(pData->TrascationId);
	/*PayloadLength*/
	*((WORD*)(&pPacket[10])) = htons(pData->PayloadLength);
	size = 12;
	return size;
}

DWORD NSSerial::Build_LE_XCMP_XNL(CHAR* pPacket, moto_protocol_xcmp_xnl_t* pData, CHAR* pXnlPacket, int length)
{
	/*unsigned char Opcode;
	unsigned long PeerId;
	unsigned short lenth;
	T_XNL          Payload;*/
	/*opcode*/
	pPacket[0] = pData->Opcode;
	/* peer id*/
	*((DWORD*)(&pPacket[1])) = htonl(pData->PeerId);
	/*length*/
	*((WORD*)(&pPacket[5])) = htons(pData->lenth);
	/*payload*/
	memcpy(&pPacket[7], pXnlPacket, length);
	return length + 7;
}

void NSSerial::RECV_LE_XCMP_XNL(work_item_t * w, const char* pData)
{
	if (NULL == pData)
	{
		return;
	}
	w->type = Recive;
	switch (ntohs(*(WORD*)(&pData[7])))
	{
	case LE_XNL_MASTER_STATUS_BRDCST:
		Unpack_XCMP_XNL_MASTER_STATUS_BRDCST((char*)pData, &(w->data.recive_data.protocol.xcmp));
		break;
	case LE_XNL_DEVICE_AUTH_KEY_REPLY:
		Unpack_XCMP_XNL_DEVICE_AUTH_KEY_REPLY((char*)pData, &(w->data.recive_data.protocol.xcmp));
		break;
	case LE_XNL_DEVICE_CONN_REPLY:
		Unpack_XCMP_XNL_DEVICE_CONN_REPLY((char*)pData, &(w->data.recive_data.protocol.xcmp));
		break;
	case LE_XNL_DEVICE_SYSMAP_BRDCST:
		break;
	case LE_XNL_DATA_MSG:
		Unpack_XCMP_XNL_DATA_MSG((char*)pData, &(w->data.recive_data.protocol.xcmp));
		break;
	default:
		break;
	}
}

void NSSerial::Unpack_XCMP_XNL_MASTER_STATUS_BRDCST(char* pData, moto_protocol_xcmp_xnl_t* networkData)
{
	/*opcode*/
	networkData->Opcode = pData[0];
	/* peer id*/
	networkData->PeerId = ntohl(*((DWORD*)(&pData[1])));
	/*length */
	networkData->lenth = ntohs(*(WORD*)(&pData[5]));
	/*xnl*/
	/*opcode*/
	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.Opcode = ntohs(*(WORD*)(&pData[7]));
	/*protocol id */
	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.ProtocolId = pData[9];
	/*xnl flags*/
	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.XnlFlags = pData[10];
	/* Destination Address */
	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.DesAddress = ntohs(*(WORD*)(&pData[11]));
	/*SourceAddress   ------  Master’s XNL address*/
	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.SourceAddress = ntohs(*(WORD*)(&pData[13]));
	m_desAddress = networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.SourceAddress;
	/*TrascationId*/
	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.TrascationId = ntohs(*(WORD*)(&pData[15]));
	/* payload length*/
	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.PayloadLength = ntohs(*(WORD*)(&pData[17]));
	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.MinorXNLVersionNumber = ntohs(*(WORD*)(&pData[19]));
	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.MajorXNVersioNumber = ntohs(*(WORD*)(&pData[21]));
	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.MasterLogicalIdentifier = ntohs(*(WORD*)(&pData[23]));
	networkData->Payload.DEVICE_MASTER_STATUS_BRDCST.DataMessageSent = pData[24];
}

void NSSerial::Unpack_XCMP_XNL_DEVICE_AUTH_KEY_REPLY(char* pData, moto_protocol_xcmp_xnl_t* networkData)
{
	/*opcode*/
	networkData->Opcode = pData[0];
	/* peer id*/
	networkData->PeerId = ntohl(*((DWORD*)(&pData[1])));
	/*length */
	networkData->lenth = ntohs(*(WORD*)(&pData[5]));
	/*xnl*/
	/*opcode*/
	networkData->Payload.DEVICE_AUTH_KEY_REPLY.Opcode = ntohs(*(WORD*)(&pData[7]));
	/*protocol id */
	networkData->Payload.DEVICE_AUTH_KEY_REPLY.ProtocolId = pData[9];
	/*xnl flags*/
	networkData->Payload.DEVICE_AUTH_KEY_REPLY.XnlFlags = pData[10];
	/* Destination Address */
	networkData->Payload.DEVICE_AUTH_KEY_REPLY.DesAddress = ntohs(*(WORD*)(&pData[11]));
	/*SourceAddress   ------  Master’s XNL address*/
	networkData->Payload.DEVICE_AUTH_KEY_REPLY.SourceAddress = ntohs(*(WORD*)(&pData[13]));
	/*TrascationId*/
	networkData->Payload.DEVICE_AUTH_KEY_REPLY.TrascationId = ntohs(*(WORD*)(&pData[15]));
	/* payload length*/
	networkData->Payload.DEVICE_AUTH_KEY_REPLY.PayloadLength = ntohs(*(WORD*)(&pData[17]));
	/*Temporary XNl Address*/
	networkData->Payload.DEVICE_AUTH_KEY_REPLY.TemporaryXnlAddress = ntohs(*(WORD*)(&pData[19]));
	/*UnencryptedAuthenticationValue*/
	memcpy(networkData->Payload.DEVICE_AUTH_KEY_REPLY.UnencryptedAuthenticationValue, &pData[21], 8);
}

void NSSerial::Unpack_XCMP_XNL_DEVICE_CONN_REPLY(char* pData, moto_protocol_xcmp_xnl_t* networkData)
{
	/*opcode*/
	networkData->Opcode = pData[0];
	/* peer id*/
	networkData->PeerId = ntohl(*((DWORD*)(&pData[1])));
	/*length */
	networkData->lenth = ntohs(*(WORD*)(&pData[5]));
	/*xnl*/
	/*opcode*/
	networkData->Payload.DEVICE_CONN_REPLY.Opcode = ntohs(*(WORD*)(&pData[7]));
	/*protocol id */
	networkData->Payload.DEVICE_CONN_REPLY.ProtocolId = pData[9];
	/*xnl flags*/
	networkData->Payload.DEVICE_CONN_REPLY.XnlFlags = pData[10];
	/* Destination Address */
	networkData->Payload.DEVICE_CONN_REPLY.DesAddress = ntohs(*(WORD*)(&pData[11]));
	/*SourceAddress   ------  Master’s XNL address*/
	networkData->Payload.DEVICE_CONN_REPLY.SourceAddress = ntohs(*(WORD*)(&pData[13]));
	/*TrascationId*/
	networkData->Payload.DEVICE_CONN_REPLY.TrascationId = ntohs(*(WORD*)(&pData[15]));
	/* payload length*/
	networkData->Payload.DEVICE_CONN_REPLY.PayloadLength = ntohs(*(WORD*)(&pData[17]));
	networkData->Payload.DEVICE_CONN_REPLY.ResultCode = pData[19];
	networkData->Payload.DEVICE_CONN_REPLY.TrascationIdBase = pData[20];
	m_sourceAddress = ntohs(*(WORD*)(&pData[21]));
	networkData->Payload.DEVICE_CONN_REPLY.XnlAddress = ntohs(*(WORD*)(&pData[22]));
	networkData->Payload.DEVICE_CONN_REPLY.LogicalAddress = ntohs(*(WORD*)(&pData[23]));
	memcpy(networkData->Payload.DEVICE_CONN_REPLY.EncryptedAuthenticationValue, &pData[24], 8);
}

void NSSerial::Unpack_XCMP_XNL_DATA_MSG(char* pData, moto_protocol_xcmp_xnl_t* networkData)
{
	/*opcode*/
	networkData->Opcode = pData[0];
	/* peer id*/
	networkData->PeerId = ntohl(*((DWORD*)(&pData[1])));
	/*length */
	networkData->lenth = ntohs(*(WORD*)(&pData[5]));
	/*xnl*/
	/*opcode*/
	networkData->Payload.XNL_DATA_MSG.Opcode = ntohs(*(WORD*)(&pData[7]));
	/*protocol id */
	networkData->Payload.XNL_DATA_MSG.ProtocolId = pData[9];
	/*xnl flags*/
	networkData->Payload.XNL_DATA_MSG.XnlFlags = pData[10];
	/* Destination Address */
	networkData->Payload.XNL_DATA_MSG.DesAddress = ntohs(*(WORD*)(&pData[11]));
	/*SourceAddress   ------  Master’s XNL address*/
	networkData->Payload.XNL_DATA_MSG.SourceAddress = ntohs(*(WORD*)(&pData[13]));
	/*TrascationId*/
	m_trascationId = ntohs(*(WORD*)(&pData[15]));
	networkData->Payload.XNL_DATA_MSG.TrascationId = ntohs(*(WORD*)(&pData[15]));
	/* payload length*/
	networkData->Payload.XNL_DATA_MSG.PayloadLength = ntohs(*(WORD*)(&pData[17]));
	if (LE_XNL_XCMP_RADIO_STATUS_REPLY == ntohs(*(WORD*)(&pData[19]))) // reply 得到序列号  
	{
		networkData->Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.Opcode = LE_XNL_XCMP_RADIO_STATUS_REPLY;
		if (0 == (networkData->Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.Result = pData[21]))
		{
			memcpy(networkData->Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.ProductSerialNumber, &pData[23], 10);
		}
	}
	else if (LE_XNL_XCMP_RADIO_STATUS_BRDCST == ntohs(*(WORD*)(&pData[19])))  //broadcast 得到序列号
	{
		memcpy(networkData->Payload.XNL_DATA_MSG.Payload.RadioStatus.RadioStatusReply.ProductSerialNumber, &pData[22], 10);
	}
}

void NSSerial::ClearSerialLink()
{
	TRYLOCK(m_serialMutex);
	pLinkItem it = popFront(&m_serialLink);
	while (it)
	{
		if (NULL != it->data)
		{
			delete (work_item_t *)it->data;
			it->data = NULL;
		}
		freeList(it);
		it = popFront(&m_serialLink);
	}
	RELEASELOCK(m_serialMutex);
}

void NSSerial::SendXnlToMaster(work_item_t *w, unsigned long timeout /*= TIMEOUT_LE*/)
{
	send_data_t* pSend = &w->data.send_data;
	//if (sendDataUdp(m_pMasterXqttnet, pSend->net_data, pSend->net_lenth, (SOCKADDR_IN*)pSend->send_to, sizeof(SOCKADDR_IN)))
	//{
	if (m_pCallParam->sendNetDataBase(pSend->net_data, pSend->net_lenth, pSend->send_to))
	{
		if (0 != timeout)
		{
			pSend->timeout_send = GetTickCount() + timeout;
		}
		//m_pLog->AddLog("send xnl to master !");
		if (NULL != w)
		{
			delete w;
			w = NULL;
		}
	}
}

xnl_status_enum NSSerial::GetXnlStatus()
{
	return m_xnl_status_enum;
}

void NSSerial::SetXnlStatus(xnl_status_enum value)
{
	if (m_xnl_status_enum != value)
	{
		char temp[64] = { 0 };
		sprintf_s(temp, "=====Serial Status From %d To %d=====", m_xnl_status_enum, value);
		m_pLog->AddLog(Ns_Log_Info, temp);
		m_xnl_status_enum = value;
		if (GET_SERIAL_SUCCESS == m_xnl_status_enum)
		{
				onsystemstatuschange_info_t info = { 0 };
				info.type = System_DeviceInfoStatus;
				info.value = WL_SERIL_SUC;
				NS_SafeSystemStatusChangeEvent(&info);
			//if (g_manager)
			//{
			//	g_manager->setDeviceInfoStatus(WL_SERIL_SUC);
			//}
		}
		else
		{
				onsystemstatuschange_info_t info = { 0 };
				info.type = System_DeviceInfoStatus;
				info.value = WL_SERIL_FAL;
				NS_SafeSystemStatusChangeEvent(&info);
			//if (g_manager)
			//{
			//	g_manager->setDeviceInfoStatus(WL_SERIL_FAL);
			//}
		}
	}
}

