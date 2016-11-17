
#include "../lib/radio/common.h"
#pragma once
#define SEND_LENTH   4
#define RECV_LENTH   28
#define RECV_THARGET_LENTH 19

typedef struct tagThreadARS
{
	SOCKET           mySocket;
	HWND	         hwnd;
	SOCKADDR_IN      remote_addr;                 // ���յ�Message�����Ե�IP Address
	int              ARSLength;                   // Ҫ���͵�Message�ĳ���
	char             RcvBuffer[RECV_LENTH];   // ���ջ��������ֽڼ��㣬ʵ�ʽ��յ���������Unicode
	char             SendBuffer[SEND_LENTH];
	unsigned long    radioID;
}ThreadARS;
class CRadioARS
{
public:
	CRadioARS();
	~CRadioARS();
	bool InitARSSocket(DWORD dwAddress/*,CRemotePeer * pRemote*/);
	bool CloseARSSocket(SOCKET* s);
	static DWORD WINAPI ReceiveDataThread(LPVOID lpParam);
	bool sendArsAck(DWORD dwRadioID, int CaiNet);
	void RecvData();
private:
	bool m_RcvSocketOpened;
	ThreadARS *m_ThreadARS;
	//CRemotePeer* pRemotePeer;
};

