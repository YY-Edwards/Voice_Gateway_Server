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
	/*发送网络数据*/
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
	/*初始化，创建所有线程并启动*/
	void Init();
	/*停止线程，释放资源*/
	void UnInit();
	/*清空work链表*/
	void ClearWorkDataLink();
	/*清空timeout链表*/
	void ClearTimeoutDataLink();
	/*清空 peer 链表*/
	void ClearPeersLink();
	/*清空ambe链表*/
	void ClearAmbeLink();
	/*清空 record 链表*/
	void ClearRecordLink();
	/*取出当前的work item*/
	work_item_t *PopWorkItem();
	/*取出当前的timeout item*/
	work_item_t *PopTimeoutItem();
	/*取出当前的ambe item*/
	work_item_t *PopAmbeItem();
	/*获取当前le的状态*/
	le_status_enum GetLeStatus();
	/*设置当前le的状态*/
	void SetLeStatus(le_status_enum value);
	/*work 线程*/
	static unsigned int __stdcall WorkThreadProc(void* pParam);
	/*timeout 线程*/
	static unsigned int __stdcall TimeoutThreadProc(void* pParam);
	/*ambe 线程*/
	static unsigned int __stdcall AmbeDataThreadProc(void* pParam);
	/*check records 线程*/
	static unsigned int __stdcall CheckRecordsThreadProc(void* pParam);
	/*work 线程执行函数*/
	void WorkThread();
	/*timeout 线程执行函数*/
	void TimeoutThread();
	/*ambe 线程执行函数*/
	void AmbeDataThread();
	/*check records 线程执行函数*/
	/* m_pXQTTNet 接收回调*/
	void CheckRecordsThread();
	static void OnRecv(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len);
	/*m_pXQTTNet 断开连接回调*/
	static void OnDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*m_pXQTTNet 出错回调*/
	static void OnError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*m_pXQTTNet 发送完成回调*/
	static void OnSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient);
	/* m_pXQTTNet 接收回调执行函数*/
	void onRecv(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len);
	/*m_pXQTTNet 断开连接回调执行函数*/
	void onDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*m_pXQTTNet 出错回调执行函数*/
	void onError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*m_pXQTTNet 发送完成回调执行函数*/
	void onSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient);
	/*设置序列号回调函数*/
	static void SetSerialNumberCallback(unsigned char* pData, void* pArguments);
	/*添加timeout item*/
	void AddTimeoutItem(work_item_t* p);
	/*添加 ambe item*/
	void AddAmbeItem(work_item_t* p);
	/*添加record item*/
	void AddRecordItem(NSRecordFile* p);
	/* 查找timeout item 中 已经收到reply指令的item 并移除*/
	void FindTimeOutItemAndDelete(unsigned long peerId, const char Opcode);
	/*根据条件查找time out item中的item*/
	static bool FuncFindItem(const void* pValue, const void* pCondition);
	/*根据条件查找peers item中的item 并移除*/
	CP2PPeer* FindPeersItemAndRemove(find_peer_condition_t* pCondition);
	/*根据条件查找peer*/
	static bool FuncFindPeer(const void* pValue, const void* pCondition);
	/*根据条件查找peer*/
	CP2PPeer* FindPeersItem(find_peer_condition_t* pCondition);
	/*根据条件查找/添加records item中的item*/
	NSRecordFile* FindOrAddRecordsItem(find_record_condition_t* condition, bool &bFind);
	/*根据条件查找records item中的item*/
	NSRecordFile* FindRecordsItem(find_record_condition_t* condition);
	/*根据条件查找record*/
	static bool FuncFindRecord(const void* pValue, const void* pCondition);
	/*解析接收到的0x91*/
	void Handle_Le_Status_Starting(work_item_t* currentItem, item_oprate_enum &OpreateFlag);
	/*接收0x91的超时处理*/
	void Handle_Le_Status_Starting_Timeout(work_item_t* currentItem, item_oprate_enum &OpreateFlag, work_item_type_enum value);
	/*解析接收到的0x93*/
	void Handle_Le_Status_Wait_Map(work_item_t* currentItem, item_oprate_enum &OpreateFlag);
	/*接收0x93超时处理*/
	void Handle_Le_Status_Wait_Map_Timeout(work_item_t* currentItem, item_oprate_enum &OpreateFlag, work_item_type_enum value);
	/*接收心跳包及peer注册包处理*/
	void Handle_Le_Status_Alive(const char Opcode, work_item_t* currentItem, item_oprate_enum &OpreateFlag);
	/*接收心跳包及peer注册包超时处理*/
	void Handle_Le_Status_Alive_Timeout(const char Opcode, work_item_t* currentItem, item_oprate_enum &OpreateFlag, work_item_type_enum value);
	/*处理当前数据*/
	bool HandleRetryAndTimingSend(work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*解析接收到的p2p 语音*/
	void Handle_P2P_Call(char *dst, const char* pAmbePacket);
	/*发送数据到中继台*/
	void SendDataToMaster(work_item_t *w ,unsigned long timeout = TIMEOUT_LE);
	/*发送xnl数据到中继台*/
	void SendXnlToMaster(work_item_t *w, unsigned long timeout = TIMEOUT_LE);
	/*解析接收到的map*/
	void ParseMapBroadcast(T_LE_PROTOCOL_93* p);
	/*xnl 连接秘钥加密*/
	void EncryptAuthenticationValue(unsigned char* un_Auth, unsigned char* en_Auth);
	/*发送0x90*/
	void SEND_LE_MASTER_PEER_REGISTRATION_REQUEST(work_item_t* w);
	/*发送0x91*/
	void SEND_LE_NOTIFICATION_MAP_REQUEST(work_item_t* w);
	/*发送0x96*/
	void SEND_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(work_item_t* w ,unsigned long timing = 0);
	/*解析接收到的0x91*/
	void RECV_LE_MASTER_PEER_REGISTRATION_RESPONSE(work_item_t * w, const char* pData);
	/*解析接收到的0x93*/
	void RECV_LE_NOTIFICATION_MAP_BROADCAST(work_item_t * w, const char* pData);
	/*解析接收到的0x94*/
	void RECV_LE_PEER_REGISTRATION_REQUEST(work_item_t * w, const char* pData);
	/*解析接收到的0x95*/
	void RECV_LE_PEER_REGISTRATION_RESPONSE(work_item_t * w, const char* pData);
	/*解析接收到的0x97*/
	void RECV_LE_MASTER_KEEP_ALIVE_RESPONSE(work_item_t * w, const char* pData);
	/*解析接收到的0x98*/
	void RECV_LE_PEER_KEEP_ALIVE_REQUEST(work_item_t * w, const char* pData);
	/*解析接收到的0x99*/
	void RECV_LE_PEER_KEEP_ALIVE_RESPONSE(work_item_t * w, const char* pData);
	/*解析接收到的p2p 的voice call*/
	void RECV_VOICE_CALL(work_item_t *w, const char * pData, unsigned char callType);
	/*填充0x90发送buffer*/
	DWORD Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90* pData);
	/*填充0x92发送buffer*/
	DWORD Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92* pData);
	/*填充0x96发送buffer*/
	DWORD Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96* pData);
	/*将接收到的0x91赋值给结构体*/
	void Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91& networkData);
	/*将接收到的0x93赋值给结构体*/
	void Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93& networkData);
	/*将接收到的0x98赋值给结构体*/
	void Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98& networkData);
	void Unpack_XCMP_XNL_MASTER_STATUS_BRDCST(char* pData, moto_protocol_xcmp_xnl_t* networkData);
	/*将接收到的0x94赋值给结构体*/
	void Unpack_LE_PEER_REGISTRATION_REQUEST(char* pData, T_LE_PROTOCOL_94* networkData);
	/*将接收到的0x95赋值给结构体*/
	void Unpack_LE_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_95* networkData);
	/*将接收到的0x97赋值给结构体*/
	void Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_97* networkData);
	/*将接收到的0x98赋值给结构体*/
	void Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98* networkData);
	/*将接收到的0x99赋值给结构体*/
	void Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99* networkData);
	/*将接收到的p2p voice 赋值给结构体*/
	void Unpack_VOICE_CALL(char* pData, T_P2P_CALL* networkData, unsigned char callType);
	void SetSerialNumber(unsigned char* pSerial);
	/*判断是否是本地ip*/
	bool FindLocalIP(unsigned long addr);
	void GetIpList(void * p);
	void DeleteWorkItem(work_item_t *w);
	bool WriteVoiceFrame(moto_protocol_p2p_t& call, DWORD dwCallType, BOOL isCheckTimeout);
	moto_protocol_p2p_t FillThisCall(P2P_CALL_HEADER call_header);
	void RemoveRecordsItem(NSRecordFile* p);
};

