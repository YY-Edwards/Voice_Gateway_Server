#include "stdafx.h"

AllCommand      m_allCommand;
list <AllCommand>allCommandList;

DispatchOperate::DispatchOperate()
{
	m_queryMode = -1;
	pXnlConnection = NULL;
	dwIP = 0;
	dwip = 0;
	textConnectResult = false;
	ARSConnectResult = false;
	GPSConnectResult = false;
	callID = -1;

}
//DispatchOperate::DispatchOperate(CRemotePeer * pRemotePeer)
//{
//	m_queryMode = -1;
//	pXnlConnection = NULL;
//	dwIP = 0;
//	dwip = 0;
//	textConnectResult = false;
//	ARSConnectResult = false;
//	GPSConnectResult = false;
//
//}

DispatchOperate::~DispatchOperate()
{
}
int DispatchOperate::Connect(CRemotePeer* pRemote,const char* ip, const char * pIP, int callId)
{
	// Connection
	
	callID = callId;
	AddAllCommand(pRemote,callId, RADIO_CONNECT);
	if (INADDR_NONE == inet_addr(pIP))
	{
		dwIP = inet_addr(ip);
		unsigned char iptemp[4];
		memcpy(iptemp, (void *)&dwIP, 4);
		iptemp[3] = 1;
		dwip = *((DWORD *)iptemp);

	}
	else
	{
		dwIP = inet_addr(pIP);
		dwip = inet_addr(ip);
		unsigned char iptemp[4];
		memcpy(iptemp, (void *)&dwip, 4);
		iptemp[3] = 1;
		dwip = *((DWORD *)iptemp);
	}
	CreateThread(NULL, 0, TCPConnectionThread, this, THREAD_PRIORITY_NORMAL, NULL);
	CreateThread(NULL, 0, TimeOutThread, this, THREAD_PRIORITY_NORMAL, NULL);
	if (pRemote != NULL)
	{
		pRemotePeer = pRemote;
	}
	return 0;

}
int DispatchOperate::call(CRemotePeer* pRemote, int id, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "单呼";
#endif
	unsigned long target_radio_id = id;
	unsigned char call_type = 0x04;     //0x06 单呼
	AddAllCommand(pRemote,callId, PRIVATE_CALL);

	try
	{
		if (pXnlConnection != NULL)
		{
			if (target_radio_id != 0)
			{
				pXnlConnection->setRemotePeer(pRemote);
				pXnlConnection->send_xcmp_call_ctrl_request(0x01, call_type, 0x01, target_radio_id, 0);
				pXnlConnection->send_xcmp_tx_ctrl_request(0x01, 0x00);


				return 0;
			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "单呼失败";
#endif
	}

	return 1;
}
int DispatchOperate::groupCall(CRemotePeer* pRemote, int id, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "组呼";
#endif
	unsigned long target_radio_id = id;
	unsigned char call_type = 0x06;     //0x06 组呼
	AddAllCommand(pRemote,callId, GROUP_CALL);
	try
	{

		if ((pXnlConnection != NULL) && (pXnlConnection->m_bConnected == TRUE))
		{
			if (target_radio_id != 0)
			{
				pXnlConnection->setRemotePeer(pRemote);
				pXnlConnection->send_xcmp_call_ctrl_request(0x01, call_type, 0x01, 0, target_radio_id);
				pXnlConnection->send_xcmp_tx_ctrl_request(0x01, 0x00);

				return 0;
			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "组呼失败";
#endif
	}

	return 1;
}
int DispatchOperate::allCall(CRemotePeer* pRemote, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "全呼";
#endif
	unsigned long target_radio_id = 255;   //全呼
	unsigned char call_type = 0x06;
	AddAllCommand(pRemote,callId, ALL_CALL);
	try
	{
		if ((pXnlConnection != NULL) && (pXnlConnection->m_bConnected == TRUE))
		{
			if (target_radio_id != 0)
			{
				pXnlConnection->setRemotePeer(pRemote);
				pXnlConnection->send_xcmp_call_ctrl_request(0x01, call_type, 0x01, 0, target_radio_id);
				pXnlConnection->send_xcmp_tx_ctrl_request(0x01, 0x00);

				return 0;
			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "全呼失败";
#endif
	}
	return 1;

}
int DispatchOperate::stopCall(CRemotePeer* pRemote, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "停止呼叫";
#endif
	unsigned char call_type = 0x04;
	AddAllCommand(pRemote,callId, STOP_CALL);
	try
	{
		if (pXnlConnection != NULL)
		{
			pXnlConnection->setRemotePeer(pRemote);
			pXnlConnection->send_xcmp_tx_ctrl_request(0x02, 0x00);

			return 0;

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "停止呼叫失败";
#endif
	}

	return 1;
}
int DispatchOperate::remotePowerOn(CRemotePeer* pRemote, int id, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "遥开";
#endif
	unsigned long target_radio_id = 0;
	target_radio_id = id;
	AddAllCommand(pRemote,callId, REMOTE_OPEN);
	try
	{
		if (pXnlConnection != NULL)
		{
			if (target_radio_id != 0)
			{
				pXnlConnection->setRemotePeer(pRemote);
				pXnlConnection->send_xcmp_rmt_radio_ctrl_request(0x02, 0x01, 0x01, target_radio_id);

				return 0;
			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "遥开失败";
#endif
	}

	return 1;
}
int DispatchOperate::remotePowerOff(CRemotePeer* pRemote, int id, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "遥闭";
#endif
	unsigned long target_radio_id = id;
	AddAllCommand(pRemote,callId, REMOTE_CLOSE);
	try
	{
		if (pXnlConnection != NULL)
		{

			if (target_radio_id != 0)
			{
				pXnlConnection->setRemotePeer(pRemote);
				pXnlConnection->send_xcmp_rmt_radio_ctrl_request(0x01, 0x01, 0x01, target_radio_id);

			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "遥闭失败";
#endif
	}
	return 1;
}
int DispatchOperate::radioCheck(CRemotePeer* pRemote, int id, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "在线检测";
#endif
	unsigned long target_radio_id = 0;
	target_radio_id = id;
	AddAllCommand(pRemote,callId, CHECK_RADIO_ONLINE);
	try
	{
		if (pXnlConnection != NULL)
		{

			if (target_radio_id != 0)
			{
				pXnlConnection->setRemotePeer(pRemote);
				pXnlConnection->send_xcmp_rmt_radio_ctrl_request(0x00, 0x01, 0x01, target_radio_id);

				return 0;
			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "在线检测失败";
#endif
	}
	return 1;

}
int DispatchOperate::wiretap(CRemotePeer* pRemote, int id, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "远程监听";
#endif
	unsigned long target_radio_id = 0;
	target_radio_id = id;
	AddAllCommand(pRemote,callId, REMOTE_MONITOR);
	try
	{
		if ((pXnlConnection != NULL) && (pXnlConnection->m_bConnected == TRUE))
		{

			if (target_radio_id != 0)
			{
				pXnlConnection->setRemotePeer(pRemote);
				pXnlConnection->send_xcmp_rmt_radio_ctrl_request(0x03, 0x01, 0x01, target_radio_id);

				return 0;
			}
			else
			{
				return 1;
			}
		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "远程监听失败";
#endif
	}
	return 1;

}
int DispatchOperate::sendSms(CRemotePeer* pRemote, int id, wchar_t* message, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "单发";
#endif
	AddAllCommand(pRemote,callId, SEND_PRIVATE_MSG);
	try
	{
		pTextMsg.setRemotePeer(pRemote);
		bool sendTextResult = pTextMsg.SendMsg(callId, message, id, 12);             //12:单发
		
		if (sendTextResult)
		{

			return 0;
		}
		else
		{
			return 1;
		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "单发失败";
#endif
	}
	

}
int DispatchOperate::sendGroupSms(CRemotePeer* pRemote, int id, wchar_t* message, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "组发";
#endif
	AddAllCommand(pRemote,callId, SEND_GROUP_MSG);
	pTextMsg.setRemotePeer(pRemote);
	bool sendTextResult = pTextMsg.SendMsg(callId, message, id, 225);          //225:组发
	try
	{
		if (sendTextResult)
		{


			return 0;
		}
		else
		{
			return 1;
		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "组发失败";
#endif
	}

}
int DispatchOperate::getGps(CRemotePeer* pRemote, int id, int queryMode, int cycle, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "gps查询";
#endif
	gpsDic[id] = queryMode;


	//m_queryMode = queryMode;
	try
	{


		switch (queryMode)
		{
		case GPS_IMME_COMM:
			AddAllCommand(pRemote,callId, GPS_IMME_COMM);
			break;
		case GPS_TRIGG_COMM:
			AddAllCommand(pRemote,callId, GPS_TRIGG_COMM);
			break;
		case GPS_IMME_CSBK:
			AddAllCommand(pRemote,callId, GPS_IMME_CSBK);
			break;
		case GPS_TRIGG_CSBK:
			AddAllCommand(pRemote,callId, GPS_TRIGG_CSBK);
			break;
		case GPS_IMME_CSBK_EGPS:
			AddAllCommand(pRemote,callId, GPS_IMME_CSBK_EGPS);
			break;
		case GPS_TRIGG_CSBK_EGPS:
			AddAllCommand(pRemote,callId, GPS_TRIGG_CSBK_EGPS);
			break;
		}
		bool sendQueryGpsResult = pRadioGPS.SendQueryGPS(id, queryMode, cycle);
		if (sendQueryGpsResult)
		{


			return 0;
		}
		else
		{
			return 1;
		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "gps查询失败";
#endif
	}

}

int DispatchOperate::cancelPollGps(CRemotePeer* pRemote, int id, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "取消周期查询";
#endif
	AddAllCommand(pRemote,callId, STOP_QUERY_GPS);
	m_queryMode = gpsDic[id];
	try
	{
		bool stopQueryGPSResult = pRadioGPS.StopQueryTriggeredGPS(id, m_queryMode);

		if (stopQueryGPSResult)
		{

			return 0;
		}
		else
		{
			return 1;
		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "取消周期查询失败";
#endif
	}
}
int DispatchOperate::getOverturnGps(CRemotePeer* pRemote, const char* ip, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "gps翻转";
#endif
	try
	{
		bool result = pRadioGPS.InitGPSOverturnSocket(inet_addr(ip));
		if (result)
		{

			return 0;
		}
		else
		{
			return 1;
		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "";
#endif
	}
}
DWORD WINAPI DispatchOperate::TCPConnectionThread(LPVOID lpParam)
{
	DispatchOperate * pDispatchOperate = new DispatchOperate;
	pDispatchOperate = (DispatchOperate *)lpParam;
	pDispatchOperate->RadioConnect();
	//pDispatchOperate->TcpConnect();

	return 1;
}
int DispatchOperate::RadioConnect()
{

	//udp Connection
	//text  Connection                                      //0:连接成功  1：udp 连接失败  2：tcp 连接失败  3： udp、tcp均失败
	SOCKET mSokset;
	//if (!textConnectResult)
	{
		textConnectResult = pTextMsg.InitSocket(&mSokset, dwIP, pRemotePeer);
	}

	//	ARS Connection
	//if (!ARSConnectResult)
	{
		ARSConnectResult = pRadioARS.InitARSSocket(dwIP,pRemotePeer);
	}

	//	GPS Connection
	//if (!GPSConnectResult)
	{
		GPSConnectResult = pRadioGPS.InitGPSSocket(dwIP, pRemotePeer);
	}

	if (pXnlConnection == NULL)
	{
		list<AllCommand>::iterator it;
		pXnlConnection = CXNLConnection::CreatConnection(dwip, 8002, "0x152C7E9D0x38BE41C70x71E96CA40x6CAC1AFC",
			strtoul("0x9E3779B9", NULL, 16));
		
		for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
		{
			{
				if (pXnlConnection == NULL)    //0:调度业务和数据业务都连接失败 1： 调度业务连接成功，数据业务连接失败 2：调度业务连接失败，数据业务连接成功 3. 调度业务和数据业务都连接成功
				{
					if (textConnectResult && ARSConnectResult && GPSConnectResult)
					{
						
#if DEBUG_LOG
						LOG(INFO) << "数据连接成功，调度业务连接失败";
#endif
						list<AllCommand>::iterator it;
						for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
						{
							if (it->callId == callID)
							{
								std::map<std::string, std::string> args;
								std::string callJsonStr = CRpcJsonParser::buildResponse("2", it->callId, 0, "2", args);
								if (pRemotePeer != NULL)
								{
									pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
								}
								allCommandList.erase(it++);
							}
							break;
						}
						return 2;
						break;
					}
					else
					{
						
#if DEBUG_LOG
						LOG(INFO) << "数据连接失败，调度业务连接失败";
#endif
						list<AllCommand>::iterator it;
						for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
						{
							if (it->callId == callID)
							{
								std::map<std::string, std::string> args;
								std::string callJsonStr = CRpcJsonParser::buildResponse("0", it->callId, 0, "0", args);
								if (pRemotePeer != NULL)
								{
									pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
								}
								allCommandList.erase(it++);
							}
							break;
						}
						return 0;
						break;
					}
				}
				else
				{
					if (textConnectResult && ARSConnectResult && GPSConnectResult)
					{
						
#if DEBUG_LOG
						LOG(INFO) << "数据连接成功，调度业务连接成功";
#endif
						list<AllCommand>::iterator it;
						for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
						{
							if (it->callId == callID)
							{
								std::map<std::string, std::string> args;
								std::string callJsonStr = CRpcJsonParser::buildResponse("3", it->callId, 0, "3", args);
								if (pRemotePeer != NULL)
								{
									pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
								}
								allCommandList.erase(it++);
							}
							break;
						}
						return 3;
						break;
					}
					else
					{
						
#if DEBUG_LOG
						LOG(INFO) << "数据连接失败，调度业务连接成功";
#endif
						list<AllCommand>::iterator it;
						for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
						{
							if (it->callId == callID)
							{
								std::map<std::string, std::string> args;
								std::string callJsonStr = CRpcJsonParser::buildResponse("0", it->callId, 1, "1", args);
								if (pRemotePeer != NULL)
								{
									pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
								}
								allCommandList.erase(it++);
							}
							break;
						}
						return 1;
						break;
					}
				}
			}
		}
	}
	return 0;
}

void DispatchOperate::AddAllCommand(CRemotePeer* pRemote,int callId, int command)
{
	m_allCommand.pRemote = pRemote;
	m_allCommand.callId = callId;
	m_allCommand.command = command;
	m_allCommand.ackNum = 0;
	m_allCommand.timeOut = 30000;     //ms
	m_allCommand.timeCount = 0;
	allCommandList.push_back(m_allCommand);
}
DWORD WINAPI DispatchOperate::TimeOutThread(LPVOID lpParam)
{
	DispatchOperate * pDispatchOperate = new DispatchOperate();
	pDispatchOperate = (DispatchOperate *)(lpParam);
	pDispatchOperate->TimeOut();
	return 1;
}
void DispatchOperate::TimeOut()
{
	list<AllCommand>::iterator it;
	while (true)
	{

		for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
		{
			it->timeCount++;
			if (it->timeCount % (it->timeOut / 100) == 0)
			{
				//if (myCallBackFunc != NULL)
				//{
				//	unsigned char str[30] = { 0 };
				//	/*if ((it->command == REMOTE_CLOSE) || (it->command == REMOTE_OPEN) || (it->command == REMOTE_MONITOR) || (it->command == CHECK_RADIO_ONLINE))
				//	{
				//	allCommandList.erase(it);
				//	continue;
				//	}
				//	else*/
				//	//{
				//	sprintf_s((char *)str, sizeof(str), "result:1");
				//	//}

				//	onData(myCallBackFunc, it->seq, it->command, (char *)str, sizeof(str));

				//	for (list<AllCommand>::iterator ittmp = allCommandList.begin(); ittmp != allCommandList.end(); ++ittmp)
				//	{

				//		if ((ittmp->command == SEND_PRIVATE_MSG) || (ittmp->command == SEND_GROUP_MSG))
				//		{
				//			//allCommandList.erase(ittmp);
				//			ittmp->timeCount = 150;
				//			//allCommandList.push_back(*ittmp);
				//		}
				//	}

				//	allCommandList.erase(it);

				//	break;
				//}

			}

		}
		Sleep(100);
	}
}
int DispatchOperate::getLic(const char* licPath)
{


	//char* strmac;
	//strmac = pXnlConnection->readmac;
	//CString  filename(licPath);
	////char buffer[256];
	//unsigned char temp[48];
	//memset(temp, 0, 48);
	////GetCurrentDirectory(256, (LPWSTR)buffer);
	////filename.Format(_T("%s"), licPath);
	////filename += "\\serial.lic";
	//CStdioFile file;
	//CString strText;
	//CString szLine;
	//if (file.Open(filename, CFile::modeRead))
	//{

	//	file.Read(temp, 48);
	//	file.Close();
	//}


	////解密
	//unsigned char key[16];
	//unsigned char input[16];
	//unsigned char output[16];

	//char serial[11];                          //Master  Serial
	//unsigned char date[11];
	//unsigned char peerNum[5];
	//memset(key, 0, 16);
	//memset(input, 0, 16);
	//memset(output, 0, 16);

	//memset(serial, 0, 11);
	//memset(date, 0, 11);
	//memset(peerNum, 0, 5);
	//memcpy(key, "ji1hua!@#567ADef", 16);
	//Aes1 m_Aes1(16, key);
	//for (int i = 0; i <strText.GetLength(); i++)
	//{
	//	temp[i] = strText.GetAt(i);
	//}
	//for (int j = 0; j <= 2; j++)
	//{
	//	memcpy(input, temp + j * 16, 16);
	//	m_Aes1.InvCipher(input, output);
	//	if (j == 0)
	//	{
	//		memcpy(serial, output, 8);
	//	}
	//	if (j == 1)
	//	{
	//		memcpy(serial + 8, output, 2);           //    Master Serial   
	//		memcpy(date, output + 2, 6);
	//	}

	//	if (j == 2)
	//	{
	//		memcpy(date + 6, output, 4);
	//		memcpy(peerNum, output + 4, 4);
	//	}

	//}
	//CString cstr_serial, cstr_date, cstr_peer, strDate, cstr_serial1;

	//cstr_serial = (unsigned char *)serial;              //序列号
	//if (pXnlConnection == NULL)
	//{
	//	return 1;
	//}
	//cstr_serial1 = pXnlConnection->readmac;


	//cstr_date = (unsigned char *)date;                  //使用期限 
	//cstr_peer = (unsigned char *)peerNum;                //中继台数

	//int year = 0, month = 0, day = 0;
	//sscanf_s((char *)date, "%4d-%2d-%2d", &year, &month, &day);
	//year = (year > 2000) ? year - 2000 : 0;

	//if (memcmp(cstr_serial, cstr_serial1, 10))
	//{
	//	return 1;
	//}
	//else
	//{
	//	if (cstr_date == _T("0000-00-00"))
	//	{
	//		return  0;
	//	}
	//	else
	//	{
	//		SYSTEMTIME st;
	//		GetLocalTime(&st);
	//		strDate.Format(_T("%4d-%02d-%02d"), st.wYear, st.wMonth, st.wDay);
	//		if (cstr_date.Compare(strDate) < 0)
	//		{
	//			return (year << 24) + (month << 16) + (day << 8) + 1;                             //超出授权时间
	//		}
	//		else
	//		{
	//			return (year << 24) + (month << 16) + (day << 8);
	//		}
	//	}

	//}
return 1;
}

int DispatchOperate::tcpConnect(const char* ip)
{
	dwip = inet_addr(ip);
	unsigned char iptemp[4];
	memcpy(iptemp, (void *)&dwip, 4);
	iptemp[3] = 1;
	dwip = *((DWORD *)iptemp);
	CreateThread(NULL, 0, TCPConnectionThread, this, THREAD_PRIORITY_NORMAL, NULL);
	CreateThread(NULL, 0, TimeOutThread, this, THREAD_PRIORITY_NORMAL, NULL);
	return 0;
}
void DispatchOperate::TcpConnect()
{

	pXnlConnection = CXNLConnection::CreatConnection(dwip, 8002, "0x152C7E9D0x38BE41C70x71E96CA40x6CAC1AFC",
		strtoul("0x9E3779B9", NULL, 16));
	if (pXnlConnection == NULL)
	{

		/*if (myCallBackFunc != NULL)
		{
			unsigned char str[30] = { 0 };
			sprintf_s((char *)str, sizeof(str), "result:1");
			onData(myCallBackFunc, 1, TCP_CONNECT, (char *)str, sizeof(str));

		}*/
	}
	else
	{
		/*if (myCallBackFunc != NULL)
		{
			unsigned char str[30] = { 0 };
			sprintf_s((char *)str, sizeof(str), "result:0");
			onData(myCallBackFunc, 1, TCP_CONNECT, (char *)str, sizeof(str));

		}*/
	}

}
int DispatchOperate::radioUdpConnect(const char* ip)
{
	//udp Connection
	//text  Connection                                      //0:连接成功  1：udp 连接失败  2：tcp 连接失败  3： udp、tcp均失败
	dwIP = inet_addr(ip);
	SOCKET mSokset;
	//if (!textConnectResult)
	//{
	//textConnectResult = pTextMsg.InitSocket(&mSokset, dwIP);
	//}

	//	ARS Connection
	//if (!ARSConnectResult)
	//{
//	ARSConnectResult = pRadioARS.InitARSSocket(dwIP);
	//}

	//	GPS Connection
	//if (!GPSConnectResult)
	///{
	//GPSConnectResult = pRadioGPS.InitGPSSocket(dwIP);
	//}

	if (textConnectResult && ARSConnectResult && GPSConnectResult)
	{
		/*if (myCallBackFunc != NULL)
		{
			unsigned char str[30] = { 0 };
			sprintf_s((char *)str, sizeof(str), "result:0");
			onData(myCallBackFunc, 1, RADIO_UDP_CONNECT, (char *)str, sizeof(str));

		}*/
	}
	else
	{
		/*if (myCallBackFunc != NULL)
		{
			unsigned char str[30] = { 0 };
			sprintf_s((char *)str, sizeof(str), "result:1");
			onData(myCallBackFunc, 1, RADIO_UDP_CONNECT, (char *)str, sizeof(str));

		}*/
	}
	return 0;
}
int DispatchOperate::mnisUdpConnect(const char* ip)
{
	//udp Connection
	//text  Connection                                      //0:连接成功  1：udp 连接失败  2：tcp 连接失败  3： udp、tcp均失败
	dwIP = inet_addr(ip);
	SOCKET mSokset;
	//if (!textConnectResult)
	//{
	//textConnectResult = pTextMsg.InitSocket(&mSokset, dwIP);
	//}

	//	ARS Connection
	//if (!ARSConnectResult)
	//{
	//ARSConnectResult = pRadioARS.InitARSSocket(dwIP);
	//}

	//	GPS Connection
	//	if (!GPSConnectResult)
	//{
	//GPSConnectResult = pRadioGPS.InitGPSSocket(dwIP);
	//	}

	if (textConnectResult && ARSConnectResult && GPSConnectResult)
	{
		/*if (myCallBackFunc != NULL)
		{
			unsigned char str[30] = { 0 };
			sprintf_s((char *)str, sizeof(str), "result:0");
			onData(myCallBackFunc, 1, MNIS_UDP_CONNECT, (char *)str, sizeof(str));

		}*/
	}
	else
	{
		/*if (myCallBackFunc != NULL)
		{
			unsigned char str[30] = { 0 };
			sprintf_s((char *)str, sizeof(str), "result:1");
			onData(myCallBackFunc, 1, MNIS_UDP_CONNECT, (char *)str, sizeof(str));

		}*/
	}
	return 0;
}
