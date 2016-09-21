//The MOTOTRBO ADP XCMP Console Demo Application software is considered Motorola Confidential Restricted information and 
//contains intellectual property owned by Motorola, Inc., which is protected by U.S. copyright laws and international 
//copyright treaties.  Unauthorized reproduction or distribution of this software is strictly prohibited.  2009 Motorola, 
//Inc. All Rights Reserved.

#ifndef _XNL_MSG_DEF_H
#define _XNL_MSG_DEF_H

/* XNL message size definition */
#define XNL_MSG_HDR_SIZE 14
#define XNL_AUTH_KEY_REQ_SIZE XNL_MSG_HDR_SIZE
#define XNL_CONNECTION_REQUEST_SIZE (XNL_MSG_HDR_SIZE + 12)
#define XNL_DATA_MSG_ACK_SIZE XNL_MSG_HDR_SIZE

/* XNL message opcode definition */
#define XNL_MASTER_STATUS_BROADCAST 0x0002
#define XNL_DEVICE_MASTER_QUERY     0X0003
#define XNL_AUTH_KEY_REQ            0x0004
#define XNL_AUTH_KEY_REPLY          0x0005
#define XNL_CONNECTION_REQUEST      0x0006
#define XNL_CONNECTION_REPLY        0x0007
#define XNL_DEVICE_SYSMAP_REQUEST   0x0008
#define XNL_SYSMAP_BRDCST           0x0009
#define XNL_DATA_MSG                0x000B
#define XNL_DATA_MSG_ACK            0x000C

/* constant definition */
#define XNL_SUCESS    0x01
#define XNL_FAILURE   0x00

/* Below are XNL messages definition */

/* XNL message common header definition */
typedef struct{
    unsigned short msg_len;
    unsigned short xnl_opcode;
    unsigned char protocol_id;
    char xnl_flag;
    unsigned short dst_addr;
    unsigned short src_addr;     
    unsigned short trans_id;
    unsigned short payload_len;
}xnl_msg_hdr_t;

typedef struct{
    xnl_msg_hdr_t msg_hdr;
    unsigned int xnl_ver;
    unsigned short master_logical_id;
    char is_data_msg_sent;

}xnl_master_status_broadcast_t;

typedef struct{
    xnl_msg_hdr_t msg_hdr;
}xnl_auth_key_request_t;

typedef struct{
    xnl_msg_hdr_t msg_hdr;
    unsigned short tmp_xnl_addr;
    char auth_seed[8];
}xnl_auth_key_reply_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned short preferred_xnl_addr;
    char dev_type;
    char auth_index;
    char auth_key[8];
}xnl_conn_request_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned char result_code;
    unsigned char trans_id_base;
    unsigned short assigned_xnl_addr;
    unsigned short logical_addr;
    unsigned char encrypted_seed[8];
}xnl_conn_reply_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned short size_of_sysmap;
    unsigned char sysmap_arry[1]; /* Place holder for the sysmap array */
}xnl_sysmap_brdcst_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;        
}xnl_data_msg_ack_t;



#endif /* _XNL_MSG_DEF_H */
