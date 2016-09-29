#include "stdafx.h"

bool g_dongle_open = false;
BOOL g_net_connect = false;
unsigned long g_callId = 0;
unsigned long g_localGroup = 3;
unsigned long g_localRadioId = 5;
unsigned long g_localPeerId = 120;
unsigned char g_callType = 0x4f;
DECLINE_REASON_CODE_INFO g_callRequstDeclineReasonCodeInfo = { 0 };
_RECORD_TYPE_VALUE g_recordType = CPC;