#include "stdafx.h"
#include "TextMsg.h"
#include  "time.h"

#pragma comment(lib, "wsock32.lib")

#define MSG_PORT   4007
CTextMsg::CTextMsg()
: m_RcvSocketOpened(false)
{
	m_nSendSequenceNumber = 0;
	 m_ThreadMsg = new ThreadMsg;

}
CTextMsg::~CTextMsg()
{
	if (!m_ThreadMsg)
	{
		delete m_ThreadMsg;
	}
	
}
bool CTextMsg::InitSocket( DWORD dwAddress/*, CRemotePeer * pRemote*/)
{

	/*pRemotePeer = pRemote;*/
	//CString			 strError;
	SOCKADDR_IN      addr;					//   The   local   interface   address   
	WSADATA			 wsda;					//   Structure   to   store   info

	CloseSocket();
	BOOL bReuseaddr = FALSE;
	setsockopt(m_ThreadMsg->mySocket, SOL_SOCKET, SO_DONTLINGER, (const char*)&bReuseaddr, sizeof(BOOL));

	int ret = WSAStartup(MAKEWORD(1, 1), &wsda);     //   Load   version   1.1   of   Winsock

	m_ThreadMsg->mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);   //   Create   an   UDP   socket

	if (m_ThreadMsg->mySocket == SOCKET_ERROR)				//   Socket create Error
	{
		//AfxMessageBox(_T("Socket��ʼ������"));
		CloseSocket();

		return FALSE;
	}

	//   Fill   in   the   interface   information  


	addr.sin_family = AF_INET;
	addr.sin_port = htons(MSG_PORT);
	addr.sin_addr.s_addr = dwAddress;
	ret = ::bind(m_ThreadMsg->mySocket, (struct sockaddr *) &addr, sizeof(addr));
	if ( ret== SOCKET_ERROR)
	{
		int b = WSAGetLastError();
		//AfxMessageBox(_T("�󶨶˿ڴ���"));
		CloseSocket();
		return FALSE;
	}
	m_RcvSocketOpened = true;
	CreateThread(NULL, 0, ReceiveDataThread, this, THREAD_PRIORITY_NORMAL +1, NULL);
	//AfxBeginThread(ReceiveDataThread, (LPVOID)&m_ThreadMsg, THREAD_PRIORITY_NORMAL);
	//AfxBeginThread(RecvThread,(LPVOID)&m_UDPThreadMsg,THREAD_PRIORITY_NORMAL);
	return TRUE;
}


bool CTextMsg::CloseSocket()
{
	if (m_RcvSocketOpened)        // ֻ����ǰ���Ѿ����ˣ����б�Ҫ�رգ�����û�б�Ҫ��
	{
		closesocket(m_ThreadMsg->mySocket);							        // Close socket

		WSACleanup();

		m_RcvSocketOpened = FALSE;
	}
	return TRUE;
}


DWORD WINAPI CTextMsg::ReceiveDataThread(LPVOID lpParam)
{

	//CTextMsg * pTextMsg = new CTextMsg;
	CTextMsg * pTextMsg  = (CTextMsg *)lpParam;
	while (1)
	{
		pTextMsg->RecvMsg();
	}

	return 1;
}

std::string CTextMsg::ParseUserMsg(TextMsg* HandleMsg, int * len)
{
	UINT16			MsgSize;
	FirstHeader		FstHeader;
	SecondHeader    callIddHeader;
	UINT8			AddressSize;
	static TCHAR	szMessage[MAX_MESSAGE_LENGTH];
	std::string         ParsedMsg;
	int             MsgOffset;                                // ��ʽ��Message��TextPayload�е���ʼƫ����

	memset((char*)szMessage, 0, sizeof(szMessage));             // ��֪��������� unicode ʹ�� sizeof �Ƿ���ȷ��

	MsgSize = HandleMsg->MsgSize;
	FstHeader = HandleMsg->FstHeader;
	AddressSize = HandleMsg->AddressSize;

	MsgOffset = 0;

	if (AddressSize)
	{ // ���յ��������д���Address
		MsgOffset = MsgOffset + AddressSize;   // ����Address��ռ���ֽڵĳ���

		if (FstHeader.ExtensionBit)                  // ���� Subsequent Header�� Subsequent Header���ܴ���2��
		{
			memcpy((char*)&callIddHeader, &HandleMsg->TextPayload[MsgOffset], sizeof(SecondHeader));
			MsgOffset = MsgOffset + sizeof(SecondHeader);

			if (callIddHeader.Extension)
			{
				MsgOffset = MsgOffset + sizeof(ThirdHeader);
			}
		}
	}
	else
	{ // ���յ�����Ϣ�в���Address
		if (FstHeader.ExtensionBit)                  // ���� Subsequent Header�� Subsequent Header���ܴ���2��
		{
			memcpy((char*)&callIddHeader, &HandleMsg->TextPayload[MsgOffset], sizeof(SecondHeader));
			MsgOffset = MsgOffset + sizeof(SecondHeader);

			if (callIddHeader.Extension)
			{
				MsgOffset = MsgOffset + sizeof(ThirdHeader);
			}
		}
	}


	memcpy((char*)szMessage, &HandleMsg->TextPayload[MsgOffset], MsgSize - MsgOffset - 2);
	//*len = MsgSize - MsgOffset - 2;
	ParsedMsg = TCHAR2STRING(szMessage);
	//	memcpy((char*)szMessage, &HandleMsg->TextPayload[MsgOffset], MsgSize - MsgOffset - 2);
	//	ParsedMsg.Format(_T("%s"), szMessage);
	//ParsedMsg = HandleMsg->TextPayload[MsgOffset];

	return ParsedMsg;

}

std::string CTextMsg::TCHAR2STRING(TCHAR * STR)

{

	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);

	char* chRtn = new char[iLen*sizeof(char)];

	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);

	std::string str(chRtn);

	return str;

}



UINT8 CTextMsg::GetSeqNumber(TextMsg* HandleMsg)
{

	UINT16			MsgSize;
	FirstHeader		FstHeader;
	SecondHeader    callIddHeader;
	ThirdHeader     TrdHeader;
	UINT8			AddressSize;
	UINT8           TxtSeqNum;

	char*           Address;
	int             OffSet;

	MsgSize = HandleMsg->MsgSize;
	FstHeader = HandleMsg->FstHeader;
	AddressSize = HandleMsg->AddressSize;

	OffSet = 0;

	if (AddressSize)
	{ // ���յ��������д���Address
		Address = new char[AddressSize];             // ��̬�����ڴ棬���ڱ��� Address
		memcpy(Address, HandleMsg->TextPayload, AddressSize);

		OffSet = AddressSize;                       // ����Address��ռ���ֽڵĳ���,֮ǰ��ͷ��Ϣ���������TextPayload�У�����������ƫ������ʱ��Ӧ�ð������ǵĳ���

		if (FstHeader.ExtensionBit)                  // ���� Subsequent Header�� Subsequent Header���ܴ��ڶ��
		{
			memcpy((char*)&callIddHeader, &HandleMsg->TextPayload[OffSet], sizeof(SecondHeader));
			TxtSeqNum = callIddHeader.SequenceNum;       // ȡ�ý��յ���Message���к��еĵ�5λ
			OffSet = OffSet + sizeof(SecondHeader);

			if (callIddHeader.Extension)
			{
				memcpy((char*)&TrdHeader, &HandleMsg->TextPayload[OffSet], sizeof(ThirdHeader));
				UINT8 MsbSeq = TrdHeader.SequenceNum; // ȡ�ý��յ���Message���к��еĸ�2λ
				TxtSeqNum = TxtSeqNum | (MsbSeq << 5);
			}
		}

		delete[] Address;                                    // �ͷ���������ڴ�
	}
	else
	{ // ���յ�����Ϣ�в���Address
		if (FstHeader.ExtensionBit)                  // ���� Subsequent Header
		{
			memcpy((char*)&callIddHeader, &HandleMsg->TextPayload[OffSet], sizeof(SecondHeader));
			TxtSeqNum = callIddHeader.SequenceNum;       // ȡ�ý��յ���Message���к��еĵ�5λ
			OffSet = OffSet + sizeof(SecondHeader);

			if (callIddHeader.Extension)
			{
				memcpy((char*)&TrdHeader, &HandleMsg->TextPayload[OffSet], sizeof(ThirdHeader));
				UINT8 MsbSeq = TrdHeader.SequenceNum; // ȡ�ý��յ���Message���к��еĸ�2λ
				TxtSeqNum = TxtSeqNum | (MsbSeq << 5);
			}
		}
	}

	return TxtSeqNum;
}
bool CTextMsg::ReplyMsgACK(ThreadMsg* Msg, UINT8 SeqNumber)
{
	//����ACK
	UINT16			MsgSize;
	FirstHeader		FstHeader;
	UINT8			AddressSize;
	SecondHeader    callIddHeader;
	ThirdHeader     TrdHeader;
	int             OffSet;

	//���First Header������
	FstHeader.ExtensionBit = 1;
	FstHeader.AckRequired = 0;
	FstHeader.Reserved = 0;
	FstHeader.Control = 1;
	FstHeader.PDUType = 0xF;

	UINT8 nTemp;
	memcpy((char*)&nTemp, (char*)&FstHeader, 1);

	MsgSize = sizeof(FirstHeader);

	// AddressSize �� Address �ֶβ�����ACK�в���Ҫ����˲���д
	AddressSize = 0;
	MsgSize = MsgSize + sizeof(AddressSize);

	if (SeqNumber <= 31)               //Sequence Nuber ֻ��ҪSecond Header
	{
		callIddHeader.Extension = 1;
		callIddHeader.SequenceNum = SeqNumber;
		callIddHeader.Reserved = 0;

		TrdHeader.Extension = 0;
		TrdHeader.SequenceNum = 0;    // Sequence Number �� ��2λ
		TrdHeader.Reserved = 0;

		MsgSize = MsgSize + sizeof(SecondHeader)+sizeof(ThirdHeader);
	}
	else
	{                                 //Sequence Number ��ҪSecond Header �� Third Header
		callIddHeader.Extension = 1;
		callIddHeader.SequenceNum = (SeqNumber & 0x1F);        // Sequence Number �� ��5λ
		callIddHeader.Reserved = 0;

		TrdHeader.Extension = 0;
		TrdHeader.SequenceNum = ((SeqNumber >> 5) & 0x3);    // Sequence Number �� ��2λ
		TrdHeader.Reserved = 0;

		MsgSize = MsgSize + sizeof(SecondHeader)+sizeof(ThirdHeader);
	}

	Msg->MsgLength = MsgSize + sizeof(MsgSize);

	OffSet = 0;
	MsgSize = htons(MsgSize);                                                       // ת����Message��Ϣ����Ҫ�Ĵ����ʽ
	memcpy(&Msg->RcvBuffer[OffSet], (char*)&MsgSize, sizeof(MsgSize));              // ���Message Size

	OffSet = OffSet + sizeof(MsgSize);
	memcpy(&Msg->RcvBuffer[OffSet], (char*)&FstHeader, sizeof(FirstHeader));        // ���First Header

	OffSet = OffSet + sizeof(FirstHeader);
	memcpy(&Msg->RcvBuffer[OffSet], (char*)&AddressSize, sizeof(AddressSize));      // ���AddressSize

	OffSet = OffSet + sizeof(AddressSize);
	memcpy(&Msg->RcvBuffer[OffSet], (char*)&callIddHeader, sizeof(SecondHeader));   // ���SecondHeader

	OffSet = OffSet + sizeof(SecondHeader);
	memcpy(&Msg->RcvBuffer[OffSet], (char*)&TrdHeader, sizeof(ThirdHeader));    // ���ThirdHeader

	//Sleep(2000);
	//   Ready   to   send   data   
	int ret = sendto(Msg->mySocket, Msg->RcvBuffer, Msg->MsgLength, 0, (struct sockaddr*)&Msg->remote_addr, sizeof(Msg->remote_addr));
	if (ret == SOCKET_ERROR)
	{
		//CString strError;
		//strError.Format(_T("Error\nCall   to   sendto(s,   szMessage,   iMessageLen,   0,   (SOCKADDR_IN   *)   &addr,   sizeof(addr));   failed   with:\n%d\n"), WSAGetLastError());
	}

	return TRUE;
}


bool CTextMsg::SendMsg(int callId, LPTSTR message, DWORD dwRadioID, int CaiNet)
{
	//����ACK
	UINT16			MsgSize;
	FirstHeader		FstHeader;
	UINT8			AddressSize;
	SecondHeader    callIddHeader;
	ThirdHeader     TrdHeader;
	int             OffSet;
	char			buf[1024];


	//���First Header������
	FstHeader.ExtensionBit = 1;
	FstHeader.AckRequired = 1;
	FstHeader.Reserved = 1;
	FstHeader.Control = 0;
	FstHeader.PDUType = 0;

	UINT8 nTemp;
	memcpy((char*)&nTemp, (char*)&FstHeader, 1);

	MsgSize = sizeof(FirstHeader);

	// AddressSize �� Address �ֶβ�����ACK�в���Ҫ����˲���д
	AddressSize = 0;
	MsgSize = MsgSize + sizeof(AddressSize);

	callIddHeader.Extension = 1;
	callIddHeader.SequenceNum = m_nSendSequenceNumber & 0x1f;
	callIddHeader.Reserved = 0;

	TrdHeader.Extension = 0;
	TrdHeader.SequenceNum = (m_nSendSequenceNumber >> 5) & 0x03;    // Sequence Number �� ��2λ
	TrdHeader.Reserved = 0x04;	  // UCS-LE format

	MsgSize = MsgSize + sizeof(SecondHeader)+wcslen(message) * 2 + sizeof(ThirdHeader);

	m_ThreadMsg->MsgLength = MsgSize + sizeof(MsgSize);

	OffSet = 0;
	MsgSize = htons(MsgSize);                                                       // ת����Message��Ϣ����Ҫ�Ĵ����ʽ
	memcpy(&buf[OffSet], (char*)&MsgSize, sizeof(MsgSize));              // ���Message Size

	OffSet = OffSet + sizeof(MsgSize);
	memcpy(&buf[OffSet], (char*)&FstHeader, sizeof(FirstHeader));        // ���First Header

	OffSet = OffSet + sizeof(FirstHeader);
	memcpy(&buf[OffSet], (char*)&AddressSize, sizeof(AddressSize));      // ���AddressSize

	OffSet = OffSet + sizeof(AddressSize);
	memcpy(&buf[OffSet], (char*)&callIddHeader, sizeof(SecondHeader));   // ���SecondHeader

	OffSet = OffSet + sizeof(SecondHeader);
	memcpy(&buf[OffSet], (char*)&TrdHeader, sizeof(ThirdHeader));    // ���ThirdHeader

	OffSet = OffSet + sizeof(ThirdHeader);


	int len = wcslen(message) * 2;
	if (len != 0)
	{
		wcscpy_s((LPTSTR)&buf[OffSet], len, message);
		memcpy(m_ThreadMsg->SendBuffer, buf, m_ThreadMsg->MsgLength);
	}
	////��m_nSendSequenceNumberд��list
	//list<AllCommand>::iterator it;
	//for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
	//{
	//	if (it->callId == callId)
	//	{
	//		it->ackNum = m_nSendSequenceNumber;
	//	}
	//}
	m_nSendSequenceNumber++;
	if (m_nSendSequenceNumber & 0x80)
	{
		m_nSendSequenceNumber = 0;
	}
	memset((void *)&m_ThreadMsg->remote_addr, 0, sizeof(struct sockaddr_in));
	m_ThreadMsg->remote_addr.sin_family = AF_INET;
	m_ThreadMsg->remote_addr.sin_port = htons(4007);
	unsigned long radio_ip = ((CaiNet << 24) + dwRadioID) & 0xffffffff;
	m_ThreadMsg->remote_addr.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)((radio_ip >> 24) & 0xff);
	m_ThreadMsg->remote_addr.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)((radio_ip >> 16) & 0xff);
	m_ThreadMsg->remote_addr.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)((radio_ip >> 8) & 0xff);
	m_ThreadMsg->remote_addr.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)(radio_ip & 0xff);
	int bytesSend = sendto(m_ThreadMsg->mySocket, (const char*)m_ThreadMsg->SendBuffer, m_ThreadMsg->MsgLength, 0, (struct sockaddr*)&m_ThreadMsg->remote_addr, sizeof(m_ThreadMsg->remote_addr));
	if (-1 == bytesSend)
	{
		int a = GetLastError();
//		list<AllCommand>::iterator it;
//		m_allCommandListLocker.lock();
//		for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
//		{
//			//if (it->ackNum == SeqNum)
//			{
//				if (pRemotePeer != NULL&& pRemotePeer == it->pRemote &&it->radioId == m_ThreadMsg->radioID)
//				{
//					ArgumentType args;
//					args["Source"] = FieldValue(m_ThreadMsg->radioID);
//					args["contents"] = FieldValue("");
//					args["status"] = FieldValue(REMOTE_FAILED);
//					if (it->command == SEND_PRIVATE_MSG)
//					{
//						args["type"] = FieldValue(PRIVATE);
//					}
//					else if (it->command == SEND_GROUP_MSG)
//					{
//						args["type"] = FieldValue(GROUP);
//					}
//					std::string callJsonStr = CRpcJsonParser::buildCall("messageStatus", ++seq, args, "radio");
//					pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
//					it = allCommandList.erase(it);
//					break;
//				}
//
//			}
//		}
//		m_allCommandListLocker.unlock();
#if DEBUG_LOG
		LOG(INFO) << "���Ͷ���ʧ��";
#endif
		return false;
	}
#if DEBUG_LOG
	LOG(INFO) << "���Ͷ��ųɹ� ";
#endif
	return true;
}
void CTextMsg::RecvMsg()
{

	int   iRemoteAddrLen;            //   Contains   the   length   of   remte_addr,   passed   to   recvfrom   
	int   ret;
	int    iMessageLen;

	//Ready to receive data
	iRemoteAddrLen = sizeof(m_ThreadMsg->remote_addr);
	iMessageLen = MAX_MESSAGE_LENGTH;     //Set to the length of szMessage buffer
	ret = recvfrom(m_ThreadMsg->mySocket, m_ThreadMsg->RcvBuffer, iMessageLen, 0, (struct sockaddr*)&m_ThreadMsg->remote_addr, &iRemoteAddrLen);

	if (ret == SOCKET_ERROR)
	{
		//strError.Format(_T("Error\nCall to recvfrom(s, Msg->RcvBuffer, iMessageLen, 0, (struct sockaddr *)&remote_addr, &iRemoteAddrLen); failed   with:\n%d\n"), WSAGetLastError());
		//break;
	}
	m_ThreadMsg->radioID = (m_ThreadMsg->remote_addr.sin_addr.S_un.S_un_b.s_b2 << 16) + (m_ThreadMsg->remote_addr.sin_addr.S_un.S_un_b.s_b3 << 8) + m_ThreadMsg->remote_addr.sin_addr.S_un.S_un_b.s_b4;
	m_ThreadMsg->RcvBuffer[MESSAGE_BUFFER - 1] = '\0';
	m_ThreadMsg->RcvBuffer[MESSAGE_BUFFER] = '\0';         // ��Ϊ��Unicode�����԰����������ֽڶ���Ϊ \0
	char s[12];
	sprintf_s(s, "%d", m_ThreadMsg->radioID);
	std::string stringId = s;
	if (ret == TEXTLENTH_1 || ret == TEXTLENTH_2)
	{
		//iBytes = 5: TMS Service Availability Acknowledgement 
		//iBytes = 6: TMS Acknowledgement 
		//�յ���̨������ȷ����Ϣʱ����������
		//return 0;
		
			TextMsg			HandleMsg;
			FirstHeader		FstHeader;
			memset((char*)&HandleMsg, 0, sizeof(HandleMsg));
			memcpy((char*)&HandleMsg, m_ThreadMsg->RcvBuffer, MESSAGE_BUFFER);
			HandleMsg.MsgSize = ntohs(HandleMsg.MsgSize);             // �������ϴ��������Ǵ�˶����16λ���ݣ�������Ҫת����x86��С�˶�������
			FstHeader = HandleMsg.FstHeader;
		
			UINT8 SeqNum = GetSeqNumber(&HandleMsg);
			memset(m_ThreadMsg->RcvBuffer, 0, MESSAGE_BUFFER);		  // ���յ�����Ϣ�Ѿ��������������������ý���Buffer��Ϊ����ACK��Buffer�������Ҫ��ʹ�õ�ʱ�����
			std::list<Command>::iterator it;
			m_timeOutListLocker.lock();
			for (it = timeOutList.begin(); it != timeOutList.end(); ++it)
			{
				if (it->ackNum == SeqNum)
				{
					if (myCallBackFunc != NULL)
					{
					/*	ArgumentType args;
						args["Source"] = FieldValue(NULL);
						args["Target"] = FieldValue(m_ThreadMsg->radioID);
						args["contents"] = FieldValue("");
						args["status"] = FieldValue(REMOTE_SUCESS);
						if (it->command == SEND_PRIVATE_MSG)
						{
							args["type"] = FieldValue(PRIVATE);
						}
						else if (it->command == SEND_GROUP_MSG)
						{
							args["type"] = FieldValue(GROUP);
						}						
						std::string callJsonStr = CRpcJsonParser::buildCall("messageStatus", ++seq, args, "radio");
						pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());*/
						
						Respone r;
						r.target = m_ThreadMsg->radioID;
						r.msgStatus = SUCESS;
						r.msg = "";
						onData(myCallBackFunc,it->tp, ++it->callId, it->command, r);
						it = timeOutList.erase(it);
						break;
					}
					
				}
				
			}
			m_timeOutListLocker.unlock();
			
	}
	else if (ret > TEXTLENTH_2)  //User Text Message
	{
		//���յ����Żظ�ack
		TextMsg			HandleMsg;
		FirstHeader		FstHeader;
		memset((char*)&HandleMsg, 0, sizeof(HandleMsg));
		memcpy((char*)&HandleMsg, m_ThreadMsg->RcvBuffer, MESSAGE_BUFFER);
		HandleMsg.MsgSize = ntohs(HandleMsg.MsgSize);             // �������ϴ��������Ǵ�˶����16λ���ݣ�������Ҫת����x86��С�˶�������
		FstHeader = HandleMsg.FstHeader;

		if (FstHeader.AckRequired)
		{														  // ���յ���Text Message�ظ�һ��ACK

			UINT8 SeqNum = GetSeqNumber(&HandleMsg);
			memset(m_ThreadMsg->RcvBuffer, 0, MESSAGE_BUFFER);		  // ���յ�����Ϣ�Ѿ��������������������ý���Buffer��Ϊ����ACK��Buffer�������Ҫ��ʹ�õ�ʱ�����
			ReplyMsgACK(m_ThreadMsg, SeqNum);
		}
		if (!FstHeader.Control)
		{

			if (myCallBackFunc != NULL)
			{
				int len = 0;

				
				std::string message = ParseUserMsg(&HandleMsg, &len);
			
				Respone r;
				r.source = m_ThreadMsg->radioID;
				r.msgStatus = SUCESS;
				r.msg = message;
				onData(myCallBackFunc, peer, seq , RECV_MSG, r);
			
#if DEBUG_LOG
				LOG(INFO) << "���ն���  ondata ";

#endif
			}
			//try
//			{
//				//CString cstrTime = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");                         //��ȡϵͳʱ��
//				time_t t = time(0);
//				tm timeinfo;
//				char tmp[64];
//				localtime_s(&timeinfo, &t);
//				strftime(tmp, sizeof(tmp), "%Y/%m/%d %H:%M:%S", &timeinfo);
//				std::string message = ParseUserMsg(&HandleMsg, 0);
//				std::string strTime = tmp;
//				char radioID[512];
//				sprintf_s(radioID, 512, "%d", m_ThreadMsg->radioID);
//				//cstring to string   time
//				//string strTime = WChar2Ansi(cstrTime.GetBuffer(cstrTime.GetLength()));
//				//cstring to string   message 
//				//string strMsg = WChar2Ansi(message.GetBuffer(message.GetLength()));
//				ArgumentType args;
//				args["Source"] = FieldValue(radioID);
//				args["contents"] = FieldValue(message.c_str());
//				args["type"] = FieldValue(PRIVATE);
//				std::string callJsonStrRes = CRpcJsonParser::buildCall("message", ++seq, args,"radio");
//		
//				if (pRemotePeer != NULL)
//				{
//					pRemotePeer->sendResponse((const char *)callJsonStrRes.c_str(), callJsonStrRes.size());
//#if DEBUG_LOG
//					LOG(INFO) << "���յ����� �� " + callJsonStrRes;
//#endif
//				}
//				else
//				{
//#if DEBUG_LOG
//					LOG(INFO) << "���յ����ţ����Ǵ˶��ŵ�Ŀ�ĵ�û����tcp���ӣ�";
//#endif
//				}
//
				//�鿴״̬��״̬�����ı�ʱ��֪ͨ��Tserver
	
				if (radioStatus.find(stringId) == radioStatus.end())
				{
					RadioStatus st;
					st.id = m_ThreadMsg->radioID;
					st.status = RADIO_STATUS_ONLINE;
					radioStatus[stringId] = st;
					Respone r;
					r.source = m_ThreadMsg->radioID;
					r.arsStatus = SUCESS;
					onData(myCallBackFunc, peer, ++seq, RADIO_ARS, r);
				}
				else if (radioStatus[stringId].status == RADIO_STATUS_OFFLINE)
				{
					radioStatus[stringId].status = RADIO_STATUS_ONLINE;
					Respone r;
					r.source = m_ThreadMsg->radioID;
					r.arsStatus = SUCESS;
					onData(myCallBackFunc, peer, ++seq, RADIO_ARS, r);
				}
//			}
//			catch (std::exception e)
//			{
//
//			}
		} 
	}
}

std::string CTextMsg::WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0) return std::string("");
	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");
	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;
	std::string strTemp(pszDst);
	return strTemp;
}
void CTextMsg::setRemotePeer(CRemotePeer * pRemote)
{
	pRemotePeer = pRemote;
}
