#include "stdafx.h"
#include "RadioGps.h"
#include "../DataScheduling.h"

#pragma comment(lib, "wsock32.lib")
#define GPS_PORT  4001
CRadioGps::CRadioGps(CDataScheduling *pMnis)
{
	m_RcvSocketOpened = false;
	m_ThreadGps = new ThreadGPS;
	m_ThreadGpsOverturn = new ThreadGPSOverturn;
	m_pMnis = pMnis;
	m_gpsThread = true;

	interval = 0;
	iBconNum = 0;
	isEme = false;
}


CRadioGps::~CRadioGps()
{
	if (m_ThreadGps)
	{
		delete m_ThreadGps;
	}
	if (m_ThreadGpsOverturn)
	{
		delete m_ThreadGpsOverturn;
	}
}
bool CRadioGps::InitGPSSocket(DWORD dwAddress/*,  CRemotePeer * pRemote*/)
{
	//pRemotePeer = pRemote;
	//CString			 strError;
	SOCKADDR_IN      addr;					//   The   local   interface   address   
	WSADATA			 wsda;					//   Structure   to   store   info
	int ret = WSAStartup(MAKEWORD(1, 1), &wsda);     //   Load   version   1.1   of   Winsock

	CloseGPSSocket();
	BOOL bReuseaddr = FALSE;
	setsockopt(m_ThreadGps->mySocket, SOL_SOCKET, SO_DONTLINGER, (const char*)&bReuseaddr, sizeof(BOOL));

	m_ThreadGps->mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);   //   Create   an   UDP   socket
	

	if (m_ThreadGps->mySocket == SOCKET_ERROR)				//   Socket create Error
	{
		//AfxMessageBox(_T("Socket初始化错误！"));
		CloseGPSSocket();

		return FALSE;
	}

	//   Fill   in   the   interface   information  


	addr.sin_family = AF_INET;
	addr.sin_port = htons(GPS_PORT);
	addr.sin_addr.s_addr = dwAddress;
	ret = ::bind(m_ThreadGps->mySocket, (struct sockaddr *) &addr, sizeof(addr));
	if (ret == SOCKET_ERROR)
	{
		int b = WSAGetLastError();
		//CloseGPSSocket(s);
		return FALSE;
	}
	m_RcvSocketOpened = true;
	m_gWth =  CreateThread(NULL,0, ReceiveDataThread, this, THREAD_PRIORITY_NORMAL, NULL);

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
		CloseGPSSocket();

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

bool CRadioGps::CloseGPSSocket()
{

	if (m_RcvSocketOpened)        // 只有在前面已经打开了，才有必要关闭，否则没有必要了
	{
		closesocket(m_ThreadGps->mySocket);							        // Close socket

		WSACleanup();

		m_RcvSocketOpened = FALSE;
		if (m_gWth)
		{
			m_gpsThread = false;
			WaitForSingleObject(m_gWth, 1000);
			CloseHandle(m_gWth);
		}
	}
	return TRUE;
}

bool CRadioGps::SendQueryGPS( DWORD dwRadioID,int queryMode,double cycle)
{
	int gpsCycle = static_cast<int>(cycle);
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
		m_ThreadGps->SendBuffer[1] = Triggered_Location_Request_Tokens_Length-1;
		m_ThreadGps->SendBuffer[2] = Location_RequestID_Start;
		m_ThreadGps->SendBuffer[3] = 0x03;
		m_ThreadGps->SendBuffer[4] = 0x00;
		m_ThreadGps->SendBuffer[5] = 0x00;
		m_ThreadGps->SendBuffer[6] = 0x01;
		m_ThreadGps->SendBuffer[7] = Start_Trigger_Element;
		m_ThreadGps->SendBuffer[8] = Start_Interval_Element_uint;
		m_ThreadGps->SendBuffer[9] = 0xff & gpsCycle;
		//m_ThreadGps->SendBuffer[11] = 0x54;  //Specifies that altitude information is required
		//m_ThreadGps->SendBuffer[12] = 0x57;  //Specifies that horizontal direction information is requested
		m_ThreadGps->gpsLength = SEND_TRG_QUERY_LENTH-1;
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
			m_ThreadGps->SendBuffer[12] = 0xff & gpsCycle;
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
				m_ThreadGps->SendBuffer[15] = 0xff & gpsCycle;
				m_ThreadGps->gpsLength = SEND_TRG_CSBK_EGPD_QUERY_LENTH;
			}
			
	}
		break;
	case GPS_TRIGG_COMM_INDOOR:
		if (!isEme)
		{
			m_ThreadGps->SendBuffer[0] = Triggered_Location_Request;
			m_ThreadGps->SendBuffer[1] = Triggered_Location_Request_Tokens_Indoor_Length;
			m_ThreadGps->SendBuffer[2] = Location_RequestID_Start;
			m_ThreadGps->SendBuffer[3] = 0x04;
			m_ThreadGps->SendBuffer[4] = 0x24;
			m_ThreadGps->SendBuffer[5] = 0x68;
			m_ThreadGps->SendBuffer[6] = 0xAC;
			m_ThreadGps->SendBuffer[7] = 0xE0;
			m_ThreadGps->SendBuffer[8] = 0x6c;   //becon only
			m_ThreadGps->SendBuffer[9] = 0x7A;  //request-bcon-maj-min-time
			m_ThreadGps->SendBuffer[10] = iBconNum & 0xff;  //becon number
			m_ThreadGps->SendBuffer[11] = Start_Trigger_Element;
			m_ThreadGps->SendBuffer[12] = Start_Interval_Element_uint;
			m_ThreadGps->SendBuffer[13] = 0xff & interval;
			m_ThreadGps->gpsLength = SEND_IMM_QUERY_LENTH+4;
		}
		else
		{
			m_ThreadGps->SendBuffer[0] = Triggered_Location_Request;
			m_ThreadGps->SendBuffer[1] = Triggered_Location_Request_Tokens_Indoor_Length-1;
			m_ThreadGps->SendBuffer[2] = Location_RequestID_Start;
			m_ThreadGps->SendBuffer[3] = 0x04;
			m_ThreadGps->SendBuffer[4] = 0x24;
			m_ThreadGps->SendBuffer[5] = 0x68;
			m_ThreadGps->SendBuffer[6] = 0xAC;
			m_ThreadGps->SendBuffer[7] = 0xE0;
			m_ThreadGps->SendBuffer[8] = 0x6c;   //becon only
			m_ThreadGps->SendBuffer[9] = 0x7A;  //request-bcon-maj-min-time
			m_ThreadGps->SendBuffer[10] = iBconNum & 0xff;  //becon number
			m_ThreadGps->SendBuffer[11] = 0x4A;    //REQUEST_TRIGGER_CONDITION
			m_ThreadGps->SendBuffer[12] = 0x02;    //EMERGENCY_CONDITION
			m_ThreadGps->gpsLength = SEND_IMM_QUERY_LENTH+3;
		}
		
		break;
	case GPS_TRIGG_CSBK_INDOOR:
		memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
		m_ThreadGps->SendBuffer[0] = Triggered_Location_Request;
		m_ThreadGps->SendBuffer[1] = Triggered_Location_Request_Tokens_Indoor_Length;
		m_ThreadGps->SendBuffer[2] = Location_RequestID_Start;
		m_ThreadGps->SendBuffer[3] = 0x04;
		m_ThreadGps->SendBuffer[4] = 0x24;
		m_ThreadGps->SendBuffer[5] = 0x68;
		m_ThreadGps->SendBuffer[6] = 0xac;
		m_ThreadGps->SendBuffer[7] = 0xe0;
		m_ThreadGps->SendBuffer[8] = CSBK_Start_Require_Data;
		m_ThreadGps->SendBuffer[9] = CSBK_Require_Data_Length;
		m_ThreadGps->SendBuffer[10] = Request_LRRP_CSBK;
		m_ThreadGps->SendBuffer[11] = 0x6c;   //becon only
		m_ThreadGps->SendBuffer[12] = 0x7A;  //request-bcon-maj-min-time
		m_ThreadGps->SendBuffer[13] = iBconNum & 0xff;
		m_ThreadGps->SendBuffer[14] = Start_Trigger_Element;
		m_ThreadGps->SendBuffer[15] = Start_Interval_Element_uint;
		if (interval == 7)
		{
			m_ThreadGps->SendBuffer[1] = CSBK_Triggered_Location_Request_Tokens_Length_ufloat+1;
			m_ThreadGps->SendBuffer[15] = Start_Interval_Element_ufloat;
			m_ThreadGps->SendBuffer[16] = 0x07;
			m_ThreadGps->SendBuffer[17] = 0x40;            //7.5s
			m_ThreadGps->gpsLength = SEND_TRG_CSBK_QUERY_LENTH+4;
		}
		else
		{
			m_ThreadGps->SendBuffer[1] = CSBK_Triggered_Location_Request_Tokens_Length_uint+1;
			m_ThreadGps->SendBuffer[15] = Start_Interval_Element_uint;
			m_ThreadGps->SendBuffer[16] = 0xff & interval;
			m_ThreadGps->gpsLength = SEND_TRG_CSBK_QUERY_LENTH +3;
		}
		break;
	case GPS_TRIGG_CSBK_EGPS_INDOOR:
		
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
		return false;
	}
	return true;
}
bool CRadioGps::StopQueryTriggeredGPS(DWORD dwRadioID, int	queryMode)
{
	if (queryMode == GPS_TRIGG_COMM_INDOOR || queryMode == GPS_TRIGG_CSBK_INDOOR)
	{
		memset(m_ThreadGps->SendBuffer, 0, sizeof(m_ThreadGps->SendBuffer));
		m_ThreadGps->SendBuffer[0] = 0x0F;   //Immediate Location Request
		m_ThreadGps->SendBuffer[1] = 0x06;   //XML协议报中包含8组数据
		m_ThreadGps->SendBuffer[2] = 0x22;   //Start of request-id element
		m_ThreadGps->SendBuffer[3] = 0x04;
		m_ThreadGps->SendBuffer[4] = 0x24;
		m_ThreadGps->SendBuffer[5] = 0x68;
		m_ThreadGps->SendBuffer [6] = 0xAC;
		m_ThreadGps->SendBuffer[7] = 0xE0;
		m_ThreadGps->gpsLength = SEND_STOP_LENTH;
	}
	else // if (queryMode == GPS_TRIGG_COMM || queryMode == GPS_TRIGG_CSBK || queryMode == GPS_TRIGG_CSBK_EGPS)
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
		/*try
		{
			list<AllCommand>::iterator it;
			for (it = allCommandList.begin(); it != allCommandList.end(); it++)
			{
				if (it->radioId == m_ThreadGps->radioID)
				{
					char radioID[512];
					sprintf_s(radioID, 512, "%d", m_ThreadGps->radioID);
					ArgumentType args;
					args["Target"] = FieldValue(m_ThreadGps->radioID);
					args["Type"] = FieldValue(it->querymode);
					args["Cycle"] = FieldValue(it->cycle);
					args["Operate"] = FieldValue(STOP);
					int status = -1;
					if (m_ThreadGps->RcvBuffer[7] == Location_Operate_Sucess)
					{
						status = REMOTE_SUCESS;
					}
					else
					{
						status = REMOTE_FAILED;
					}
					args["Status"] = FieldValue(status);
					std::string callJsonStrRes = CRpcJsonParser::buildCall("sendGpsStatus", it->callId, args, "radio");
					if (pRemotePeer != NULL)
					{
						pRemotePeer->sendResponse((const char *)callJsonStrRes.c_str(), callJsonStrRes.size());
						it = allCommandList.erase(it);
						break;
					}
				}
			}
		}
		catch (std::exception e)
		{

		}*/
		return false;
	}

	return true;
}
DWORD WINAPI CRadioGps::ReceiveDataThread(LPVOID lpParam)
{
	CRadioGps * radioGps = (CRadioGps *)lpParam;

	if (radioGps!=NULL)
	{
		radioGps->RecvData();
	}
	return 1;
}
void CRadioGps::RecvData()
{
	while (m_gpsThread)
	{
		int   iRemoteAddrLen;            //   Contains   the   length   of   remte_addr,   passed   to   recvfrom   
		int   ret = 0, bytes = 0;
		int    iMessageLen;
	
		iRemoteAddrLen = sizeof(m_ThreadGps->remote_addr);
		iMessageLen = MAX_RECV_LENGTH;
		const int immLocRepLenth = 28;   //Immediate Location Report报文长度
		const int trgLocReqLenth = 17;    //triggere Location Report 报文长度
		const int csbkLocReqLenth = 19;
		ret = recvfrom(m_ThreadGps->mySocket, m_ThreadGps->RcvBuffer, iMessageLen, 0, (struct sockaddr*)&m_ThreadGps->remote_addr, &iRemoteAddrLen);

		bytes = recvfrom(m_ThreadGpsOverturn->mySocket, m_ThreadGpsOverturn->RcvBuffer, iMessageLen, 0, (struct sockaddr*)&m_ThreadGpsOverturn->remote_addr, &iRemoteAddrLen);
		unsigned long a = 0, b = 0;
		float speed = -1;
		int valid = 1;
		int queryMode = -1;
		int operate = -1;
		double lat = -1, lon = -1;
		if (ret != SOCKET_ERROR || bytes != SOCKET_ERROR)
		{
			m_ThreadGps->radioID = (m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b2 << 16) + (m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b3 << 8) + m_ThreadGps->remote_addr.sin_addr.S_un.S_un_b.s_b4;
			char radioID[512], strLon[512], strLat[512], strValid[512], strSpeed[512];
			sprintf_s(radioID, 512, "%d", m_ThreadGps->radioID);
			if (m_ThreadGps->RcvBuffer[0] == Triggered_Location_Stop_Answer || m_ThreadGps->RcvBuffer[0] == Triggered_location_Start_Answer)
			{
				try
				{
					std::list<Command>::iterator it;
					m_timeOutListLocker.lock();
					for (it = timeOutList.begin(); it != timeOutList.end(); it++)
					{
						if (it->radioId == m_ThreadGps->radioID)
						{
							if (m_ThreadGps->RcvBuffer[0] == Triggered_Location_Stop_Answer)
							{
								operate = STOP;

							}
							else  if (m_ThreadGps->RcvBuffer[0] == Triggered_location_Start_Answer)
							{
								operate = START;
							}
							if (myCallBackFunc != NULL)
							{
								int len = m_ThreadGps->RcvBuffer[1];
								if (m_ThreadGps->RcvBuffer[len+1] == Location_Operate_Sucess)
								{
									Respone r = { 0 };
									r.target = m_ThreadGps->radioID;
									r.gpsStatus = SUCESS;
									r.querymode = it->querymode;
									r.cycle = it->cycle;
									r.operate = operate;
									onData(myCallBackFunc, it->command, r);
								}
								else
								{
									Respone  r = { 0 };;
									r.source = m_ThreadGps->radioID;
									r.gpsStatus = UNSUCESS;
									r.querymode = it->querymode;
									r.cycle = it->cycle;
									r.operate = operate;
									onData(myCallBackFunc, it->command, r);
								}

#if DEBUG_LOG
								LOG(INFO) << "接收到gps ondata ";
#endif

							}
						}
					}
					m_timeOutListLocker.unlock();
				}
				catch (std::exception e)
				{

				}
			}
			else
			{
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
					queryMode = GPS_TRIGG_COMM;

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
					queryMode = GPS_TRIGG_CSBK;
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
					queryMode = GPS_TRIGG_CSBK_EGPS;
				}
				else if (ret >= RECV_TRG_INDOOR_LENTH && m_ThreadGps->RcvBuffer[8] == beacon_data && m_ThreadGps->RcvBuffer[9]== bcon_maj_min_time)
				{
					int num = 0;
					std::list <BconMajMinTimeReport> bconList;
					num = ((unsigned char)m_ThreadGps->RcvBuffer[10]) & 0xff;
					if (num == 0)
					{

					}
					else
					{
						std::list<BconMajMinTimeReport> mBcon;
						for (int i = 0; i < num; i++)
						{
							BconMajMinTimeReport bcon;
							
							bcon.Major = ntohs(*((unsigned short *)&m_ThreadGps->RcvBuffer[11+i*6]));
							bcon.Minor = ntohs(*((unsigned short *)&m_ThreadGps->RcvBuffer[13+i*6]));
							bcon.TimeStamp = ntohs(*((unsigned short *)&m_ThreadGps->RcvBuffer[15+i*6]));
							mBcon.push_back(bcon);
						}
						std::list<Command>::iterator it;
						int count = 0;
						m_timeOutListLocker.lock();
						for (it = timeOutList.begin(); it != timeOutList.end(); it++)
						{
							if (it->radioId == atoi(radioID))
							{
								if (myCallBackFunc != NULL)
								{
									Respone r = { 0 };
									r.source = m_ThreadGps->radioID;
									r.becon = mBcon;
									onData(myCallBackFunc, RECV_LOCATION_INDOOR, r);
									count++;
									break;
								}
							}
						}
						m_timeOutListLocker.unlock();
						if (count == 0)
						{
							if (myCallBackFunc != NULL)
							{
							
								Respone r = { 0 };
								r.source = m_ThreadGps->radioID;
								r.becon = mBcon;
								onData(myCallBackFunc, RECV_LOCATION_INDOOR, r);
							}

						}
					
					}
					//return;
		
				}
				else if ((ret == 9 || bytes == 9 || ret == 10 || bytes == 10) && (m_ThreadGps->RcvBuffer[0] == Immediate_Location_Report || m_ThreadGps->RcvBuffer[0] == Triggered_Location_Report))
				{
					valid = 0;
					speed = -1;
				}
				else
				{
					//return;
				}
				if (lat != -1 && lon != -1)
				{

					try
					{

						time_t t = time(0);
						tm timeinfo;
						char tmp[64];
						localtime_s(&timeinfo, &t);
						strftime(tmp, sizeof(tmp), "%Y/%m/%d %H:%M:%S", &timeinfo);
						std::string strTime = tmp;

						sprintf_s(strLon, 512, "%lf", lon);
						sprintf_s(strLat, 512, "%lf", lat);
						sprintf_s(strSpeed, 512, "%f", speed);
						sprintf_s(strValid, 512, "%d", valid);
						BOOL result = false;
						if (strValid == "0")
						{
							result = false;                      //无效
						}
						else if (strValid == "1")
						{
							result = true;                     //有效
						}
						std::list<Command>::iterator it;
						int count = 0;
						m_timeOutListLocker.lock();
						for (it = timeOutList.begin(); it != timeOutList.end(); it++)
						{
							if (it->radioId == atoi(radioID))
								/*if (it->command == GPS_IMME_COMM || it->command == GPS_IMME_CSBK || it->command == GPS_IMME_CSBK_EGPS
									|| it->command== GPS_TRIGG_COMM || it->command == GPS_TRIGG_CSBK || it->command ==GPS_TRIGG_CSBK_EGPS)*/
							{
								if (myCallBackFunc != NULL)
								{
									Respone r = { 0 };
									r.source = m_ThreadGps->radioID;
									r.lat = lat;
									r.lon = lon;
									r.speed = speed;
									r.valid = valid;
									onData(myCallBackFunc, RECV_GPS, r);
									count++;
									break;
								}
							}
						}
						m_timeOutListLocker.unlock();
						if (count == 0)
						{
							if (myCallBackFunc != NULL)
							{
								Respone r = { 0 };
								r.source = m_ThreadGps->radioID;
								r.lat = lat;
								r.lon = lon;
								r.speed = speed;
								r.valid = valid;
								onData(myCallBackFunc, RECV_GPS, r);
							}

						}
					}
					catch (std::exception e)
					{

					}
				}
			}
			m_pMnis->updateOnLineRadioInfo(atoi(radioID), RADIO_STATUS_ONLINE, (STOP == operate) ? (-1) : (queryMode));
		}
		else
		{
			return;
		}
		Sleep(100);
	}
}
void CRadioGps::locationIndoorConfig(int Interval, int iBeaconNumber, bool isEmergency)
{
	interval = Interval;
	iBconNum = iBeaconNumber;
	isEme = isEmergency;
}
