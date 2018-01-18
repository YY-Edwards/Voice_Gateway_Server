#ifndef WLNSNET_H
#define WLNSNET_H

#include "NSNetBase.h"
#include "linklist.h"
#include "mutex.h"
#include "ringbuffer.h"

#define RING_SIZE_AMBE_SEND 64

class NSLog;
class NSWLPeer;
class NSSerial;
class NSRecordFile;
class NSManager;

class NSWLNet :public NSNetBase
{
public:
	NSWLNet();
	~NSWLNet();
	
	/*����netģ��*/
	virtual int StartNet(StartNetParam* p);
	/*��ȡ���к�*/
	virtual void GetSerial(char* &pSerial, int &length);
	/*������������*/
	virtual int sendNetDataBase(const char* pData, int len, void* send_to);
	virtual void HandleAmbeData(void* pData, unsigned long length);
	/*��ȡ��ǰLE״̬*/
	virtual le_status_enum LeStatus();
	virtual void GetStartNetParam(StartNetParam* p);
	/*���ӹ���������*/
	void AddWorkItem(work_item_t* p);
	/*����SitePeer*/
	void SetSitePeer(NSWLPeer* pVlaue);
	/*udp��������*/
	void SendQuestToMasterAndAddWorkTimeOutsItem(work_item_t* p, unsigned long timeOut = TIMEOUT_LE);
	/*���ӳ�ʱ��ʱ����������*/
	void AddWorkTimeOutItem(work_item_t* p);
	void CallStart(make_call_param_t *p);
	void CallStop();
	call_thread_status_enum CallThreadStatus();
	//CALL_OPERATE_PARAM CurCallCmd;
	void CallStopUnnormal();
	/*��peer����ʱ,ͬʱɾ��Timeout����ص�item*/
	void TimeoutsItemDeleteAboutPeer(NSWLPeer* peer);
private:
	StartNetParam m_netParam;//���ò���
	pXQTTNet m_pMasterXqttnet;//���м̳���ָ��
	NSLog* m_pLog;//log��
	struct sockaddr_in m_sockaddrMaster;//���м�ip addr
	HANDLE m_pWorkThread;//�����߳̾��
	HANDLE m_pTimeOutThread;//��ʱ�߳̾��
	HANDLE m_pAmbeDataThread;//���յ���Ambe���ݴ����߳̾��
	HANDLE m_pCheckRecordsThread;
	HANDLE m_pCallThread;//ͨ���߳̾��
	//HANDLE m_getSerailThread;//��ȡ���к��߳̾��
	bool m_bThreadWork;//�̹߳�����ʶ
	le_status_enum m_leStatus;//LE״̬
	xnl_status_enum m_xnl_status_enum;//xnl״̬
	pLinkItem m_workItems;//������
	LOCKERTYPE m_mutexWorkItems;//��������
	pLinkItem m_workTimeOutItems;//��ʱ������
	LOCKERTYPE m_mutexWorkTimeOutItems;//��ʱ��������
	pLinkItem m_peers;//peer��
	LOCKERTYPE m_mutexPeers;//peer����
	pLinkItem m_callBackPeers;//
	LOCKERTYPE m_mutexCallBackPeers;
	pLinkItem m_ambes;//ambe���ݼ�
	LOCKERTYPE m_mutexAmbes;
	pLinkItem m_records;//������¼��
	LOCKERTYPE m_mutexRecords;//������¼����
	NSWLPeer* m_sitePeer;//���ڷ���ͨ��
	local_ip_list_t m_localIpList;//����IP��ַ��
	unsigned char m_serialNumber[SIZE_SERIAL];//���к�
	NSSerial* m_pSerial;//���к���
	//NSManager* m_pManager;
	decline_reason_code_info_t m_callFailInfo;
	NSWLPeer* m_currentSendVoicePeer;
	unsigned long m_callId;
	call_thread_status_enum m_callThreadStatus;
	make_call_param_t m_makeCallParam;
	char m_burstType;//��ǰ��burstType
	WORD m_SequenceNumber;
	DWORD m_Timestamp;
	pRingBuffer m_ringAmbeSend;
	LOCKERTYPE m_mutexAmbeSend;
	LOCKERTYPE m_mutexSend;
	T_WL_PROTOCOL_21 m_vcBurst;//����voice burst�齨
	T_WL_PROTOCOL_19 m_vcEnd;//����voice end�齨
	send_ambe_voice_encoded_frames_t m_startAmbe;
	int m_TxSubCount;
	send_ambe_voice_encoded_frames_t m_burstAmbe;
	send_net_data_ambe_t m_sendBuffer;
	HANDLE m_callThreadTimer;
	unsigned int m_timerId;
	NSRecordFile *m_localRecordFile;
	
	void Build_T_WL_PROTOCOL_19(T_WL_PROTOCOL_19& networkData);
	short Build_WL_VC_VOICE_END_BURST(CHAR* pPacket, T_WL_PROTOCOL_19* pData);
	static void PASCAL SendAmbeDataProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
	void SendAmbeData();
	void ContinueCallThread();
	void getWirelineAuthentication(char* pPacket, short &size);
	short Build_WL_VC_VOICE_BURST(CHAR* pPacket, T_WL_PROTOCOL_21* pData);
	void initVoiceBurst();
	void Build_T_WL_PROTOCOL_21(T_WL_PROTOCOL_21& networkData, bool start);
	void clearRingAmbeSend();
	int sizeRingAmbeSend();
	void AddRingAmbeItem(send_ambe_voice_encoded_frames_t* item);
	void PopRingAmbeItem(send_ambe_voice_encoded_frames_t* item);
	void ResetRingAmbe();
	NSWLPeer* SitePeer();
	void sendWorkItemNetData(work_item_t* p);
	/*��ȡ�ؽ�peer*/
	NSWLPeer* FindCallBacksItemAndRemove(make_call_param_t* p);
	/*���ӻؽ�Peer*/
	void AddCallBacksItem(NSWLPeer* peer);
	/*����ؽ�peer*/
	void clearCallBacks();
	/*����ͨ��*/
	void Make_Call(make_call_param_t* p);
	void ReadyMakeCall();
	/*ͨ���߳�*/
	static unsigned int __stdcall CallThreadProc(void* pArguments);
	/*ͨ���߳�ִ�к���*/
	void CallThread();
	unsigned long CallId();
	void setCallId(unsigned long value);
	void setCallThreadStatus(call_thread_status_enum value);
	NSWLPeer* CurrentSendVoicePeer();
	void setCurrentSendVoicePeer(NSWLPeer* value);
	/*��ȡ����ͨ����������Ϣ*/
	void getCallRequestRltInfo(decline_reason_code_info_t &declineReasonCodeInfo);
	/*�Ƴ�������¼*/
	void RemoveRecordsItem(NSRecordFile* p);
	/*Ѱ��record�ķ���*/
	static bool FuncFindRecord(const void* pValue, const void* pCondition);
	/*Ѱ�ҷ���������record������*/
	NSRecordFile* FindRecordsItem(find_record_condition_t* condition);
	/*Ѱ��record��������������*/
	NSRecordFile* FindOrAddRecordsItem(find_record_condition_t* condition,bool &bFind,NSWLPeer* peer);
	/*���������ݰ���ȡambe����*/
	void Handle_WL_PROTOCOL_21_Ambe_Data(char *dst, const char* pAmbePacket);
	/*����������¼������*/
	void AddRecordsItem(NSRecordFile* p);
	/*���������¼��*/
	void clearRecords();
	/*net���ݽ��ջص�*/
	static void OnRecive(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len);
	/*net���ݽ��ջص�ִ�к���*/
	void onRecive(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len);
	/*net�Ͽ��ص�*/
	static void OnDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*net�Ͽ��ص�ִ�к���*/
	void onDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*net������ɻص�,��δʹ��*/
	static void OnSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient);
	/*net������ɻص�ִ�к���,��δʹ��*/
	void onSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient);
	/*net����ص�*/
	static void OnError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*net����ص�ִ�к���*/
	void onError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*0x90��� CPC,IPSC*/
	DWORD Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90* pData);
	/*0x90��� LCP*/
	DWORD Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90_LCP* pData);
	/*0x93��� CPC,IPSC*/
	DWORD Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92* pData);
	/*0x93��� LCP*/
	DWORD Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92_LCP* pData);
	/*0x96��� CPC,IPSC*/
	DWORD Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96* pData);
	/*0x96��� LCP*/
	DWORD Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96_LCP* pData);
	/*0x90����������*/
	void Build_WorkItem_LE_90(work_item_t* p);
	/*0x92����������*/
	void Build_WorkItem_LE_92(work_item_t* p);
	/*0x96����������*/
	void Build_WorkItem_LE_96(work_item_t* p, unsigned long timing = 0);
	/*���������߳�*/
	int allThreadStart();
	/*ֹͣ�����߳�*/
	void allThreadStop();
	/*�����߳�*/
	static unsigned int __stdcall WorkThreadProc(void* pArguments);
	/*�����߳�ִ�к���*/
	void WorkThread();
	/*��ʱ�߳�*/
	static unsigned int __stdcall TimeOutThreadProc(void* pArguments);
	/*��ʱ�߳�ִ�к���*/
	void TimeOutThread();
	/*recive ambe���ݴ����߳�*/
	static unsigned int __stdcall AmbeDataThreadProc(void* pArguments);
	/*recive ambe���ݴ����߳�ִ�к���*/
	void AmbeDataThread();
	/*��鵱ǰ������¼�Ƿ�ʱ*/
	static unsigned int __stdcall CheckRecordsThreadProc(void* pArguments);
	/*��鵱ǰ������¼�Ƿ�ʱִ�к���*/
	void CheckRecordsThread();
	/*������м�net�ṹ��*/
	void clearMasterXqttnet();
	/*��չ�����*/
	void clearWorkItems();
	/*��ճ�ʱ������*/
	void clearWorkTimeOutItems();
	/*���peer*/
	void clearPeers();
	/*��ȡ��ǰXnl״̬*/
	xnl_status_enum GetXnlStatus();
	/*����LE״̬*/
	void SetLeStatus(le_status_enum value);
	///*����Xnl״̬*/
	void SetXnlStatus(xnl_status_enum value);
	/*��������������*/
	work_item_t* PopWorkItem();
	/*������ʱ����������*/
	work_item_t* PopWorkTimeOutItem();
	/*����Peer������*/
	void AddPeersItem(NSWLPeer* p);
	/*Ѱ�ҷ���������Peer������*/
	NSWLPeer* FindPeersItem(find_peer_condition_t* pCondition);
	/*Ѱ�ҷ���������Peer������Ƴ�*/
	NSWLPeer* FindPeersItemAndRemove(find_peer_condition_t* pCondition);
	/*map������*/
	void Handle_MapBroadcast(T_LE_PROTOCOL_93* p, T_LE_PROTOCOL_93_LCP* pLcp);
	/*��ȡ��ǰ����IP�б�*/
	void GetIpList(void * p);
	/*�ж��Ƿ�Ϊ����*/
	bool FindLocalIP(unsigned long addr);
	/*Ѱ��peer�ķ���*/
	static bool FuncFindPeer(const void* pValue, const void* pCondition);
	/*Ѱ��item�ķ���*/
	static bool FuncFindItem(const void* pValue, const void* pCondition);
	/*Ѱ��peer��������������*/
	NSWLPeer* FindOrAddPeersItem(wl_peer_build_param_t* param);
	/*���0x91*/
	void Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91& networkData);
	/*���0x91 lcp*/
	void Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91_LCP& networkData);
	/*���0x93*/
	void Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93& networkData);
	/*���0x93 lcp*/
	void Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93_LCP& networkData);
	/*���0x94*/
	void Unpack_LE_PEER_REGISTRATION_REQUEST(char* pData, T_LE_PROTOCOL_94& networkData);
	/*���0x95*/
	void Unpack_LE_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_95& networkData);
	/*���0x98*/
	void Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98& networkData);
	/*���0x98 lcp*/
	void Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98_LCP& networkData);
	/*���0x99*/
	void Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99& networkData);
	/*���0x99 lcp*/
	void Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99_LCP& networkData);
	/*���0x03 wl*/
	void Unpack_WL_REGISTRATION_GENERAL_OPS(char* pData, T_WL_PROTOCOL_03& networkData);
	/*���0x02 wl*/
	void Unpack_WL_REGISTRATION_STATUS(char* pData, T_WL_PROTOCOL_02& networkData);
	/*���0x11 wl*/
	void Unpack_WL_CHNL_STATUS(char* pData, T_WL_PROTOCOL_11& networkData);
	/*���0x12 wl*/
	void Unpack_WL_CHNL_STATUS_QUERY(char* pData, T_WL_PROTOCOL_12& networkData);
	/*���0x16 wl*/
	void Unpack_WL_VC_CHNL_CTRL_STATUS(char* pData, T_WL_PROTOCOL_16& networkData);
	/*���0x18 wl*/
	void Unpack_WL_VC_VOICE_START(char* pData, T_WL_PROTOCOL_18& networkData);
	/*���0x19 wl*/
	void Unpack_WL_VC_VOICE_END_BURST(char* pData, T_WL_PROTOCOL_19& networkData);
	/*���0x20 wl*/
	void Unpack_WL_VC_CALL_SESSION_STATUS(char* pData, T_WL_PROTOCOL_20& networkData);
	/*���0x21 wl*/
	void Unpack_WL_VC_VOICE_BURST(char* pData, T_WL_PROTOCOL_21& networkData);
	/*���0x97*/
	void Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_97& networkData);
	/*���0x97 lcp*/
	void Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_97_LCP& networkData);
	/*Starting״̬�´����յ������ݰ�*/
	void Handle_Le_Status_Starting_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag);
	/*WaitMap״̬�´����յ������ݰ�*/
	void Handle_Le_Status_WaitMap_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag);
	/*Alive״̬�´����յ������ݰ�*/
	void Handle_Le_Status_Alive_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag);
	/*Alive״̬�´�����wl�ݰ�*/
	void Handle_Wl_Status_Alive_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag);
	/*Starting״̬TimeOut�߳��´����͵����ݰ�*/
	void Handle_Le_Status_Starting_TimeOut_Send(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*Starting״̬TimeOut�߳��´����յ������ݰ�*/
	void Handle_Le_Status_Starting_TimeOut_Recive(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*WaitMap״̬TimeOut�߳��´����͵����ݰ�*/
	void Handle_Le_Status_WaitMap_TimeOut_Send(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*WaitMap״̬TimeOut�߳��´����յ������ݰ�*/
	void Handle_Le_Status_WaitMap_TimeOut_Recive(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*Alive״̬TimeOut�߳��´����͵�wl���ݰ�*/
	void Handle_Wl_Status_Alive_TimeOut_Send(const char wirelineOpcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*Alive״̬TimeOut�߳��´����͵����ݰ�*/
	void Handle_Le_Status_Alive_TimeOut_Send(const char Opcode, work_item_t* curItem, item_oprate_enum &bDOpreateFlagel);
	/*Alive״̬TimeOut�߳��´����յ������ݰ�*/
	void Handle_Le_Status_Alive_TimeOut_Recive(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*Alive״̬TimeOut�߳��´����յ���wl���ݰ�*/
	void Handle_Wl_Status_Alive_TimeOut_Recive(const char wlOpcode, moto_protocol_wl_t* protocol, item_oprate_enum &OpreateFlag);
	/*�ط��Ͷ�ʱ���ʹ���*/
	bool HandleRetryAndTimingSend(work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*ɾ��ָ����Timeout�����Ӽ�*/
	void findTimeOutItemAndDelete(unsigned long peerId, const char Opcode, const char wlOpcode, work_item_type_enum type);
	/*�Ƴ�������ָ����Timeout�����Ӽ�*/
	work_item_t* findTimeOutItemAndRemove(unsigned long peerId, const char Opcode, const char wlOpcode, work_item_type_enum type);
	/*ɾ��ָ���Ĺ����Ӽ�*/
	void findItemAndDelete(unsigned long peerId, const char Opcode, const char wlOpcode);
	/*���кŻ�ȡ�ص�*/
	static void SetSerialNumberCallback(unsigned char* pData, void* pArguments);
	/*���кŻ�ȡ�ص�ִ�з���*/
	void SetSerialNumber(unsigned char* pSerial);
	/*wl���ݰ����*/
	void Unpack_WL(const char* pData, int len, moto_protocol_wl_t* &p, item_oprate_enum &OpreateFlag);
	/*���ambe��*/
	void clearAmbes();
	/*����ambe������*/
	void AddAmbesItem(work_item_t* p);
	/*����ambe������*/
	work_item_t* PopAmbesItem();
	/*����ͨ��Session*/
	bool Handle_WL_PROTOCOL_20(find_record_condition_t* condition, unsigned char callSessionStatus);

};
#endif