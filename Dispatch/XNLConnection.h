//The MOTOTRBO ADP XCMP Console Demo Application software is considered Motorola Confidential Restricted information and 
//contains intellectual property owned by Motorola, Inc., which is protected by U.S. copyright laws and international 
//copyright treaties.  Unauthorized reproduction or distribution of this software is strictly prohibited.  2009 Motorola, 
//Inc. All Rights Reserved.

#pragma once
//#include <winsock2.h>
#include "xnl_messages_def.h"
#include "xcmp_msg_defs.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "tcpCommon.h"
#define WM_RX_XCMP_MESSAGE (WM_USER + 100) 

typedef enum
{
    XNL_WAIT_MASTER_BRDCST = 1,
    XNL_WAIT_AUTH_KEY_REPLY,
    XNL_WAIT_CONN_REPLY,
    XNL_WAIT_SYSMAP_BRDCST,
    XNL_WAIT_DEV_INIT_STATUS,
    XNL_WAIT_DEV_INIT_COMPLETE,
    XNL_INIT_COMPLETE,
}XNL_CONNECTION_STATE;

typedef struct msg_queue{
    char * p_msg;
    struct msg_queue *next;
	char * sessionId;
}MSG_QUEUE_T;

typedef enum{
    XNL_CONNECTION_SUCCESS = 0,
    XNL_CONNECTION_FAILURE = 1,
    XNL_RCV_XCMP_MSG = 2,
    XNL_SENT_XCMP_MSG = 3,
}XNL_EVENT;

typedef struct {
    XNL_EVENT event;
    char *p_msg;   
}XNL_NOTIFY_MSG;
class CXNLConnection
{
private:
    // private constructor, can only create this class with CreatConnection()
	
	CXNLConnection(SOCKET s, std::string auth_key, unsigned long delta);

public:
    ~CXNLConnection(void);

public:
    // to create a instance
	static CXNLConnection* CreatConnection(DWORD ip_addr, int port, std::string auth_key, unsigned long delta);
    BOOL send_xcmp_brightness_msg(unsigned char function, unsigned char intensity);
    BOOL send_xcmp_pui_brdcst(unsigned char pui_type,
                                     unsigned short pui_id,
                                     char           pui_state,
                                     char           pui_state_min,
                                     char           pui_state_max);
	BOOL send_xcmp_call_ctrl_request(std::string sessionId, 
									 unsigned char function,
                                     unsigned char call_type,
                                     unsigned char addr_type,
                                     unsigned long rmt_addr,
                                     unsigned long group_id);
   BOOL send_xcmp_rmt_radio_ctrl_request(std::string sessionId,
												 unsigned char feature,
                                                 unsigned char operation,
                                                 unsigned char addr_type,
                                                 unsigned long rmt_addr);
   BOOL CXNLConnection::send_xcmp_menu_navigation_request(unsigned char function,
                                                      unsigned char display_mode,
                                                      unsigned short menu_id,
                                                      unsigned short  count);
   BOOL send_xcmp_chan_zone_selection_request(unsigned char function,
                                                           unsigned short zone_num,
                                                           unsigned short chan_num);
   BOOL send_xcmp_tx_ctrl_request(unsigned char function, unsigned char mode);

   BOOL send_xcmp_radio_status_request(unsigned char condition);
   

private:
    // stub to use _beginThreadEx(), if need inherit run() instead
    static unsigned __stdcall _workThread(void*); 
    virtual unsigned int run(void);

    // process message
    // default behavior is to send message to main window, inherit it if needed
    virtual void OnXnlMessageProcess(char* pBuf);
    void OnXCMPMessageProcess(char *pBuf);
    BOOL recv_xnl_message(char **pp_rcv_msg);
    BOOL send_xnl_message(char * p_msg_buf);
    void decode_xnl_master_status_broadcast(char *p_msg_buf);
    void decode_xnl_auth_key_reply(char *p_msg_buf);
    void decode_xnl_connection_reply(char *p_msg_buf);
    void decode_xnl_sysmap_brdcst(char *p_msg_buf);
    void decode_xnl_data_msg(char *p_msg_buf);
    void decode_xnl_data_msg_ack(char *p_msg_buf);
    void decode_xcmp_dev_init_status(char *p_msg_buf);
    void send_xnl_auth_key_request();
    void send_xnl_connection_request(unsigned char *p_auth_key);
    void send_xcmp_dev_init_status();
    void send_xnl_data_msg_ack(char *p_xcmp_data_msg);
    void init_xnl_header_of_xcmp_msg(char *p_msg, int payload_len);
    void encipher(unsigned long *const v, 
          unsigned long *const w,
          const unsigned long *const k);
    void decipher(unsigned long *const v,
          unsigned long *const w,
          const unsigned long *const k);
    void enqueue_msg(char *p_msg,std::string sessionId);
    MSG_QUEUE_T * dequeue_msg();
	void decode_xcmp_radio_status_reply(char *p_msg);
public:
    BOOL    m_bConnected;
    unsigned long m_XCMP_ver; /* XCMP version */
	char readmac[11];

private:
    HWND    m_hWnd;
    HANDLE  m_hThread;
    SOCKET  m_socket;
//    BOOL    m_bIsDisplayRadio;
    BOOL    m_bWaitForAck;
    BOOL    m_bCloseSocket;
    HANDLE  m_hEvent;
    int conn_retry;
    unsigned long m_delta;
    XNL_CONNECTION_STATE m_XnlState;
    unsigned long m_auth_key[4]; 
    unsigned long encrypted_seed[2]; /* Used to verify the master device */
    unsigned char m_trans_id_base;     /* used as the upper byte of the transaction Id for subsequent requests */
    unsigned char m_TxXCMPCount;
    unsigned char m_tx_xnl_flag;
    unsigned char m_rx_xnl_flag;
    unsigned short m_xnl_src_addr;
    unsigned short m_xnl_dst_addr;
    MSG_QUEUE_T*   m_pSendQueHdr;
    MSG_QUEUE_T*   m_pSendQueTail;
    char *m_pLastSendMsg;
	void controlReply(unsigned char result, unsigned char transactionIdBase, unsigned char txXcmpCount,std::string radioId);
	void controlBroadcast(unsigned char result,std::string radioId,int type);
	void radioStatusChange(std::string radioId);
	void callReply( unsigned char result, unsigned char transactionIdBase, unsigned char txXcmpCount, std::string radioId);
	void callBroadcast(unsigned char flag, std::string radioId);
	
};
