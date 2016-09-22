#include "stdafx.h"
#include "TextMsg.h"



#pragma comment(lib, "wsock32.lib")


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
bool CTextMsg::InitSocket(SOCKET *s ,DWORD dwAddress)
{

	//CString			 strError;
	SOCKADDR_IN      addr;					//   The   local   interface   address   
	WSADATA			 wsda;					//   Structure   to   store   info

	CloseSocket(&m_ThreadMsg->mySocket);
	BOOL bReuseaddr = FALSE;
	setsockopt(m_ThreadMsg->mySocket, SOL_SOCKET, SO_DONTLINGER, (const char*)&bReuseaddr, sizeof(BOOL));

	int ret = WSAStartup(MAKEWORD(1, 1), &wsda);     //   Load   version   1.1   of   Winsock

	*s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);   //   Create   an   UDP   socket

	if (*s == SOCKET_ERROR)				//   Socket create Error
	{
		//AfxMessageBox(_T("Socket��ʼ������"));
		CloseSocket(s);

		return FALSE;
	}

	//   Fill   in   the   interface   information  


	addr.sin_family = AF_INET;
	addr.sin_port = htons(4007);
	addr.sin_addr.s_addr = dwAddress;
	ret = ::bind(*s, (struct sockaddr *) &addr, sizeof(addr));
	if ( ret== SOCKET_ERROR)
	{
		int b = WSAGetLastError();
		//AfxMessageBox(_T("�󶨶˿ڴ���"));
		CloseSocket(s);
		return FALSE;
	}
	m_ThreadMsg->mySocket = *s;
	m_RcvSocketOpened = true;
	CreateThread(NULL, 0, ReceiveDataThread, this, THREAD_PRIORITY_NORMAL +1, NULL);
	//AfxBeginThread(ReceiveDataThread, (LPVOID)&m_ThreadMsg, THREAD_PRIORITY_NORMAL);
	//AfxBeginThread(RecvThread,(LPVOID)&m_UDPThreadMsg,THREAD_PRIORITY_NORMAL);
	return TRUE;
}


bool CTextMsg::CloseSocket(SOCKET* s)
{
	if (m_RcvSocketOpened)        // ֻ����ǰ���Ѿ����ˣ����б�Ҫ�رգ�����û�б�Ҫ��
	{
		closesocket(*s);							        // Close socket

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

CString CTextMsg::ParseUserMsg(TextMsg* HandleMsg ,int * len)
{
	UINT16			MsgSize;
	FirstHeader		FstHeader;
	SecondHeader    callIddHeader;
	UINT8			AddressSize;
	static TCHAR	szMessage[MAX_MESSAGE_LENGTH];
	CString         ParsedMsg;
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

	//memcpy((char*)szMessage, &HandleMsg->TextPayload[MsgOffset], MsgSize - MsgOffset - 2);
	//*len = MsgSize - MsgOffset - 2;
	//ParsedMsg = szMessage;
	memcpy((char*)szMessage, &HandleMsg->TextPayload[MsgOffset], MsgSize - MsgOffset - 2);
	ParsedMsg.Format(_T("%s"), szMessage);
	

	return ParsedMsg;
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
	wcscpy_s((LPTSTR)&buf[OffSet], len, message);
	memcpy(m_ThreadMsg->SendBuffer, buf, m_ThreadMsg->MsgLength);

	//��m_nSendSequenceNumberд��list
	list<AllCommand>::iterator it;
	for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
	{
		if (it->callId == callId)
		{
			it->ackNum = m_nSendSequenceNumber;
		}
	}
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
			list<AllCommand>::iterator it;
			for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
			{
				if (it->ackNum == SeqNum)
				{
					/*if (myCallBackFunc != NULL)
					{
						unsigned char str[30] = { 0 };
						sprintf_s((char *)str, sizeof(str), "result:0");
						onData(myCallBackFunc, it->seq, SEND_PRIVATE_MSG, (char *)str, sizeof(str));
#if DEBUG_LOG
						LOG(INFO) << "���ն��ŷ��ͳɹ�ack  ondata ";
#endif
					}*/
					//if (pDispatchPort != NULL)
					{
						////ƴ��json
						//rapidjson::Document document;
						//Document::AllocatorType& allocator = document.GetAllocator();
						//Value root(kObjectType);
						//root.AddMember("callId", it->callId, allocator);
						//root.AddMember("status", 1, allocator);                //1:���ͳɹ�
						//StringBuffer buffer;
						//Writer<StringBuffer> writer(buffer);
						//root.Accept(writer);
						//std::string reststring = buffer.GetString();
						//pDispatchPort->sendResultToClient(reststring);
					}
					allCommandList.erase(it++);
				}
				break;
			}
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
			CString  strID;
			strID.Format(_T("%lu"), m_ThreadMsg->radioID);
			CString message = ParseUserMsg(&HandleMsg, 0);
//			if (myCallBackFunc != NULL)
//			{
//				int len = 0;
//
//				CString str, strID;
//				CString message = ParseUserMsg(&HandleMsg, &len);
//				strID.Format(_T("%lu"), m_ThreadMsg->radioID);
//				str = "id:" + strID + ";content:" + message;
//
//
//				len = WideCharToMultiByte(CP_ACP, 0, str, wcslen(str), NULL, 0, NULL, NULL);
//				char * m_char = new char[len + 1];
//				WideCharToMultiByte(CP_ACP, 0, str, wcslen(str), m_char, len, NULL, NULL);
//				m_char[len] = '\0';
//
//
//				//unsigned char* data = (unsigned char*)(LPCTSTR)str;
//				onData(myCallBackFunc, 1, RECV_MSG, m_char, len);
//				delete[] m_char;
//#if DEBUG_LOG
//				LOG(INFO) << "���ն���  ondata ";
//				
//#endif
//			}
			//if (pDispatchPort != NULL)
			{
				//ƴ��json

				////ƴ��json
				//rapidjson::Document document;
				//Document::AllocatorType& allocator = document.GetAllocator();
				//Value root(kObjectType);
				//Value child(kObjectType);
				//child.AddMember("type", "onRecvMsg", allocator);
				//child.AddMember("srcRadioID", strID, allocator);
				//CString strTime = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");                         //��ȡϵͳʱ��
				//child.AddMember("date", strTime, allocator);
				//child.AddMember("message", message, allocator);
				//root.AddMember("result", child, allocator);
				//StringBuffer buffer;
				//Writer<StringBuffer> writer(buffer);
				//root.Accept(writer);
				//std::string reststring = buffer.GetString();
				//pDispatchPort->sendResultToClient(reststring);

			}
			 
		}
		
	}
	
}



