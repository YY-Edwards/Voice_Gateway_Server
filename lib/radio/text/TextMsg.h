#pragma once
#ifdef MSG_DLL_API
#else
#define MSG_DLL_API  __declspec(dllexport)
#endif


#include "../common.h"

#define MAX_MESSAGE_LENGTH			(512 + 1)           // 最大可以接收1024个Unicode的字符，多一个字符位置是保存字符串结尾,目前 Text Message 的最大长度是 140 unicode.
#define MESSAGE_BUFFER              MAX_MESSAGE_LENGTH * 2
#define UINT8    unsigned   __int8
#define UINT16   unsigned   __int16
#define TEXTLENTH_1   5
#define TEXTLENTH_2   6

typedef struct tagThreadMsg
{
	SOCKET           mySocket;
	HWND	         hwnd;
	SOCKADDR_IN      remote_addr;                 // 接收到Message所来自的IP Address
	int              MsgLength;                   // 要发送的Message的长度
	char             RcvBuffer[MESSAGE_BUFFER];   // 接收缓冲区按字节计算，实际接收的内容则是Unicode
	char             SendBuffer[MESSAGE_BUFFER];
	unsigned long    radioID;
}ThreadMsg;


typedef struct tagFirstHeader
{
	UINT8    PDUType : 4;              // Types are associated with the control/user setting. For example, there may be a PDU Type 0 for control and a PDU Type 0 for user data (see 3.2 PDU Types for more information.  
	UINT8    Control : 1;              // If this bit is set, it indicates that this is a control message for the protocol. The protocol layer uses control messages to exchange information with its peer. If this bit is clear, this is user data (i.e. text message).
	UINT8    Reserved : 1;              // This bit is reserved for future use.
	UINT8    AckRequired : 1;              // The use of this bit depends upon the PDU itself. For the purposes of this bit, the PDU is considered an acknowledgement PDU or a non-acknowledgement PDU.
	UINT8    ExtensionBit : 1;              // If this bit is set, there are additional, optional header octet(s) that further define the message. 
}FirstHeader;

typedef struct tagSecondHeader
{
	UINT8    SequenceNum : 5;                // Remaining bits hold the sequence number for this message. This matches the originating message sequence number.
	UINT8    Reserved : 2;                // Reserved for future use.
	UINT8    Extension : 1;                // Set to 1 if size of Sequence Number field overflows.
}SecondHeader;

typedef struct tagThirdHeader
{
	UINT8    Reserved : 5;                // Reserved for future use
	UINT8    SequenceNum : 2;                // MSB of sequence number (if overflow needed).
	UINT8    Extension : 1;                // Clear to 0: no other optional headers to follow 
}ThirdHeader;

typedef struct tagTextMsg
{                                                  // 这个结构只包括了Message头中固定的部分，其他变化的部分都在TextPayload中，处理的时候需要注意
	unsigned short  MsgSize;                       // This indicates the number of octets to follow. The size DOES NOT include the two Message Size octets.
	FirstHeader     FstHeader;                     // This header includes basic information such as the packet data unit (PDU) type.
	UINT8           AddressSize;                   // This field indicates the size of the address field that follows in octets.
	char            TextPayload[MESSAGE_BUFFER];   // 接收缓冲区按字节计算，实际接收的内容则是Unicode
}TextMsg;

class MSG_DLL_API CTextMsg
{
public:
	CTextMsg();
	~CTextMsg();
	bool InitSocket(DWORD dwAddress/*, CRemotePeer * pRemote*/);
	bool CloseSocket(SOCKET* s);
	static DWORD WINAPI ReceiveDataThread(LPVOID lpParam);
	std::string ParseUserMsg(TextMsg* HandleMsg, int* len);
	UINT8 GetSeqNumber(TextMsg* HandleMsg);
	bool ReplyMsgACK(ThreadMsg* Msg, UINT8 SeqNumber);
	bool SendMsg( int callId, LPTSTR message, DWORD dwRadioID, int CaiNet);
	void setRemotePeer(CRemotePeer * pRemote);
	void RecvMsg();
	std::string WChar2Ansi(LPCWSTR pwszSrc);
	std::string TCHAR2STRING(TCHAR * STR);
private:
	int m_nSendSequenceNumber;
	bool m_RcvSocketOpened;
	ThreadMsg * m_ThreadMsg;
	CRemotePeer* pRemotePeer;
	
	
};

