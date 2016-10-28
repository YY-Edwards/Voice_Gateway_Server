#include "stdafx.h"
#include "RadioGps.h"

#pragma comment(lib, "wsock32.lib")

CRadioGps::CRadioGps()
{
	m_RcvSocketOpened = false;
	m_ThreadGps = new ThreadGPS;
	m_ThreadGpsOverturn = new ThreadGPSOverturn;
}


CRadioGps::~CRadioGps()
{
	if (!m_ThreadGps)
	{
		delete m_ThreadGps;
	}
	if (!m_ThreadGpsOverturn)
	{
		delete m_ThreadGpsOverturn;
	}
}
bool CRadioGps::InitGPSSocket(DWORD dwAddress,  CRemotePeer * pRemote)
{
	pRemotePeer = pRemote;
	//CString			 strError;
	SOCKADDR_IN      addr;					//   The   local   interface   address   
	WSADATA			 wsda;					//   Structure   to   store   info
	int ret = WSAStartup(MAKEWORD(1, 1), &wsda);     //   Load   version   1.1   of   Winsock

	CloseGPSSocket(&m_ThreadGps->mySocket);
	BOOL bReuseaddr = FALSE;
	setsockopt(m_ThreadGps->mySocket, SOL_SOCKET, SO_DONTLINGER, (const char*)&bReuseaddr, sizeof(BOOL));

	m_ThreadGps->mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);   //   Create   an   UDP   socket
	

	if (m_ThreadGps->mySocket == SOCKET_ERROR)				//   Socket create Error
	{
		//AfxMessageBox(_T("Socket初始化错误！"));
		CloseGPSSocket(&m_ThreadGps->mySocket);

		return FALSE;
	}

	//   Fill   in   the   interface   information  


	addr.sin_family = AF_INET;
	addr.sin_port = htons(4001);
	addr.sin_addr.s_addr = dwAddress;
	ret = ::bind(m_ThreadGps->mySocket, (struct sockaddr *) &addr, sizeof(addr));
	if (ret == SOCKET_ERROR)
	{
		int b = WSAGetLastError();
		//CloseGPSSocket(s);
		return FALSE;
	}
	m_RcvSocketOpened = true;
	CreateThread(NULL,0, ReceiveDataThread, this, THREAD_PRIORITY_NORMAL, NULL);

	return TRUE;
}
bool CRadioGps::InitGPSOverturnSocket(DWORD dwAddress)
{

	SOCKADDR_IN      addr;					//   The   local   interface   address   
	WSADATA			 wsda;					//   Structure   to   store   info
	int ret = WSAStartup(MAKEWORD(1, 1), &wsda);     //   Load   version   1.1   of   Winsock


	m_ThreadGpsOverturn->mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);   //   Create   an   UDP   socket


	if (m_ThreadGpsOverturn->mySocket == SOCKET_ERROR)				//   Socket create Error
	{
		//AfxMessageBox(_T("Socket初始化错误！"));
		CloseGPSSocket(&m_ThreadGpsOverturn->mySocket);

		return FALSE;
	}

	//   Fill   in   the   interface   information  


	addr.sin_family = AF_INET;
	addr.sin_port = htons(4001);
	addr.sin_addr.s_addr = dwAddress;
	ret = ::bind(m_ThreadGpsOverturn->mySocket, (struct sockaddr *) &addr, sizeof(addr));
	if (ret == SOCKET_ERROR)
	{
		int b = WSAGetLastError();
		//CloseGPSSocket(s);
		return FALSE;
	}
	m_RcvSocketOpened = true;
	return TRUE;
}

bool CRadioGps::CloseGPSSocket(SOCKET* s)
{
	if (m_RcvSocketOpened)        // 只有在前面已经打开了，才有必要关闭，否则没有必要了
	{
		closesocket(*s);							        // Close socket

		WSACleanup();

		m_RcvSocketOpened = FALSE;
	}
	return TRUE;
}

bool CRadioGps::SendQueryGPS( DWORD dwRadioID,int queryMode,int cycle)
{

	int gpsMode = queryMode;
	memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
	switch (queryMode)
	{
	case GPS_IMME_COMM:
	{
		memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
		m_ThreadGps->SendBuffer[0] = Immediate_Location_Request;
		m_ThreadGps->SendBuffer[1] = Immediate_Location_Request_Tokens_Length;  
		m_ThreadGps->SendBuffer[2] = Location_RequestID_Start;
		m_ThreadGps->SendBuffer[3] = 0x04;   
		m_ThreadGps->SendBuffer[4] = 0x12;
		m_ThreadGps->SendBuffer[5] = 0x34;
		m_ThreadGps->SendBuffer[6] = 0x56;
		m_ThreadGps->SendBuffer[7] = 0x78;  
		m_ThreadGps->SendBuffer[8] = Immediate_Location_Request_Time;   
		m_ThreadGps->SendBuffer[9] = Immediate_Location_RequestID_Speed;   
		//m_ThreadGps->SendBuffer[10] = 0x55;  //Specifies that altitude information is required
		//m_ThreadGps->SendBuffer[11] = 0x57;  //Specifies that horizontal direction information is requested
		m_ThreadGps->gpsLength = SEND_IMM_QUERY_LENTH;
	}
		break;
	case GPS_TRIGG_COMM:
	{
		memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
		m_ThreadGps->SendBuffer[0] = Triggered_Location_Request;
		m_ThreadGps->SendBuffer[1] = Triggered_Location_Request_Tokens_Length;
		m_ThreadGps->SendBuffer[2] = Location_RequestID_Start;
		m_ThreadGps->SendBuffer[3] = 0x04;
		m_ThreadGps->SendBuffer[4] = 0x24;
		m_ThreadGps->SendBuffer[5] = 0x68;
		m_ThreadGps->SendBuffer[6] = 0xAC;
		m_ThreadGps->SendBuffer[7] = 0xE0;
		m_ThreadGps->SendBuffer[8] = Start_Trigger_Element;
		m_ThreadGps->SendBuffer[9] = Start_Interval_Element_uint;
		m_ThreadGps->SendBuffer[10] = 0xff&cycle;
		//m_ThreadGps->SendBuffer[11] = 0x54;  //Specifies that altitude information is required
		//m_ThreadGps->SendBuffer[12] = 0x57;  //Specifies that horizontal direction information is requested
		m_ThreadGps->gpsLength = SEND_TRG_QUERY_LENTH;
	}
		break;
	case GPS_IMME_CSBK:
   {
		memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
		memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
		m_ThreadGps->SendBuffer[0] = Immediate_Location_Request;   
		m_ThreadGps->SendBuffer[1] = Immediate_Location_Request_Tokens_Length;  
		m_ThreadGps->SendBuffer[2] = Location_RequestID_Start;   
		m_ThreadGps->SendBuffer[3] = 0x03;  
		m_ThreadGps->SendBuffer[4] = 0x00;
		m_ThreadGps->SendBuffer[5] = 0x00;
		m_ThreadGps->SendBuffer[6] = 0x01;
		m_ThreadGps->SendBuffer[7] = CSBK_Start_Require_Data;   
		m_ThreadGps->SendBuffer[8] = CSBK_Require_Data_Length;  
		m_ThreadGps->SendBuffer[9] = Request_LRRP_CSBK;   
		m_ThreadGps->gpsLength = SEND_IMM_CSBK_QUERY_LENTH;
	}
		break;
	case GPS_TRIGG_CSBK:
	{
		memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
		m_ThreadGps->SendBuffer[0] = Triggered_Location_Request;
		m_ThreadGps->SendBuffer[2] = Location_RequestID_Start;
		m_ThreadGps->SendBuffer[3] = 0x03;
		m_ThreadGps->SendBuffer[4] = 0x00;
		m_ThreadGps->SendBuffer[5] = 0x00;
		m_ThreadGps->SendBuffer[6] = 0x01;
		m_ThreadGps->SendBuffer[7] = CSBK_Start_Require_Data;
		m_ThreadGps->SendBuffer[8] = CSBK_Require_Data_Length;
		m_ThreadGps->SendBuffer[9] = Request_LRRP_CSBK;
		m_ThreadGps->SendBuffer[10] = Start_Trigger_Element;
		if (cycle == 7)
		{
			m_ThreadGps->SendBuffer[1] = CSBK_Triggered_Location_Request_Tokens_Length_ufloat;
			m_ThreadGps->SendBuffer[11] = Start_Interval_Element_ufloat;
			m_ThreadGps->SendBuffer[12] = 0x07;
			m_ThreadGps->SendBuffer[13] = 0x40;            //7.5s
			m_ThreadGps->gpsLength = SEND_TRG_CSBK_QUERY_LENTH;
		}
		else
		{
			m_ThreadGps->SendBuffer[1] = CSBK_Triggered_Location_Request_Tokens_Length_uint;
			m_ThreadGps->SendBuffer[11] = Start_Interval_Element_uint;
			m_ThreadGps->SendBuffer[12] = 0xff&cycle;
			m_ThreadGps->gpsLength = SEND_TRG_CSBK_QUERY_LENTH-1;
		}
		
	}
		break;
	case GPS_IMME_CSBK_EGPS:
	{
		memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
		m_ThreadGps->SendBuffer[0] = Immediate_Location_Request;
		m_ThreadGps->SendBuffer[1] = Immediate_Location_Request_Tokens_Length;
		m_ThreadGps->SendBuffer[2] = Location_RequestID_Start;
		m_ThreadGps->SendBuffer[3] = 0x03;
		m_ThreadGps->SendBuffer[4] = 0x00;
		m_ThreadGps->SendBuffer[5] = 0x00;
		m_ThreadGps->SendBuffer[6] = 0x01;
		m_ThreadGps->SendBuffer[7] = CSBK_Location_Request_Time;
		m_ThreadGps->SendBuffer[8] = CSBK_Require_Speed_Horizontal;
		m_ThreadGps->SendBuffer[9] = CSBK_Require_Direction_Horizontal;
		m_ThreadGps->SendBuffer[10] = CSBK_Start_Require_Data;
		m_ThreadGps->SendBuffer[11] = CSBK_Require_Data_Length;
		m_ThreadGps->SendBuffer[12] = Request_LRRP_CSBK;
		m_ThreadGps->gpsLength = SEND_IMM_CSBK_EGPD_QUERY_LENTH;
	}
		break;
	case GPS_TRIGG_CSBK_EGPS:
	{
			memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
			m_ThreadGps->SendBuffer[0] = Triggered_Location_Request;
			m_ThreadGps->SendBuffer[2] = Location_RequestID_Start;
			m_ThreadGps->SendBuffer[3] = 0x03;
			m_ThreadGps->SendBuffer[4] = 0x00;
			m_ThreadGps->SendBuffer[5] = 0x00;
			m_ThreadGps->SendBuffer[6] = 0x01;
			m_ThreadGps->SendBuffer[7] = CSBK_Location_Request_Time;
			m_ThreadGps->SendBuffer[8] = CSBK_Require_Speed_Horizontal;
			m_ThreadGps->SendBuffer[9] = CSBK_Require_Direction_Horizontal;
			m_ThreadGps->SendBuffer[10] = CSBK_Start_Require_Data;
			m_ThreadGps->SendBuffer[11] = CSBK_Require_Data_Length;
			m_ThreadGps->SendBuffer[12] = Request_LRRP_CSBK;
			m_ThreadGps->SendBuffer[13] = Start_Trigger_Element;
			if (cycle == 7)
			{
				m_ThreadGps->SendBuffer[1] = 0x0f;
				m_ThreadGps->SendBuffer[14] = Start_Interval_Element_ufloat;
				m_ThreadGps->SendBuffer[15] = 0x07;
				m_ThreadGps->SendBuffer[16] = 0x40;            //7.5s
				m_ThreadGps->gpsLength = SEND_TRG_CSBK_EGPD_QUERY_LENTH + 1;
			}
			else if (cycle == 480)
			{
				m_ThreadGps->SendBuffer[1] = 0x0f;
				m_ThreadGps->SendBuffer[14] = Start_Interval_Element_uint;
				m_ThreadGps->SendBuffer[15] = 0x01;
				m_ThreadGps->SendBuffer[16] = 0xE0;            //480s
				m_ThreadGps->gpsLength = SEND_TRG_CSBK_EGPD_QUERY_LENTH + 1;
			}
			else
			{
				m_ThreadGps->SendBuffer[1] = 0x0e;
				m_ThreadGps->SendBuffer[14] = Start_Interval_Element_uint;
				m_ThreadGps->SendBuffer[15] = 0xff & cycle;
				m_ThreadGps->gpsLength = SEND_TRG_CSBK_EGPD_QUERY_LENTH;
			}
			
	}
		break;
	default:
		break;
	}
	
	memset((void *)&m_ThreadGps->remote_addr, 0, sizeof(struct sockaddr_in));
	m_ThreadGps->remote_addr.sin_family = AF_INET;
	m_ThreadGps->remote_addr.sin_port = htons(4001);
	unsigned long radio_ip = ((12 << 24) + dwRadioID) & 0xffffffff;
	m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)((radio_ip >> 24) & 0xff);
	m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)((radio_ip >> 16) & 0xff);
	m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)((radio_ip >> 8) & 0xff);
	m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)(radio_ip & 0xff);
	int bytesSend = sendto(m_ThreadGps->mySocket, (const char*)m_ThreadGps->SendBuffer, m_ThreadGps->gpsLength, 0, (struct sockaddr*)&m_ThreadGps->remote_addr, sizeof(m_ThreadGps->remote_addr));
	if (-1 == bytesSend)
	{
		int a = GetLastError();
		/*CString str;
		str += "result:0";
		unsigned char* data = (unsigned char*)(LPCTSTR)str;*/
		/*if (myCallBackFunc != NULL)
		{
			unsigned char str[30] = { 0 };
			sprintf_s((char *)str, sizeof(str), "id:%d;result:0", dwRadioID);
			onData(myCallBackFunc, 1, gpsMode, (char *)str, 1);
			return false;
		}*/
		
	}
	/*if (myCallBackFunc!=NULL)
	{
		unsigned char str[30] = { 0 };
		sprintf_s((char *)str, sizeof(str), "id:%d;result:1", dwRadioID);
		onData(myCallBackFunc, 1, gpsMode, (char *)str, sizeof(str));
	}*/
	return true;
}
bool CRadioGps::StopQueryTriggeredGPS(DWORD dwRadioID, int	queryMode)
{
	if (queryMode == GPS_TRIGG_COMM)
	{
		memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
		m_ThreadGps->SendBuffer[0] = 0x0F;   //Immediate Location Request
		m_ThreadGps->SendBuffer[1] = 0x06;   //XML协议报中包含8组数据
		m_ThreadGps->SendBuffer[2] = 0x22;   //Start of request-id element
		m_ThreadGps->SendBuffer[3] = 0x04;
		m_ThreadGps->SendBuffer[4] = 0x24;
		m_ThreadGps->SendBuffer[5] = 0x68;
		m_ThreadGps->SendBuffer[6] = 0xAC;
		m_ThreadGps->SendBuffer[7] = 0xE0;

		m_ThreadGps->gpsLength = SEND_STOP_LENTH;
	}
	else if (queryMode == GPS_TRIGG_CSBK || queryMode == GPS_TRIGG_CSBK_EGPS)
	{
		memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
		m_ThreadGps->SendBuffer[0] = 0x0F;   //Immediate Location Request
		m_ThreadGps->SendBuffer[1] = 0x05;   //XML协议报中包含8组数据
		m_ThreadGps->SendBuffer[2] = 0x22;   //Start of request-id element
		m_ThreadGps->SendBuffer[3] = 0x03;
		m_ThreadGps->SendBuffer[4] = 0x00;
		m_ThreadGps->SendBuffer[5] = 0x00;
		m_ThreadGps->SendBuffer[6] = 0x01;
		m_ThreadGps->gpsLength = SEND_STOP_LENTH -1;
	}
	
	memset((void *)&m_ThreadGps->remote_addr, 0, sizeof(struct sockaddr_in));
	m_ThreadGps->remote_addr.sin_family = AF_INET;
	m_ThreadGps->remote_addr.sin_port = htons(4001);
	unsigned long radio_ip = ((12 << 24) + dwRadioID) & 0xffffffff;
	m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)((radio_ip >> 24) & 0xff);
	m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)((radio_ip >> 16) & 0xff);
	m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)((radio_ip >> 8) & 0xff);
	m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)(radio_ip & 0xff);
	int bytesSend = sendto(m_ThreadGps->mySocket, (const char*)m_ThreadGps->SendBuffer, m_ThreadGps->gpsLength, 0, (struct sockaddr*)&m_ThreadGps->remote_addr, sizeof(m_ThreadGps->remote_addr));
	if (-1 == bytesSend)
	{
		int a = GetLastError();
	/*	if (myCallBackFunc != NULL)
		{
			unsigned char str[30] = { 0 };
			sprintf_s((char *)str, sizeof(str), "id:%d;result:0", dwRadioID);
			onData(myCallBackFunc, 1, STOP_QUERY_GPS, (char *)str, sizeof(str));
		}
		return false;*/
	}
	/*if (myCallBackFunc != NULL)
	{
		unsigned char str[30] = { 0 };
		sprintf_s((char *)str, sizeof(str), "id:%d;result:1", dwRadioID);
		onData(myCallBackFunc, 1, STOP_QUERY_GPS, (char *)str, sizeof(str));
	}*/
	return true;
}
DWORD WINAPI CRadioGps::ReceiveDataThread(LPVOID lpParam)
{
	//CRadioGps * radioGps  = new CRadioGps;
	//radioGps = (CRadioGps *)lpParam;
	CRadioGps * radioGps = (CRadioGps *)lpParam;

	while (1)
	{
		radioGps->RecvData();
		Sleep(100);
	}
	
}
void CRadioGps::RecvData()
{
	int   iRemoteAddrLen;            //   Contains   the   length   of   remte_addr,   passed   to   recvfrom   
	int   ret = 0 , bytes=0;
	int    iMessageLen;
	double lat = 0, lon = 0;
	iRemoteAddrLen = sizeof(m_ThreadGps->remote_addr);
	iMessageLen = MAX_RECV_LENGTH;
	const int immLocRepLenth = 28;   //Immediate Location Report报文长度
	const int trgLocReqLenth = 17;    //triggere Location Report 报文长度
	const int csbkLocReqLenth = 19;
	unsigned long a = 0, b = 0;
	float speed = -1;
	int valid = 1;

	ret = recvfrom(m_ThreadGps->mySocket, m_ThreadGps->RcvBuffer, iMessageLen, 0, (struct sockaddr*)&m_ThreadGps->remote_addr, &iRemoteAddrLen);

	bytes = recvfrom(m_ThreadGpsOverturn->mySocket, m_ThreadGpsOverturn->RcvBuffer, iMessageLen, 0, (struct sockaddr*)&m_ThreadGpsOverturn->remote_addr, &iRemoteAddrLen);

	if (ret != SOCKET_ERROR || bytes != SOCKET_ERROR)
	{
		m_ThreadGps->radioID = (m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b2 << 16) + (m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b3 << 8) + m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b4;
		if ((ret == RECV_IMME_LENTH || bytes == RECV_IMME_LENTH) && m_ThreadGps->RcvBuffer[0] == Immediate_Location_Report && m_ThreadGps->RcvBuffer[1] == 0x1a)
		{
			unsigned long llat = 0, llon = 0;
			unsigned long a = 0, b = 0, c = 0, d = 0;
			//解析纬度
			a = ((unsigned long)m_ThreadGps->RcvBuffer[15]) & 0xff;
			b = ((unsigned long)m_ThreadGps->RcvBuffer[16]) & 0xff;
			c = ((unsigned long)m_ThreadGps->RcvBuffer[17]) & 0xff;
			d = ((unsigned long)m_ThreadGps->RcvBuffer[18]) & 0xff;
			llat = ((a << 24) | (b << 16) | (c << 8) | d) & 0xffffffff;
			//解析经度
			a = b = c = d = 0;
			a = ((unsigned long)m_ThreadGps->RcvBuffer[19]) & 0xff;
			b = ((unsigned long)m_ThreadGps->RcvBuffer[20]) & 0xff;
			c = ((unsigned long)m_ThreadGps->RcvBuffer[21]) & 0xff;
			d = ((unsigned long)m_ThreadGps->RcvBuffer[22]) & 0xff;
			llon = ((a << 24) | (b << 16) | (c << 8) | d) & 0xffffffff;

			lat = ((double)llat) / 2147483648 * 90;
			lon = ((double)llon) / 2147483648 * 180;

			a = ((unsigned long)m_ThreadGps->RcvBuffer[26]) & 0xff;
			b = ((unsigned long)m_ThreadGps->RcvBuffer[27]) & 0xff;
			speed = (((float)a) + ((float)b) / 128.0f)*3.6f;
		}
		else if ((ret == RECV_TRG_LENTH || bytes == RECV_TRG_LENTH) && m_ThreadGps->RcvBuffer[0] == Triggered_Location_Report && m_ThreadGps->RcvBuffer[1] == 0x0F)
		{
			unsigned long llat = 0, llon = 0;
			unsigned long a = 0, b = 0, c = 0, d = 0;
			//解析纬度
			a = ((unsigned long)m_ThreadGps->RcvBuffer[9]) & 0xff;
			b = ((unsigned long)m_ThreadGps->RcvBuffer[10]) & 0xff;
			c = ((unsigned long)m_ThreadGps->RcvBuffer[11]) & 0xff;
			d = ((unsigned long)m_ThreadGps->RcvBuffer[12]) & 0xff;
			llat = ((a << 24) | (b << 16) | (c << 8) | d) & 0xffffffff;
			//解析经度
			a = b = c = d = 0;
			a = ((unsigned long)m_ThreadGps->RcvBuffer[13]) & 0xff;
			b = ((unsigned long)m_ThreadGps->RcvBuffer[14]) & 0xff;
			c = ((unsigned long)m_ThreadGps->RcvBuffer[15]) & 0xff;
			d = ((unsigned long)m_ThreadGps->RcvBuffer[16]) & 0xff;
			llon = ((a << 24) | (b << 16) | (c << 8) | d) & 0xffffffff;

			lat = ((double)llat) / 2147483648 * 90;
			lon = ((double)llon) / 2147483648 * 180;

			speed = -1;

		}
		else if ((ret == RECV_CSBK_LENTH || bytes == RECV_CSBK_LENTH) && (m_ThreadGps->RcvBuffer[0] == Immediate_Location_Report || m_ThreadGps->RcvBuffer[0] == Triggered_Location_Report) && m_ThreadGps->RcvBuffer[1] == 0x11)
		{

			unsigned long llat = 0, llon = 0;
			unsigned long a = 0, b = 0, c = 0, d = 0;
			//解析纬度
			a = ((unsigned long)m_ThreadGps->RcvBuffer[8]) & 0xff;
			b = ((unsigned long)m_ThreadGps->RcvBuffer[9]) & 0xff;
			c = ((unsigned long)m_ThreadGps->RcvBuffer[10]) & 0xff;
			d = ((unsigned long)m_ThreadGps->RcvBuffer[11]) & 0xff;
			llat = ((a << 24) | (b << 16) | (c << 8) | d) & 0xffffffff;
			//解析经度
			a = b = c = d = 0;
			a = ((unsigned long)m_ThreadGps->RcvBuffer[12]) & 0xff;
			b = ((unsigned long)m_ThreadGps->RcvBuffer[13]) & 0xff;
			c = ((unsigned long)m_ThreadGps->RcvBuffer[14]) & 0xff;
			d = ((unsigned long)m_ThreadGps->RcvBuffer[15]) & 0xff;
			llon = ((a << 24) | (b << 16) | (c << 8) | d) & 0xffffffff;

			lat = ((double)llat) / 2147483648 * 90;
			lon = ((double)llon) / 2147483648 * 180;
			speed = -1;
		}
		else if (/*(ret == RECV_CSBK_EGPS_LENTH || bytes == RECV_CSBK_EGPS_LENTH) &&*/ (m_ThreadGps->RcvBuffer[0] == Immediate_Location_Report || m_ThreadGps->RcvBuffer[0] == Triggered_Location_Report) && (m_ThreadGps->RcvBuffer[1] == 0x1e || m_ThreadGps->RcvBuffer[1] == 0x1c))
		{
			unsigned long llat = 0, llon = 0;
			unsigned long a = 0, b = 0, c = 0, d = 0;
			//解析纬度
			a = ((unsigned long)m_ThreadGps->RcvBuffer[14]) & 0xff;
			b = ((unsigned long)m_ThreadGps->RcvBuffer[15]) & 0xff;
			c = ((unsigned long)m_ThreadGps->RcvBuffer[16]) & 0xff;
			d = ((unsigned long)m_ThreadGps->RcvBuffer[17]) & 0xff;
			llat = ((a << 24) | (b << 16) | (c << 8) | d) & 0xffffffff;
			//解析经度
			a = b = c = d = 0;
			a = ((unsigned long)m_ThreadGps->RcvBuffer[18]) & 0xff;
			b = ((unsigned long)m_ThreadGps->RcvBuffer[19]) & 0xff;
			c = ((unsigned long)m_ThreadGps->RcvBuffer[20]) & 0xff;
			d = ((unsigned long)m_ThreadGps->RcvBuffer[21]) & 0xff;
			llon = ((a << 24) | (b << 16) | (c << 8) | d) & 0xffffffff;

			lat = ((double)llat) / 2147483648 * 90;
			lon = ((double)llon) / 2147483648 * 180;

			a = ((unsigned long)m_ThreadGps->RcvBuffer[23]) & 0xff;
			b = ((unsigned long)m_ThreadGps->RcvBuffer[24]) & 0xff;
			speed = (((float)a) + ((float)b) / 128.0f)*3.6f;
		}
		else if ((ret == 9 || bytes == 9 || ret == 10 || bytes == 10) && (m_ThreadGps->RcvBuffer[0] == Immediate_Location_Report || m_ThreadGps->RcvBuffer[0] == Triggered_Location_Report))
		{
			valid = 0;
			speed = -1;
		}
		else
		{
			return;
		}
		try
		{
			time_t t = time(0);
			tm timeinfo;
			char tmp[64];
			localtime_s(&timeinfo, &t);
			strftime(tmp, sizeof(tmp), "%Y/%m/%d %H:%M:%S", &timeinfo);
			string strTime = tmp;
			char radioID[512], strLon[512],strLat[512],strValid[512],strSpeed[512];
			sprintf_s(radioID, 512, "%d", m_ThreadGps->radioID);
			sprintf_s(strLon, 512, "%d", lon);
			sprintf_s(strLat, 512, "%d", lat);
			sprintf_s(strSpeed, 512, "%d", speed);
			sprintf_s(strValid, 512, "%d", valid);


			ArgumentType args;
			FieldValue gps(FieldValue::TObject);
			gps.setKeyVal("lon", FieldValue(strLon));
			gps.setKeyVal("lat", FieldValue(strLat));
			gps.setKeyVal("valid", FieldValue(strValid));
			gps.setKeyVal("speed", FieldValue(strSpeed));
			//gps.setKeyVal("date", FieldValue(strTime.c_str()));
			FieldValue result(FieldValue::TObject);
			result.setKeyVal("Source", FieldValue(radioID));
			result.setKeyVal("gps",gps);
			

			if (m_ThreadGps->RcvBuffer[0] == Immediate_Location_Report)
			{
				args["contents"] = result;
				std::string callJsonStr = CRpcJsonParser::buildCall("SendGps", ++seq, args, "radio");
				if (pRemotePeer != NULL)
				{
					pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());

				}
			}
			else if (m_ThreadGps->RcvBuffer[0] == Triggered_Location_Report)
			{
				args["param"] = result;
				std::string callJsonStr = CRpcJsonParser::buildCall("SendGps", ++seq, args, "radio");
				if (pRemotePeer != NULL)
				{
					pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());

				}
			}
			/*args["id"] = radioID;
			args["valid"] = strValid;
			args["lon"] = strLon;
			args["lat"] = strLat;
			args["speed"] = strSpeed;*/
			
		}
		catch (std::exception e)
		{

		}
	
		/*CString str, strID, strLon, strLat;
		strID.Format(_T("%lu"), m_ThreadGps->radioID);
		strLon.Format(_T("%f"), lon);
		strLat.Format(_T("%f"), lat);*/
		/*if (myCallBackFunc != NULL)
		{
			unsigned char str[100] = { 0 };
			sprintf_s((char *)str, sizeof(str), "id:%d;valid:%d;lon:%.6f;lat:%.6f;speed:%.3f", m_ThreadGps->radioID, valid, lon, lat, speed);
			unsigned char* data = (unsigned char*)(LPCTSTR)str;
			onData(myCallBackFunc, 1, GPS_RECV, (char *)data, sizeof(str));
#if DEBUG_LOG
			LOG(INFO) << "接收到gps ondata ";
#endif

		}*/

	}
	else
	{
		return;
	}
	

}