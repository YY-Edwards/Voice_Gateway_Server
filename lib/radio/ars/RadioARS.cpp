#include "stdafx.h"
#include "RadioARS.h"
#include "../DataScheduling.h"

#define ARS_PORT  4005
CRadioARS::CRadioARS(CDataScheduling *pMnis)
: m_RcvSocketOpened(false)
{
	m_ThreadARS = new ThreadARS;
	m_pMnis = pMnis;
}


CRadioARS::~CRadioARS()
{
	if (!m_ThreadARS)
	{
		delete m_ThreadARS;
	}
	
}
bool CRadioARS::InitARSSocket(DWORD dwAddress/*,CRemotePeer * pRemote*/)
{

	//pRemotePeer = pRemote;

	SOCKADDR_IN      addr;					//   The   local   interface   address   
	WSADATA			 wsda;					//   Structure   to   store   info
	
	int ret = WSAStartup(MAKEWORD(1, 1), &wsda);     //   Load   version   1.1   of   Winsock
	CloseARSSocket();
	BOOL bReuseaddr = FALSE;
	setsockopt(m_ThreadARS->mySocket, SOL_SOCKET, SO_DONTLINGER, (const char*)&bReuseaddr, sizeof(BOOL));
	
	m_ThreadARS->mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);   //   Create   an   UDP   socket
	
	if (m_ThreadARS->mySocket == SOCKET_ERROR)				//   Socket create Error
	{
		CloseARSSocket();
		return FALSE;
	}

	//   Fill   in   the   interface   information  

	addr.sin_family = AF_INET;
	addr.sin_port = htons(ARS_PORT);
	addr.sin_addr.s_addr = dwAddress;
	ret = ::bind(m_ThreadARS->mySocket, (struct sockaddr *) &addr, sizeof(addr));
	if (ret == SOCKET_ERROR)
	{
		int b = WSAGetLastError();
		//CloseARSSocket(s);
		return FALSE;
	}
	m_RcvSocketOpened = true;
	CreateThread(NULL,0, ReceiveDataThread, this, THREAD_PRIORITY_NORMAL, NULL);

	return TRUE;
}


bool CRadioARS::CloseARSSocket()
{
	if (m_RcvSocketOpened)        // 只有在前面已经打开了，才有必要关闭，否则没有必要了
	{
		closesocket(m_ThreadARS->mySocket);							        // Close socket

		WSACleanup();

		m_RcvSocketOpened = FALSE;
	}
	return TRUE;
}

bool CRadioARS::sendArsAck(DWORD dwRadioID, int CaiNet)
{
	memset(m_ThreadARS->SendBuffer, 0, sizeof(m_ThreadARS->SendBuffer));
	m_ThreadARS->SendBuffer[0] = 0x00;   //Immediate Location Request
	m_ThreadARS->SendBuffer[1] = 0x02;   //XML协议报中包含8组数据
	m_ThreadARS->SendBuffer[2] = 0xbf;   //Start of request-id element
	m_ThreadARS->SendBuffer[3] = 0x04;   //request-id value Start


	m_ThreadARS->ARSLength = SEND_LENTH;

	memset((void *)&m_ThreadARS->remote_addr, 0, sizeof(struct sockaddr_in));
	m_ThreadARS->remote_addr.sin_family = AF_INET;
	m_ThreadARS->remote_addr.sin_port = htons(4007);
	unsigned long radio_ip = ((CaiNet << 24) + dwRadioID) & 0xffffffff;
	m_ThreadARS->remote_addr.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)((radio_ip >> 24) & 0xff);
	m_ThreadARS->remote_addr.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)((radio_ip >> 16) & 0xff);
	m_ThreadARS->remote_addr.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)((radio_ip >> 8) & 0xff);
	m_ThreadARS->remote_addr.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)(radio_ip & 0xff);
	int bytesSend = sendto(m_ThreadARS->mySocket, (const char*)m_ThreadARS->SendBuffer, m_ThreadARS->ARSLength, 0, (struct sockaddr*)&m_ThreadARS->remote_addr, sizeof(m_ThreadARS->remote_addr));
	if (-1 == bytesSend)
	{
		int a = GetLastError();

		return false;
	}
	return true;
}
DWORD WINAPI CRadioARS::ReceiveDataThread(LPVOID lpParam)
{

	//CRadioARS * radioARS = new CRadioARS;
	CRadioARS * radioARS = (CRadioARS *)lpParam;
	while (1)
	{
		radioARS->RecvData();
		Sleep(10);
	
	}
	return 1;
}
void CRadioARS::RecvData()
{
	int   iRemoteAddrLen;            //   Contains   the   length   of   remte_addr,   passed   to   recvfrom   
	int   ret;
	int    iMessageLen;

	//Ready to receive data
	iRemoteAddrLen = sizeof(m_ThreadARS->remote_addr);
	iMessageLen = RECV_LENTH;
	ret = recvfrom(m_ThreadARS->mySocket, m_ThreadARS->RcvBuffer, iMessageLen, 0, (struct sockaddr*)&m_ThreadARS->remote_addr, &iRemoteAddrLen);

	if (ret == SOCKET_ERROR)
	{
		//strError.Format(_T("Error\nCall to recvfrom(s, Msg->RcvBuffer, iMessageLen, 0, (struct sockaddr *)&remote_addr, &iRemoteAddrLen); failed   with:\n%d\n"), WSAGetLastError());
		//break;
		memset(m_ThreadARS->RcvBuffer, 0, RECV_LENTH);
	}

	m_ThreadARS->radioID = (m_ThreadARS->remote_addr.sin_addr.S_un.S_un_b.s_b2 << 16) + (m_ThreadARS->remote_addr.sin_addr.S_un.S_un_b.s_b3 << 8) + m_ThreadARS->remote_addr.sin_addr.S_un.S_un_b.s_b4;
	unsigned short xcmp_opcode = 0;
	xcmp_opcode = ntohs(*((unsigned short *)(m_ThreadARS->RcvBuffer + 2)));
	unsigned char ars_code = 0;
	ars_code = ntohs(*((unsigned short *)(m_ThreadARS->RcvBuffer + 1)));
	char s[12];
	sprintf_s(s, "%d", m_ThreadARS->radioID);
	std::string stringId = s;
	if (xcmp_opcode == 0xf020 || xcmp_opcode == 0xf040)
	{
		//send ars ack  to radio

		memset(m_ThreadARS->SendBuffer, 0, sizeof(m_ThreadARS->SendBuffer));
		m_ThreadARS->SendBuffer[0] = 0x00;   //Immediate Location Request
		m_ThreadARS->SendBuffer[1] = 0x02;   //XML协议报中包含8组数据
		m_ThreadARS->SendBuffer[2] = 0xbf;   //Start of request-id element
		m_ThreadARS->SendBuffer[3] = 0x04;   //request-id value Start


		m_ThreadARS->ARSLength = SEND_LENTH;

		memset((void *)&m_ThreadARS->remote_addr, 0, sizeof(struct sockaddr_in));
		m_ThreadARS->remote_addr.sin_family = AF_INET;
		m_ThreadARS->remote_addr.sin_port = htons(4005);
		unsigned long radio_ip = ((12 << 24) + m_ThreadARS->radioID) & 0xffffffff;
		m_ThreadARS->remote_addr.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)((radio_ip >> 24) & 0xff);
		m_ThreadARS->remote_addr.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)((radio_ip >> 16) & 0xff);
		m_ThreadARS->remote_addr.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)((radio_ip >> 8) & 0xff);
		m_ThreadARS->remote_addr.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)(radio_ip & 0xff);
		int bytesSend = sendto(m_ThreadARS->mySocket, (const char*)m_ThreadARS->SendBuffer, m_ThreadARS->ARSLength, 0, (struct sockaddr*)&m_ThreadARS->remote_addr, sizeof(m_ThreadARS->remote_addr));
		if (-1 == bytesSend)
		{
			int a = GetLastError();
		}


#if DEBUG_LOG
		LOG(INFO) << "对讲机开机ars ondata ";
#endif
	//	if (peer != NULL)
		//{
			////查看状态，状态发生改变时，通知特Tserver
			//if (g_radioStatus.find(stringId) == g_radioStatus.end())
			//{
			//	RadioStatus st;
			//	st.id = m_ThreadARS->radioID;
			//	st.status = RADIO_STATUS_ONLINE;
			//	g_radioStatus[stringId] = st;
			//	Respone r;
			//	r.source = m_ThreadARS->radioID;
			//	r.arsStatus = SUCESS;
			//	onData(myCallBackFunc, peer, ++seq, RADIO_ARS, r);
			//}
			//else if (g_radioStatus[stringId].status == RADIO_STATUS_OFFLINE)
			//{
			//	g_radioStatus[stringId].status = RADIO_STATUS_ONLINE;
			//	Respone r;
			//	r.source = m_ThreadARS->radioID;
			//	r.arsStatus = SUCESS;
			//	onData(myCallBackFunc, peer, ++seq, RADIO_ARS, r);
			//	/*arg["IsOnline"] = FieldValue("True");
			//	std::string callJsonStr = CRpcJsonParser::buildCall("sendArs", seq, arg, "radio");
			//	peer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
			//	seq++;*/
			//}
			//}
			m_pMnis->updateOnLineRadioInfo(atoi(stringId.c_str()),RADIO_STATUS_ONLINE);
		}
		else if (ars_code == 0x31)
		{

#if DEBUG_LOG
			LOG(INFO) << "对讲机关机ars ondata ";
#endif
			//if (peer != NULL)
			//{
				//查看状态，状态发生改变时，通知特Tserver

				//if (g_radioStatus.find(stringId) == g_radioStatus.end())
				//{
				//	RadioStatus st;
				//	st.id = m_ThreadARS->radioID;
				//	st.status = RADIO_STATUS_OFFLINE;
				//	g_radioStatus[stringId] = st;
				//	Respone r;
				//	r.source = m_ThreadARS->radioID;
				//	r.arsStatus = UNSUCESS;
				//	onData(myCallBackFunc, peer, ++seq, RADIO_ARS, r);
				//}
				//else if (g_radioStatus[stringId].status == RADIO_STATUS_ONLINE)
				//{
				//	g_radioStatus[stringId].status = RADIO_STATUS_OFFLINE;

				//	Respone r;
				//	r.source = m_ThreadARS->radioID;
				//	r.arsStatus = UNSUCESS;
				//	onData(myCallBackFunc, peer, ++seq, RADIO_ARS, r);
				//}
				m_pMnis->updateOnLineRadioInfo(atoi(stringId.c_str()), RADIO_STATUS_OFFLINE);
		//}
	}
}

