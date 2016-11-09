#include "stdafx.h"




#include <Dbt.h>
#include <initguid.h>
#include <Ndisguid.h>


#define CLS_NAME _T("DUMMY_CLASS")
#define HWND_MESSAGE     ((HWND)-3)

int seq;
list <AllCommand>allCommandList;
std::mutex m_allCommandListLocker;
bool isUdpConnect = false;
bool isTcpConnect = false;
string m_radioIP = "0.0.0.0";
string m_mnisIP = "0.0.0.0";
map <string, status> radioStatus;
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

	CreateThread(NULL, 0, WorkThread, this, THREAD_PRIORITY_NORMAL, NULL);
	CreateThread(NULL, 0, RadioUsbStatusThread, this, THREAD_PRIORITY_NORMAL, NULL);
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
	m_radioIP = ip;
	m_mnisIP = pIP;
	return 0;

}
int DispatchOperate::call(CRemotePeer* pRemote, int id, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "单呼";
#endif
	unsigned long target_radio_id = id;
	unsigned char call_type = 0x04;     //0x06 单呼


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
int DispatchOperate::getGps(CRemotePeer* pRemote, int id, int queryMode, double cycle, int callId)
{
#if DEBUG_LOG
	LOG(INFO) << "gps查询";
#endif
	gpsDic[id] = queryMode;


	//m_queryMode = queryMode;
	try
	{


	
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
	DispatchOperate * pDispatchOperate = (DispatchOperate *)lpParam;
	while (true)
	{
		if (!isTcpConnect || !isUdpConnect)
		{
			pDispatchOperate->RadioConnect();	
		}
		Sleep(30000);
	}
	
	
	//pDispatchOperate->TcpConnect();

	return 1;
}
int DispatchOperate::RadioConnect()
{
	std::lock_guard<std::mutex> locker(m_connectLocker);
	//udp Connection
	//text  Connection                                      //0:连接成功  1：udp 连接失败  2：tcp 连接失败  3： udp、tcp均失败
	SOCKET mSokset;
	if (!isUdpConnect)
	{
		textConnectResult = pTextMsg.InitSocket(&mSokset, dwIP, pRemotePeer);
		ARSConnectResult = pRadioARS.InitARSSocket(dwIP, pRemotePeer);
		GPSConnectResult = pRadioGPS.InitGPSSocket(dwIP, pRemotePeer);
		if (textConnectResult && ARSConnectResult && GPSConnectResult )
		{
			isUdpConnect = true;
		}
		else
		{
			isUdpConnect = false;
		}
	}


	if (pXnlConnection == NULL || !isTcpConnect)
	{
		pXnlConnection = CXNLConnection::CreatConnection(dwip, 8002, "0x152C7E9D0x38BE41C70x71E96CA40x6CAC1AFC",
			strtoul("0x9E3779B9", NULL, 16));
		//std::lock_guard <std::mutex> locker(m_allCommandListLocker);
		//m_allCommandListLocker.lock();
		//::EnterCriticalSection(&cs);
		list<AllCommand>::iterator it;
		for (it = allCommandList.begin(); it != allCommandList.end(); it++)
		{
			if (it->command == RADIO_CONNECT)
			{
				if (pXnlConnection == NULL)    //0:调度业务和数据业务都连接失败 1： 调度业务连接成功，数据业务连接失败 2：调度业务连接失败，数据业务连接成功 3. 调度业务和数据业务都连接成功
				{
					isTcpConnect = false;
				}
				else
				{
					isTcpConnect = true;
				}
				sendConnectStatusToClient(it->pRemote);
				//it = allCommandList.erase(it);
				break;
			}
			
		}
		
		
		//m_allCommandListLocker.unlock();
	//	::LeaveCriticalSection(&cs);
	}
	return 0;
}

void DispatchOperate::AddAllCommand(CRemotePeer* pRemote,SOCKET s, int command, string radioIP, string mnisIP, string gpsIP,int id, wchar_t* text, double cycle, int querymode, int callId)
{
	std::lock_guard<std::mutex> locker(m_addCommandLocker);
	AllCommand      m_allCommand;
	m_allCommand.pRemote = pRemote;
	m_allCommand.s = s;
	m_allCommand.callId = callId;
	m_allCommand.command = command;
	m_allCommand.ackNum = 0;
	m_allCommand.timeOut = 30000;     //ms
	m_allCommand.timeCount = 0;
	m_allCommand.radioIP = radioIP;
	m_allCommand.mnisIP = mnisIP;
	m_allCommand.radioId = id;
	m_allCommand.querymode = querymode;
	m_allCommand.cycle = cycle;
	m_allCommand.gpsIP = gpsIP;
	m_allCommand.text = text;
	allCommandList.push_back(m_allCommand);
	commandList.push_back(m_allCommand);

	seq = callId +1;
}
DWORD WINAPI DispatchOperate::TimeOutThread(LPVOID lpParam)
{
	DispatchOperate * pDispatchOperate = (DispatchOperate *)(lpParam);
	while (true)
	{
		pDispatchOperate->TimeOut();
		Sleep(100);
	}
	return 1;
}
void DispatchOperate::TimeOut()
{
	list<AllCommand>::iterator it;
	std::lock_guard <std::mutex> locker(m_allCommandListLocker);
	for (it = allCommandList.begin(); it != allCommandList.end(); ++it)
	{
		it->timeCount++;
		//if (it->timeCount>= 3)                       //超时3次则返回发送失败      
		//{
		//	allCommandList.erase(it);
		//	if (pRemotePeer != NULL && pRemotePeer == it->pRemote)
		//	{
		//		std::map<std::string, std::string> args;
		//		std::string callJsonStr = CRpcJsonParser::buildResponse("1", it->callId, 0, "1", args);
		//		pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());	
		//	}
		//}
		//else if (it->timeOut==30000)     //超时时间为30s
		//{
		//	allCommandList.erase(it);
		//	commandList.push_back(*it);
	//	}
		if (it->timeCount % (it->timeOut / 100) == 0)
		{
			
			if (pRemotePeer != NULL && pRemotePeer == it->pRemote)
			{
				int operate = -1;
				int type = -1;
				switch (it->command)
				{
				case  RADIO_CONNECT:					
					break;
				case PRIVATE_CALL:
					operate = PRIVATE;
				case GROUP_CALL:
					operate = GROUP;
				case ALL_CALL:
					operate = ALL;
					try
					{
						ArgumentType args;
						args["Status"] = FieldValue(REMOTE_FAILED);
						args["Target"] = FieldValue(it->radioId);
						args["Operate"] = FieldValue(operate);
						args["Type"] = FieldValue(START);
						std::string callJsonStr = CRpcJsonParser::buildCall("callStatus", ++seq, args, "radio");
						if (it->pRemote != NULL)
						{
							it->pRemote->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
						}

					}
					catch (std::exception e)
					{

					}
					break;
				case STOP_CALL:
					try
					{
						ArgumentType args;
						args["Status"] = FieldValue(REMOTE_FAILED);
						args["Target"] = FieldValue(it->radioId);
						args["Operate"] = FieldValue(STOP);
						args["Type"] = FieldValue(it->querymode);
						std::string callJsonStr = CRpcJsonParser::buildCall("callStatus", ++seq, args, "radio");
						if (it->pRemote != NULL)
						{
							it->pRemote->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
						}

					}
					catch (std::exception e)
					{

					}
					break;
				case REMOTE_CLOSE:
					type = OFF;
				case REMOTE_OPEN:
					type = ON;
				case CHECK_RADIO_ONLINE:
					type = RADIOCHECK;
				case REMOTE_MONITOR:
					type = MONITOR;
					try
					{
						ArgumentType args;
						args["Status"] = FieldValue(REMOTE_CONNECT_FAOLED);
						args["Target"] = FieldValue(it->radioId);
						args["Type"] = FieldValue(type);
						std::string callJsonStr = CRpcJsonParser::buildCall("controlStatus", ++seq, args, "radio");
						if (it->pRemote != NULL)
						{
							it->pRemote->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
						}

					}
					catch (std::exception e)
					{

					}
					break;
				case SEND_PRIVATE_MSG:
					operate = PRIVATE;
				case SEND_GROUP_MSG:
					operate = GROUP;
					try
					{
						ArgumentType args;
						args["Source"] = FieldValue(it->radioId);
						args["contents"] = FieldValue("");
						args["status"] = FieldValue(REMOTE_FAILED);
						args["type"] = FieldValue(operate);
						std::string callJsonStr = CRpcJsonParser::buildCall("messageStatus", ++seq, args, "radio");
						if (it->pRemote != NULL)
						{
							it->pRemote->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
						}
					}
					catch (std::exception e)
					{

					}			
					break;
				case  GPS_IMME_COMM:
				case GPS_TRIGG_COMM:
				case GPS_IMME_CSBK:
				case GPS_TRIGG_CSBK:
				case GPS_IMME_CSBK_EGPS:
				case GPS_TRIGG_CSBK_EGPS:
					operate = START;
				case STOP_QUERY_GPS:
					operate = STOP;
					try
					{
						ArgumentType args;
						args["Target"] = FieldValue(it->radioId);
						args["Type"] = FieldValue(it->querymode);
						args["Cycle"] = FieldValue(it->cycle);
						args["Operate"] = FieldValue(operate);
						args["Status"] = FieldValue(REMOTE_FAILED);
						std::string callJsonStrRes = CRpcJsonParser::buildCall("sendGpsStatus", it->callId, args, "radio");
						if (it->pRemote != NULL)
						{
							it->pRemote->sendResponse((const char *)callJsonStrRes.c_str(), callJsonStrRes.size());
						}
					}
					catch (std::exception e)
					{

					}
					break;
				default:
					break;
				}
				if (it->command != RADIO_CONNECT)
				{
					it = allCommandList.erase(it);
				}
				
			}
		
		}
		else if (it->command != RADIO_CONNECT)
		{
			commandList.push_back(*it);
		}
		break;
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
	if (!textConnectResult)
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
DWORD WINAPI DispatchOperate::WorkThread(LPVOID lpParam)
{
	DispatchOperate * pDispatchOperate = (DispatchOperate *)(lpParam);
	while (true)
	{
		pDispatchOperate->WorkThreadFunc();
		Sleep(10);
	}
	return 1;
}
DWORD WINAPI DispatchOperate::RadioUsbStatusThread(LPVOID lpParam)
{
	DispatchOperate * pDispatchOperate = (DispatchOperate *)(lpParam);
	pDispatchOperate->radioUsbStatus(); 
	return 1;
}
void DispatchOperate::sendConnectStatusToClient(CRemotePeer* pRemote)
{
	
	if (isUdpConnect)
	{
		if (isTcpConnect)
		{
#if DEBUG_LOG
			LOG(INFO) << "数据连接成功，调度业务连接成功";      //DATA_SUCESS_DISPATCH_SUCESS
#endif

			if (pRemote != NULL)
			{

				ArgumentType args;
				args["getType"] = RADIO_CONNECT;
				args["info"] = FieldValue(DATA_SUCESS_DISPATCH_SUCESS);
				std::string callJsonStr = CRpcJsonParser::buildCall("status", ++seq, args, "radio");
				if (pRemotePeer != NULL)
				{
					pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
						
				}

			}

		}
		else
		{
#if DEBUG_LOG
			LOG(INFO) << "数据连接成功，调度业务连接失败";              //DATA_SUCESS_DISPATCH_FAIL
#endif
			if (pRemote != NULL)
			{
				ArgumentType args;
				args["getType"] = RADIO_CONNECT;
				args["info"] = FieldValue(DATA_SUCESS_DISPATCH_FAILED);
				std::string callJsonStr = CRpcJsonParser::buildCall("status", ++seq, args, "radio");
				if (pRemotePeer != NULL)
				{
					pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
						
				}

			}
		}
	}
	else
	{
		if (isTcpConnect)
		{
#if DEBUG_LOG
			LOG(INFO) << "数据连接失败，调度业务连接成功";                 //DATA_FAIL_DISPATCH_SUCESS
#endif
			if (pRemote != NULL)
			{

				ArgumentType args;
				args["getType"] = RADIO_CONNECT;
				args["info"] = FieldValue(DATA_FAILED_DISPATCH_SUCESS);
				std::string callJsonStr = CRpcJsonParser::buildCall("status", ++seq, args, "radio");
				if (pRemotePeer != NULL)
				{
					pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
						
				}

			}
		}
		else
		{
#if DEBUG_LOG
			LOG(INFO) << "数据连接失败，调度业务连接失败";     //DATA_FAIL_DISPATCH_FAIL
#endif
			if (pRemote != NULL)
			{

				ArgumentType args;
				args["getType"] = RADIO_CONNECT;
				args["info"] = FieldValue(DATA_FAILED_DISPATCH_FAILED);
				std::string callJsonStr = CRpcJsonParser::buildCall("status", ++seq, args, "radio");
				if (pRemotePeer != NULL)
				{
					pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
						
				}
			}
		}
	}
}
void DispatchOperate::sendRadioStatusToClient(CRemotePeer* pRemote)
{
	if (pRemote != NULL)
	{
		map<string, status>::iterator it;
		ArgumentType args;
		args["getType"] = RADIO_STATUS;
		FieldValue info(FieldValue::TArray);
		for (it = radioStatus.begin(); it != radioStatus.end(); it++)
		{
			FieldValue element(FieldValue::TObject);
			
			element.setKeyVal("radioId", FieldValue(it->second.id));
			bool isGps = false;
			if (it->second.gpsQueryMode != 0)
			{
				isGps = true;
			}
			bool isArs = false;
			if (it->second.status != 0)
			{
				isArs = true;
			}
			element.setKeyVal("IsInGps", FieldValue(isGps));
			element.setKeyVal("IsOnline", FieldValue(isArs));

			info.push(element);
			//info.setKeyVal(it->first.c_str(),element);
		}
		
		args["info"] = info;
		std::string callJsonStr = CRpcJsonParser::buildCall("status", ++seq, args, "radio");
		if (pRemotePeer != NULL)
		{
			pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
		}
	}
}
void DispatchOperate::sendCallStatusToClient()
{

}

void DispatchOperate::WorkThreadFunc()
{
	list<AllCommand>::iterator it;
	//std::lock_guard <std::mutex> locker(m_allCommandListLocker);
	for (it = commandList.begin(); it != commandList.end(); ++it)
	{
		switch (it->command)
		{
		case  RADIO_CONNECT:
			if (it->radioIP != "" )
			{
				m_dispatchOperate[it->s]->Connect(it->pRemote, it->radioIP.c_str(), it->mnisIP.c_str(), it->callId);
			}
			else
			{
				m_dispatchOperate[it->s]->sendConnectStatusToClient(it->pRemote);
			}
			break;
		case PRIVATE_CALL:
			m_dispatchOperate[it->s]->call(it->pRemote, it->radioId, it->callId);
			break;
		case GROUP_CALL:
			m_dispatchOperate[it->s]->groupCall(it->pRemote, it->radioId, it->callId);
			break;
		case ALL_CALL:
			m_dispatchOperate[it->s]->allCall(it->pRemote, it->callId);
			break;
		case REMOTE_CLOSE:
			m_dispatchOperate[it->s]->remotePowerOff(it->pRemote, it->radioId, it->callId);
			break;
		case REMOTE_OPEN:
			m_dispatchOperate[it->s]->remotePowerOn(it->pRemote, it->radioId, it->callId);
			break;
		case CHECK_RADIO_ONLINE:
			m_dispatchOperate[it->s]->radioCheck(it->pRemote, it->radioId, it->callId);
			break;
		case REMOTE_MONITOR:
			m_dispatchOperate[it->s]->wiretap(it->pRemote, it->radioId, it->callId);
			break;
		case SEND_PRIVATE_MSG:
			m_dispatchOperate[it->s]->sendSms(it->pRemote, it->radioId, it->text, it->callId);
			break;
		case SEND_GROUP_MSG:
			m_dispatchOperate[it->s]->sendGroupSms(it->pRemote, it->radioId, it->text, it->callId);
			break;
		case  GPS_IMME_COMM:
			m_dispatchOperate[it->s]->getGps(it->pRemote, it->radioId, it->querymode, it->cycle, it->callId);
			break;
		case GPS_TRIGG_COMM:
			m_dispatchOperate[it->s]->getGps(it->pRemote, it->radioId, it->querymode, it->cycle, it->callId);
			break;
		case GPS_IMME_CSBK:
			m_dispatchOperate[it->s]->getGps(it->pRemote, it->radioId, it->querymode, it->cycle, it->callId);
			break;
		case GPS_TRIGG_CSBK:
			m_dispatchOperate[it->s]->getGps(it->pRemote, it->radioId, it->querymode, it->cycle, it->callId);
			break;
		case GPS_IMME_CSBK_EGPS:
			m_dispatchOperate[it->s]->getGps(it->pRemote, it->radioId, it->querymode, it->cycle, it->callId);
			break;
		case GPS_TRIGG_CSBK_EGPS:
			m_dispatchOperate[it->s]->getGps(it->pRemote, it->radioId, it->querymode, it->cycle, it->callId);
			break;
		case STOP_QUERY_GPS:
			m_dispatchOperate[it->s]->cancelPollGps(it->pRemote, it->radioId, it->callId);
			break;
		case STOP_CALL:
			m_dispatchOperate[it->s]->stopCall(it->pRemote, it->callId);
			break;
		case RADIO_ARS:
			m_dispatchOperate[it->s]->sendRadioStatusToClient(it->pRemote);
			break;
		default:
			break;
		}
		it = commandList.erase(it);
		break;	
	}
}

LRESULT message_handler(HWND__* hwnd, UINT uint, WPARAM wparam, LPARAM lparam)
{
	switch (uint)
	{
	case WM_NCCREATE: // before window creation
		return true;
		break;

	case WM_CREATE: // the actual creation of the window
	{
						// you can get your creation params here..like GUID..
						LPCREATESTRUCT params = (LPCREATESTRUCT)lparam;
						GUID InterfaceClassGuid = *((GUID*)params->lpCreateParams);
						DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
						ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
						NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
						NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
						NotificationFilter.dbcc_classguid = InterfaceClassGuid;
						HDEVNOTIFY dev_notify = RegisterDeviceNotification(hwnd, &NotificationFilter,
							DEVICE_NOTIFY_WINDOW_HANDLE);
						if (dev_notify == NULL)
						{
							throw std::runtime_error("Could not register for devicenotifications!");
						}
						break;
	}

	case WM_DEVICECHANGE:
	{
							PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lparam;
							PDEV_BROADCAST_DEVICEINTERFACE lpdbv = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
							std::string path;
							if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
							{
								switch (wparam)
								{
								case DBT_DEVICEARRIVAL:
									
									break;

								case DBT_DEVICEREMOVECOMPLETE:
									isUdpConnect = false;
									isTcpConnect = false;
									break;
								}
							}
							break;
	}

	}
	return 0L;
}
void DispatchOperate::radioUsbStatus()
{
	HWND hWnd = NULL;
	WNDCLASSEX wx;
	ZeroMemory(&wx, sizeof(wx));

	wx.cbSize = sizeof(WNDCLASSEX);
	wx.lpfnWndProc = reinterpret_cast<WNDPROC>(message_handler);
	wx.hInstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(0));
	wx.style = CS_HREDRAW | CS_VREDRAW;
	wx.hInstance = GetModuleHandle(0);
	wx.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wx.lpszClassName = CLS_NAME;

	GUID guid = GUID_DEVINTERFACE_NET;

	if (RegisterClassEx(&wx))
	{
		hWnd = CreateWindow(CLS_NAME, _T("DevNotifWnd"), WS_ICONIC,
			0, 0, CW_USEDEFAULT, 0, HWND_MESSAGE,
			NULL, GetModuleHandle(0), (void*)&guid);
	}

	if (hWnd == NULL)
	{
		throw std::runtime_error("Could not create message window!");
	}

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}