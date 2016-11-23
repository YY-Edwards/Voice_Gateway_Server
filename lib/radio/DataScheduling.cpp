#include "stdafx.h"
#include "DataScheduling.h"

void(*myCallBackFunc)( int,  Respone);
void onData(void(*func)(int,Respone),int call, Respone data);
std::mutex m_timeOutListLocker;
std::list <Command> timeOutList;

CDataScheduling::CDataScheduling()
{
	isUdpConnect = false;
	m_serverFunHandler = nullptr;
	pRadioARS = new CRadioARS(this);
	pRadioGPS = new CRadioGps(this);
	pRadioMsg = new CTextMsg(this);
	m_workThread = true;
	m_timeoutThread = true;
	CreateThread(NULL, 0, workThread, this, THREAD_PRIORITY_NORMAL, NULL);
	CreateThread(NULL, 0, timeOutThread, this, THREAD_PRIORITY_NORMAL, NULL);
}
CDataScheduling::~CDataScheduling()
{
}

bool CDataScheduling::radioConnect(const char* ip)
{
	if (myCallBackFunc != NULL )
	{
		addUdpCommand(MNIS_CONNECT, ip, "", 0, _T(""), 0, 0);
	
		return true;
	}
	return false;
}
void CDataScheduling::radioDisConnect()
{
	disConnect();
	/*if (myCallBackFunc != NULL)
	{
		addUdpCommand(MNIS_DIS_CONNECT,"","",0,_T(""),0,0);
	}*/
}
bool CDataScheduling::radioGetGps(DWORD dwRadioID, int queryMode, double cycle)
{

	if (myCallBackFunc != NULL)
	{
	
		switch (queryMode)
		{
		case GPS_IMME_COMM:
			addUdpCommand( GPS_IMME_COMM, "", "", int(dwRadioID), _T(""), cycle, queryMode );
			/*sendAck("gps", callId, int(dwRadioID));*/
			break;
		case GPS_TRIGG_COMM:
			addUdpCommand( GPS_TRIGG_COMM, "", "", int(dwRadioID), _T(""), cycle, queryMode);
			break;
		case GPS_IMME_CSBK:
			addUdpCommand( GPS_IMME_CSBK, "", "", int(dwRadioID), _T(""), cycle, queryMode);
			break;
		case GPS_TRIGG_CSBK:
			addUdpCommand( GPS_TRIGG_CSBK, "", "", int(dwRadioID), _T(""), cycle, queryMode);
			break;
		case GPS_IMME_CSBK_EGPS:
			addUdpCommand( GPS_IMME_CSBK_EGPS, "", "", int(dwRadioID), _T(""), cycle, queryMode);
			break;
		case GPS_TRIGG_CSBK_EGPS:
			addUdpCommand( GPS_TRIGG_CSBK_EGPS, "", "", int(dwRadioID), _T(""), cycle, queryMode);
			break;
		default:
			break;
		}
		return true;
	}
	return false;
}
bool CDataScheduling::radioStopGps(DWORD dwRadioID, int	queryMode)
{

	if (myCallBackFunc != NULL)
	{
		addUdpCommand( STOP_QUERY_GPS, "", "", int(dwRadioID), _T(""), 0, queryMode);
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
bool CDataScheduling::radioSendMsg(LPTSTR message, DWORD dwRadioID,  int type)
{

	if (myCallBackFunc != NULL)
	{
		if (type == GROUP)
		{
			addUdpCommand( SEND_GROUP_MSG, "", "", int(dwRadioID), message, 0, 0);
		}
		else if (type == PRIVATE)
		{
			addUdpCommand( SEND_PRIVATE_MSG, "", "", int(dwRadioID), message, 0, 0);
		}
		return true;
	}
	return false;
}
void CDataScheduling::getRadioStatus( int type)
{
	if (myCallBackFunc != NULL)
	{
		if (type == CONNECT_STATUS)
		{
			addUdpCommand( CONNECT_STATUS, "", "", 0, _T(""), 0, 0);
		}
		else if (type == RADIO_STATUS)
		{
			addUdpCommand( RADIO_STATUS, "", "", 0, _T(""), 0, 0);
		}
	}
}
void CDataScheduling::connect( const char* ip)
{
	int result = 1;
	if (INADDR_NONE != inet_addr(ip)) 
	if( !isUdpConnect) 
	if( lastIP != ip)
	{
		
		DWORD dwIP = inet_addr(ip);
		if (pRadioMsg->InitSocket(dwIP))
		if (pRadioARS->InitARSSocket(dwIP))
		if (pRadioGPS->InitGPSSocket(dwIP))
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
			Respone r = { 0 };
			r.connectStatus = result;
			onData(myCallBackFunc, MNIS_CONNECT,r);
			it = timeOutList.erase(it);
			break;
		}
	}
	m_timeOutListLocker.unlock();
}
void CDataScheduling::disConnect()
{
	m_workThread = false;
	m_timeoutThread = false;
	timeOutList.clear();
	workList.clear();
	pRadioARS->CloseARSSocket();
	pRadioGPS->CloseGPSSocket();
	pRadioMsg->CloseSocket();

}
void CDataScheduling::getGps(DWORD dwRadioID, int queryMode, double cycle)
{
	pRadioGPS->SendQueryGPS(dwRadioID, queryMode, cycle);
}
void CDataScheduling::stopGps(DWORD dwRadioID, int	queryMode)
{
	pRadioGPS->StopQueryTriggeredGPS(dwRadioID, queryMode);
	
}
void CDataScheduling::sendMsg(int callId, LPTSTR message, DWORD dwRadioID, int CaiNet)
{
	pRadioMsg->SendMsg(callId, message, dwRadioID, CaiNet);
}
void CDataScheduling::initGPSOverturnSocket(DWORD dwAddress)
{
	
}
void  CDataScheduling::setCallBackFunc(void(*callBackFunc)( int, Respone))
{
	myCallBackFunc = callBackFunc;
	m_serverFunHandler = callBackFunc;
}
void onData(void(*func)( int, Respone),  int call, Respone data)
{

	try
	{
		func(call, data);
	}
	catch (double)
	{

	}

}
void CDataScheduling::addUdpCommand(int command, std::string radioIP, std::string gpsIP, int id, wchar_t* text, double cycle, int querymode)
{
	Command      m_command;
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
	timeOutList.push_back(m_command);
	workList.push_back(m_command);

}
DWORD WINAPI CDataScheduling::timeOutThread(LPVOID lpParam)
{
	CDataScheduling * p = (CDataScheduling *)(lpParam);
	if (p!=NULL)
	{
		p->timeOut();
	
	}
	return 1;
}
DWORD WINAPI CDataScheduling::workThread(LPVOID lpParam)
{
	CDataScheduling * p = (CDataScheduling *)(lpParam);
	if (p != NULL)
	{
		p->workThreadFunc();
	}
	return 1;
}
void CDataScheduling::workThreadFunc()
{
	while (m_workThread)
	{
		std::list<Command>::iterator it;
		for (it = workList.begin(); it != workList.end(); ++it)
		{
			switch (it->command)
			{
			case  MNIS_CONNECT:
				if (it->radioIP != "")
				{
					connect(it->radioIP.c_str());
				}
				break;
			case CONNECT_STATUS:
				if (myCallBackFunc != NULL)
				{
					Respone r = { 0 };
					r.connectStatus = isUdpConnect;
					onData(myCallBackFunc, CONNECT_STATUS, r);
					break;
				}
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
			case MNIS_DIS_CONNECT:
				break;
			default:
				break;
			}
			it = workList.erase(it);
			break;
		}
		Sleep(10);
	}
}

void CDataScheduling::timeOut()
{
	while (m_timeoutThread)
	{
		std::list<Command>::iterator it;
		std::lock_guard <std::mutex> locker(m_timeOutListLocker);
		Respone r = { 0 };
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
						r.msgStatus = UNSUCESS;
						r.msg = "";
						r.msgType = PRIVATE;
						onData(myCallBackFunc, it->command, r);
						break;
					case SEND_GROUP_MSG:
						r.target = it->radioId;
						r.msgStatus = UNSUCESS;
						r.msg = "";
						r.msgType = GROUP;
						onData(myCallBackFunc, it->command, r);
						break;
					case  GPS_IMME_COMM:
					case GPS_TRIGG_COMM:
					case GPS_IMME_CSBK:
					case GPS_TRIGG_CSBK:
					case GPS_IMME_CSBK_EGPS:
					case GPS_TRIGG_CSBK_EGPS:
						r.target = it->radioId;
						r.gpsStatus = UNSUCESS;
						r.cycle = it->cycle;
						r.querymode = it->querymode;
						r.gpsType = START;
						onData(myCallBackFunc, it->command, r);
						break;
					case STOP_QUERY_GPS:
						r.target = it->radioId;
						r.gpsStatus = UNSUCESS;
						r.cycle = it->cycle;
						r.querymode = it->querymode;
						r.gpsType = STOP;
						onData(myCallBackFunc, it->command, r);
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
		Sleep(100);
	}
}
void CDataScheduling::sendConnectStatusToClient()
{
	Respone r = { 0 };
	int result = -1;
	if (isUdpConnect)
	{
		result = 0;
		r.connectStatus = SUCESS;
	}
	else
	{
		result = 1;
		r.connectStatus = UNSUCESS;
	}
	if (myCallBackFunc !=NULL)
	{
		onData(myCallBackFunc,CONNECT_STATUS, r);
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
			Respone r = { 0 };
			r.rs = g_radioStatus;
			onData(myCallBackFunc, it->command, r);
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

void CDataScheduling::updateOnLineRadioInfo(int radioId, int status, int gpsQueryMode)
{
	Respone response = { 0 };
	response.arsStatus = (RADIO_STATUS_ONLINE == status) ? (SUCESS) : (UNSUCESS);
	//response.arsStatus = RADIO_STATUS_ONLINE == status;
	response.source = radioId;
	char strRadioId[32] = { 0 };
	sprintf_s(strRadioId, "%d", radioId);
	std::map<std::string, RadioStatus>::iterator itor = g_radioStatus.find(strRadioId);
	/*上锁*/
	g_radioStatusLocker.lock();
	if (itor != g_radioStatus.end())
	{
		//刷新记录
		if (gpsQueryMode != -1)
		{
			itor->second.gpsQueryMode = gpsQueryMode;
		}
		itor->second.status = status;
		itor->second.id = radioId;
		/*当前列表存在*/
		if (status != itor->second.status)
		{
			/*状态变更为在线*/
			//通知界面
			sendToClient(RADIO_ARS, response);
		}
	}
	else
	{
		/*当前列表不存在*/
		if (0 == status)
		{
			/*状态变更为不在线*/
			// do nothing
		}
		else
		{
			/*状态变更为在线*/
			//追加记录
			RadioStatus newRadio;
			newRadio.gpsQueryMode = gpsQueryMode;
			newRadio.status = status;
			newRadio.id = radioId;
			g_radioStatus[strRadioId] = newRadio;
			//通知界面
			sendToClient(RADIO_ARS, response);
		}
	}
	/*解锁*/
	g_radioStatusLocker.unlock();
}

void CDataScheduling::sendToClient(int callFuncId, Respone response)
{
	if (m_serverFunHandler)
	{
		m_serverFunHandler(callFuncId, response);
	}
}


