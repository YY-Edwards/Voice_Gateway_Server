#ifndef WLPEER_H
#define WLPEER_H

#include "NSStruct.h"

class NSWLNet;
class NSLog;
class NSRecordFile;

class NSWLPeer
{
public:
	NSWLPeer(wl_peer_build_param_t* p);
	~NSWLPeer();

	/*��ȡ��ǰPeer��PeerID*/
	unsigned long PeerId();
	/*wlע��*/
	void WL_REGISTRATION();
	/*leע��*/
	void LE_PEER_REGISTRATION();
	/*�յ�leע������Ĵ���*/
	void Handle_LE_PEER_REGISTRATION_REQUEST_Recive();
	/*�յ�leע���Ӧ�Ĵ���*/
	void Handle_LE_PEER_REGISTRATION_RESPONSE_Recive();
	/*�յ����м���������Ӧ�Ĵ���*/
	void Handle_LE_PEER_KEEP_ALIVE_RESPONSE_Recive();
	/*�յ����м�����������Ĵ���*/
	void Handle_LE_PEER_KEEP_ALIVE_REQUEST_Recive();
	/*�����Ƿ�Ϊ������*/
	void setRemote3rdParty(bool value);
	/*wlע��״̬*/
	wl_reg_status WlRegStatus();
	/*����wlע��״̬*/
	void setWlRegStatus(wl_reg_status value);
	void setSlotNumber(slot_number_enum value);
	work_item_t* Build_WL_VC_CHNL_CTRL_REQUEST(make_call_param_t* p);
	slot_number_enum SlotNumber();
	SOCKADDR_IN m_sockaddr;//�����SOCKADDR
	NSRecordFile* RecordFile();
	void setRecordFile(NSRecordFile* value);
private:
	NSWLNet* m_pNet;//�ϼ���
	unsigned long m_peerId;//�����peerid,Ψһ��ʶ��
	StartNetParam m_netParam;//��ǰ���绷����������
	bool m_bRemote3rdParty;//�Ƿ��������ʶ
	NSLog* m_pLog;//��־ָ��
	wl_reg_status m_wlRegStatus;//wlע��״̬
	slot_number_enum m_slotNumber;//��ǰ����ʹ���ŵ�
	NSRecordFile* m_recordFile;
	
	/*����0x01 wl������*/
	void Build_WorkItem_WL_01(work_item_t* &p1,work_item_t* &p2);
	/*����0x94������*/
	void Build_WorkItem_LE_94(work_item_t* p);
	/*����0x95������*/
	void Build_WorkItem_LE_95(work_item_t* p);
	/*����0x98������*/
	void Build_WorkItem_LE_98(work_item_t* p, unsigned long timing = 0);
	/*����0x99������*/
	void Build_WorkItem_LE_99(work_item_t* p);
	/*����wl 0x13������*/
	void Build_WorkItem_WL_13(work_item_t* p,make_call_param_t* param);
	/*0x01 wl���*/
	DWORD Build_WL_REGISTRATION_REQUEST(CHAR* pPacket, T_WL_PROTOCOL_01* pData);
	/*0x94���*/
	DWORD Build_LE_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_94* pData);
	/*0x95���*/
	DWORD Build_LE_PEER_REGISTRATION_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_95* pData);
	/*0x98���*/
	DWORD Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98* pData);
	/*0x98 lcp���*/
	DWORD Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98_LCP* pData);
	/*0x99���*/
	DWORD Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99* pData);
	/*0x99 lcp���*/
	DWORD Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99_LCP* pData);
	/*�鿴��ǰ�Ƿ�Ϊ������*/
	bool Remote3rdParty();
	/*����ָ���㷨���ɶ�Ӧ����*/
	void getWirelineAuthentication(char* pPacket, DWORD &size);
	DWORD Build_WL_VC_CHNL_CTRL_REQUEST(CHAR* pPacket, T_WL_PROTOCOL_13* pData);
};

#endif