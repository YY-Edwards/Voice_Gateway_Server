#include "stdafx.h"
#include "../include/P2PPeer.h"
#include "../include/P2PNet.h"
#include "../include/NSLog.h"


CP2PPeer::CP2PPeer(p2p_peer_build_param_t* p)
: m_peerId(p->peerId)
, m_p2pNet(p->pNet)
, m_netParam(*(p->pNetParam))
{
	memset(&m_socket, 0, sizeof(struct sockaddr_in));
	m_socket.sin_family = AF_INET;
	m_socket.sin_addr.s_addr = p->addr;
	m_socket.sin_port = htons(p->port);
}


CP2PPeer::~CP2PPeer()
{
}
unsigned long CP2PPeer::PeerId()
{
	return m_peerId;
}
void CP2PPeer::LE_PEER_REGISTRATION()
{
	work_item_t* w = new work_item_t;
	memset(w, 0, sizeof(work_item_t));
	SEND_LE_PEER_REGISTRATION_REQUEST(w);
	m_p2pNet->AddWorkItem(w);
}
void CP2PPeer::Handle_NetPack(char Opcode)
{
	switch (Opcode)
	{
	case LE_PEER_REGISTRATION_REQUEST:
	{
		work_item_t* w = new work_item_t;
		memset(w, 0, sizeof(work_item_t));
		SEND_LE_PEER_REGISTRATION_RESPONSE(w);
		m_p2pNet->AddWorkItem(w);
	}
		break;
	default:
		break;
	}
}
void CP2PPeer::SEND_LE_PEER_REGISTRATION_REQUEST(work_item_t* w)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = &m_socket;
	w->data.send_data.timeout_try = 0xffff;  
	w->data.send_data.pFrom = this;
	if (IPSC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_94 networkData = { 0 };
		networkData.Opcode = LE_PEER_REGISTRATION_REQUEST;
		networkData.peerID = m_netParam.local_peer_id;
		networkData.currentLinkProtocolVersion = P2P_IPSC_CURRENTLPVERSION;
		networkData.oldestLinkProtocolVersion = P2P_IPSC_OLDESTPVERSION;
		w->data.send_data.protocol.le.PROTOCOL_94 = networkData;
		w->data.send_data.net_lenth = Build_LE_PEER_REGISTRATION_REQUEST(w->data.send_data.net_data, &networkData);
	}
	else if (CPC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_94 networkData = { 0 };
		networkData.Opcode = LE_PEER_REGISTRATION_REQUEST;
		networkData.peerID = m_netParam.local_peer_id;
		networkData.currentLinkProtocolVersion = P2P_CPC_CURRENTLPVERSION;
		networkData.oldestLinkProtocolVersion = P2P_CPC_OLDESTPVERSION;
		w->data.send_data.net_lenth = Build_LE_PEER_REGISTRATION_REQUEST(w->data.send_data.net_data, &networkData);
		networkData.length = w->data.send_data.net_lenth;
		w->data.send_data.protocol.le.PROTOCOL_94 = networkData;
		
	}
}
void CP2PPeer::SEND_LE_PEER_REGISTRATION_RESPONSE(work_item_t* w)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = &m_socket;
	w->data.send_data.timeout_try = TIMEOUT_TRY_COUNT;
	w->data.send_data.pFrom = this;
	if (IPSC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_95 networkData = { 0 };
		networkData.Opcode = LE_PEER_REGISTRATION_RESPONSE;
		networkData.peerID = m_netParam.local_peer_id;
		networkData.currentLinkProtocolVersion = P2P_IPSC_CURRENTLPVERSION;
		networkData.oldestLinkProtocolVersion = P2P_IPSC_OLDESTPVERSION;
		w->data.send_data.protocol.le.PROTOCOL_95 = networkData;
		w->data.send_data.net_lenth = Build_LE_PEER_REGISTRATION_RESPONSE(w->data.send_data.net_data, &networkData);
	}
	else if (CPC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_95 networkData = { 0 };
		networkData.Opcode = LE_PEER_REGISTRATION_RESPONSE;
		networkData.peerID = m_netParam.local_peer_id;
		networkData.currentLinkProtocolVersion = P2P_CPC_CURRENTLPVERSION;
		networkData.oldestLinkProtocolVersion = P2P_CPC_OLDESTPVERSION;
		w->data.send_data.protocol.le.PROTOCOL_95 = networkData;
		w->data.send_data.net_lenth = Build_LE_PEER_REGISTRATION_RESPONSE(w->data.send_data.net_data, &networkData);
	}
}
void CP2PPeer::SEND_LE_PEER_KEEP_ALIVE_RESPONSE(work_item_t* w)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = &m_socket;
	w->data.send_data.timeout_try = TIMEOUT_TRY_COUNT;
	w->data.send_data.pFrom = this;
	if (IPSC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_99 networkData = { 0 };
		networkData.Opcode = LE_PEER_KEEP_ALIVE_RESPONSE;
		networkData.peerID = m_netParam.local_peer_id;
		networkData.peerMode = P2P_IPSC_MODE;
		networkData.peerServices = P2P_IPSC_SERVICES;
		w->data.send_data.protocol.le.PROTOCOL_99 = networkData;
		w->data.send_data.net_lenth = Build_LE_PEER_KEEP_ALIVE_RESPONSE(w->data.send_data.net_data, &networkData);
	}
	else if (CPC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_99 networkData = { 0 };
		networkData.Opcode = LE_PEER_KEEP_ALIVE_RESPONSE;
		networkData.peerID = m_netParam.local_peer_id;
		networkData.peerMode = P2P_CPC_MODE;
		networkData.peerServices = P2P_CPC_SERVICES;
		w->data.send_data.protocol.le.PROTOCOL_99 = networkData;
		w->data.send_data.net_lenth = Build_LE_PEER_KEEP_ALIVE_RESPONSE(w->data.send_data.net_data, &networkData);
	}
}
void CP2PPeer::SEND_LE_PEER_KEEP_ALIVE_REQUEST(work_item_t* w,unsigned long timing)
{
	if (NULL == w)
	{
		return;
	}
	w->type = Send;
	w->data.send_data.send_to = &m_socket;
	w->data.send_data.timeout_try = TIMEOUT_TRY_COUNT;
	w->data.send_data.pFrom = this;
	if (0 != timing)
	{
		w->data.send_data.timing = GetTickCount() + timing;
	}
	if (IPSC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_98 networkData = { 0 };
		networkData.Opcode = LE_PEER_KEEP_ALIVE_REQUEST;
		networkData.peerID = m_netParam.local_peer_id;
		networkData.peerMode = P2P_IPSC_MODE;
		networkData.peerServices = P2P_IPSC_SERVICES;
		w->data.send_data.protocol.le.PROTOCOL_98 = networkData;
		w->data.send_data.net_lenth = Build_LE_PEER_KEEP_ALIVE_REQUEST(w->data.send_data.net_data, &networkData);
	}
	else if (CPC == m_netParam.work_mode)
	{
		T_LE_PROTOCOL_98 networkData = { 0 };
		networkData.Opcode = LE_PEER_KEEP_ALIVE_REQUEST;
		networkData.peerID = m_netParam.local_peer_id;
		networkData.peerMode = P2P_CPC_MODE;
		networkData.peerServices = P2P_CPC_SERVICES;
		w->data.send_data.protocol.le.PROTOCOL_98 = networkData;
		w->data.send_data.net_lenth = Build_LE_PEER_KEEP_ALIVE_REQUEST(w->data.send_data.net_data, &networkData);
	}
}
DWORD CP2PPeer::Build_LE_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_94* pData)
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
DWORD CP2PPeer::Build_LE_PEER_REGISTRATION_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_95* pData)
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
DWORD CP2PPeer::Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99* pData)
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
DWORD CP2PPeer::Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98* pData)
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