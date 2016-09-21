//The MOTOTRBO ADP XCMP Console Demo Application software is considered Motorola Confidential Restricted information and 
//contains intellectual property owned by Motorola, Inc., which is protected by U.S. copyright laws and international 
//copyright treaties.  Unauthorized reproduction or distribution of this software is strictly prohibited.  2009 Motorola, 
//Inc. All Rights Reserved.

#include "stdafx.h"
#include <process.h>
#include ".\xnlconnection.h"

/*******************************************************************************
*
* FUNCTION NAME : CXNLConnection
*
*---------------------------------- PURPOSE ------------------------------------
* It is a construction function.
*---------------------------------- SYNOPSIS -----------------------------------
*
* PARAMETERS:   s        - Socket for XCMP communication with the radio.
*               auth_key - String for Motorola Licensed XNL authentication key
*               delta    - Motorola licensed Delta value
*               hWnd     - windows handle that is used by the XCMP/XNL thread to
*                          send messages to the windows.
*
* RETURN VALUE: This function is exited only when the thread is closed/terminated.
*
*******************************************************************************/

CXNLConnection::CXNLConnection(SOCKET s, CString auth_key, unsigned long delta)
{
    m_socket = s;
  
    m_delta = delta;
    m_bConnected = FALSE;
    m_hThread = 0;
    m_XnlState = XNL_WAIT_MASTER_BRDCST;
    /* Remove the space from the string */
    auth_key.Remove(' ');
    /* get the 16 bytes key from the string, now the auth_key format is similar as
     * 0x112233440x556677880x99aabbcc0xddeeff00. Start from the 1st byte, each 10 
     * bytes can be converted to a unsigned long integer.
     */
   
	m_auth_key[0] = 0x152C7E9D;
	m_auth_key[1] = 0x38BE41C7;

	m_auth_key[2] = 0x71E96CA4;
	m_auth_key[3] = 0x6CAC1AFC;
   
    encrypted_seed[0] = 0;
    encrypted_seed[1] = 0;
    m_trans_id_base = 0;
    m_bWaitForAck = FALSE;
    m_bCloseSocket = FALSE;
    m_hEvent = NULL;
    conn_retry = 0;
    encrypted_seed[0] = 0; 
    encrypted_seed[1] = 0;
    m_trans_id_base = 0;     
    m_TxXCMPCount = 0;
    m_tx_xnl_flag = 0;
    m_rx_xnl_flag = 8; /* Set it to an invalid value */
    m_xnl_src_addr = 0;
    m_xnl_dst_addr = 0;
    m_pSendQueHdr = NULL;
    m_pSendQueTail = NULL;
    m_pLastSendMsg = NULL;
    m_XCMP_ver = 0;
    m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	memset(readmac, 0, 11);
}

CXNLConnection::~CXNLConnection(void)
{
    MSG_QUEUE_T* p_tmp = NULL;

    if (m_socket != INVALID_SOCKET)
    {
        m_bCloseSocket = TRUE;
        WaitForSingleObject(m_hThread, INFINITE);
        CloseHandle(m_hEvent);
        /* Free the sending queue */
        for (; m_pSendQueHdr; )
        {
            p_tmp = m_pSendQueHdr;
            m_pSendQueHdr = m_pSendQueHdr->next;
            free(p_tmp->p_msg);
            free(p_tmp);
        }

        if (m_pLastSendMsg != NULL)
        {
            free(m_pLastSendMsg);
        }
    }
    
}

///////////////////////////////////////////////////////////////////////////////
// create an instance, return the pointer to the instance if success, else return false 
CXNLConnection* CXNLConnection::CreatConnection(DWORD ip_addr = 0, int port = 8002, 
                                                CString auth_key = _T(""), unsigned long delta = 0)
{

    SOCKADDR_IN target_addr = {0};  /* The destination IP address */
    WSADATA         wsda;           /* Structure to store info    */
                
    WSAStartup(MAKEWORD(2,2),   &wsda);       /* Load version 2.2 of Winsock */
    SOCKET s = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);  
    
    if(s == INVALID_SOCKET)             //   Socket create Error
    {   
        WSACleanup();
        return NULL;
    }   
    
    //   Fill   in   the   interface   information   
    target_addr.sin_family      = AF_INET;   
    target_addr.sin_port        = htons(port);   
	target_addr.sin_addr.s_addr = ip_addr;
       
    if(SOCKET_ERROR == connect(s,(LPSOCKADDR)&target_addr, sizeof(target_addr)))
    {
		int a = GetLastError();
        closesocket(s);
        WSACleanup();
        return NULL;
    }
    else
    {
        //Create a thread to receive the message from the socket
        CXNLConnection * p = new CXNLConnection(s, auth_key, delta);
         

        p->m_hThread = (HANDLE)_beginthreadex(NULL, 0, &_workThread, (void*)p, 0, NULL);
        
        return p;
    }
}

///////////////////////////////////////////////////////////////////////////////
unsigned __stdcall CXNLConnection::_workThread( void* pThis)
{
    return ((CXNLConnection*)pThis)->run();
}


/*******************************************************************************
*
* FUNCTION NAME : run
*
*---------------------------------- PURPOSE ------------------------------------
* This function first checks whether there are data received on the socket. If true,
* then receive the XNL message and process it. Second, it checks whether there are
* messages need to send. If ture, then send the message through the socket.
*---------------------------------- SYNOPSIS -----------------------------------
*
* PARAMETERS:   None. 
*
* RETURN VALUE: This function is exited only when the thread is closed/terminated.
*
*******************************************************************************/
unsigned int CXNLConnection::run(void)
{
    MSG_QUEUE_T *p_send_msg_node = NULL;
    char *p_send_msg = NULL;
    char *p_rcv_msg = NULL;
    BOOL bSocketErr = TRUE;
    fd_set fdread;
    struct timeval read_timeval = {0, 10000};/* wait 10ms */
    unsigned long StartTime = GetTickCount();
    unsigned long CurTime = 0;
    int retry = 0;
    int ret = 0;
    
    SetEvent(m_hEvent);

    while (1)
    {
        /* Receive a Message first */
        
        /* Clear the read set before calling select */
        FD_ZERO(&fdread);
        /* Add m_socket to the read set */
        FD_SET(m_socket, &fdread);

        /* Non-blocking operation */
        if ((ret = select(0, &fdread, NULL, NULL, &read_timeval)) == SOCKET_ERROR)
        {
            break;
        }
        else if (ret > 0)
        {
            if (FD_ISSET(m_socket, &fdread))
            {
                /* receive the xnl message */
                bSocketErr = recv_xnl_message(&p_rcv_msg);

                /* check whether there are errors occurred during the message receiving */
                if (bSocketErr == FALSE)
                {
                    break;
                }
                
                /* Process the received xnl message */
                OnXnlMessageProcess(p_rcv_msg);
                
                /* free the buffer */
                free(p_rcv_msg);
            }
        }

        /* The next is to send an xcmp message. First check whether there is a message need to resend. */
        if (m_bWaitForAck == FALSE) /* there is no message need to resend  */
        {
            p_send_msg_node = dequeue_msg(); /* Dequeue an XCMP message from the sending queue */
            if (p_send_msg_node != NULL)
            {
                p_send_msg = p_send_msg_node->p_msg;
                /* Need to set the XNL flag and trans id */
                ((xnl_msg_hdr_t *)p_send_msg)->xnl_flag = m_tx_xnl_flag;
                *((char *)(&((xnl_msg_hdr_t *)p_send_msg)->trans_id)) = m_trans_id_base;
                *((char *)(&((xnl_msg_hdr_t *)p_send_msg)->trans_id) + 1) = m_TxXCMPCount;

                bSocketErr = send_xnl_message(p_send_msg);

                /* check whether there are errors occurred during the message sending */
                if (bSocketErr == FALSE)
                {
                    break;
                }

                /* For each XCMP message, an ACK message shall be received. */
                m_bWaitForAck = TRUE; 

                /* Save the message for retry */
                m_pLastSendMsg = p_send_msg;
                StartTime = GetTickCount();

                /* free the node */
                free(p_send_msg_node);
            }
        }
        else  /* Re-send the xcmp message */
        {
            CurTime = GetTickCount();
            if ((CurTime - StartTime) >= 500) /* resend the message after 500ms */
            {
                bSocketErr = send_xnl_message(m_pLastSendMsg);
                /* check whether there are errors occurred during the message sending */
                if (bSocketErr == FALSE)
                {
                    break;
                }
                
                StartTime = CurTime;
                retry ++;

                if (retry == 4) /* Discard the message after retry 4 times */
                {
                    /* reset m_bWaitForAck */
                    m_bWaitForAck = FALSE;
                    if (m_tx_xnl_flag < 7)
                    {
                        m_tx_xnl_flag ++;
                    }
                    else
                    {
                        m_tx_xnl_flag = 0;
                    }
                    m_TxXCMPCount ++;
                    free(m_pLastSendMsg);
                    m_pLastSendMsg = NULL;
                    retry = 0;
                }
            }
            
        }
        
        /* check whether the user manually disconnected the socket connection. */
        if (m_bCloseSocket == TRUE)
        {
            break;
        }
    }

    closesocket(m_socket);
    WSACleanup();
    return 0;
}

/*******************************************************************************
*
* FUNCTION NAME : recv_xnl_message
*
*---------------------------------- PURPOSE ------------------------------------
* This function is to receive an XNL message through the socket.
*---------------------------------- SYNOPSIS -----------------------------------
*
* PARAMETERS:   pp_rcv_msg - pointer to a buffer, it is used to return the 
                             received XNL message. 
*
* RETURN VALUE: If the function successfully receives an XNL message, then return
*               TRUE, otherwise FALSE.
*
*******************************************************************************/
BOOL CXNLConnection::recv_xnl_message(char * * pp_rcv_msg)
{
    int expected_size       = 0;
    int total_size          = 0;
    char * p_msg_buf        = NULL;
    char * p_cur_buf        = NULL;
    int len                         = 0;
    unsigned short msg_len = 0;
    BOOL ret = TRUE;
    
    /* Receive the first 2 bytes to get the message length */
    p_cur_buf = (char *)&msg_len;
    expected_size = sizeof(msg_len);
    while (expected_size > 0)
    {
        len = recv(m_socket, p_cur_buf, expected_size, 0);
        if (len == SOCKET_ERROR)
        {
            ret = FALSE;
            break;
        }
        else if (len == 0) /* the socket is closed */
        {
            ret = FALSE;
            break;
        }
        else
        {
            expected_size   -= len;
            total_size += len;
            p_cur_buf += len;
        }
    }
    if (ret == FALSE)
    {
        return (ret);
    }
    /* continue to receive the payload */        
    expected_size = ntohs(msg_len);
    /* malloc a buffer to receive the message */
    p_msg_buf = (char *) malloc(expected_size + 2);

    if (p_msg_buf == NULL)
    {
        return (FALSE);
    }

    memset(p_msg_buf, 0, expected_size + 2);
    /* Copy the received 2 bytes to the message  */
    memcpy(p_msg_buf, (char *)&msg_len, sizeof(msg_len));

    p_cur_buf = p_msg_buf + 2;
    /* Continue to receive the whole XNL message */
    while (expected_size > 0)
    {
        len = recv(m_socket, p_cur_buf, expected_size, 0);
        if (len == SOCKET_ERROR)
        {
            ret = FALSE;
            break;
        }
        else if (len == 0)
        {
            ret = FALSE;
            break;
        }
        else
        {
            expected_size -= len;
            p_cur_buf += len;
            total_size += len;
        }
    }
    *pp_rcv_msg = p_msg_buf;

    return (ret);
}

void CXNLConnection::OnXnlMessageProcess(char* pBuf)
{
    unsigned short xnl_opcode = 0;
    XNL_NOTIFY_MSG *p_notify_msg = (XNL_NOTIFY_MSG *)malloc(sizeof(XNL_NOTIFY_MSG));
    unsigned short msg_len = 0;
    char * p_msg_buf = NULL;
    
    if ((pBuf == NULL) || (p_notify_msg == NULL))
    {
        return;
    }    

    msg_len = ntohs(*((unsigned short *)pBuf)) + 2;
    p_msg_buf = (char *)malloc(msg_len);
    memcpy(p_msg_buf, pBuf, msg_len);
    /* Forward the received message to main window to display the raw data message. */
    p_notify_msg->event = XNL_RCV_XCMP_MSG;
    p_notify_msg->p_msg = p_msg_buf;
    //::PostMessage(m_hWnd, WM_RX_XCMP_MESSAGE, NULL, (LPARAM)p_notify_msg);


    /* Get the xnl_opcode, byte 3 and byte 4 is the xnl opcode */
    xnl_opcode = ntohs(*((unsigned short *)(pBuf + 2)));
	

    switch (xnl_opcode)
    {
        case XNL_MASTER_STATUS_BROADCAST:
            /* start the xnl connection */
            decode_xnl_master_status_broadcast(pBuf);
            break;
            
        case XNL_AUTH_KEY_REPLY:
            if (m_XnlState == XNL_WAIT_AUTH_KEY_REPLY)
            {
                decode_xnl_auth_key_reply(pBuf);
            }
            /* ignore this message when the app is in other states */
            break;
            
        case XNL_CONNECTION_REPLY:
            if (m_XnlState == XNL_WAIT_CONN_REPLY)
            {
                decode_xnl_connection_reply(pBuf);
            }
            /* ignore this message when the app is in other states */
            break;
            
        case XNL_SYSMAP_BRDCST:
            if (m_XnlState >= XNL_WAIT_SYSMAP_BRDCST)
            {
                decode_xnl_sysmap_brdcst(pBuf);
            }
            break;
            
        case XNL_DATA_MSG:
            if (m_XnlState >= XNL_WAIT_DEV_INIT_STATUS)
            {
                decode_xnl_data_msg(pBuf);
            }
            break;
            
        case XNL_DATA_MSG_ACK:
            if (m_XnlState >= XNL_WAIT_DEV_INIT_STATUS)
            {
				
                decode_xnl_data_msg_ack(pBuf);
            }
            break;
            
        default: /* Unknown Opcode, just ignore it */
            break;

    }
    
}

void CXNLConnection::decode_xnl_master_status_broadcast(char * p_msg_buf)
{
    xnl_master_status_broadcast_t * p_msg = (xnl_master_status_broadcast_t *)p_msg_buf;
    if ((p_msg != NULL) && (ntohs(p_msg->msg_hdr.xnl_opcode) == XNL_MASTER_STATUS_BROADCAST))
    {
        /* Get the xnl address of the master device */
        m_xnl_dst_addr = ntohs(((xnl_msg_hdr_t *)p_msg)->src_addr);
        send_xnl_auth_key_request();
        m_XnlState = XNL_WAIT_AUTH_KEY_REPLY;
    }
}

void CXNLConnection::send_xnl_auth_key_request()
{
    /* malloc a buffer for the auth key request message */
    xnl_auth_key_request_t * p_msg = (xnl_auth_key_request_t *)malloc(sizeof(xnl_auth_key_request_t));

    if (p_msg == NULL)
    {
        return;
    }
    /* compose the xnl authentication key request */
    p_msg->msg_hdr.msg_len = htons(XNL_AUTH_KEY_REQ_SIZE - 2); /* exclude the 2 bytes length field */
    p_msg->msg_hdr.xnl_opcode = htons(XNL_AUTH_KEY_REQ);
    p_msg->msg_hdr.xnl_flag = 0;
    p_msg->msg_hdr.protocol_id = 0;
    p_msg->msg_hdr.src_addr = 0;
    p_msg->msg_hdr.dst_addr = htons(m_xnl_dst_addr);
    p_msg->msg_hdr.trans_id = 0;
    p_msg->msg_hdr.payload_len = 0;

    send_xnl_message((char *)p_msg);
    free(p_msg);

}

void CXNLConnection::decode_xnl_auth_key_reply(char *p_msg_buf)
{
    xnl_auth_key_reply_t *p_msg = (xnl_auth_key_reply_t *)p_msg_buf;


    if ((p_msg != NULL) && (ntohs(p_msg->msg_hdr.xnl_opcode) == XNL_AUTH_KEY_REPLY))
    {
        /* Get the temporary xnl address from the message */
        m_xnl_src_addr = ntohs(p_msg->tmp_xnl_addr);
        send_xnl_connection_request((unsigned char*)p_msg->auth_seed);
        m_XnlState = XNL_WAIT_CONN_REPLY;
    }
}

void CXNLConnection::send_xnl_connection_request(unsigned char *p_auth_seed)
{
    unsigned long * p_int = (unsigned long *)p_auth_seed;
    xnl_conn_request_t *p_msg = (xnl_conn_request_t *)malloc(sizeof(xnl_conn_request_t));

    if (p_msg == NULL)
    {
        return;
    }
    p_msg->msg_hdr.msg_len = htons(XNL_CONNECTION_REQUEST_SIZE - 2);
    p_msg->msg_hdr.xnl_opcode = htons(XNL_CONNECTION_REQUEST);
    p_msg->msg_hdr.xnl_flag = 0;
    p_msg->msg_hdr.protocol_id = 0;
    p_msg->msg_hdr.src_addr = htons(m_xnl_src_addr);
    p_msg->msg_hdr.dst_addr = htons(m_xnl_dst_addr);
    p_msg->msg_hdr.trans_id = 0;
    p_msg->msg_hdr.payload_len = htons(0x000c);
    p_msg->preferred_xnl_addr = 0;
    p_msg->dev_type = 0x0A;
    p_msg->auth_index = 0x01;

    *p_int = ntohl(*p_int);
    *(p_int + 1) = ntohl(*(p_int+1));

    /* encrypted the authentication seed */
    encipher((unsigned long *)p_auth_seed, encrypted_seed, m_auth_key);
    encrypted_seed[0] = htonl(encrypted_seed[0]);
    encrypted_seed[1] = htonl(encrypted_seed[1]);
    memcpy(p_msg->auth_key, encrypted_seed, sizeof(encrypted_seed));

    send_xnl_message((char *)p_msg);
    free(p_msg);
}

void CXNLConnection::encipher(unsigned long *const v, 
                              unsigned long *const w,
                              const unsigned long *const k)
{
    register unsigned long y=v[0], z=v[1], sum=0;   
    register unsigned long delta= m_delta;
    register unsigned long a=k[0], b=k[1], c=k[2], d=k[3];
    register unsigned long n=32;

    while(n-->0)
    {
        sum += delta;
        y += (z << 4)+a ^ z+sum ^ (z >> 5)+b;
        z += (y << 4)+c ^ y+sum ^ (y >> 5)+d;
    }

    w[0]=y; w[1]=z;
}

void CXNLConnection::decipher(unsigned long *const v,
                 unsigned long *const w,
                 const unsigned long *const k)
{
    register unsigned long y=v[0], z=v[1];
    register unsigned long delta = m_delta;
    register unsigned long sum = delta * 32;               
    register unsigned long a=k[0],b=k[1], c=k[2], d=k[3];
    register unsigned long n=32;                         
                             
    /* sum = delta<<5, in general sum = delta * n */     
    while(n-->0)                                         
    {                                                    
         z -= (y << 4)+c ^ y+sum ^ (y >> 5)+d;           
         y -= (z << 4)+a ^ z+sum ^ (z >> 5)+b;           
         sum -= delta;                                   
    }                                                    
                             
    w[0]=y; w[1]=z;                                      
}

void CXNLConnection::decode_xnl_connection_reply(char *p_msg_buf)
{
    xnl_conn_reply_t * p_msg = (xnl_conn_reply_t *)p_msg_buf;
    unsigned long deciphered_seed[2] = {0};
    unsigned long *p_encripted_seed = NULL;
    XNL_NOTIFY_MSG *p_notify_msg = (XNL_NOTIFY_MSG *)malloc(sizeof(XNL_NOTIFY_MSG));
        
    if ((p_msg != NULL) && (ntohs(p_msg->msg_hdr.xnl_opcode) == XNL_CONNECTION_REPLY))
    {
        if (p_msg->result_code == XNL_SUCESS) /* xnl connection success */
        {
            /* Verify the Master device's authentication key */
            p_encripted_seed = (unsigned long *)p_msg->encrypted_seed;
            /* Convert the byte order to Little Endian */
            *p_encripted_seed = ntohl(*p_encripted_seed);
            *(p_encripted_seed + 1) = ntohl(*(p_encripted_seed + 1));
            decipher(p_encripted_seed, deciphered_seed, m_auth_key);

            /* convert the deciphered_seed to Big endian */
            deciphered_seed[0] = htonl(deciphered_seed[0]);
            deciphered_seed[1] = htonl(deciphered_seed[1]);

            /* Compare the deciphered_seed */
            if ((deciphered_seed[0] == encrypted_seed[0]) && (deciphered_seed[1] == encrypted_seed[1]))
            {
                m_trans_id_base = p_msg->trans_id_base;
                /* Get the new assigned xnl address from the message */
                m_xnl_src_addr = ntohs(p_msg->assigned_xnl_addr);  
                
                m_XnlState = XNL_WAIT_SYSMAP_BRDCST;
                free(p_notify_msg); 
            }
            else /* Reject the xnl connection */
            {
                m_XnlState = XNL_WAIT_MASTER_BRDCST;
                p_notify_msg->event = XNL_CONNECTION_FAILURE;
                p_notify_msg->p_msg = NULL;
              //  ::PostMessage(m_hWnd, WM_RX_XCMP_MESSAGE, NULL, (LPARAM)p_notify_msg);
            }
                        
            
        }
        else /* xnl connection failure */
        {
            /* Retry the authentication process by sending out a new AUTH_KEY_REQUEST message */
            if (conn_retry < 3)
            {
                send_xnl_auth_key_request();
                m_XnlState = XNL_WAIT_AUTH_KEY_REPLY;

                conn_retry ++;
            
                /* Send out XNL_CONNECTION_FAILURE event to notify the main process.
                 */
                p_notify_msg->event = XNL_CONNECTION_FAILURE;
                p_notify_msg->p_msg = NULL;
               // ::PostMessage(m_hWnd, WM_RX_XCMP_MESSAGE, NULL, (LPARAM)p_notify_msg);
            }
        }
    }
}

void CXNLConnection::decode_xnl_sysmap_brdcst(char *p_msg_buf)
{
    xnl_sysmap_brdcst_t *p_msg = (xnl_sysmap_brdcst_t *)p_msg_buf;
    if ((p_msg != NULL) && (ntohs(p_msg->msg_hdr.xnl_opcode) == XNL_SYSMAP_BRDCST))
    {
        /* Since this PC application just communicates with the radio, it does 
         * not care the other xcmp devices. So no need to parse this message. */
        if (m_XnlState == XNL_WAIT_SYSMAP_BRDCST)
        {
            m_XnlState = XNL_WAIT_DEV_INIT_STATUS;
        }
        /* else just keep the current state. It's for the case that the application 
         * receives the broadcast when the xnl connection has already created.*/
        
    }
}

/* Call xcmp handler to process the xcmp message */
void CXNLConnection::decode_xnl_data_msg(char * p_msg_buf)
{
    xnl_msg_hdr_t * p_msg = (xnl_msg_hdr_t *)p_msg_buf;
    
    if (p_msg != NULL)
    {
        /* First send back an ACK message for the received xnl data message */
        send_xnl_data_msg_ack(p_msg_buf);

        /* Validate the received xnl data message */                
        /* Make sure the destination address is correct (target to the app) */
        if ((ntohs(p_msg->dst_addr) != 0) && (ntohs(p_msg->dst_addr) != m_xnl_src_addr))
        {
            /* Just ignore this message */
        }
        else if (ntohs(p_msg->src_addr) != m_xnl_dst_addr) /* Make sure the source address is correct (only receive messages from the radio) */
        {
            /* Just ignore this message */
        }
        else
        {
            /* Check the xnl_flag to determine whether it's a new xcmp message or resent message */
            if (m_rx_xnl_flag != p_msg->xnl_flag)
            {
                OnXCMPMessageProcess(p_msg_buf);
                /* save the xnl_flag */
                m_rx_xnl_flag = p_msg->xnl_flag;
    
            }
            /* Ignore the resent message */
        }

    }

}

void CXNLConnection::decode_xnl_data_msg_ack(char * p_msg_buf)
{
    xnl_data_msg_ack_t *p_msg = (xnl_data_msg_ack_t *)p_msg_buf;

    if ((p_msg_buf != NULL) && (m_bWaitForAck == TRUE))
    {
		list<AllCommand>::iterator it;
		for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
		{
			if (it->ackNum == p_msg->msg_hdr.trans_id)
			{

		//		if (pDispatchPort != NULL)
				{
					////拼接json
					//rapidjson::Document document;
					//Document::AllocatorType& allocator = document.GetAllocator();
					//Value root(kObjectType);
					//root.AddMember("sn", it->sn, allocator);
					//root.AddMember("status", 0, allocator);
					//StringBuffer buffer;
					//Writer<StringBuffer> writer(buffer);
					//root.Accept(writer);
					//std::string reststring = buffer.GetString();
					//pDispatchPort->sendResultToClient(reststring);
					//unsigned char str[30] = { 0 };
					//sprintf_s((char *)str, sizeof(str), "result:0");

					//onData(myCallBackFunc, it->seq, it->command, (char *)str, sizeof(str));

				}
				allCommandList.erase(it++);
				break;
			}
		}

        /* check the xnl flag */
        if ((m_tx_xnl_flag == p_msg->msg_hdr.xnl_flag))
        {
            /* reset m_bWaitForAck */
            m_bWaitForAck = FALSE;
            if (m_tx_xnl_flag < 7)
            {
                m_tx_xnl_flag ++;
            }
            else
            {
                m_tx_xnl_flag = 0;
            }
            m_TxXCMPCount ++;
            /* free the saved msg */
            if (m_pLastSendMsg != NULL)
            {
                free(m_pLastSendMsg);
                m_pLastSendMsg = NULL;
            }
            /* Stop the timer */
        }
    }
}

void CXNLConnection::OnXCMPMessageProcess(char * pBuf)
{
	unsigned short xcmp_opcode = 0;

	if (pBuf == NULL)
	{
		return;
	}

	xcmp_opcode = ntohs(*((unsigned short *)(pBuf + sizeof(xnl_msg_hdr_t))));


	unsigned short xnl_opcode = 0;        //add by lcc
	unsigned short check_result = 0;       //add by lcc
	unsigned char rmt_fuction = 0;
	unsigned char  rmt_type_code = 0;
	unsigned char  rmt_result_code = 0;
	unsigned short xnl_transationid = 0;
	unsigned short  indicatorid = 0;
	xnl_opcode = ntohs(*((unsigned short *)(pBuf + 2)));  // add by lcc
	xnl_transationid = ntohs(*((unsigned short *)(pBuf + 5)));
	check_result = ntohs(*((unsigned short *)(pBuf + 16)));  // add by lcc
	rmt_fuction = ntohs(*((unsigned short *)(pBuf + 18)));
	rmt_type_code = ntohs(*((unsigned short *)(pBuf + sizeof(xnl_msg_hdr_t)+1)));
	rmt_result_code = ntohs(*((unsigned short *)(pBuf + sizeof(xnl_msg_hdr_t)+1)));
	indicatorid = ntohs(*((unsigned short *)(pBuf + sizeof(xnl_msg_hdr_t)+ 5)));
	list<AllCommand>::iterator it;
	switch (xcmp_opcode)
	{
	case XCMP_DEVICE_INIT_STATUS_BRDCST:
		decode_xcmp_dev_init_status(pBuf);
		send_xcmp_radio_status_request(0x08);             //read serial
		break;
		//在线检测
	case 0XB41C:
		
		for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
		{
			if (0x000B == xnl_opcode && 0xB41C == xcmp_opcode  )
			{
				//拼接json
				rapidjson::Document document;
				Document::AllocatorType& allocator = document.GetAllocator();
				Value root(kObjectType);
				root.AddMember("sn", it->sn, allocator);
				BOOL rmtflag = FALSE;
				if (rmt_type_code == 0x00)                                                   //在线检测
				{
					if (0x0010 == check_result/* & 0x00FF)*/)
					{
						rmtflag = TRUE; 
						root.AddMember("status", 1, allocator);// 1:在线
					}
					else
					{
						rmtflag = false; 
						root.AddMember("status", 0, allocator);;//0:不在线
					}
					StringBuffer buffer;
					Writer<StringBuffer> writer(buffer);
					root.Accept(writer);
					std::string reststring = buffer.GetString();
					//pDispatchPort->sendResultToClient(reststring);
					allCommandList.erase(it++);
				}
				else if (rmt_type_code == 0x01)
				{
					if (0x0110 == check_result/* & 0x00FF)*/)                                    //摇闭
					{
						rmtflag = true;                                   //成功    
						root.AddMember("status", 1, allocator);
					} 
					else
					{
						rmtflag = false;                               //失败
						root.AddMember("status", 0, allocator);
					}
					StringBuffer buffer;
					Writer<StringBuffer> writer(buffer);
					root.Accept(writer);
					std::string reststring = buffer.GetString();
					//pDispatchPort->sendResultToClient(reststring);
					allCommandList.erase(it++);
				}
				else if (rmt_type_code == 0x02)
				{
					if (0x0210 == check_result/* & 0x00FF)*/)                                     //摇开
					{
						rmtflag = true;                                  //成功
						root.AddMember("status", 1, allocator);
					}
					else
					{
						rmtflag = FALSE;                                //失败
						root.AddMember("status", 0, allocator);
					}
					StringBuffer buffer;
					Writer<StringBuffer> writer(buffer);
					root.Accept(writer);
					std::string reststring = buffer.GetString();
					//pDispatchPort->sendResultToClient(reststring);
					allCommandList.erase(it++);
				}
				else if (rmt_type_code == 0x03)
				{
					if (0x0310 == check_result/* & 0x00FF)*/)                                    //远程监听
					{
						rmtflag = true;                                   //成功
						root.AddMember("status", 1, allocator);
					}
					else
					{
						rmtflag = false;                                   // 失败
						root.AddMember("status",0, allocator);
					}
					StringBuffer buffer;
					Writer<StringBuffer> writer(buffer);
					root.Accept(writer);
					std::string reststring = buffer.GetString();
					//pDispatchPort->sendResultToClient(reststring);
					allCommandList.erase(it++);
				}
			
			
		}

		//if (0x000B == xnl_opcode && 0xB41C == xcmp_opcode)
		//{
		//	BOOL rmtflag = FALSE;
		//	if (rmt_type_code ==0x00)    //在线检测
		//	{
		//		if (0x0010 == check_result/* & 0x00FF)*/)
		//		{
		//			rmtflag = TRUE;               // 1:在线
		//		/*	if (myCallBackFunc != NULL)
		//			{
		//				unsigned char str[20] = { 0 };
		//				sprintf_s((char *)str, sizeof(str), "result:1");
		//				onData(myCallBackFunc, 1, CHECK_RADIO_ONLINE, (char*)str, sizeof(str));
		//			}*/

		//		}
		//		else
		//		{
		//			rmtflag = TRUE;               // 0:不在线
		//			/*if (myCallBackFunc != NULL)
		//			{
		//				unsigned char str[20] = { 0 };
		//				sprintf_s((char *)str, sizeof(str), "result:0");
		//				onData(myCallBackFunc, 1, CHECK_RADIO_ONLINE, (char*)str, sizeof(str));
		//			}*/
		//		}

		//	}
		//	else if (rmt_type_code == 0x01)
		//	{
		//		if (0x0110 == check_result/* & 0x00FF)*/)              //摇闭
		//		{
		//			rmtflag = FALSE;              //1:成功
		//			/*if (myCallBackFunc != NULL)
		//			{
		//				unsigned char str[20] = { 0 };
		//				sprintf_s((char *)str, sizeof(str), "result:1");
		//				onData(myCallBackFunc, 1, REMOTE_CLOSE, (char*)str, sizeof(str));
		//			}*/
		//		}
		//		else
		//		{
		//			//if (myCallBackFunc != NULL)  //  0:失败
		//			//{
		//			//	unsigned char str[20] = { 0 };
		//			//	sprintf_s((char *)str, sizeof(str), "result:0");
		//			//	onData(myCallBackFunc, 1, REMOTE_CLOSE, (char*)str, sizeof(str));
		//			//}
		//		}
		//	}
		//	else if (rmt_type_code == 0x02)
		//	{
		//		if (0x0210 == check_result/* & 0x00FF)*/)              //摇开
		//		{
		//			rmtflag = FALSE;              //1:成功
		//			/*if (myCallBackFunc != NULL)
		//			{
		//				unsigned char str[20] = { 0 };
		//				sprintf_s((char *)str, sizeof(str), "result:1");
		//				onData(myCallBackFunc, 1, REMOTE_OPEN, (char*)str, sizeof(str));
		//			}*/
		//		}
		//		else
		//		{
		//			rmtflag = FALSE;              //0:失败
		//			/*if (myCallBackFunc != NULL)
		//			{
		//				unsigned char str[20] = { 0 };
		//				sprintf_s((char *)str, sizeof(str), "result:0");
		//				onData(myCallBackFunc, 1, REMOTE_OPEN, (char*)str, sizeof(str));
		//			}*/
		//		}

		//	}
		//	else if (rmt_type_code == 0x03)
		//	{
		//		if (0x0310 == check_result/* & 0x00FF)*/)              //远程监听
		//		{
		//			rmtflag = FALSE;              //1:成功
		//			/*if (myCallBackFunc != NULL)
		//			{
		//				unsigned char str[20] = { 0 };
		//				sprintf_s((char *)str, sizeof(str), "result:1");
		//				onData(myCallBackFunc, 1, REMOTE_MONITOR, (char*)str, sizeof(str));
		//			}*/
		//		}
		//		else
		//		{
		//			rmtflag = FALSE;              //0:不成功
		//			/*if (myCallBackFunc != NULL)
		//			{
		//				unsigned char str[20] = { 0 };
		//				sprintf_s((char *)str, sizeof(str), "result:0");
		//				onData(myCallBackFunc, 1, REMOTE_MONITOR, (char*)str, sizeof(str));
		//			}*/
		//		}
		//	}
		//	
		
			break;
		}
	case 0XB413:                 //紧急报警
		if (0x000B == xnl_opcode && 0xB413 == xcmp_opcode)
		{
			
						//1:成功
			/*if (myCallBackFunc != NULL)
			{
				unsigned char str[30] = { 0 };
				sprintf_s((char *)str, sizeof(str), "result:1");
				onData(myCallBackFunc, 1, EMERGENCY, (char *)str, sizeof(str));
			}*/
			
			break;
		}
		break;
	case 0X841C:              //远程
		//if (0x000B == xnl_opcode && 0X841C == xcmp_opcode)
		//{

		//	BOOL rmtflag = FALSE;
		//	if (0x00 == rmt_result_code)
		//	{
		//		rmtflag = TRUE;               //1:成功
		//		if (myCallBackFunc != NULL)
		//		{
		//			unsigned char str[30] = { 0 };
		//			sprintf_s((char *)str, sizeof(str), "result:1");
		//			onData(myCallBackFunc, 1, REMOTE_REPLY, (char *)str, sizeof(str));
		//		}
		//	}
		//
		//	else
		//	{
		//		rmtflag = FALSE;              //0:失败
		//		if (myCallBackFunc != NULL)
		//		{
		//			unsigned char str[30] = { 0 };
		//			sprintf_s((char *)str, sizeof(str), "result:0");
		//			onData(myCallBackFunc, 1, REMOTE_REPLY, (char *)str, sizeof(str));
		//		}
		//	}

		//}
		break;
	case XCMP_RADIO_STATUS_REPLY:
		{
			decode_xcmp_radio_status_reply(pBuf);
			/*if (myCallBackFunc != NULL)
			{
				unsigned char str[30] = { 0 };
				sprintf_s((char *)str, sizeof(str), "result:1");
				onData(myCallBackFunc, 1, LICENSE, (char *)str, sizeof(str));
			}*/
		}
		break;
	case XCMP_CALL_CTRL_BRDCST:             //XCMP_CALL_CTRL_REPLY 
		switch (*((char*)(pBuf + sizeof(xnl_msg_hdr_t)+3)))
		{
		case 0x04:
		//	if (pDispatchPort != NULL)
			{
				//拼接json
				//拼接json
				rapidjson::Document document;
				Document::AllocatorType& allocator = document.GetAllocator();
				Value root(kObjectType);
				root.AddMember("sn", it->sn, allocator);
				root.AddMember("status", 1, allocator);                                //1：呼叫开始
				StringBuffer buffer;
				Writer<StringBuffer> writer(buffer);
				root.Accept(writer);
				std::string reststring = buffer.GetString();
			//	pDispatchPort->sendResultToClient(reststring);
			}
			allCommandList.erase(it++);
			/*if (myCallBackFunc != NULL)
			{
				unsigned char str[30] = { 0 };
				sprintf_s((char *)str, sizeof(str), "result:1");
				onData(myCallBackFunc, 1, CALL_START, (char *)str, sizeof(str));
			}*/
			break;
		case 0x03:
		//	if (pDispatchPort != NULL)
			{
				//拼接json
				rapidjson::Document document;
				Document::AllocatorType& allocator = document.GetAllocator();
				Value root(kObjectType);
				root.AddMember("sn", it->sn, allocator);
				root.AddMember("status", 2, allocator);        //2:呼叫结束
				StringBuffer buffer;
				Writer<StringBuffer> writer(buffer);
				root.Accept(writer);
				std::string reststring = buffer.GetString();
			//	pDispatchPort->sendResultToClient(reststring);
			}
			allCommandList.erase(it++);
			/*if (myCallBackFunc != NULL)
			{
				unsigned char str[30] = { 0 };
				sprintf_s((char *)str, sizeof(str), "result:1");
				onData(myCallBackFunc, 1, CALL_END, (char *)str, sizeof(str));
			}*/
			break;

		}

	     default: /* just forward the xcmp message to the application */
		/* The XCMP message has already forwarded to the Main process, so do nothing here. */
		break;
	}
}


void CXNLConnection::decode_xcmp_dev_init_status(char * p_msg_buf)
{
    xcmp_dev_init_status_t *p_msg = (xcmp_dev_init_status_t *)p_msg_buf;
    XNL_NOTIFY_MSG *p_notify_msg = NULL; 
    int offset = 0;
    
    if ((p_msg != NULL) && (ntohs(p_msg->msg_hdr.xnl_opcode) == XNL_DATA_MSG) &&
        (ntohs(p_msg->xcmp_opcode) == XCMP_DEVICE_INIT_STATUS_BRDCST))
    {
        
        /* parse the xcmp device init status message */
        if (p_msg->dev_init_type == 0x00) /* Device Init Status */
        {
            
            if (ntohs(p_msg->dev_status) != 0x0000)/* check device's status */
            {
                /* The radio reports that it encounters error during power up */
                /* what should the radio do next? */
            }
            else /* the radio is ok */
            {
               /* Get the XCMP version */
               memcpy((unsigned char *)&m_XCMP_ver, p_msg->xcmp_ver, 4);
               m_XCMP_ver = ntohl(m_XCMP_ver);
               /* this application is only applicable for display radio */
               while (offset < p_msg->array_size) /* parse the device descriptor */
               {
                   switch (*(p_msg_buf + 11 + offset))
                   {
                       case XCMP_DEVICE_FAMILY:
                           /* check whether it's MOTOTRBO radio */
                           
                           break;
                           
                       case XCMP_DEVICE_DISPLAY:
                           /* check whether it's display radio */
                           break;
                           
                       case XCMP_DEVICE_RF_BAND:
                           /* This PC application doesn't care this descriptor */
                           break;
                           
                       case XCMP_DEVICE_GPIO_CTRL:
                           /* This PC application doesn't care this descriptor */
                           break;
                           
                       case XCMP_DEVICE_RADIO_TYPE:
                           /* This PC application doesn't care this descriptor */
                           break;
                           
                       case XCMP_DEVICE_KEYPAD:
                           /* This PC application doesn't care this descriptor */
                           break;
                           
                       case XCMP_DEVICE_CHANNEL_KNOB:
                           /* This PC application doesn't care this descriptor */
                           break;
                           
                       case XCMP_DEVICE_VIRTUAL_PERSONALITY_SUPPORT:
                           /* This PC application doesn't care this descriptor */
                           break;
                           
                       default: /* Just ignore unknown descriptor */
                           break;
                   }
                   offset += 2;
               }
               send_xcmp_dev_init_status();
            }
            m_XnlState = XNL_WAIT_DEV_INIT_COMPLETE;
            
        }
        else if (p_msg->dev_init_type == 0x01) /* Device Init Complete */
        {
            m_XnlState = XNL_INIT_COMPLETE;
            m_bConnected = TRUE;
            p_notify_msg = (XNL_NOTIFY_MSG *)malloc(sizeof(XNL_NOTIFY_MSG));
            p_notify_msg->event = XNL_CONNECTION_SUCCESS;
            p_notify_msg->p_msg = NULL;
           // ::PostMessage(m_hWnd, WM_RX_XCMP_MESSAGE, NULL, (LPARAM)p_notify_msg);
        }
        else /* wrong message */
        {
            /* it's radio's error. report the error */
        }
    }
}

void CXNLConnection::send_xcmp_dev_init_status()
{
    xcmp_dev_init_status_t *p_xcmp_msg = (xcmp_dev_init_status_t *)malloc(sizeof(xcmp_dev_init_status_t) + 3);
    int payload_len = sizeof(xcmp_dev_init_status_t) + 3 - sizeof(xnl_msg_hdr_t);

    if (p_xcmp_msg == NULL)
    {
        return;
    }
    p_xcmp_msg->xcmp_opcode = htons(XCMP_DEVICE_INIT_STATUS_BRDCST);
    p_xcmp_msg->xcmp_ver[0] = 0x00;
    p_xcmp_msg->xcmp_ver[1] = 0x00;
    p_xcmp_msg->xcmp_ver[2] = 0x00;
    p_xcmp_msg->xcmp_ver[3] = m_XCMP_ver;  /* echo the xcmp version */
    p_xcmp_msg->dev_init_type = 0x00;
    p_xcmp_msg->dev_type = 0x0A;
    p_xcmp_msg->dev_status = 0x0000;
    p_xcmp_msg->array_size = 0x04;
    p_xcmp_msg->dev_descriptor_array[0] = 0x00;  /* XCMP_DEVICE_FAMILY */
    p_xcmp_msg->dev_descriptor_array[1] = 0x00;  
    p_xcmp_msg->dev_descriptor_array[2] = 0x02;  /* XCMP_DEVICE_DISPLAY */
    p_xcmp_msg->dev_descriptor_array[3] = 0xFF;
    
    init_xnl_header_of_xcmp_msg((char *)p_xcmp_msg, payload_len);
    /* Add the message to the sending queue */
    enqueue_msg((char *)p_xcmp_msg);

}

void CXNLConnection::init_xnl_header_of_xcmp_msg(char *p_msg, int payload_len)
{
    xnl_msg_hdr_t *p_xnl_hdr = (xnl_msg_hdr_t *)p_msg;
    
    if (p_msg != NULL)
    {
          p_xnl_hdr->msg_len = htons(sizeof(xnl_msg_hdr_t) + payload_len - 2);
          p_xnl_hdr->xnl_opcode = htons(XNL_DATA_MSG);
          p_xnl_hdr->protocol_id = 0x01; /* xcmp command message */
          p_xnl_hdr->dst_addr = htons(m_xnl_dst_addr);
          p_xnl_hdr->src_addr = htons(m_xnl_src_addr);
          p_xnl_hdr->payload_len = htons(payload_len);
          /* xnl_flag and trans_id will be set when it is sent out, here just set them to 0 */
          p_xnl_hdr->xnl_flag = 0;
          p_xnl_hdr->trans_id = 0;
    }
}

BOOL CXNLConnection::send_xcmp_brightness_msg(unsigned char function, unsigned char intensity)
{
    xcmp_brightness_request_t *p_msg = (xcmp_brightness_request_t *) malloc(sizeof(xcmp_brightness_request_t));
    int payload_len = sizeof(xcmp_brightness_request_t) - sizeof(xnl_msg_hdr_t);

    if (p_msg == NULL)
    {
        return (FALSE);
    }

    p_msg->xcmp_opcode = htons(XCMP_BRIGHTNESS_REQ);
    p_msg->function = function;
    p_msg->intensity = intensity;

    init_xnl_header_of_xcmp_msg((char *)p_msg, payload_len);
    /* Add the message to the sending queue */
    enqueue_msg((char *)p_msg);
    
    return (TRUE);
}

BOOL CXNLConnection::send_xcmp_pui_brdcst(unsigned char pui_type,
                                                 unsigned short pui_id,
                                                 char           pui_state,
                                                 char           pui_state_min,
                                                 char           pui_state_max)
{
    xcmp_pui_broadcast_t *p_msg = (xcmp_pui_broadcast_t *)malloc(sizeof(xcmp_pui_broadcast_t));

    if (p_msg == NULL)
    {
        return FALSE;
    }

    p_msg->xcmp_opcode = htons(XCMP_PHYSICAL_USER_INPUT_BRDCST);
    p_msg->pui_source = 0x00; /* Control Head / Control Device Input */
    p_msg->pui_type = pui_type;
    p_msg->pui_id = htons(pui_id);
    p_msg->pui_state = pui_state;
    p_msg->pui_state_min = pui_state_min;
    p_msg->pui_state_max = pui_state_max;

    init_xnl_header_of_xcmp_msg((char *)p_msg, 9);
    /* Add the message to the sending queue */
    enqueue_msg((char *)p_msg);
    
    return (TRUE);
}

BOOL CXNLConnection::send_xcmp_call_ctrl_request(unsigned char function,
                                                 unsigned char call_type,
                                                 unsigned char addr_type,
                                                 unsigned long rmt_addr,
                                                 unsigned long group_id)
{
#if DEBUG_LOG
	LOG(INFO) << "呼叫开始  ";
#endif
    xcmp_call_ctrl_request_t *p_msg = NULL;
    unsigned short            msg_size = sizeof(xcmp_call_ctrl_request_t);
    unsigned char *p_addr = (unsigned char *)&rmt_addr;
    unsigned char * p_cur_msg = NULL;
    xcmp_group_id_t tmp_group_id = {0};

    if (call_type == 0x06) /* if the call type is group call, then need to add group id structure in the msg */
    {
        msg_size += sizeof(xcmp_group_id_t); 
    }

    if (addr_type == 0x01) /* if the addr type is MOTOTRBO ID, then the addr size must be set to 3 */
    {
        msg_size += 3; /* 1 byte for addr structure and the other 2 bytes for address port */
    }
    else if (addr_type == 0x05) /* It's a MDC ID */
    {
        msg_size += 2; /* add 2 bytes for address port */
    }
    else 
    {
        /* Invalid address type */
        return FALSE;
    }

    p_msg = (xcmp_call_ctrl_request_t *)malloc(msg_size);

    if (p_msg == NULL)
    {
        return FALSE;
    }
    memset((char *)p_msg, 0, msg_size);

    p_msg->xcmp_opcode = htons(XCMP_CALL_CTRL_REQ);
    p_msg->function = function; /* Control Head / Control Device Input */
    p_msg->call_type = call_type;
    p_msg->rmt_addr.addr_type = addr_type;

    rmt_addr = htonl(rmt_addr);
    
    if (addr_type == 0x01) /* if the addr type is MOTOTRBO ID, then the addr size must be set to 3 */
    {
    
        p_msg->rmt_addr.addr_size = 3;
        /* since MOTOTRBO ID just has 3 bytes, so we shall copy the addr starting from the second byte */
        memcpy(p_msg->rmt_addr.rmt_addr, p_addr + 1, 3);
        p_cur_msg = (unsigned char *)p_msg + sizeof(xcmp_call_ctrl_request_t) + 3;
    }
    else /* it is a MDC ID, the size must be set to 2 */
    {
        p_msg->rmt_addr.addr_size = 2;
        memcpy(p_msg->rmt_addr.rmt_addr, p_addr + 2, 2);
        p_cur_msg = (unsigned char *)p_msg + sizeof(xcmp_call_ctrl_request_t) + 2;
    }

    if (call_type == 0x06) /* init the group id structure */
    {
        tmp_group_id.id_size = 3;
        /* Convert the byte order to Big Endian */
        group_id = htonl(group_id);
        /* since the group id just has 3 bytes, so we shall copy the id starting from the second byte */
        memcpy(tmp_group_id.group_id, (char *)&group_id + 1, 3);
        memcpy(p_cur_msg, (char *)&tmp_group_id, 4);
    }

	list<AllCommand>::iterator it;
	for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
	{
		/*if (it->seq == seq)
		{
			it->ackNum = p_msg->msg_hdr.trans_id;
		}*/
	}

    init_xnl_header_of_xcmp_msg((char *)p_msg, msg_size - sizeof(xnl_msg_hdr_t));
    /* Add the message to the sending queue */
    enqueue_msg((char *)p_msg);

    return (TRUE);
}

BOOL CXNLConnection::send_xcmp_rmt_radio_ctrl_request(unsigned char feature,
                                                 unsigned char operation,
                                                 unsigned char addr_type,
                                                 unsigned long rmt_addr)
{
	if (feature == 0x00)
	{
#if DEBUG_LOG
		LOG(INFO) << "打开ptt ";
#endif
	}
	else if (feature == 0x01)
	{
#if DEBUG_LOG
		LOG(INFO) << "打开ptt ";
#endif
	}
	else if (feature == 0x02)
	{
#if DEBUG_LOG
		LOG(INFO) << "遥开 ";
#endif
	}
	else if (feature == 0x03)
	{
#if DEBUG_LOG
		LOG(INFO) << "远程监听 ";
#endif
	}
    xcmp_rmt_radio_ctrl_request_t *p_msg = NULL;
    unsigned short            msg_size = sizeof(xcmp_rmt_radio_ctrl_request_t) + 3; /* the address size is 3, so need to plus 1 */
    unsigned char *p_addr = (unsigned char *)&rmt_addr;
    unsigned char * p_cur_msg = NULL;

    p_msg = (xcmp_rmt_radio_ctrl_request_t *)malloc(msg_size);

    if (p_msg == NULL)
    {
        return FALSE;
    }
    memset((char *)p_msg, 0, msg_size);

    p_msg->xcmp_opcode = htons(XCMP_RMT_RADIO_CTRL_REQ);
    p_msg->feature = feature;
    p_msg->operation = operation;
    p_msg->rmt_addr.addr_type = addr_type;
    p_msg->rmt_addr.addr_size = 3;

    rmt_addr = htonl(rmt_addr);
    
    if (addr_type == 0x01) /* if the addr type is MOTOTRBO ID, then the addr size must be set to 3 */
    {
    
        p_msg->rmt_addr.addr_size = 3;
        /* since MOTOTRBO ID just has 3 bytes, so we shall copy the addr starting from the second byte */
        memcpy(p_msg->rmt_addr.rmt_addr, p_addr + 1, 3);
    }

    init_xnl_header_of_xcmp_msg((char *)p_msg, msg_size - sizeof(xnl_msg_hdr_t));
    /* Add the message to the sending queue */
    enqueue_msg((char *)p_msg);


	list<AllCommand>::iterator it;
	for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
	{
		/*if (it->seq == seq)
		{
			it->ackNum = p_msg->msg_hdr.trans_id;
		}*/
	}
    
    return (TRUE);
}

BOOL CXNLConnection::send_xcmp_menu_navigation_request(unsigned char function,
                                                      unsigned char display_mode,
                                                      unsigned short menu_id,
                                                      unsigned short  count)
{
    xcmp_menu_navigation_request_t *p_msg = NULL;
    int payload_len = 0;
    

    p_msg = (xcmp_menu_navigation_request_t *)malloc(sizeof(xcmp_menu_navigation_request_t));

    if (p_msg == NULL)
    {
        return FALSE;
    }

    p_msg->xcmp_opcde = htons(XCMP_MENU_NAVIGATION_REQ);
    p_msg->function = function;
    p_msg->display_mode = display_mode;
    p_msg->menu_id = htons(menu_id);
    
    if (m_XCMP_ver == 1)
    {
       p_msg->data[0] = count;
       payload_len = sizeof(xcmp_menu_navigation_request_t) - sizeof(xnl_msg_hdr_t)- 1;
    }
    else
    {
        count = htons(count);
        memcpy(p_msg->data, (unsigned char *)&count, 2);
        payload_len = sizeof(xcmp_menu_navigation_request_t) - sizeof(xnl_msg_hdr_t);
    }

    init_xnl_header_of_xcmp_msg((char *)p_msg, payload_len);

    /* Add the message to the sending queue */
    enqueue_msg((char *)p_msg/*, TRUE*/);
    
    return (TRUE);       
}


BOOL CXNLConnection::send_xcmp_chan_zone_selection_request(unsigned char function,
                                                                        unsigned short zone_num,
                                                                        unsigned short chan_num)
{
    xcmp_chan_zone_selection_request_t *p_msg = (xcmp_chan_zone_selection_request_t *) malloc(sizeof(xcmp_chan_zone_selection_request_t));
    int payload_len = sizeof(xcmp_chan_zone_selection_request_t) - sizeof(xnl_msg_hdr_t) - 1;
    char * p_cur_ch = NULL;
 
    if (p_msg == NULL)
    {
        return (FALSE);
    }
 
    p_msg->xcmp_opcode = htons(XCMP_CHAN_SELECTION_REQ);
    p_msg->function = function;

    p_cur_ch = (char *)&p_msg->function;
    /* copy the zone number into the message */
    zone_num = htons(zone_num);
    memcpy(p_cur_ch + 1, (char *)&zone_num, sizeof(zone_num));

    /* copy the chan number into the message */
    chan_num = htons(chan_num);
    memcpy(p_cur_ch + 3, (char *)&chan_num, sizeof(chan_num));
 
    init_xnl_header_of_xcmp_msg((char *)p_msg, payload_len);
    /* Add the message to the sending queue */
    enqueue_msg((char *)p_msg);
    
    return (TRUE);

}

BOOL CXNLConnection::send_xcmp_tx_ctrl_request(unsigned char function, unsigned char mode)
{
	if (function == 0x01 && mode == 0x00)
	{
#if DEBUG_LOG
		LOG(INFO) << "打开ptt ";
#endif
	}
	else if (function == 0x02 && mode == 0x00)
	{
#if DEBUG_LOG
		LOG(INFO) << "呼叫停止 ";
#endif
	}
    xcmp_tx_ctrl_request_t *p_msg = (xcmp_tx_ctrl_request_t *)malloc(sizeof(xcmp_tx_ctrl_request_t));
    int payload_len = sizeof(xcmp_tx_ctrl_request_t) - sizeof(xnl_msg_hdr_t);

    if (p_msg == NULL)
    {
        return FALSE;
    }

    p_msg->xcmp_opcode = htons(XCMP_TX_CTRL_REQ);
    p_msg->function = function;
    p_msg->mode = mode;

	list<AllCommand>::iterator it;
	for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
	{
		/*if (it->seq == seq)
		{
			it->ackNum = p_msg->msg_hdr.trans_id;
		}*/
	}
    init_xnl_header_of_xcmp_msg((char *)p_msg, payload_len);
    /* Add the message to the sending queue */
    enqueue_msg((char *)p_msg);
    
    return (TRUE);

}

BOOL CXNLConnection::send_xcmp_radio_status_request(unsigned char condition)
{
    xcmp_radio_status_request_t *p_msg = (xcmp_radio_status_request_t *)malloc(sizeof(xcmp_radio_status_request_t));
    int payload_len = sizeof(xcmp_radio_status_request_t) - sizeof(xnl_msg_hdr_t) - 1;

    if (p_msg == NULL)
    {
        return FALSE;
    }
    
    p_msg->xcmp_opcode = htons(XCMP_RADIO_STATUS_REQ);
    p_msg->condition = condition;

    init_xnl_header_of_xcmp_msg((char *)p_msg, payload_len);
    /* Add the message to the sending queue */
    enqueue_msg((char *)p_msg);
    
    return (TRUE);
}

/* p_xcmp_data_msg - pointer to the received xcmp data message */
void CXNLConnection::send_xnl_data_msg_ack(char * p_xcmp_data_msg)
{
    xnl_data_msg_ack_t *p_msg_ack = (xnl_data_msg_ack_t *)malloc(sizeof(xnl_data_msg_ack_t));

    if (p_msg_ack == NULL)
    {
        return;
    }
    
    p_msg_ack->msg_hdr.msg_len = htons(XNL_DATA_MSG_ACK_SIZE - 2);
    p_msg_ack->msg_hdr.xnl_opcode = htons(XNL_DATA_MSG_ACK);
    p_msg_ack->msg_hdr.protocol_id = 0x01;  /* xcmp command message */
    p_msg_ack->msg_hdr.xnl_flag = ((xnl_msg_hdr_t *)p_xcmp_data_msg)->xnl_flag;
    p_msg_ack->msg_hdr.dst_addr = ((xnl_msg_hdr_t *)p_xcmp_data_msg)->src_addr;
    p_msg_ack->msg_hdr.src_addr = htons(m_xnl_src_addr);;
    p_msg_ack->msg_hdr.trans_id = ((xnl_msg_hdr_t *)p_xcmp_data_msg)->trans_id;
    p_msg_ack->msg_hdr.payload_len = 0;

    /* For ACK message, don't add it to the sending queue, but directly send out it */
    send_xnl_message((char *)p_msg_ack);
    free(p_msg_ack);

}

BOOL CXNLConnection::send_xnl_message(char *p_msg_buf)
{
    int len = 0;
    int n_left = 0;
    unsigned short msg_len = 0;
    char *p_cur_buf = p_msg_buf;
    XNL_NOTIFY_MSG *p_notify_msg = (XNL_NOTIFY_MSG *)malloc(sizeof(XNL_NOTIFY_MSG));
    char *p_raw_data = NULL;
    
    BOOL ret = TRUE;
    
    if (p_msg_buf == NULL)
    {
        return (ret);
    }
    /* The first two bytes is the xnl message's length field */
    msg_len = ntohs(*((unsigned short *)p_msg_buf)) + 2; /* 2 is the length of "length field". */
    n_left = msg_len;

    while (n_left > 0)
    {
        len = send(m_socket, p_msg_buf, msg_len, 0);
        if (len == SOCKET_ERROR)
        {
            ret = FALSE;
            break;
        }
        else if (len == 0)
        {
            ret = FALSE;
            break;
        }
        n_left -= len;
        p_cur_buf += len;
    }

    /* Send the message to main window for raw data display */
    if (msg_len == 0xef00)
    {
        msg_len = 0;
    }
    p_notify_msg->event = XNL_SENT_XCMP_MSG;
    /* copy the sending message */
    p_raw_data = (char *)malloc(msg_len);
    memcpy(p_raw_data, p_msg_buf, msg_len);
    p_notify_msg->p_msg = p_raw_data;
   // ::PostMessage(m_hWnd, WM_RX_XCMP_MESSAGE, NULL, (LPARAM)p_notify_msg);

    
    return (ret);
}

/* Add the message to the list tail. */
void CXNLConnection::enqueue_msg(char * p_msg)
{
    MSG_QUEUE_T *p_new_node = NULL;
    
    if (p_msg != NULL)
    {
        /* Create a node for the message */
        p_new_node = (MSG_QUEUE_T *)malloc(sizeof(MSG_QUEUE_T));

        if (p_new_node == NULL)
        {
            return;
        }

        p_new_node->p_msg = p_msg;
        p_new_node->next = NULL;
        
        WaitForSingleObject(m_hEvent, INFINITE);
        if (m_pSendQueHdr == NULL)
        {
            m_pSendQueHdr = p_new_node;
            m_pSendQueTail = p_new_node;
        }
        else /* Append the message to the list tail */
        {
            m_pSendQueTail->next = p_new_node;
            m_pSendQueTail = p_new_node;
        }
        SetEvent(m_hEvent);
    }

}

/* Remove the header from the list */
MSG_QUEUE_T * CXNLConnection::dequeue_msg()
{
    MSG_QUEUE_T *p_cur_node = NULL;
    
    WaitForSingleObject(m_hEvent, INFINITE);
    if (m_pSendQueHdr != NULL)
    {
        /* Remove the first node of the list */
        p_cur_node = m_pSendQueHdr;
        m_pSendQueHdr = m_pSendQueHdr->next;

        if (m_pSendQueHdr == NULL)
        {
            m_pSendQueTail = NULL;
        }
    }
    SetEvent(m_hEvent);

    return (p_cur_node);
}
void CXNLConnection::decode_xcmp_radio_status_reply(char *p_msg)
{
	
	if (p_msg != NULL)
	{
		xcmp_radio_status_reply_t *p_radio_status_reply = (xcmp_radio_status_reply_t *)p_msg;

		unsigned short xcmp_opcode = 0;
		unsigned char result = 0;
		xcmp_opcode = ntohs(*(unsigned short *)(p_msg + sizeof(xnl_msg_hdr_t)));
		result = ntohs(*(unsigned short *)(p_msg + sizeof(xnl_msg_hdr_t)+2));

		char mac[11];
		memcpy(mac, p_msg + sizeof(xnl_msg_hdr_t)+4, 10);
		mac[10] = '\0';

		if (xcmp_opcode == 0X800E && result == 0x08)
		{
			memcpy(readmac, mac, 11);
			
		}
	

	}
}
