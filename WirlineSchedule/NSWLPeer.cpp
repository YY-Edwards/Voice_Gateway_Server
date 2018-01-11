#include "stdafx.h"
#include "NSWLPeer.h"
#include "NSWLNet.h"
#include "HMAC_SHA1.h"
#include "NSLog.h"
#include "Manager.h"

#define AUTHENTIC_ID_SIZE	4
#define VENDER_KEY_SIZE		20

static const unsigned char AuthenticId[AUTHENTIC_ID_SIZE] = { 0x01, 0x02, 0x00, 0x0d };
static const unsigned char VenderKey[VENDER_KEY_SIZE] = { 0x6b, 0xe5, 0xff, 0x95, 0x6a, 0xb5, 0xe8, 0x82, 0xa8, 0x6f, 0x29, 0x5f, 0x9d, 0x9d, 0x5e, 0xcf, 0xe6, 0x57, 0x61, 0x5a };

NSWLPeer::NSWLPeer(wl_peer_build_param_t* p)
:m_peerId(p->peerId)
, m_pNet(p->pNet)
, m_netParam(*(p->pNetParam))
, m_bRemote3rdParty(false)
, m_pLog(NSLog::instance())
, m_wlRegStatus(WL_REG_UNKNOWN)
, m_slotNumber(NULL_SLOT)
, m_recordFile(NULL)
{
	memset(&m_sockaddr, 0, sizeof(struct sockaddr_in));
	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_addr.s_addr = p->addr;
	m_sockaddr.sin_port = htons(p->port);
#if _DEBUG
	in_addr addr;
	addr.S_un.S_addr = p->addr;
	char temp[128] = { 0 };
	sprintf_s(temp, "m_peerId:%lu,ip:%s,port:%u", m_peerId, inet_ntoa(addr), p->port);
	m_pLog->AddLog(temp);
#endif // _DEBUG

}

NSWLPeer::~NSWLPeer()
{
	m_recordFile = NULL;
}

unsigned long NSWLPeer::PeerId()
{
	return m_peerId;
}

void NSWLPeer::WL_REGISTRATION()
{
	/*wire line 注册*/
	work_item_t* pItem1 = new work_item_t;
	memset(pItem1, 0, sizeof(work_item_t));
	work_item_t* pItem2 = new work_item_t;
	memset(pItem2, 0, sizeof(work_item_t));
	Build_WorkItem_WL_01(pItem1, pItem2);
	if (pItem1)
	{
		m_pNet->AddWorkItem(pItem1);
	}
	if (pItem2)
	{
		m_pNet->AddWorkItem(pItem2);
	}
}

void NSWLPeer::Build_WorkItem_WL_01(work_item_t* &p1, work_item_t* &p2)
{
	if (NULL == p1 || NULL == p2)
	{
		return;
	}
	p1->type = Send;
	p1->data.send_data.send_to = &m_sockaddr;
	p1->data.send_data.timeout_try = 0xffff;
	p1->data.send_data.pFrom = this;
	T_WL_PROTOCOL_01 *pNetworkData = NULL;
	send_data_t* pSendData = &p1->data.send_data;
	pNetworkData = &pSendData->protocol.wl.PROTOCOL_01;
	pNetworkData->currentLinkProtocolVersion = Wireline_Protocol_Version;
	pNetworkData->numberOfRegistrationEntries = NUMBER_REGIS_ENTRIES;
	pNetworkData->oldestLinkProtocolVersion = Wireline_Protocol_Version;
	pNetworkData->Opcode = WL_PROTOCOL;
	pNetworkData->peerID = m_netParam.local_peer_id;
	pNetworkData->registrationID = REGISTRATION_ID;
	pNetworkData->registrationPduID = REGISTRATION_PDU_ID;
	pNetworkData->registrationSlotNumber = BOTH_SLOT1_SLOT2;
	pNetworkData->wirelineOpcode = WL_REGISTRATION_REQUEST;
	pNetworkData->wirelineStatusRegistration = REGISTERED_VOICE_SERVICE;
	pNetworkData->wlRegistrationEntries[0].VoiceAttributes = REGISTERED_VOICE_SERVICE;
	pNetworkData->wlRegistrationEntries[0].AddressType = AllIndividualCall;
	pNetworkData->wlRegistrationEntries[1].VoiceAttributes = REGISTERED_VOICE_SERVICE;
	pNetworkData->wlRegistrationEntries[1].AddressType = AllTalkGroupCall;
	//依据:devspec_nai_voice_csbk_0102.pdf line 998
	if (LCP == m_netParam.work_mode)
	{
		/*判定是否为第三方peer*/
		if (0xFF000000 & PeerId())
		{
			setRemote3rdParty(false);
		}
		else
		{
			setRemote3rdParty(true);
		}
		if (!Remote3rdParty())
		{
			if (0x00FFFFFF & PeerId())
			{
				pNetworkData->registrationSlotNumber = BOTH_SLOT1_SLOT2;
				pSendData->net_lenth = Build_WL_REGISTRATION_REQUEST(pSendData->net_data, pNetworkData);
			}
			else
			{
				m_pNet->SetSitePeer(this);
				delete p1;
				p1 = NULL;
			}
		}
		delete p2;
		p2 = NULL;
	}
	else if (CPC == m_netParam.work_mode)
	{
		if (!Remote3rdParty())
		{
			if (0 != PeerId())
			{
				pNetworkData->registrationSlotNumber = BOTH_SLOT1_SLOT2;
				pSendData->net_lenth = Build_WL_REGISTRATION_REQUEST(pSendData->net_data, pNetworkData);
			}
			else
			{
				m_pNet->SetSitePeer(this);
				delete p1;
				p1 = NULL;
			}
		}
		delete p2;
		p2 = NULL;
	}
	else
	{
		if (!Remote3rdParty())
		{
			pNetworkData->registrationSlotNumber = SLOT1;
			pSendData->net_lenth = Build_WL_REGISTRATION_REQUEST(pSendData->net_data, pNetworkData);

			//memcpy(p2, p1, sizeof(work_item_t));
			*p2 = *p1;
			pSendData = &p2->data.send_data;
			pNetworkData = &pSendData->protocol.wl.PROTOCOL_01;
			pNetworkData->registrationSlotNumber = SLOT2;
			pSendData->net_lenth = Build_WL_REGISTRATION_REQUEST(pSendData->net_data, pNetworkData);
		}
		else
		{
			delete p1;
			p1 = NULL;
			delete p2;
			p2 = NULL;
		}
	}
}

bool NSWLPeer::Remote3rdParty()
{
	return m_bRemote3rdParty;
}

void NSWLPeer::setRemote3rdParty(bool value)
{
	if (value != m_bRemote3rdParty)
	{
		m_bRemote3rdParty = value;
	}
}

DWORD NSWLPeer::Build_WL_REGISTRATION_REQUEST(CHAR* pPacket, T_WL_PROTOCOL_01* pData)
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

void NSWLPeer::getWirelineAuthentication(char* pPacket, DWORD &size)
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

void NSWLPeer::LE_PEER_REGISTRATION()
{
	work_item_t* p = new work_item_t;
	memset(p, 0, sizeof(work_item_t));
	Build_WorkItem_LE_94(p);
	m_pNet->AddWorkItem(p);
}

void NSWLPeer::Build_WorkItem_LE_94(work_item_t* p)
{
	if (NULL == p)
	{
		return;
	}
	work_mode_enum workMode = m_netParam.work_mode;
	send_data_t* pSendData = &p->data.send_data;
	p->type = Send;
	pSendData->send_to = &m_sockaddr;
	pSendData->timeout_try = 0xffff;
	pSendData->pFrom = this;
	T_LE_PROTOCOL_94* pNetworkData = NULL;
	if (LCP == workMode)
	{
		pNetworkData = &pSendData->protocol.le_lcp.PROTOCOL_94_LCP;
		pNetworkData->currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
		pNetworkData->oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
	}
	else
	{
		pNetworkData = &pSendData->protocol.le.PROTOCOL_94;
		if (CPC == workMode)
		{
			pNetworkData->currentLinkProtocolVersion = CPC_CURRENTLPVERSION;
			pNetworkData->oldestLinkProtocolVersion = CPC_OLDESTPVERSION;
		}
		else
		{
			pNetworkData->currentLinkProtocolVersion = IPSC_CURRENTLPVERSION;
			pNetworkData->oldestLinkProtocolVersion = IPSC_OLDESTPVERSION;
		}
	}
	pNetworkData->Opcode = LE_PEER_REGISTRATION_REQUEST;
	pNetworkData->peerID = m_netParam.local_peer_id;
	pSendData->net_lenth = Build_LE_PEER_REGISTRATION_REQUEST(pSendData->net_data, pNetworkData);
}

DWORD NSWLPeer::Build_LE_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_94* pData)
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

void NSWLPeer::Build_WorkItem_LE_95(work_item_t* p)
{
	if (NULL == p)
	{
		return;
	}
	work_mode_enum workMode = m_netParam.work_mode;
	send_data_t* pSendData = &p->data.send_data;
	p->type = Send;
	pSendData->send_to = &m_sockaddr;
	pSendData->timeout_try = 0xffff;
	pSendData->pFrom = this;
	T_LE_PROTOCOL_95* pNetworkData = NULL;
	if (LCP == workMode)
	{
		pNetworkData = &pSendData->protocol.le_lcp.PROTOCOL_95_LCP;
		pNetworkData->currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
		pNetworkData->oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
	}
	else
	{
		pNetworkData = &pSendData->protocol.le.PROTOCOL_95;
		if (CPC == workMode)
		{
			pNetworkData->currentLinkProtocolVersion = CPC_CURRENTLPVERSION;
			pNetworkData->oldestLinkProtocolVersion = CPC_OLDESTPVERSION;
		}
		else
		{
			pNetworkData->currentLinkProtocolVersion = IPSC_CURRENTLPVERSION;
			pNetworkData->oldestLinkProtocolVersion = IPSC_OLDESTPVERSION;
		}
	}
	pNetworkData->Opcode = LE_PEER_REGISTRATION_RESPONSE;
	pNetworkData->peerID = m_netParam.local_peer_id;
	pSendData->net_lenth = Build_LE_PEER_REGISTRATION_RESPONSE(pSendData->net_data, pNetworkData);
}

DWORD NSWLPeer::Build_LE_PEER_REGISTRATION_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_95* pData)
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

void NSWLPeer::Handle_LE_PEER_REGISTRATION_REQUEST_Recive()
{
	/*立即发送0x95*/
	work_item_t* p = new work_item_t;
	memset(p, 0, sizeof(work_item_t));
	Build_WorkItem_LE_95(p);
	m_pNet->sendNetDataBase(p->data.send_data.net_data, p->data.send_data.net_lenth, &m_sockaddr);
	delete p;
	p = NULL;
}

void NSWLPeer::Handle_LE_PEER_REGISTRATION_RESPONSE_Recive()
{
	work_item_t* p = new work_item_t;
	memset(p, 0, sizeof(work_item_t));
	Build_WorkItem_LE_98(p);
	m_pNet->AddWorkItem(p);
}

void NSWLPeer::Build_WorkItem_LE_98(work_item_t* p, unsigned long timing /*= 0*/)
{
	if (NULL == p)
	{
		return;
	}
	work_mode_enum workMode = m_netParam.work_mode;
	send_data_t* pSendData = &p->data.send_data;
	p->type = Send;
	pSendData->send_to = &m_sockaddr;
	pSendData->timeout_try = TIMEOUT_TRY_COUNT_98;
	pSendData->pFrom = this;
	if (0 != timing)
	{
		pSendData->timing = GetTickCount() + timing;
	}
	if (LCP == workMode)
	{
		T_LE_PROTOCOL_98_LCP* pNetworkData = NULL;
		pNetworkData = &pSendData->protocol.le_lcp.PROTOCOL_98_LCP;
		pNetworkData->currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
		pNetworkData->oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
		pNetworkData->Opcode = LE_PEER_KEEP_ALIVE_REQUEST;
		pNetworkData->peerID = m_netParam.local_peer_id;
		pNetworkData->peerMode = LCP_MODE;
		pNetworkData->peerServices = LCP_SERVICES;
		pSendData->net_lenth = Build_LE_PEER_KEEP_ALIVE_REQUEST(pSendData->net_data, pNetworkData);
	}
	else
	{
		T_LE_PROTOCOL_98* pNetworkData = NULL;
		pNetworkData = &pSendData->protocol.le.PROTOCOL_98;
		pNetworkData->Opcode = LE_PEER_KEEP_ALIVE_REQUEST;
		pNetworkData->peerID = m_netParam.local_peer_id;
		if (CPC == workMode)
		{
			pNetworkData->peerMode = CPC_MODE;
			pNetworkData->peerServices = CPC_SERVICES;
		}
		else
		{
			pNetworkData->peerMode = IPSC_MODE;
			pNetworkData->peerServices = IPSC_SERVICES;
		}
		pSendData->net_lenth = Build_LE_PEER_KEEP_ALIVE_REQUEST(pSendData->net_data, pNetworkData);
	}

}

DWORD NSWLPeer::Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98* pData)
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

DWORD NSWLPeer::Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98_LCP* pData)
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

void NSWLPeer::Handle_LE_PEER_KEEP_ALIVE_REQUEST_Recive()
{
	/*立即发送0x99*/
	work_item_t* p = new work_item_t;
	memset(p, 0, sizeof(work_item_t));
	Build_WorkItem_LE_99(p);
	m_pNet->sendNetDataBase(p->data.send_data.net_data, p->data.send_data.net_lenth, &m_sockaddr);
	delete p;
	p = NULL;
}

void NSWLPeer::Build_WorkItem_LE_99(work_item_t* p)
{
	if (NULL == p)
	{
		return;
	}
	work_mode_enum workMode = m_netParam.work_mode;
	send_data_t* pSendData = &p->data.send_data;
	p->type = Send;
	pSendData->send_to = &m_sockaddr;
	pSendData->timeout_try = TIMEOUT_TRY_COUNT;
	pSendData->pFrom = this;
	if (LCP == workMode)
	{
		T_LE_PROTOCOL_99_LCP* pNetworkData = NULL;
		pNetworkData = &pSendData->protocol.le_lcp.PROTOCOL_99_LCP;
		pNetworkData->currentLinkProtocolVersion = LCP_CURRENTLPVERSION;
		pNetworkData->oldestLinkProtocolVersion = LCP_OLDESTPVERSION;
		pNetworkData->Opcode = LE_PEER_KEEP_ALIVE_RESPONSE;
		pNetworkData->peerID = m_netParam.local_peer_id;
		pNetworkData->peerMode = LCP_MODE;
		pNetworkData->peerServices = LCP_SERVICES;
		pSendData->net_lenth = Build_LE_PEER_KEEP_ALIVE_RESPONSE(pSendData->net_data, pNetworkData);
	}
	else
	{
		T_LE_PROTOCOL_99* pNetworkData = NULL;
		pNetworkData = &pSendData->protocol.le.PROTOCOL_99;
		pNetworkData->Opcode = LE_PEER_KEEP_ALIVE_RESPONSE;
		pNetworkData->peerID = m_netParam.local_peer_id;
		if (CPC == workMode)
		{
			pNetworkData->peerMode = CPC_MODE;
			pNetworkData->peerServices = CPC_SERVICES;
		}
		else
		{
			pNetworkData->peerMode = IPSC_MODE;
			pNetworkData->peerServices = IPSC_SERVICES;
		}
		pSendData->net_lenth = Build_LE_PEER_KEEP_ALIVE_RESPONSE(pSendData->net_data, pNetworkData);
	}
}

DWORD NSWLPeer::Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99* pData)
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

DWORD NSWLPeer::Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99_LCP* pData)
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

void NSWLPeer::Handle_LE_PEER_KEEP_ALIVE_RESPONSE_Recive()
{
	/*定时发送*/
	work_item_t* p = new work_item_t;
	memset(p, 0, sizeof(work_item_t));
	Build_WorkItem_LE_98(p, g_timing_alive_time_peer);
	m_pNet->AddWorkTimeOutItem(p);
}

wl_reg_status NSWLPeer::WlRegStatus()
{
	return m_wlRegStatus;
}

void NSWLPeer::setWlRegStatus(wl_reg_status value)
{
	if (m_wlRegStatus != value)
	{
		char temp[128] = { 0 };
		sprintf_s(temp, "=====PeerId %lu WlRegStatus from %d to %d=====", PeerId(), m_wlRegStatus, value);
		m_pLog->AddLog(temp);
		m_wlRegStatus = value;
		if (WL_REG_SUCCESS == m_wlRegStatus)
		{
			if (g_manager)
			{
				g_manager->setWireLanStatus(WL_REGISTER_SUC);
			}
		}
		else
		{
			if (g_manager)
			{
				g_manager->setWireLanStatus(WL_REGISTER_FAL);
			}
		}
	}
}

slot_number_enum NSWLPeer::SlotNumber()
{
	return m_slotNumber;
}

void NSWLPeer::setSlotNumber(slot_number_enum value)
{
	m_slotNumber = value;
}

work_item_t* NSWLPeer::Build_WL_VC_CHNL_CTRL_REQUEST(make_call_param_t* p)
{
	work_item_t* item = new work_item_t;
	memset(item, 0, sizeof(work_item_t));
	Build_WorkItem_WL_13(item, p);
	return item;
}

DWORD NSWLPeer::Build_WL_VC_CHNL_CTRL_REQUEST(CHAR* pPacket, T_WL_PROTOCOL_13* pData)
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

void NSWLPeer::Build_WorkItem_WL_13(work_item_t* p, make_call_param_t* param)
{
	if (NULL == p)
	{
		return;
	}
	send_data_t* send = &p->data.send_data;
	send->pFrom = this;
	send->send_to = &m_sockaddr;
	T_WL_PROTOCOL_13* networkData = &send->protocol.wl.PROTOCOL_13;
	networkData->accessCriteria = Access_Criteria_Polite_Access;
	networkData->callAttributes = CALL_ATTRIBUTES;//clear call
	networkData->callID = param->callID;
	networkData->callType = param->callType;
	networkData->CSBKArguments = CSBK_ARGUMENTS;
	networkData->currentLinkProtocolVersion = Wireline_Protocol_Version;
	networkData->oldestLinkProtocolVersion = Wireline_Protocol_Version;
	networkData->Opcode = WL_PROTOCOL;
	networkData->peerID = m_netParam.local_peer_id;
	networkData->preambleDuration = PREAMBLE_DURATION;
	networkData->sourceID = m_netParam.local_radio_id;
	networkData->targetID = param->targetID;
	networkData->wirelineOpcode = WL_VC_CHNL_CTRL_REQUEST;
	networkData->slotNumber = param->slotNumber;
	send->net_lenth = Build_WL_VC_CHNL_CTRL_REQUEST(send->net_data, networkData);
}

NSRecordFile* NSWLPeer::RecordFile()
{
	return m_recordFile;
}

void NSWLPeer::setRecordFile(NSRecordFile* value)
{
	m_recordFile = value;
}
