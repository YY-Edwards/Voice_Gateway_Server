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

	/*获取当前Peer的PeerID*/
	unsigned long PeerId();
	/*wl注册*/
	void WL_REGISTRATION();
	/*le注册*/
	void LE_PEER_REGISTRATION();
	/*收到le注册请求的处理*/
	void Handle_LE_PEER_REGISTRATION_REQUEST_Recive();
	/*收到le注册回应的处理*/
	void Handle_LE_PEER_REGISTRATION_RESPONSE_Recive();
	/*收到从中继心跳包回应的处理*/
	void Handle_LE_PEER_KEEP_ALIVE_RESPONSE_Recive();
	/*收到从中继心跳包请求的处理*/
	void Handle_LE_PEER_KEEP_ALIVE_REQUEST_Recive();
	/*设置是否为第三方*/
	void setRemote3rdParty(bool value);
	/*wl注册状态*/
	wl_reg_status WlRegStatus();
	/*设置wl注册状态*/
	void setWlRegStatus(wl_reg_status value);
	void setSlotNumber(slot_number_enum value);
	work_item_t* Build_WL_VC_CHNL_CTRL_REQUEST(make_call_param_t* p);
	slot_number_enum SlotNumber();
	SOCKADDR_IN m_sockaddr;//自身的SOCKADDR
	NSRecordFile* RecordFile();
	void setRecordFile(NSRecordFile* value);
private:
	NSWLNet* m_pNet;//上级类
	unsigned long m_peerId;//自身的peerid,唯一标识符
	StartNetParam m_netParam;//当前网络环境参数配置
	bool m_bRemote3rdParty;//是否第三方标识
	NSLog* m_pLog;//日志指针
	wl_reg_status m_wlRegStatus;//wl注册状态
	slot_number_enum m_slotNumber;//当前正在使用信道
	NSRecordFile* m_recordFile;
	
	/*构建0x01 wl工作项*/
	void Build_WorkItem_WL_01(work_item_t* &p1,work_item_t* &p2);
	/*构建0x94工作项*/
	void Build_WorkItem_LE_94(work_item_t* p);
	/*构建0x95工作项*/
	void Build_WorkItem_LE_95(work_item_t* p);
	/*构建0x98工作项*/
	void Build_WorkItem_LE_98(work_item_t* p, unsigned long timing = 0);
	/*构建0x99工作项*/
	void Build_WorkItem_LE_99(work_item_t* p);
	/*构建wl 0x13工作项*/
	void Build_WorkItem_WL_13(work_item_t* p,make_call_param_t* param);
	/*0x01 wl打包*/
	DWORD Build_WL_REGISTRATION_REQUEST(CHAR* pPacket, T_WL_PROTOCOL_01* pData);
	/*0x94打包*/
	DWORD Build_LE_PEER_REGISTRATION_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_94* pData);
	/*0x95打包*/
	DWORD Build_LE_PEER_REGISTRATION_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_95* pData);
	/*0x98打包*/
	DWORD Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98* pData);
	/*0x98 lcp打包*/
	DWORD Build_LE_PEER_KEEP_ALIVE_REQUEST(CHAR* pPacket, T_LE_PROTOCOL_98_LCP* pData);
	/*0x99打包*/
	DWORD Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99* pData);
	/*0x99 lcp打包*/
	DWORD Build_LE_PEER_KEEP_ALIVE_RESPONSE(CHAR* pPacket, T_LE_PROTOCOL_99_LCP* pData);
	/*查看当前是否为第三方*/
	bool Remote3rdParty();
	/*调用指定算法生成对应数据*/
	void getWirelineAuthentication(char* pPacket, DWORD &size);
	DWORD Build_WL_VC_CHNL_CTRL_REQUEST(CHAR* pPacket, T_WL_PROTOCOL_13* pData);
};

#endif