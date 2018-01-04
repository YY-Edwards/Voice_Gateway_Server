#ifndef NSSERIAL_H
#define NSSERIAL_H
#include "NS\mutex.h"
#include "NSStruct.h"
#include "linklist.h"

class NSLog;
class NSNetBase;

class NSSerial
{
public:
	NSSerial();
	~NSSerial();

	int Start(StartSerialParam *param);
	/*获取当前Xnl状态*/
	xnl_status_enum GetXnlStatus();
	/*设置Xnl状态*/
	void SetXnlStatus(xnl_status_enum value);
	void AddXnlItem(work_item_t* p);
	void RECV_LE_XCMP_XNL(work_item_t * w, const char* pData);
private:
	bool m_bThreadWork;
	HANDLE m_getSerailThread;//获取序列号线程句柄
	NSLog* m_pLog;
	pLinkItem m_serialLink;
	LOCKERTYPE m_serialMutex;
	unsigned char m_serialNumber[SIZE_SERIAL];
	unsigned short m_desAddress;
	unsigned short m_sourceAddress;
	unsigned short m_trascationId;
	struct sockaddr_in *m_pSockaddrMaster;
	pXQTTNet m_pMasterXqttnet;//主中继持有指针
	StartNetParam m_netParam;//配置参数
	xnl_status_enum m_xnl_status_enum;//xnl状态
	pOnSerialCallFunc m_pSerialCallBack;
	NSNetBase* m_pCallParam;

	/*序列号获取线程*/
	static unsigned int __stdcall GetSerialThreadProc(void* pArguments);
	/*序列号获取线程执行函数*/
	void GetSerialThread();
	work_item_t *PopSerialItem();
	void SetSerialNumber(unsigned char* pSerial);
	void SEND_LE_XCMP_XNL_DEVICE_MASTER_QUERY(work_item_t* w);
	void SEND_LE_XCMP_XNL_DEVICE_AUTH_KEY_REQUEST(work_item_t* w);
	void SEND_LE_XCMP_XNL_DEVICE_CONN_REQUEST(work_item_t* w);
	void SEND_LE_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(work_item_t* w);
	void SEND_LE_XCMP_XNL_DATA_MSG_ACK(work_item_t* w);
	void EncryptAuthenticationValue(unsigned char* un_Auth, unsigned char* en_Auth);
	DWORD Build_XCMP_XNL_DEVICE_MASTER_QUERY(CHAR* pPacket, T_XNL_DEVICE_MASTER_QUERY* pData);
	DWORD Build_XCMP_XNL_DEVICE_CONN_REQUEST(CHAR* pPacket, T_XNL_DEVICE_CONN_REQUEST* pData);
	DWORD Build_XCMP_XNL_DEVICE_AUTH_KEY_REQUEST(CHAR* pPacket, T_XNL_DEVICE_AUTH_KEY_REQUEST* pData);
	DWORD Build_XCMP_XNL_DATA_MSG_GET_SERIAL_REQUEST(CHAR* pPacket, T_XNL_DATA_MSG* pData);
	DWORD Build_XCMP_XNL_DATA_MSG_ACK(CHAR* pPacket, T_XNL_DATA_MSG_ACK* pData);
	DWORD Build_LE_XCMP_XNL(CHAR* pPacket, moto_protocol_xcmp_xnl_t* pData, CHAR* pXnlPacket, int length);
	void Unpack_XCMP_XNL_MASTER_STATUS_BRDCST(char* pData, moto_protocol_xcmp_xnl_t* networkData);
	void Unpack_XCMP_XNL_DEVICE_AUTH_KEY_REPLY(char* pData, moto_protocol_xcmp_xnl_t* networkData);
	void Unpack_XCMP_XNL_DEVICE_CONN_REPLY(char* pData, moto_protocol_xcmp_xnl_t* networkData);
	void Unpack_XCMP_XNL_DATA_MSG(char* pData, moto_protocol_xcmp_xnl_t* networkData);
	void ClearSerialLink();
	void SendXnlToMaster(work_item_t *w, unsigned long timeout = TIMEOUT_LE);
};


#endif