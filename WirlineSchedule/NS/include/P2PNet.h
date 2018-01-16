#pragma once
#include "NSNetBase.h"
#include "linklist.h"
#include "mutex.h"
#include "NSLog.h"
#include "P2PPeer.h"
#include "NSSerial.h"
#include "NSManager.h"
#define   MILLISECONDS  1000

class NSRecordFile;
class NSManager;
class CP2PNet :
	public NSNetBase
{
public:
	CP2PNet(NSManager* pManager);
	~CP2PNet();
	virtual int StartNet(StartNetParam* p);
	virtual void GetSerial(char* &pSerial, int &length);
	virtual void HandleAmbeData(void* pData, unsigned long length);
	/*������������*/
	virtual int sendNetDataBase(const char* pData, int len, void* send_to);
	void AddWorkItem(work_item_t* p);
private:
	StartNetParam m_netParam;
	bool m_isWork;  // work thread 
	bool m_isTimeoutWork; //timeout thread
	bool m_isAmbeDataWork;  // ambe thread
	bool m_isGetSerialWork;  //serial thread
	bool m_isCheckRecords;   //check records thread
	pXQTTNet m_pXQTTNet;
	HANDLE m_workThread;    //work thread handle
	HANDLE m_timeoutThread; //timeout thread handle
	HANDLE m_ambeDataThread; //ambe thread handle
	HANDLE m_getSerailThread; //serial thread handle
	HANDLE m_recordsThread;  //records thread handle
	LOCKERTYPE m_p2pDataMutex; //work item locker
	LOCKERTYPE m_p2pTimeoutDataMutex; //timeout item locker
	LOCKERTYPE m_peersMutex; //peers item locker
	LOCKERTYPE m_ambeMutex;
	LOCKERTYPE m_recordMutex;
	LOCKERTYPE m_sendMutex;
	pLinkItem m_p2pDataLink;
	pLinkItem m_p2pTimeoutDataLink;
	pLinkItem m_peersLink;
	pLinkItem m_ambeLink;
	pLinkItem m_recordLink;
	CP2PPeer* m_sitePeer;
	NSLog* m_log;
	SOCKADDR_IN m_masterSocket;
	le_status_enum m_le_status_enum;
	local_ip_list_t m_localIpList;
	unsigned char m_serialNumber[SIZE_SERIAL];
	NSSerial* m_pSerial;
	NSManager *m_manager;
	/*��ʼ�������������̲߳�����*/
	void Init();
	/*ֹͣ�̣߳��ͷ���Դ*/
	void UnInit();
	/*���work����*/
	void ClearWorkDataLink();
	/*���timeout����*/
	void ClearTimeoutDataLink();
	/*��� peer ����*/
	void ClearPeersLink();
	/*���ambe����*/
	void ClearAmbeLink();
	/*��� record ����*/
	void ClearRecordLink();
	/*ȡ����ǰ��work item*/
	work_item_t *PopWorkItem();
	/*ȡ����ǰ��timeout item*/
	work_item_t *PopTimeoutItem();
	/*ȡ����ǰ��ambe item*/
	work_item_t *PopAmbeItem();
	/*��ȡ��ǰle��״̬*/
	le_status_enum GetLeStatus();
	/*���õ�ǰle��״̬*/
	void SetLeStatus(le_status_enum value);
	/*work �߳�*/
	static unsigned int __stdcall WorkThreadProc(void* pParam);
	/*timeout �߳�*/
	static unsigned int __stdcall TimeoutThreadProc(void* pParam);
	/*ambe �߳�*/
	static unsigned int __stdcall AmbeDataThreadProc(void* pParam);
	/*check records �߳�*/
	static unsigned int __stdcall CheckRecordsThreadProc(void* pParam);
	/*work �߳�ִ�к���*/
	void WorkThread();
	/*timeout �߳�ִ�к���*/
	void TimeoutThread();
	/*ambe �߳�ִ�к���*/
	void AmbeDataThread();
	/*check records �߳�ִ�к���*/
	/* m_pXQTTNet ���ջص�*/
	void CheckRecordsThread();
	static void OnRecv(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len);
	/*m_pXQTTNet �Ͽ����ӻص�*/
	static void OnDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*m_pXQTTNet ����ص�*/
	static void OnError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*m_pXQTTNet ������ɻص�*/
	static void OnSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient);
	/* m_pXQTTNet ���ջص�ִ�к���*/
	void onRecv(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len);
	/*m_pXQTTNet �Ͽ����ӻص�ִ�к���*/
	void onDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*m_pXQTTNet ����ص�ִ�к���*/
	void onError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*m_pXQTTNet ������ɻص�ִ�к���*/
	void onSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient);
	/*�������кŻص�����*/
	static void SetSerialNumberCallback(unsigned char* pData, void* pArguments);
	/*���timeout item*/
	void AddTimeoutItem(work_item_t* p);
	/*��� ambe item*/
	void AddAmbeItem(work_item_t* p);
	/*���record item*/
	void AddRecordItem(NSRecordFile* p);
	/* ����timeout item �� �Ѿ��յ�replyָ���item ���Ƴ�*/
	void FindTimeOutItemAndDelete(unsigned long peerId, const char Opcode);
	/*������������time out item�е�item*/
	static bool FuncFindItem(const void* pValue, const void* pCondition);
	/*������������peers item�е�item ���Ƴ�*/
	CP2PPeer* FindPeersItemAndRemove(find_peer_condition_t* pCondition);
	/*������������peer*/
	static bool FuncFindPeer(const void* pValue, const void* pCondition);
	/*������������peer*/
	CP2PPeer* FindPeersItem(find_peer_condition_t* pCondition);
	/*������������/���records item�е�item*/
	NSRecordFile* FindOrAddRecordsItem(find_record_condition_t* condition, bool &bFind);
	/*������������records item�е�item*/
	NSRecordFile* FindRecordsItem(find_record_condition_t* condition);
	/*������������record*/
	static bool FuncFindRecord(const void* pValue, const void* pCondition);
	/*�������յ���0x91*/
	void Handle_Le_Status_Starting(work_item_t* currentItem, item_oprate_enum &OpreateFlag);
	/*����0x91�ĳ�ʱ����*/
	void Handle_Le_Status_Starting_Timeout(work_item_t* currentItem, item_oprate_enum &OpreateFlag, work_item_type_enum value);
	/*�������յ���0x93*/
	void Handle_Le_Status_Wait_Map(work_item_t* currentItem, item_oprate_enum &OpreateFlag);
	/*����0x93��ʱ����*/
	void Handle_Le_Status_Wait_Map_Timeout(work_item_t* currentItem, item_oprate_enum &OpreateFlag, work_item_type_enum value);
	/*������������peerע�������*/
	void Handle_Le_Status_Alive(const char Opcode, work_item_t* currentItem, item_oprate_enum &OpreateFlag);
	/*������������peerע�����ʱ����*/
	void Handle_Le_Status_Alive_Timeout(const char Opcode, work_item_t* currentItem, item_oprate_enum &OpreateFlag, work_item_type_enum value);
	/*����ǰ����*/
	bool HandleRetryAndTimingSend(work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*�������յ���p2p ����*/
	void Handle_P2P_Call(char *dst, const char* pAmbePacket);
	/*�������ݵ��м�̨*/
	void SendDataToMaster(work_item_t *w ,unsigned long timeout = TIMEOUT_LE);
	/*����xnl���ݵ��м�̨*/
	void SendXnlToMaster(work_item_t *w, unsigned long timeout = TIMEOUT_LE);
	/*�������յ���map*/
	void ParseMapBroadcast(T_LE_PROTOCOL_93* p);
	/*xnl ������Կ����*/
	void EncryptAuthenticationValue(unsigned char* un_Auth, unsigned char* en_Auth);
	/*����0x90*/
	void SEND_LE_MASTER_PEER_REGISTRATION_REQUEST(work_item_t* w);
	/*����0x91*/
	void SEND_LE_NOTIFICATION_MAP_REQUEST(work_item_t* w);
	/*����0x96*/
	void SEND_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(work_item_t* w ,unsigned long timing = 0);
	/*�������յ���0x91*/
	void RECV_LE_MASTER_PEER_REGISTRATION_RESPONSE(work_item_t * w, const char* pData);
	/*�������յ���0x93*/
	void RECV_LE_NOTIFICATION_MAP_BROADCAST(work_item_t * w, const char* pData);
	/*�������յ���0x94*/
	void RECV_LE_PEER_REGISTRATION_REQUEST(work_item_t * w, const char* pData);
	/*�������յ���0x95*/
	void RECV_LE_PEER_REGISTRATION_RESPONSE(work_item_t * w, const char* pData);
	/*�������յ���0x97*/
	void RECV_LE_MASTER_KEEP_ALIVE_RESPONSE(work_item_t * w, const char* pData);
	/*�������յ���0x98*/
	void RECV_LE_PEER_KEEP_ALIVE_REQUEST(work_item_t * w, const char* pData);
	/*�������յ���0x99*/
	void RECV_LE_PEER_KEEP_ALIVE_RESPONSE(work_item_t * w, const char* pData);
	/*�������յ���p2p ��voice call*/
	void RECV_VOICE_CALL(work_item_t *w, const char * pData, unsigned char callType);
	/*���0x90����buffer*/
	DWORD Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90* pData);
	/*���0x92����buffer*/
	DWORD Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92* pData);
	/*���0x96����buffer*/
	DWORD Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96* pData);
	/*�����յ���0x91��ֵ���ṹ��*/
	void Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91& networkData);
	/*�����յ���0x93��ֵ���ṹ��*/
	void Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93& networkData);
	/*�����յ���0x98��ֵ���ṹ��*/
	void Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98& networkData);
	void Unpack_XCMP_XNL_MASTER_STATUS_BRDCST(char* pData, moto_protocol_xcmp_xnl_t* networkData);
	/*�����յ���0x94��ֵ���ṹ��*/
	void Unpack_LE_PEER_REGISTRATION_REQUEST(char* pData, T_LE_PROTOCOL_94* networkData);
	/*�����յ���0x95��ֵ���ṹ��*/
	void Unpack_LE_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_95* networkData);
	/*�����յ���0x97��ֵ���ṹ��*/
	void Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_97* networkData);
	/*�����յ���0x98��ֵ���ṹ��*/
	void Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98* networkData);
	/*�����յ���0x99��ֵ���ṹ��*/
	void Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99* networkData);
	/*�����յ���p2p voice ��ֵ���ṹ��*/
	void Unpack_VOICE_CALL(char* pData, T_P2P_CALL* networkData, unsigned char callType);
	void SetSerialNumber(unsigned char* pSerial);
	/*�ж��Ƿ��Ǳ���ip*/
	bool FindLocalIP(unsigned long addr);
	void GetIpList(void * p);
	void DeleteWorkItem(work_item_t *w);
	bool WriteVoiceFrame(moto_protocol_p2p_t& call, DWORD dwCallType, BOOL isCheckTimeout);
	moto_protocol_p2p_t FillThisCall(P2P_CALL_HEADER call_header);
	void RemoveRecordsItem(NSRecordFile* p);
};

