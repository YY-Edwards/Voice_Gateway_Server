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
	
	/*启动net模块*/
	virtual int StartNet(StartNetParam* p);
	/*获取序列号*/
	virtual void GetSerial(char* &pSerial, int &length);
	/*发送网络数据*/
	virtual int sendNetDataBase(const char* pData, int len, void* send_to);
	virtual void HandleAmbeData(void* pData, unsigned long length);
	/*获取当前LE状态*/
	virtual le_status_enum LeStatus();
	virtual void GetStartNetParam(StartNetParam* p);
	/*增加工作集子项*/
	void AddWorkItem(work_item_t* p);
	/*设置SitePeer*/
	void SetSitePeer(NSWLPeer* pVlaue);
	/*udp发送数据*/
	void SendQuestToMasterAndAddWorkTimeOutsItem(work_item_t* p, unsigned long timeOut = TIMEOUT_LE);
	/*增加超时或定时工作集子项*/
	void AddWorkTimeOutItem(work_item_t* p);
	void CallStart(make_call_param_t *p);
	void CallStop();
	call_thread_status_enum CallThreadStatus();
	//CALL_OPERATE_PARAM CurCallCmd;
	void CallStopUnnormal();
	/*当peer更新时,同时删除Timeout中相关的item*/
	void TimeoutsItemDeleteAboutPeer(NSWLPeer* peer);
private:
	StartNetParam m_netParam;//配置参数
	pXQTTNet m_pMasterXqttnet;//主中继持有指针
	NSLog* m_pLog;//log类
	struct sockaddr_in m_sockaddrMaster;//主中继ip addr
	HANDLE m_pWorkThread;//工作线程句柄
	HANDLE m_pTimeOutThread;//超时线程句柄
	HANDLE m_pAmbeDataThread;//接收到的Ambe数据处理线程句柄
	HANDLE m_pCheckRecordsThread;
	HANDLE m_pCallThread;//通话线程句柄
	//HANDLE m_getSerailThread;//获取序列号线程句柄
	bool m_bThreadWork;//线程工作标识
	le_status_enum m_leStatus;//LE状态
	xnl_status_enum m_xnl_status_enum;//xnl状态
	pLinkItem m_workItems;//工作集
	LOCKERTYPE m_mutexWorkItems;//工作集锁
	pLinkItem m_workTimeOutItems;//超时工作集
	LOCKERTYPE m_mutexWorkTimeOutItems;//超时工作集锁
	pLinkItem m_peers;//peer集
	LOCKERTYPE m_mutexPeers;//peer集锁
	pLinkItem m_callBackPeers;//
	LOCKERTYPE m_mutexCallBackPeers;
	pLinkItem m_ambes;//ambe数据集
	LOCKERTYPE m_mutexAmbes;
	pLinkItem m_records;//语音记录集
	LOCKERTYPE m_mutexRecords;//语音记录集锁
	NSWLPeer* m_sitePeer;//用于发起通话
	local_ip_list_t m_localIpList;//本机IP地址池
	unsigned char m_serialNumber[SIZE_SERIAL];//序列号
	NSSerial* m_pSerial;//序列号类
	//NSManager* m_pManager;
	decline_reason_code_info_t m_callFailInfo;
	NSWLPeer* m_currentSendVoicePeer;
	unsigned long m_callId;
	call_thread_status_enum m_callThreadStatus;
	make_call_param_t m_makeCallParam;
	char m_burstType;//当前的burstType
	WORD m_SequenceNumber;
	DWORD m_Timestamp;
	pRingBuffer m_ringAmbeSend;
	LOCKERTYPE m_mutexAmbeSend;
	LOCKERTYPE m_mutexSend;
	T_WL_PROTOCOL_21 m_vcBurst;//用于voice burst组建
	T_WL_PROTOCOL_19 m_vcEnd;//用于voice end组建
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
	/*获取回叫peer*/
	NSWLPeer* FindCallBacksItemAndRemove(make_call_param_t* p);
	/*增加回叫Peer*/
	void AddCallBacksItem(NSWLPeer* peer);
	/*清除回叫peer*/
	void clearCallBacks();
	/*建立通话*/
	void Make_Call(make_call_param_t* p);
	void ReadyMakeCall();
	/*通话线程*/
	static unsigned int __stdcall CallThreadProc(void* pArguments);
	/*通话线程执行函数*/
	void CallThread();
	unsigned long CallId();
	void setCallId(unsigned long value);
	void setCallThreadStatus(call_thread_status_enum value);
	NSWLPeer* CurrentSendVoicePeer();
	void setCurrentSendVoicePeer(NSWLPeer* value);
	/*获取请求通话结果相关信息*/
	void getCallRequestRltInfo(decline_reason_code_info_t &declineReasonCodeInfo);
	/*移除语音记录*/
	void RemoveRecordsItem(NSRecordFile* p);
	/*寻找record的方法*/
	static bool FuncFindRecord(const void* pValue, const void* pCondition);
	/*寻找符合条件的record集子项*/
	NSRecordFile* FindRecordsItem(find_record_condition_t* condition);
	/*寻找record如果不存在则加入*/
	NSRecordFile* FindOrAddRecordsItem(find_record_condition_t* condition,bool &bFind,NSWLPeer* peer);
	/*从网络数据包获取ambe数据*/
	void Handle_WL_PROTOCOL_21_Ambe_Data(char *dst, const char* pAmbePacket);
	/*增加语音记录集子项*/
	void AddRecordsItem(NSRecordFile* p);
	/*清除语音记录集*/
	void clearRecords();
	/*net数据接收回调*/
	static void OnRecive(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len);
	/*net数据接收回调执行函数*/
	void onRecive(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, const char* pData, int len);
	/*net断开回调*/
	static void OnDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*net断开回调执行函数*/
	void onDisconn(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*net发送完成回调,暂未使用*/
	static void OnSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient);
	/*net发送完成回调执行函数,暂未使用*/
	void onSendComplete(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient);
	/*net错误回调*/
	static void OnError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*net错误回调执行函数*/
	void onError(struct _xqtt_net* pNet, struct _xqtt_net* pNetClient, int errCode);
	/*0x90打包 CPC,IPSC*/
	DWORD Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90* pData);
	/*0x90打包 LCP*/
	DWORD Build_LE_MASTER_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_90_LCP* pData);
	/*0x93打包 CPC,IPSC*/
	DWORD Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92* pData);
	/*0x93打包 LCP*/
	DWORD Build_LE_NOTIFICATION_MAP_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_92_LCP* pData);
	/*0x96打包 CPC,IPSC*/
	DWORD Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96* pData);
	/*0x96打包 LCP*/
	DWORD Build_LE_MASTER_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_96_LCP* pData);
	/*0x90工作集建立*/
	void Build_WorkItem_LE_90(work_item_t* p);
	/*0x92工作集建立*/
	void Build_WorkItem_LE_92(work_item_t* p);
	/*0x96工作集建立*/
	void Build_WorkItem_LE_96(work_item_t* p, unsigned long timing = 0);
	/*启动所用线程*/
	int allThreadStart();
	/*停止所有线程*/
	void allThreadStop();
	/*工作线程*/
	static unsigned int __stdcall WorkThreadProc(void* pArguments);
	/*工作线程执行函数*/
	void WorkThread();
	/*超时线程*/
	static unsigned int __stdcall TimeOutThreadProc(void* pArguments);
	/*超时线程执行函数*/
	void TimeOutThread();
	/*recive ambe数据处理线程*/
	static unsigned int __stdcall AmbeDataThreadProc(void* pArguments);
	/*recive ambe数据处理线程执行函数*/
	void AmbeDataThread();
	/*检查当前语音记录是否超时*/
	static unsigned int __stdcall CheckRecordsThreadProc(void* pArguments);
	/*检查当前语音记录是否超时执行函数*/
	void CheckRecordsThread();
	/*清空主中继net结构体*/
	void clearMasterXqttnet();
	/*清空工作集*/
	void clearWorkItems();
	/*清空超时工作集*/
	void clearWorkTimeOutItems();
	/*清空peer*/
	void clearPeers();
	/*获取当前Xnl状态*/
	xnl_status_enum GetXnlStatus();
	/*设置LE状态*/
	void SetLeStatus(le_status_enum value);
	///*设置Xnl状态*/
	void SetXnlStatus(xnl_status_enum value);
	/*弹出工作集子项*/
	work_item_t* PopWorkItem();
	/*弹出超时工作集子项*/
	work_item_t* PopWorkTimeOutItem();
	/*增加Peer集子项*/
	void AddPeersItem(NSWLPeer* p);
	/*寻找符合条件的Peer集子项*/
	NSWLPeer* FindPeersItem(find_peer_condition_t* pCondition);
	/*寻找符合条件的Peer集子项并移除*/
	NSWLPeer* FindPeersItemAndRemove(find_peer_condition_t* pCondition);
	/*map包处理*/
	void Handle_MapBroadcast(T_LE_PROTOCOL_93* p, T_LE_PROTOCOL_93_LCP* pLcp);
	/*获取当前电脑IP列表*/
	void GetIpList(void * p);
	/*判断是否为本机*/
	bool FindLocalIP(unsigned long addr);
	/*寻找peer的方法*/
	static bool FuncFindPeer(const void* pValue, const void* pCondition);
	/*寻找item的方法*/
	static bool FuncFindItem(const void* pValue, const void* pCondition);
	/*寻找peer如果不存在则加入*/
	NSWLPeer* FindOrAddPeersItem(wl_peer_build_param_t* param);
	/*解包0x91*/
	void Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91& networkData);
	/*解包0x91 lcp*/
	void Unpack_LE_MASTER_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_91_LCP& networkData);
	/*解包0x93*/
	void Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93& networkData);
	/*解包0x93 lcp*/
	void Unpack_LE_NOTIFICATION_MAP_BROADCAST(char* pData, T_LE_PROTOCOL_93_LCP& networkData);
	/*解包0x94*/
	void Unpack_LE_PEER_REGISTRATION_REQUEST(char* pData, T_LE_PROTOCOL_94& networkData);
	/*解包0x95*/
	void Unpack_LE_PEER_REGISTRATION_RESPONSE(char* pData, T_LE_PROTOCOL_95& networkData);
	/*解包0x98*/
	void Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98& networkData);
	/*解包0x98 lcp*/
	void Unpack_LE_PEER_KEEP_ALIVE_REQUEST(char* pData, T_LE_PROTOCOL_98_LCP& networkData);
	/*解包0x99*/
	void Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99& networkData);
	/*解包0x99 lcp*/
	void Unpack_LE_PEER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_99_LCP& networkData);
	/*解包0x03 wl*/
	void Unpack_WL_REGISTRATION_GENERAL_OPS(char* pData, T_WL_PROTOCOL_03& networkData);
	/*解包0x02 wl*/
	void Unpack_WL_REGISTRATION_STATUS(char* pData, T_WL_PROTOCOL_02& networkData);
	/*解包0x11 wl*/
	void Unpack_WL_CHNL_STATUS(char* pData, T_WL_PROTOCOL_11& networkData);
	/*解包0x12 wl*/
	void Unpack_WL_CHNL_STATUS_QUERY(char* pData, T_WL_PROTOCOL_12& networkData);
	/*解包0x16 wl*/
	void Unpack_WL_VC_CHNL_CTRL_STATUS(char* pData, T_WL_PROTOCOL_16& networkData);
	/*解包0x18 wl*/
	void Unpack_WL_VC_VOICE_START(char* pData, T_WL_PROTOCOL_18& networkData);
	/*解包0x19 wl*/
	void Unpack_WL_VC_VOICE_END_BURST(char* pData, T_WL_PROTOCOL_19& networkData);
	/*解包0x20 wl*/
	void Unpack_WL_VC_CALL_SESSION_STATUS(char* pData, T_WL_PROTOCOL_20& networkData);
	/*解包0x21 wl*/
	void Unpack_WL_VC_VOICE_BURST(char* pData, T_WL_PROTOCOL_21& networkData);
	/*解包0x97*/
	void Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_97& networkData);
	/*解包0x97 lcp*/
	void Unpack_LE_MASTER_KEEP_ALIVE_RESPONSE(char* pData, T_LE_PROTOCOL_97_LCP& networkData);
	/*Starting状态下处理收到的数据包*/
	void Handle_Le_Status_Starting_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag);
	/*WaitMap状态下处理收到的数据包*/
	void Handle_Le_Status_WaitMap_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag);
	/*Alive状态下处理收到的数据包*/
	void Handle_Le_Status_Alive_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag);
	/*Alive状态下处理数wl据包*/
	void Handle_Wl_Status_Alive_Recive(const char Opcode, work_item_t* &curItem, item_oprate_enum &OpreateFlag);
	/*Starting状态TimeOut线程下处理发送的数据包*/
	void Handle_Le_Status_Starting_TimeOut_Send(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*Starting状态TimeOut线程下处理收到的数据包*/
	void Handle_Le_Status_Starting_TimeOut_Recive(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*WaitMap状态TimeOut线程下处理发送的数据包*/
	void Handle_Le_Status_WaitMap_TimeOut_Send(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*WaitMap状态TimeOut线程下处理收到的数据包*/
	void Handle_Le_Status_WaitMap_TimeOut_Recive(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*Alive状态TimeOut线程下处理发送的wl数据包*/
	void Handle_Wl_Status_Alive_TimeOut_Send(const char wirelineOpcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*Alive状态TimeOut线程下处理发送的数据包*/
	void Handle_Le_Status_Alive_TimeOut_Send(const char Opcode, work_item_t* curItem, item_oprate_enum &bDOpreateFlagel);
	/*Alive状态TimeOut线程下处理收到的数据包*/
	void Handle_Le_Status_Alive_TimeOut_Recive(const char Opcode, work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*Alive状态TimeOut线程下处理收到的wl数据包*/
	void Handle_Wl_Status_Alive_TimeOut_Recive(const char wlOpcode, moto_protocol_wl_t* protocol, item_oprate_enum &OpreateFlag);
	/*重发和定时发送处理*/
	bool HandleRetryAndTimingSend(work_item_t* curItem, item_oprate_enum &OpreateFlag);
	/*删除指定的Timeout工作子集*/
	void findTimeOutItemAndDelete(unsigned long peerId, const char Opcode, const char wlOpcode, work_item_type_enum type);
	/*移除并返回指定的Timeout工作子集*/
	work_item_t* findTimeOutItemAndRemove(unsigned long peerId, const char Opcode, const char wlOpcode, work_item_type_enum type);
	/*删除指定的工作子集*/
	void findItemAndDelete(unsigned long peerId, const char Opcode, const char wlOpcode);
	/*序列号获取回调*/
	static void SetSerialNumberCallback(unsigned char* pData, void* pArguments);
	/*序列号获取回调执行方法*/
	void SetSerialNumber(unsigned char* pSerial);
	/*wl数据包解包*/
	void Unpack_WL(const char* pData, int len, moto_protocol_wl_t* &p, item_oprate_enum &OpreateFlag);
	/*清除ambe集*/
	void clearAmbes();
	/*增加ambe集子项*/
	void AddAmbesItem(work_item_t* p);
	/*弹出ambe集子项*/
	work_item_t* PopAmbesItem();
	/*处理通话Session*/
	bool Handle_WL_PROTOCOL_20(find_record_condition_t* condition, unsigned char callSessionStatus);

};
#endif