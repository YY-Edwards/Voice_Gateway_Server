//The MOTOTRBO ADP XCMP Console Demo Application software is considered Motorola Confidential Restricted information and 
//contains intellectual property owned by Motorola, Inc., which is protected by U.S. copyright laws and international 
//copyright treaties.  Unauthorized reproduction or distribution of this software is strictly prohibited.  2009 Motorola, 
//Inc. All Rights Reserved.

#ifndef _XCMP_MSG_DEFS_H
#define _XCMP_MSG_DEFS_H

#include "xnl_messages_def.h"

/* XCMP Opcode definition */
#define XCMP_REQUEST_OPCODE_MASK       0x0000
#define XCMP_REPLY_OPCODE_MASK         0x8000
#define XCMP_BRDCST_OPCODE_MASK        0xB000

#define XCMP_RADIO_STATUS              0x000E
#define XCMP_RADIO_STATUS_REQ          (XCMP_REQUEST_OPCODE_MASK | XCMP_RADIO_STATUS)
#define XCMP_RADIO_STATUS_REPLY	   (XCMP_REPLY_OPCODE_MASK | XCMP_RADIO_STATUS)

#define XCMP_DEVICE_INIT_STATUS        0x0400
#define XCMP_DEVICE_INIT_STATUS_BRDCST (XCMP_BRDCST_OPCODE_MASK | XCMP_DEVICE_INIT_STATUS)

#define XCMP_DISPLAY_TEXT           0x0401
#define XCMP_DISPLAY_TEXT_REQ       (XCMP_REQUEST_OPCODE_MASK | XCMP_DISPLAY_TEXT)
#define XCMP_DISPLAY_TEXT_REPLY	(XCMP_REPLY_OPCODE_MASK | XCMP_DISPLAY_TEXT)
#define XCMP_DISPLAY_TEXT_BRDCST       (XCMP_BRDCST_OPCODE_MASK | XCMP_DISPLAY_TEXT)

#define XCMP_INDICATOR_UPDATE          0x0402
#define XCMP_INDICATOR_UPDATE_BRDCST   (XCMP_BRDCST_OPCODE_MASK | XCMP_INDICATOR_UPDATE) 

#define XCMP_PHYSICAL_USER_INPUT        0x0405
#define XCMP_PHYSICAL_USER_INPUT_BRDCST (XCMP_BRDCST_OPCODE_MASK | XCMP_PHYSICAL_USER_INPUT)

#define XCMP_VOLUME_CTRL               0x0406
#define XCMP_VOLUME_CTRL_BRDCST        (XCMP_BRDCST_OPCODE_MASK | XCMP_VOLUME_CTRL)

#define XCMP_SPKR_CTRL                 0x0407
#define XCMP_SPKR_CTRL_BRDCST          (XCMP_BRDCST_OPCODE_MASK  | XCMP_SPKR_CTRL)

#define XCMP_MIC_CTRL                  0x040E
#define XCMP_MIC_CTRL_BRDCST           (XCMP_BRDCST_OPCODE_MASK  | XCMP_MIC_CTRL)


#define XCMP_BRIGHTNESS                0x0411
#define XCMP_BRIGHTNESS_REQ            (XCMP_REQUEST_OPCODE_MASK | XCMP_BRIGHTNESS)
#define XCMP_BRIGHTNESS_REPLY          (XCMP_REPLY_OPCODE_MASK   | XCMP_BRIGHTNESS)
#define XCMP_BRIGHTNESS_BRDCST         (XCMP_BRDCST_OPCODE_MASK  | XCMP_BRIGHTNESS)

#define XCMP_EMER_CTRL                  0x0413
#define XCMP_EMER_CTRL_BRDCST           (XCMP_BRDCST_OPCODE_MASK  | XCMP_EMER_CTRL)

#define XCMP_TX_CTRL                    0x0415
#define XCMP_TX_CTRL_REQ                (XCMP_REQUEST_OPCODE_MASK | XCMP_TX_CTRL)
#define XCMP_TX_CTRL_REPLY              (XCMP_REPLY_OPCODE_MASK   | XCMP_TX_CTRL)
#define XCMP_TX_CTRL_BRDCST             (XCMP_BRDCST_OPCODE_MASK  | XCMP_TX_CTRL)

#define XCMP_CALL_CTRL                  0x041E
#define XCMP_CALL_CTRL_REQ              (XCMP_REQUEST_OPCODE_MASK | XCMP_CALL_CTRL)
#define XCMP_CALL_CTRL_REPLY            (XCMP_REPLY_OPCODE_MASK   | XCMP_CALL_CTRL)
#define XCMP_CALL_CTRL_BRDCST           (XCMP_BRDCST_OPCODE_MASK  | XCMP_CALL_CTRL)

#define XCMP_RMT_RADIO_CTRL             0x041C
#define XCMP_RMT_RADIO_CTRL_REQ         (XCMP_REQUEST_OPCODE_MASK | XCMP_RMT_RADIO_CTRL)
#define XCMP_RMT_RADIO_CTRL_REPLY       (XCMP_REPLY_OPCODE_MASK   | XCMP_RMT_RADIO_CTRL)
#define XCMP_RMT_RADIO_CTRL_BRDCST      (XCMP_BRDCST_OPCODE_MASK  | XCMP_RMT_RADIO_CTRL)

#define XCMP_MENU_NAVIGATION            0x041F
#define XCMP_MENU_NAVIGATION_REQ        (XCMP_REQUEST_OPCODE_MASK | XCMP_MENU_NAVIGATION)
#define XCMP_MENU_NAVIGATION_REPLY      (XCMP_REPLY_OPCODE_MASK   | XCMP_MENU_NAVIGATION)
#define XCMP_MENU_NAVIGATION_BRDCST     (XCMP_BRDCST_OPCODE_MASK  | XCMP_MENU_NAVIGATION)

#define XCMP_CHAN_SELECTION            0x040D
#define XCMP_CHAN_SELECTION_REQ        (XCMP_REQUEST_OPCODE_MASK | XCMP_CHAN_SELECTION)
#define XCMP_CHAN_SELECTION_REPLY      (XCMP_REPLY_OPCODE_MASK   | XCMP_CHAN_SELECTION)
#define XCMP_CHAN_SELECTION_BRDCST     (XCMP_BRDCST_OPCODE_MASK  | XCMP_CHAN_SELECTION)


typedef enum 
{
    XCMP_DEVICE_FAMILY = 0x00,
    XCMP_DEVICE_DISPLAY = 0x02,
    XCMP_DEVICE_RF_BAND = 0x04,
    XCMP_DEVICE_GPIO_CTRL = 0x05,
    XCMP_DEVICE_RADIO_TYPE = 0x07,
    XCMP_DEVICE_KEYPAD = 0x09,
    XCMP_DEVICE_CHANNEL_KNOB = 0x0D,
    XCMP_DEVICE_VIRTUAL_PERSONALITY_SUPPORT = 0x0E,
} xcmpDeviceType_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char xcmp_ver[4];
    unsigned char dev_init_type;
    unsigned char dev_type;
    unsigned short dev_status;
    unsigned char array_size;
    unsigned char dev_descriptor_array[1]; /* Place holder for the descriptor array */       
}xcmp_dev_init_status_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  function;
    unsigned char  intensity;
}xcmp_brightness_request_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  pui_source;
    unsigned char  pui_type;
    unsigned short pui_id;
    char           pui_state;
    char           pui_state_min;
    char           pui_state_max;
}xcmp_pui_broadcast_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  display_region;
    unsigned char  encoding_type;
    unsigned short num_text;
    char           string[1];
}xcmp_display_text_broadcast_t;

typedef struct{
    unsigned char addr_type;
    unsigned char addr_size;
    unsigned char rmt_addr[1]; /* The length of address is variable, so this is just a place holder */
}xcmp_remote_addr_t;

typedef struct {
    unsigned char id_size;
    unsigned char group_id[3]; /* For MOTOTRBO, the id_size must be set to 3 */
}xcmp_group_id_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  function;
    unsigned char  call_type;
    xcmp_remote_addr_t rmt_addr;    
}xcmp_call_ctrl_request_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  call_type;
    unsigned char  call_state;
    xcmp_remote_addr_t rmt_addr;
}xcmp_call_ctrl_broadcast_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  feature;
    unsigned char  operation;
    xcmp_remote_addr_t rmt_addr;
}xcmp_rmt_radio_ctrl_request_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned short xcmp_opcde;
    unsigned char function;
    unsigned char display_mode;
    unsigned short menu_id;
    unsigned char data[2];        /* The size of "Count" field is depended on XCMP version */
}xcmp_menu_navigation_request_t;     

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    unsigned short xcmp_opcde;
    unsigned char result;
    unsigned char function;
    unsigned char display_mode;
    unsigned char data[1];       /* place holder for rest message data */
}xcmp_menu_navigation_reply_t;       

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  function;
    unsigned short zone_num;
    unsigned short channel_num;
}xcmp_chan_zone_selection_request_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    unsigned short xcmp_opcode;
    unsigned short zone_num;
    unsigned short channel_num;
}xcmp_chan_zone_selection_broadcast_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  function;
    unsigned char  mode;
}xcmp_tx_ctrl_request_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  result;
    unsigned char  function;
    unsigned char  mode;
    unsigned char  state;
}xcmp_tx_ctrl_reply_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  condition;
}xcmp_radio_status_request_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  result;
    unsigned char  condition;
    char           status[1];
}xcmp_radio_status_reply_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    unsigned short xcmp_opcode;
    unsigned char  status;
    xcmp_remote_addr_t rmt_addr;
}xcmp_emergency_ctrl_broadcast_t;

#endif /* _XCMP_MSG_DEFS_H */
