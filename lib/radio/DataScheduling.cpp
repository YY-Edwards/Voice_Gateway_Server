#include "stdafx.h"
#include "DataScheduling.h"

void(*myCallBackFunc)(TcpClient*, int, int, Respone);
int seq;
std::mutex m_timeOutListLocker;
std::list <Command> timeOutList;
TcpClient * peer;
void onData(void(*func)(TcpClient, int, int, Respone), TcpClient* peer, int callId, int call, Respone data);
map<std::string, RadioStatus> radioStatus;
CDataScheduling::CDataScheduling()
{
	CreateThread(NULL, 0, WorkThread, this, THREAD_PRIORITY_NORMAL, NULL);
	CreateThread(NULL, 0, TimeOutThread, this, THREAD_PRIORITY_NORMAL, NULL);
}
CDataScheduling::~CDataScheduling()
{
}

bool CDataScheduling::radioConnect(TcpClient* tp ,const char* ip, int callId)
{
	if (myCallBackFunc != NULL)
	{
		AddAllCommand(tp ,MNIS_CONNECT, ip, "", 0, _T(""), 0, 0, callId);
		return true;
	}
	return false;
}
bool CDataScheduling::radioGetGps(TcpClient* tp,DWORD dwRadioID, int queryMode, double cycle, int callId)
{

	if (myCallBackFunc != NULL)
	{
		unsigned char str[30] = { 0 };
		sprintf_s((char *)str, sizeof(str), "id:%s;result:0", int(dwRadioID));
		switch (queryMode)
		{
		case GPS_IMME_COMM:
			AddAllCommand(tp,GPS_IMME_COMM,  "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
			/*sendAck("gps", callId, int(dwRadioID));*/
			break;
		case GPS_TRIGG_COMM:
			AddAllCommand(tp,GPS_TRIGG_COMM, "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
			break;
		case GPS_IMME_CSBK:
			AddAllCommand(tp,GPS_IMME_CSBK, "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
			break;
		case GPS_TRIGG_CSBK:
			AddAllCommand(tp,GPS_TRIGG_CSBK, "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
			break;
		case GPS_IMME_CSBK_EGPS:
			AddAllCommand(tp,GPS_IMME_CSBK_EGPS, "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
			break;
		case GPS_TRIGG_CSBK_EGPS:
			AddAllCommand(tp,GPS_TRIGG_CSBK_EGPS, "", "", int(dwRadioID), _T(""), cycle, queryMode, callId);
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
		AddAllCommand(tp,STOP_QUERY_GPS, "", "", int(dwRadioID), _T(""), 0, queryMode, callId);
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
			AddAllCommand(tp,SEND_GROUP_MSG, "", "", int(dwRadioID), message, 0, 0, callId);
		}
		else if (type == PRIVATE)
		{
			AddAllCommand(tp,SEND_PRIVATE_MSG, "", "", int(dwRadioID), message, 0, 0, callId);
		}
		return true;
	}
	return false;
}
void CDataScheduling::connect( const char* ip, int callId)
{
	int result = 1;
	if (INADDR_NONE != inet_addr(ip))
	{
		
		DWORD dwIP = inet_addr(ip);
		if (pRadioMsg.InitSocket(dwIP))
		if (pRadioARS.InitARSSocket(dwIP))
		if (pRadioGPS.InitGPSSocket(dwIP))
		{
			
			result = 0;
#if DEBUG_LOG
			LOG(INFO) << "数据连接成功！";
#endif 
		}
		else
		{
			result = 1;
#if DEBUG_LOG
			LOG(INFO) << "数据连接失败！";
#endif 
		}
	
	}
	else
	{
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
	if (pRadioGPS.SendQueryGPS(dwRadioID, queryMode, cycle))
	{
		
	}
	else
	{

	}
}
void CDataScheduling::stopGps(DWORD dwRadioID, int	queryMode)
{
	if (pRadioGPS.StopQueryTriggeredGPS(dwRadioID, queryMode))
	{

	}
	else
	{

	}
	
}
void CDataScheduling::sendMsg(int callId, LPTSTR message, DWORD dwRadioID, int CaiNet)
{
	if (pRadioMsg.SendMsg(callId,message,dwRadioID,CaiNet))
	{

	}
	else
	{

	}
}
void CDataScheduling::InitGPSOverturnSocket(DWORD dwAddress)
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
void CDataScheduling::AddAllCommand(TcpClient*  tp ,int command, std::string radioIP, std::string gpsIP, int id, wchar_t* text, double cycle, int querymode, int callId)
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
DWORD WINAPI CDataScheduling::TimeOutThread(LPVOID lpParam)
{
	CDataScheduling * p = (CDataScheduling *)(lpParam);
	while (true)
	{
		p->timeOut();
		Sleep(100);
	}
	return 1;
}
DWORD WINAPI CDataScheduling::WorkThread(LPVOID lpParam)
{
	CDataScheduling * p = (CDataScheduling *)(lpParam);
	while (true)
	{
		p->WorkThreadFunc();
		Sleep(10);
	}
}
void CDataScheduling::WorkThreadFunc()
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
				case  RADIO_CONNECT:
					break;
				
				case SEND_PRIVATE_MSG:
					//operate = PRIVATE;
				case SEND_GROUP_MSG:
					/*operate = GROUP;
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

					}*/
					break;
				case  GPS_IMME_COMM:
				case GPS_TRIGG_COMM:
				case GPS_IMME_CSBK:
				case GPS_TRIGG_CSBK:
				case GPS_IMME_CSBK_EGPS:
				case GPS_TRIGG_CSBK_EGPS:
					//operate = START;
				case STOP_QUERY_GPS:
					//operate = STOP;
					/*try
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
void CDataScheduling::sendRadioStatusToClient(CRemotePeer* pRemote)
{
	if (pRemote != NULL)
	{
		std::map<std::string,RadioStatus>::iterator it;
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
		if (pRemote != NULL)
		{
			pRemote->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
		}
	}
}


