#include "stdafx.h"
#include "TextMsg.h"
#include  "time.h"

#include "../lib/rpc/include/RpcJsonParser.h"
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
bool CTextMsg::InitSocket(SOCKET *s, DWORD dwAddress, CRemotePeer * pRemote)
{

	pRemotePeer = pRemote;
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
		//AfxMessageBox(_T("Socket初始化错误！"));
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
		//AfxMessageBox(_T("绑定端口错误！"));
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
	if (m_RcvSocketOpened)        // 只有在前面已经打开了，才有必要关闭，否则没有必要了
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

string CTextMsg::ParseUserMsg(TextMsg* HandleMsg, int * len)
{
	UINT16			MsgSize;
	FirstHeader		FstHeader;
	SecondHeader    callIddHeader;
	UINT8			AddressSize;
	static TCHAR	szMessage[MAX_MESSAGE_LENGTH];
	string         ParsedMsg;
	int             MsgOffset;                                // 正式的Message在TextPayload中的起始偏移量

	memset((char*)szMessage, 0, sizeof(szMessage));             // 不知道这里针对 unicode 使用 sizeof 是否正确。

	MsgSize = HandleMsg->MsgSize;
	FstHeader = HandleMsg->FstHeader;
	AddressSize = HandleMsg->AddressSize;

	MsgOffset = 0;

	if (AddressSize)
	{ // 在收到的数据中带有Address
		MsgOffset = MsgOffset + AddressSize;   // 加上Address所占用字节的长度

		if (FstHeader.ExtensionBit)                  // 存在 Subsequent Header， Subsequent Header可能存在2个
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
	{ // 在收到的信息中不带Address
		if (FstHeader.ExtensionBit)                  // 存在 Subsequent Header， Subsequent Header可能存在2个
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
	ParsedMsg = TCHAR2STRING( szMessage);
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
	{ // 在收到的数据中带有Address
		Address = new char[AddressSize];             // 动态分配内存，用于保存 Address
		memcpy(Address, HandleMsg->TextPayload, AddressSize);

		OffSet = AddressSize;                       // 加上Address所占用字节的长度,之前的头信息并不存放在TextPayload中，因此这里计算偏移量的时候不应该包括他们的长度

		if (FstHeader.ExtensionBit)                  // 存在 Subsequent Header， Subsequent Header可能存在多个
		{
			memcpy((char*)&callIddHeader, &HandleMsg->TextPayload[OffSet], sizeof(SecondHeader));
			TxtSeqNum = callIddHeader.SequenceNum;       // 取得接收到的Message序列号中的低5位
			OffSet = OffSet + sizeof(SecondHeader);

			if (callIddHeader.Extension)
			{
				memcpy((char*)&TrdHeader, &HandleMsg->TextPayload[OffSet], sizeof(ThirdHeader));
				UINT8 MsbSeq = TrdHeader.SequenceNum; // 取得接收到的Message序列号中的高2位
				TxtSeqNum = TxtSeqNum | (MsbSeq << 5);
			}
		}

		delete[] Address;                                    // 释放所分配的内存
	}
	else
	{ // 在收到的信息中不带Address
		if (FstHeader.ExtensionBit)                  // 存在 Subsequent Header
		{
			memcpy((char*)&callIddHeader, &HandleMsg->TextPayload[OffSet], sizeof(SecondHeader));
			TxtSeqNum = callIddHeader.SequenceNum;       // 取得接收到的Message序列号中的低5位
			OffSet = OffSet + sizeof(SecondHeader);

			if (callIddHeader.Extension)
			{
				memcpy((char*)&TrdHeader, &HandleMsg->TextPayload[OffSet], sizeof(ThirdHeader));
				UINT8 MsbSeq = TrdHeader.SequenceNum; // 取得接收到的Message序列号中的高2位
				TxtSeqNum = TxtSeqNum | (MsbSeq << 5);
			}
		}
	}

	return TxtSeqNum;
}


bool CTextMsg::ReplyMsgACK(ThreadMsg* Msg, UINT8 SeqNumber)
{
	//构造ACK
	UINT16			MsgSize;
	FirstHeader		FstHeader;
	UINT8			AddressSize;
	SecondHeader    callIddHeader;
	ThirdHeader     TrdHeader;
	int             OffSet;

	//填充First Header的数据
	FstHeader.ExtensionBit = 1;
	FstHeader.AckRequired = 0;
	FstHeader.Reserved = 0;
	FstHeader.Control = 1;
	FstHeader.PDUType = 0xF;

	UINT8 nTemp;
	memcpy((char*)&nTemp, (char*)&FstHeader, 1);

	MsgSize = sizeof(FirstHeader);

	// AddressSize 和 Address 字段部分在ACK中不需要，因此不填写
	AddressSize = 0;
	MsgSize = MsgSize + sizeof(AddressSize);

	if (SeqNumber <= 31)               //Sequence Nuber 只需要Second Header
	{
		callIddHeader.Extension = 1;
		callIddHeader.SequenceNum = SeqNumber;
		callIddHeader.Reserved = 0;

		TrdHeader.Extension = 0;
		TrdHeader.SequenceNum = 0;    // Sequence Number 的 高2位
		TrdHeader.Reserved = 0;

		MsgSize = MsgSize + sizeof(SecondHeader)+sizeof(ThirdHeader);
	}
	else
	{                                 //Sequence Number 需要Second Header 和 Third Header
		callIddHeader.Extension = 1;
		callIddHeader.SequenceNum = (SeqNumber & 0x1F);        // Sequence Number 的 低5位
		callIddHeader.Reserved = 0;

		TrdHeader.Extension = 0;
		TrdHeader.SequenceNum = ((SeqNumber >> 5) & 0x3);    // Sequence Number 的 高2位
		TrdHeader.Reserved = 0;

		MsgSize = MsgSize + sizeof(SecondHeader)+sizeof(ThirdHeader);
	}

	Msg->MsgLength = MsgSize + sizeof(MsgSize);

	OffSet = 0;
	MsgSize = htons(MsgSize);                                                       // 转换成Message消息所需要的大端形式
	memcpy(&Msg->RcvBuffer[OffSet], (char*)&MsgSize, sizeof(MsgSize));              // 填充Message Size

	OffSet = OffSet + sizeof(MsgSize);
	memcpy(&Msg->RcvBuffer[OffSet], (char*)&FstHeader, sizeof(FirstHeader));        // 填充First Header

	OffSet = OffSet + sizeof(FirstHeader);
	memcpy(&Msg->RcvBuffer[OffSet], (char*)&AddressSize, sizeof(AddressSize));      // 填充AddressSize

	OffSet = OffSet + sizeof(AddressSize);
	memcpy(&Msg->RcvBuffer[OffSet], (char*)&callIddHeader, sizeof(SecondHeader));   // 填充SecondHeader

	OffSet = OffSet + sizeof(SecondHeader);
	memcpy(&Msg->RcvBuffer[OffSet], (char*)&TrdHeader, sizeof(ThirdHeader));    // 填充ThirdHeader

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
	//构造ACK
	UINT16			MsgSize;
	FirstHeader		FstHeader;
	UINT8			AddressSize;
	SecondHeader    callIddHeader;
	ThirdHeader     TrdHeader;
	int             OffSet;
	char			buf[1024];


	//填充First Header的数据
	FstHeader.ExtensionBit = 1;
	FstHeader.AckRequired = 1;
	FstHeader.Reserved = 1;
	FstHeader.Control = 0;
	FstHeader.PDUType = 0;

	UINT8 nTemp;
	memcpy((char*)&nTemp, (char*)&FstHeader, 1);

	MsgSize = sizeof(FirstHeader);

	// AddressSize 和 Address 字段部分在ACK中不需要，因此不填写
	AddressSize = 0;
	MsgSize = MsgSize + sizeof(AddressSize);

	callIddHeader.Extension = 1;
	callIddHeader.SequenceNum = m_nSendSequenceNumber & 0x1f;
	callIddHeader.Reserved = 0;

	TrdHeader.Extension = 0;
	TrdHeader.SequenceNum = (m_nSendSequenceNumber >> 5) & 0x03;    // Sequence Number 的 高2位
	TrdHeader.Reserved = 0x04;	  // UCS-LE format

	MsgSize = MsgSize + sizeof(SecondHeader)+wcslen(message) * 2 + sizeof(ThirdHeader);

	m_ThreadMsg->MsgLength = MsgSize + sizeof(MsgSize);

	OffSet = 0;
	MsgSize = htons(MsgSize);                                                       // 转换成Message消息所需要的大端形式
	memcpy(&buf[OffSet], (char*)&MsgSize, sizeof(MsgSize));              // 填充Message Size

	OffSet = OffSet + sizeof(MsgSize);
	memcpy(&buf[OffSet], (char*)&FstHeader, sizeof(FirstHeader));        // 填充First Header

	OffSet = OffSet + sizeof(FirstHeader);
	memcpy(&buf[OffSet], (char*)&AddressSize, sizeof(AddressSize));      // 填充AddressSize

	OffSet = OffSet + sizeof(AddressSize);
	memcpy(&buf[OffSet], (char*)&callIddHeader, sizeof(SecondHeader));   // 填充SecondHeader

	OffSet = OffSet + sizeof(SecondHeader);
	memcpy(&buf[OffSet], (char*)&TrdHeader, sizeof(ThirdHeader));    // 填充ThirdHeader

	OffSet = OffSet + sizeof(ThirdHeader);


	int len = wcslen(message) * 2;
	wcscpy_s((LPTSTR)&buf[OffSet], len, message);
	memcpy(m_ThreadMsg->SendBuffer, buf, m_ThreadMsg->MsgLength);

	//将m_nSendSequenceNumber写入list
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
		LOG(INFO) << "发送短信失败";
#endif
		return false;
	}
#if DEBUG_LOG
	LOG(INFO) << "发送短信成功 ";
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
	m_ThreadMsg->RcvBuffer[MESSAGE_BUFFER] = '\0';         // 因为是Unicode，所以把最后的两个字节都置为 \0
	char s[12];
	sprintf_s(s, "%d", m_ThreadMsg->radioID);
	string stringId = s;
	if (ret == TEXTLENTH_1 || ret == TEXTLENTH_2)
	{
		//iBytes = 5: TMS Service Availability Acknowledgement 
		//iBytes = 6: TMS Acknowledgement 
		//收到电台发来的确认消息时，不做处理
		//return 0;
		
			TextMsg			HandleMsg;
			FirstHeader		FstHeader;
			memset((char*)&HandleMsg, 0, sizeof(HandleMsg));
			memcpy((char*)&HandleMsg, m_ThreadMsg->RcvBuffer, MESSAGE_BUFFER);
			HandleMsg.MsgSize = ntohs(HandleMsg.MsgSize);             // 从网络上传过来的是大端对齐的16位数据，这里需要转换成x86的小端对齐数据
			FstHeader = HandleMsg.FstHeader;
		
			UINT8 SeqNum = GetSeqNumber(&HandleMsg);
			memset(m_ThreadMsg->RcvBuffer, 0, MESSAGE_BUFFER);		  // 接收到的消息已经被拷贝出来，这里利用接收Buffer作为发送ACK的Buffer，因此需要在使用的时候清空
			list<AllCommand>::iterator it;
			m_allCommandListLocker.lock();
			for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
			{
				if (it->ackNum == SeqNum)
				{
					if (pRemotePeer != NULL&& pRemotePeer == it->pRemote)
					{
						std::map<std::string, std::string> args;
						args["id"] = stringId;
						std::string callJsonStr = CRpcJsonParser::buildResponse("1", it->callId, 0, "1", args);
						pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
					}
					allCommandList.erase(it++);
				}
				break;
			}
			m_allCommandListLocker.unlock();
	}
	else if (ret > TEXTLENTH_2)  //User Text Message
	{
		//接收到短信回复ack
		TextMsg			HandleMsg;
		FirstHeader		FstHeader;
		memset((char*)&HandleMsg, 0, sizeof(HandleMsg));
		memcpy((char*)&HandleMsg, m_ThreadMsg->RcvBuffer, MESSAGE_BUFFER);
		HandleMsg.MsgSize = ntohs(HandleMsg.MsgSize);             // 从网络上传过来的是大端对齐的16位数据，这里需要转换成x86的小端对齐数据
		FstHeader = HandleMsg.FstHeader;

		if (FstHeader.AckRequired)
		{														  // 对收到的Text Message回复一个ACK

			UINT8 SeqNum = GetSeqNumber(&HandleMsg);
			memset(m_ThreadMsg->RcvBuffer, 0, MESSAGE_BUFFER);		  // 接收到的消息已经被拷贝出来，这里利用接收Buffer作为发送ACK的Buffer，因此需要在使用的时候清空
			ReplyMsgACK(m_ThreadMsg, SeqNum);
		}
		if (!FstHeader.Control)
		{
			try
			{
				//CString cstrTime = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");                         //获取系统时间
				time_t t = time(0);
				tm timeinfo;
				char tmp[64];
				localtime_s(&timeinfo, &t);
				strftime(tmp, sizeof(tmp), "%Y/%m/%d  %H:%M:%S", &timeinfo);
				string message = ParseUserMsg(&HandleMsg, 0);
				string strTime = tmp;
				char radioID[512];
				sprintf_s(radioID, 512, "%d", m_ThreadMsg->radioID);
				//cstring to string   time
				//string strTime = WChar2Ansi(cstrTime.GetBuffer(cstrTime.GetLength()));
				//cstring to string   message 
				//string strMsg = WChar2Ansi(message.GetBuffer(message.GetLength()));
				std::map<std::string, std::string> args;
				args["id"] = radioID;
				args["date"] = strTime;
				args["message"] = message;
				std::string callJsonStr = CRpcJsonParser::buildCall("onRecvMsg", 1, args);
				if (pRemotePeer != NULL)
				{
					pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
#if DEBUG_LOG
					LOG(INFO) << "接收到短信 ： " + callJsonStr;
#endif
				}
				else
				{
#if DEBUG_LOG
					LOG(INFO) << "接收到短信，但是此短信的目的地没建立tcp连接！";
#endif
				}
			}
			catch (std::exception e)
			{

			}
		} 
	}
}

string CTextMsg::WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0) return std::string("");
	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");
	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;
	std::string strTemp(pszDst);
	delete[] pszDst;
	return strTemp;
}
void CTextMsg::setRemotePeer(CRemotePeer * pRemote)
{
	pRemotePeer = pRemote;
}
