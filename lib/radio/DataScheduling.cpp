#include "stdafx.h"
#include "DataScheduling.h"

void(*myCallBackFunc)(TcpClient*, int, int, Respone);
int seq;
std::mutex m_timeOutListLocker;
std::list <Command> timeOutList;
TcpClient * peer;
void onData(void(*func)(TcpClient, int, int, Respone), TcpClient* peer, int callId, int call, Respone data);
std::map<std::string, RadioStatus> radioStatus;
std::string lastIP = "0.0.0.0";
CDataScheduling::CDataScheduling()
{
	isUdpConnect = false;
	CreateThread(NULL, 0, workThread, this, THREAD_PRIORITY_NORMAL, NULL);
	CreateThread(NULL, 0, timeOutThread, this, THREAD_PRIORITY_NORMAL, NULL);
}
CDataScheduling::~CDataScheduling()
{
}

bool CDataScheduling::radioConnect(TcpClient* tp ,const char* ip, int callId)
{
	if (myCallBackFunc != NULL )
	{
		addUdpCommand(tp ,MNIS_CONNECT, ip, "", 0, _T(""), 0, 0, callId);
	
		return true;
	}
	return false;
}
bool CDataScheduling::radioGetGps(TcpClient* tp,DWORD dwRadioID, int queryMode, double cycle, int callId)
{

	if (myCallBackFunc != NULL)
	{
	
		switch (queryMode)
		{
		case GPS_IMME_COMM:
			addUdpCommand(tp, GPS_IMME_COMM, "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
			/*sendAck("gps", callId, int(dwRadioID));*/
			break;
		case GPS_TRIGG_COMM:
			addUdpCommand(tp, GPS_TRIGG_COMM, "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
			break;
		case GPS_IMME_CSBK:
			addUdpCommand(tp, GPS_IMME_CSBK, "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
			break;
		case GPS_TRIGG_CSBK:
			addUdpCommand(tp, GPS_TRIGG_CSBK, "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
			break;
		case GPS_IMME_CSBK_EGPS:
			addUdpCommand(tp, GPS_IMME_CSBK_EGPS, "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
			break;
		case GPS_TRIGG_CSBK_EGPS:
			addUdpCommand(tp, GPS_TRIGG_CSBK_EGPS, "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
			break;
		default:
			break;
		}
		return true;
	}
	return false;
}
bool CDataScheduling::radioStopGps(TcpClient* tp,DWORD dwRadioID, int	queryMode, int callId)
{

	if (myCallBackFunc != NULL)
	{
		addUdpCommand(tp, STOP_QUERY_GPS, "", "", int(dwRadioID), _T(""), 0, queryMode, callId);
		//sendAck("gps", callId, int(dwRadioID));
		return true;
	}
	return false;
}
void CDataScheduling::sendAck(int call ,int callId, int id)
{
	if (myCallBackFunc != NULL)
	{
		unsigned char str[30] = { 0 };
		sprintf_s((char *)str, sizeof(str), "id:%s;result:0", int(id));
		//onData(myCallBackFunc, callId, call, (char *)str);
	}
	
}
bool CDataScheduling::radioSendMsg(TcpClient* tp,LPTSTR message, DWORD dwRadioID, int callId, int type)
{

	if (myCallBackFunc != NULL)
	{
		if (type == GROUP)
		{
			addUdpCommand(tp, SEND_GROUP_MSG, "", "", int(dwRadioID), message, 0, 0, callId);
		}
		else if (type == PRIVATE)
		{
			addUdpCommand(tp, SEND_PRIVATE_MSG, "", "", int(dwRadioID), message, 0, 0, callId);
		}
		return true;
	}
	return false;
}
void CDataScheduling::getRadioStatus(TcpClient* tp, int type, int callId)
{
	if (myCallBackFunc != NULL)
	{
		if (type == CONNECT_STATUS)
		{
			addUdpCommand(tp, CONNECT_STATUS, "", "", 0, _T(""), 0, 0, callId);
		}
		else if (type == RADIO_STATUS)
		{
			addUdpCommand(tp, RADIO_STATUS, "", "", 0, _T(""), 0, 0, callId);
		}
	}
}
void CDataScheduling::connect( const char* ip, int callId)
{
	int result = 1;
	if (INADDR_NONE != inet_addr(ip)) 
	if( !isUdpConnect) 
	if( lastIP != ip)
	{
		
		DWORD dwIP = inet_addr(ip);
		if (pRadioMsg.InitSocket(dwIP))
		if (pRadioARS.InitARSSocket(dwIP))
		if (pRadioGPS.InitGPSSocket(dwIP))
		{
			isUdpConnect = true;
			result = 0;
			lastIP = ip;
#if DEBUG_LOG
			LOG(INFO) << "数据连接成功！";
#endif 
		}
		else
		{
			isUdpConnect = false;
			result = 1;
#if DEBUG_LOG
			LOG(INFO) << "数据连接失败！";
#endif 
		}
	
	}
	else
	{
		isUdpConnect = false;
		result = 1;
#if DEBUG_LOG
		LOG(INFO) << "数据连接失败！";
#endif 
	}
	std::list<Command>::iterator it;
	m_timeOutListLocker.lock();
	for (it = timeOutList.begin(); it != timeOutList.end(); ++it)
	{
		if (it->command == MNIS_CONNECT)
		if (myCallBackFunc != NULL)
		{
			Respone r;
			r.connectStatus = result;
			onData(myCallBackFunc,it->tp, ++it->callId, MNIS_CONNECT,r);
			it = timeOutList.erase(it);
			break;
		}
	}
	m_timeOutListLocker.unlock();
}
void CDataScheduling::getGps(DWORD dwRadioID, int queryMode, double cycle)
{
	pRadioGPS.SendQueryGPS(dwRadioID, queryMode, cycle);
}
void CDataScheduling::stopGps(DWORD dwRadioID, int	queryMode)
{
	pRadioGPS.StopQueryTriggeredGPS(dwRadioID, queryMode);
	
}
void CDataScheduling::sendMsg(int callId, LPTSTR message, DWORD dwRadioID, int CaiNet)
{
	pRadioMsg.SendMsg(callId, message, dwRadioID, CaiNet);
}
void CDataScheduling::initGPSOverturnSocket(DWORD dwAddress)
{
	
}
void  CDataScheduling::setCallBackFunc(void(*callBackFunc)(TcpClient*, int, int, Respone))
{
	myCallBackFunc = callBackFunc;
}
void onData(void(*func)(TcpClient*, int, int, Respone), TcpClient* tp, int seq, int call, Respone data)
{

	try
	{
		func(tp,seq, call, data);
	}
	catch (double)
	{

	}

}
void CDataScheduling::addUdpCommand(TcpClient*  tp ,int command, std::string radioIP, std::string gpsIP, int id, wchar_t* text, double cycle, int querymode, int callId)
{
	
	Command      m_command;
	m_command.callId = callId;
	m_command.command = command;
	m_command.ackNum = 0;
	m_command.timeOut = 30000;     //ms
	m_command.timeCount = 0;
	m_command.radioIP = radioIP;
	m_command.radioId = id;
	m_command.querymode = querymode;
	m_command.cycle = cycle;
	m_command.gpsIP = gpsIP;
	m_command.text = text;
	m_command.tp = tp;
	timeOutList.push_back(m_command);
	workList.push_back(m_command);
	seq = callId;
	peer = tp;
}
DWORD WINAPI CDataScheduling::timeOutThread(LPVOID lpParam)
{
	CDataScheduling * p = (CDataScheduling *)(lpParam);
	while (true)
	{
		p->timeOut();
		Sleep(100);
	}
	return 1;
}
DWORD WINAPI CDataScheduling::workThread(LPVOID lpParam)
{
	CDataScheduling * p = (CDataScheduling *)(lpParam);
	while (true)
	{
		p->workThreadFunc();
		Sleep(10);
	}
}
void CDataScheduling::workThreadFunc()
{
	std::list<Command>::iterator it;
	for (it = workList.begin(); it != workList.end(); ++it)
	{
		switch (it->command)
		{
		case  MNIS_CONNECT:
			if (it->radioIP != "")
			{
				connect(it->radioIP.c_str(), it->callId);
			}
			break;
		case CONNECT_STATUS:
			break;
		case RADIO_STATUS:
			sendRadioStatusToClient();
			break;
		case SEND_PRIVATE_MSG:
			sendMsg(it->callId, it->text, it->radioId, PRIVATE_MSG_FLG);
			break;
		case SEND_GROUP_MSG:
			sendMsg(it->callId, it->text, it->radioId, GROUP_MSG_FLG);
			break;
		case  GPS_IMME_COMM:
			getGps(it->radioId, it->querymode, it->cycle);
			break;
		case GPS_TRIGG_COMM:
			getGps(it->radioId, it->querymode, it->cycle);
			break;
		case GPS_IMME_CSBK:
			getGps(it->radioId, it->querymode, it->cycle);
			break;
		case GPS_TRIGG_CSBK:
			getGps(it->radioId, it->querymode, it->cycle);
			break;
		case GPS_IMME_CSBK_EGPS:
			getGps(it->radioId, it->querymode, it->cycle);
			break;
		case GPS_TRIGG_CSBK_EGPS:
			getGps(it->radioId, it->querymode, it->cycle);
			break;
		case STOP_QUERY_GPS:
			stopGps(it->radioId, it->callId);
			break;
		
		default:
			break;
		}
		it = workList.erase(it);
		break;
	}
}

void CDataScheduling::timeOut()
{
	std::list<Command>::iterator it;
	std::lock_guard <std::mutex> locker(m_timeOutListLocker);
	Respone r;
	for (it = timeOutList.begin(); it != timeOutList.end(); it++)
	{
		it->timeCount++;
		if (it->timeCount % (it->timeOut / 100) == 0)
		{

			//if (it->pRemote != NULL)
			{
				int operate = -1;
				int type = -1;
				switch (it->command)
				{
				case  MNIS_CONNECT:
					break;
				case SEND_PRIVATE_MSG:
					r.target = it->radioId;
					r.msgStatus = FAILED;
					r.msg = "";
					r.msgType = PRIVATE;
					onData(myCallBackFunc, it->tp, ++it->callId, it->command, r);
				case SEND_GROUP_MSG:
					r.target = it->radioId;
					r.msgStatus = FAILED;
					r.msg = "";
					r.msgType = GROUP;
					onData(myCallBackFunc, it->tp, ++it->callId, it->command, r);
					break;
				case  GPS_IMME_COMM:
				case GPS_TRIGG_COMM:
				case GPS_IMME_CSBK:
				case GPS_TRIGG_CSBK:
				case GPS_IMME_CSBK_EGPS:
				case GPS_TRIGG_CSBK_EGPS:
					r.target = it->radioId;
					r.gpsStatus = FAILED;
					r.cycle = it->cycle;
					r.querymode = it->querymode;
					r.gpsType = START;
					onData(myCallBackFunc, it->tp, ++it->callId, it->command, r);
					break;
				case STOP_QUERY_GPS:
					r.target = it->radioId;
					r.gpsStatus = FAILED;
					r.cycle = it->cycle;
					r.querymode = it->querymode;
					r.gpsType = STOP;
					onData(myCallBackFunc, it->tp, ++it->callId, it->command, r);
					//operate = STOP;
					/*try
					{
						ArgumentType args;
						args["Target"] = FieldValue(it->radioId);
						args["Type"] = FieldValue(it->querymode);
						args["Cycle"] = FieldValue(it->cycle);
						args["Operate"] = FieldValue(operate);
						args["Status"] = FieldValue(REMOTE_FAILED);
						std::string callJsonStrRes = CRpcJsonParser::buildCall("c", it->callId, args, "radio");
						if (it->pRemote != NULL)
						{
							it->pRemote->sendResponse((const char *)callJsonStrRes.c_str(), callJsonStrRes.size());
						}
					}
					catch (std::exception e)
					{

					}*/
					break;
				default:
					break;
				}
				if (it->command != MNIS_CONNECT)
				{
					it = timeOutList.erase(it);
					break;
				}

			}

		}
		else if (it->command != MNIS_CONNECT)
		{
			//commandList.push_back(*it);
			//break;
		}
	}
}
void CDataScheduling::sendConnectStatusToClient()
{
	Respone r;
	int result = -1;
	if (isUdpConnect)
	{
		result = 0;
		r.connectStatus = SUCESS;
	}
	else
	{
		result = 1;
		r.connectStatus = FAILED;
	}
	if (myCallBackFunc !=NULL)
	{
		onData(myCallBackFunc,peer , seq, CONNECT_STATUS, r);
	}
}
void CDataScheduling::sendRadioStatusToClient()
{
	std::list<Command>::iterator it;
	m_timeOutListLocker.lock();
	for (it = timeOutList.begin(); it != timeOutList.end(); it++)
	{
		if (RADIO_STATUS == it->command)
		{
			Respone r;
			r.rs = radioStatus;
			onData(myCallBackFunc, it->tp, ++it->callId, it->command, r);
			it = timeOutList.erase(it);
			break;
		}
	}
	m_timeOutListLocker.unlock();
	/*if ()
	{*/
		//std::map<std::string,RadioStatus>::iterator it;
		//ArgumentType args;
		//args["getType"] = RADIO_STATUS;
		//FieldValue info(FieldValue::TArray);
		//for (it = radioStatus.begin(); it != radioStatus.end(); it++)
		//{
		//	FieldValue element(FieldValue::TObject);

		//	element.setKeyVal("radioId", FieldValue(it->second.id));
		//	bool isGps = false;
		//	if (it->second.gpsQueryMode != 0)
		//	{
		//		isGps = true;
		//	}
		//	bool isArs = false;
		//	if (it->second.status != 0)
		//	{
		//		isArs = true;
		//	}
		//	element.setKeyVal("IsInGps", FieldValue(isGps));
		//	element.setKeyVal("IsOnline", FieldValue(isArs));

		//	info.push(element);
		//	//info.setKeyVal(it->first.c_str(),element);
		//}

		//args["info"] = info;
		/*std::string callJsonStr = CRpcJsonParser::buildCall("status", ++seq, args, "radio");
		if (pRemote != NULL)
		{
			pRemote->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
		}*/
	//}
}


