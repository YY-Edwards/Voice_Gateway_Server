#pragma once
#ifdef MSG_DLL_API
#else
#define MSG_DLL_API  __declspec(dllexport)
#endif


#include "../common.h"

#define MAX_MESSAGE_LENGTH			(512 + 1)           // �����Խ���1024��Unicode���ַ�����һ���ַ�λ���Ǳ����ַ�����β,Ŀǰ Text Message ����󳤶��� 140 unicode.
#define MESSAGE_BUFFER              MAX_MESSAGE_LENGTH * 2
#define UINT8    unsigned   __int8
#define UINT16   unsigned   __int16
#define TEXTLENTH_1   5
#define TEXTLENTH_2   6

typedef struct tagThreadMsg
{
	SOCKET           mySocket;
	HWND	         hwnd;
	SOCKADDR_IN      remote_addr;                 // ���յ�Message�����Ե�IP Address
	int              MsgLength;                   // Ҫ���͵�Message�ĳ���
	char             RcvBuffer[MESSAGE_BUFFER];   // ���ջ��������ֽڼ��㣬ʵ�ʽ��յ���������Unicode
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
{                                                  // ����ṹֻ������Messageͷ�й̶��Ĳ��֣������仯�Ĳ��ֶ���TextPayload�У������ʱ����Ҫע��
	unsigned short  MsgSize;                       // This indicates the number of octets to follow. The size DOES NOT include the two Message Size octets.
	FirstHeader     FstHeader;                     // This header includes basic information such as the packet data unit (PDU) type.
	UINT8           AddressSize;                   // This field indicates the size of the address field that follows in octets.
	char            TextPayload[MESSAGE_BUFFER];   // ���ջ��������ֽڼ��㣬ʵ�ʽ��յ���������Unicode
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

