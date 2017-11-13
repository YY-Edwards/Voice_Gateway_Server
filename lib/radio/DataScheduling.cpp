#include "stdafx.h"
#include "DataScheduling.h"

void(*myCallBackFunc)( int,  Respone);
void onData(void(*func)(int,Respone),int call, Respone data);
std::mutex m_timeOutListLocker;
std::list <Command> timeOutList;
std::mutex m_workListLocker;
CDataScheduling::CDataScheduling()
{
	isUdpConnect = false;
	m_serverFunHandler = nullptr;
	pRadioARS = new CRadioARS(this);
	pRadioGPS = new CRadioGps(this);
	pRadioMsg = new CTextMsg(this);
	m_workThread = true;
	m_timeoutThread = true;
	m_mnisCfg = {0};
	m_locationCfg = {0};
	m_locationIndoorCfg = {0};

}
CDataScheduling::~CDataScheduling()
{
	if (pRadioARS)
	{
		delete pRadioARS;
	}
	if (pRadioGPS)
	{
		delete pRadioGPS;
	}
	if (pRadioMsg)
	{
		delete pRadioMsg;
	}
}

void CDataScheduling::radioConnect(mnis_t mnisCfg, location_t locationCfg, locationindoor_t locationIndoorCfg)
{
	m_mnisCfg = mnisCfg;
	m_locationCfg = locationCfg;
	m_locationIndoorCfg = locationIndoorCfg;
	connect();
}
void CDataScheduling::radioDisConnect()
{
	disConnect();

}
bool CDataScheduling::radioGetGps(DWORD dwRadioID, int type, double cycle,std::string sessionId,int operate)
{

	if (myCallBackFunc != NULL)
	{
	
		if (type == 0)
		{
			if (m_mnisCfg.LocationType == 0 && operate == 0)
			{
				addUdpCommand(GPS_IMME_COMM, "", "", int(dwRadioID), "", cycle, type, sessionId);
			}
			else if (m_mnisCfg.LocationType == 1 && operate == 0)
			{
				addUdpCommand(GPS_IMME_CSBK, "", "", int(dwRadioID), "", cycle, type, sessionId);
			}
			else if (m_mnisCfg.LocationType == 2 && operate == 0)
			{
				addUdpCommand(GPS_IMME_CSBK_EGPS, "", "", int(dwRadioID), "", cycle, type, sessionId);
			}
			else if (m_mnisCfg.LocationType == 0 && operate == 1)
			{
				addUdpCommand(GPS_TRIGG_COMM, "", "", int(dwRadioID), "", cycle, type, sessionId);
			}
			else if (m_mnisCfg.LocationType == 1 && operate == 1)
			{
				addUdpCommand(GPS_TRIGG_CSBK, "", "", int(dwRadioID), "", cycle, type, sessionId);
			}
			else if (m_mnisCfg.LocationType == 2 && operate == 1)
			{
				addUdpCommand(GPS_TRIGG_CSBK_EGPS, "", "", int(dwRadioID), "", cycle, type, sessionId);
			}
		}
		else if (type == 1)
		{
			if (m_mnisCfg.LocationType == 0)
			{
				addUdpCommand(GPS_TRIGG_COMM_INDOOR, "", "", int(dwRadioID), "", cycle, type, sessionId);
			}
			else if (m_mnisCfg.LocationType == 1)
			{
				addUdpCommand(GPS_TRIGG_CSBK_INDOOR, "", "", int(dwRadioID), "", cycle, type, sessionId);
			}
		}
		
	/*	switch (m_mnisCfg.LocationType)
		{
		case GPS_IMME_COMM:
			addUdpCommand(GPS_IMME_COMM, "", "", int(dwRadioID), "", cycle, queryMode, sessionId);
			break;
		case GPS_TRIGG_COMM:
			addUdpCommand( GPS_TRIGG_COMM, "", "", int(dwRadioID), "", cycle, queryMode,sessionId);
			break;
		case GPS_IMME_CSBK:
			addUdpCommand( GPS_IMME_CSBK, "", "", int(dwRadioID), "", cycle, queryMode,sessionId);
			break;
		case GPS_TRIGG_CSBK:
			addUdpCommand( GPS_TRIGG_CSBK, "", "", int(dwRadioID), "", cycle, queryMode,sessionId);
			break;
		case GPS_IMME_CSBK_EGPS:
			addUdpCommand( GPS_IMME_CSBK_EGPS, "", "", int(dwRadioID), "", cycle, queryMode,sessionId);
			break;
		case GPS_TRIGG_CSBK_EGPS:
			addUdpCommand( GPS_TRIGG_CSBK_EGPS, "", "", int(dwRadioID), "", cycle, queryMode,sessionId);
			break;
		case GPS_TRIGG_COMM_INDOOR:
			addUdpCommand(GPS_TRIGG_COMM_INDOOR, "", "", int(dwRadioID), "", cycle, queryMode,sessionId);
			break;
		case GPS_TRIGG_CSBK_INDOOR:
			addUdpCommand(GPS_TRIGG_CSBK_INDOOR, "", "", int(dwRadioID), "", cycle, queryMode,sessionId);
			break;
		default:
			break;
		}*/
		return true;
	}
	return false;
}
bool CDataScheduling::radioStopGps(DWORD dwRadioID, int	type,std::string sessionId)
{

	if (myCallBackFunc != NULL)
	{
		if (type = 1)
		{
			int querymode = GPS_TRIGG_COMM_INDOOR;
			addUdpCommand(STOP_QUERY_GPS, "", "", int(dwRadioID), "", 0, querymode, sessionId);
		}
		else
		{
			addUdpCommand(STOP_QUERY_GPS, "", "", int(dwRadioID), "", 0, type, sessionId);
		}
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
bool CDataScheduling::radioSendMsg(std::string message, DWORD dwRadioID,  int type,std::string sessionId)
{

	if (myCallBackFunc != NULL)
	{
		if (type == GROUP)
		{
			addUdpCommand( SEND_GROUP_MSG, "", "", int(dwRadioID), message, 0, 0,sessionId);
		}
		else if (type == PRIVATE)
		{
			addUdpCommand( SEND_PRIVATE_MSG, "", "", int(dwRadioID), message, 0, 0,sessionId);
		}
		return true;
	}
	return false;
}
void CDataScheduling::getRadioStatus( int type,std::string sessionId)
{
	if (myCallBackFunc != NULL)
	{
		if (type == CONNECT_STATUS)
		{
			addUdpCommand( CONNECT_STATUS, "", "", 0, "", 0, 0,sessionId);
		}
		else if (type == RADIO_STATUS)
		{
			addUdpCommand( RADIO_STATUS, "", "", 0, "", 0, 0,sessionId);
		}
	}
}
void CDataScheduling::connect()
{
	m_workThread = true;
	m_timeoutThread = true;
	m_hWthd = CreateThread(NULL, 0, workThread, this, THREAD_PRIORITY_NORMAL, NULL);
	m_hTthd = CreateThread(NULL, 0, timeOutThread, this, THREAD_PRIORITY_NORMAL, NULL);
	int result = 1;
	if (INADDR_NONE != inet_addr(m_mnisCfg.Host)) 
	if( !isUdpConnect) 
	if (lastIP != m_mnisCfg.Host)
	{
		
		DWORD dwIP = inet_addr(m_mnisCfg.Host);
		if (pRadioMsg->InitSocket(dwIP,m_mnisCfg.MessagePort,m_mnisCfg.ID))
		if (pRadioARS->InitARSSocket(dwIP,m_mnisCfg.ArsPort))
		if (pRadioGPS->InitGPSSocket(dwIP,m_mnisCfg.GpsPort))
		{	
			isUdpConnect = true;
			result = 0;
			lastIP = m_mnisCfg.Host;

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
	Respone r = { 0 };
	r.connectStatus = result;
	onData(myCallBackFunc,  CONNECT_STATUS, r);
}
void CDataScheduling::disConnect()
{
	if (m_hWthd && isUdpConnect)
	{
		m_workThread = false;
		WaitForSingleObject(m_hWthd,1000);
		CloseHandle(m_hWthd);
	}
	if (m_hTthd && isUdpConnect)
	{
		m_timeoutThread = false;
		WaitForSingleObject(m_hTthd, 1000);
		CloseHandle(m_hTthd);
	}
	timeOutList.clear();
	workList.clear();
	pRadioARS->CloseARSSocket();
	pRadioGPS->CloseGPSSocket();
	pRadioMsg->CloseSocket();
	isUdpConnect = false;
}
void CDataScheduling::getGps(DWORD dwRadioID, int queryMode, double cycle,int cai)
{
	pRadioGPS->SendQueryGPS(dwRadioID, queryMode, cycle, cai);
}
void CDataScheduling::stopGps(DWORD dwRadioID, int	queryMode,int cai)
{
	pRadioGPS->StopQueryTriggeredGPS(dwRadioID, queryMode,cai);
	
}
void CDataScheduling::sendMsg(std::string  sessionId, std::string message, DWORD dwRadioID, int CaiNet)
{
	pRadioMsg->SendMsg(sessionId, message, dwRadioID, CaiNet);
}
void CDataScheduling::initGPSOverturnSocket(DWORD dwAddress)
{
	pRadioGPS->InitGPSOverturnSocket(dwAddress);
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
void CDataScheduling::addUdpCommand(int command, std::string radioIP, std::string gpsIP, int id, std::string text, double cycle, int querymode,std::string sessionId)
{
	Command      m_command;
	m_command.command = command;
	m_command.ackNum = 0;
	m_command.timeOut = m_mnisCfg.TomeoutSeconds * 1000;     //ms
	m_command.timeCount = 0;
	m_command.radioIP = radioIP;
	m_command.radioId = id;
	m_command.querymode = querymode;
	m_command.cycle = cycle;
	m_command.gpsIP = gpsIP;
	m_command.text = text;
	m_command.sessionId = sessionId;
	std::lock_guard <std::mutex> locker(m_timeOutListLocker);
	timeOutList.push_back(m_command);
	std::lock_guard <std::mutex> wlocker(m_workListLocker);
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
		std::lock_guard <std::mutex> locker(m_workListLocker);
		std::list<Command>::iterator it;
		for (it = workList.begin(); it != workList.end(); ++it)
		{
			switch (it->command)
			{
			case  MNIS_CONNECT:
				/*if (it->radioIP != "")
				{
					connect(it->radioIP.c_str());
				}*/
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
				sendMsg(it->sessionId, it->text, it->radioId,m_mnisCfg.CAI );
				break;
			case SEND_GROUP_MSG:
				sendMsg(it->sessionId, it->text, it->radioId, m_mnisCfg.GroupCAI);
				break;
			case  GPS_IMME_COMM:
				getGps(it->radioId, it->command, it->cycle,m_mnisCfg.CAI);
				break;
			case GPS_TRIGG_COMM:
				getGps(it->radioId, it->command, it->cycle, m_mnisCfg.CAI);
				break;
			case GPS_IMME_CSBK:
				getGps(it->radioId, it->command, it->cycle, m_mnisCfg.CAI);
				break;
			case GPS_TRIGG_CSBK:
				getGps(it->radioId, it->command, it->cycle, m_mnisCfg.CAI);
				break;
			case GPS_IMME_CSBK_EGPS:
				getGps(it->radioId, it->command, it->cycle, m_mnisCfg.CAI);
				break;
			case GPS_TRIGG_CSBK_EGPS:
				getGps(it->radioId, it->command, it->cycle, m_mnisCfg.CAI);
				break;
			case GPS_TRIGG_CSBK_INDOOR:
				getGps(it->radioId, it->command, it->cycle, m_mnisCfg.CAI);
				break;
			case GPS_TRIGG_COMM_INDOOR:
				getGps(it->radioId, it->command, it->cycle, m_mnisCfg.CAI);
				break;
			case STOP_QUERY_GPS:
				stopGps(it->radioId, it->querymode, m_mnisCfg.CAI);
				break;
			case MNIS_DIS_CONNECT:
				break;
			default:
				break;
			}
			it = workList.erase(it);
			break;
		}
		Sleep(100);
	}
}

void CDataScheduling::timeOut()
{
	while (m_timeoutThread)
	{
		std::list<Command>::iterator it;
		std::lock_guard <std::mutex> locker(m_timeOutListLocker);
		Respone r = { 0 };
		for (it = timeOutList.begin(); it != timeOutList.end(); ++it)
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
						r.sessionId = it->sessionId;
						r.target = it->radioId;
						r.msgStatus = UNSUCESS;
						r.msg = "";
						r.msgType = PRIVATE;
						onData(myCallBackFunc, it->command, r);
						break;
					case SEND_GROUP_MSG:
						r.sessionId = it->sessionId;
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
						r.sessionId = it->sessionId;
						r.target = it->radioId;
						r.gpsStatus = UNSUCESS;
						r.cycle = it->cycle;
						r.querymode = it->querymode;
						r.gpsType = START;
						onData(myCallBackFunc, it->command, r);
						break;
					case STOP_QUERY_GPS:
						r.sessionId = it->sessionId;
						r.target = it->radioId;
						r.gpsStatus = UNSUCESS;
						r.cycle = it->cycle;
						r.querymode = it->querymode;
						r.gpsType = STOP;
						onData(myCallBackFunc, it->command, r);
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
	std::lock_guard <std::mutex> locker(m_timeOutListLocker);
	for (it = timeOutList.begin(); it != timeOutList.end(); it++)
	{
		if (RADIO_STATUS == it->command)
		{
			Respone r = { 0 };
			r.rs = g_radioStatus;
			r.sessionId = it->sessionId;
			onData(myCallBackFunc, it->command, r);
			it = timeOutList.erase(it);
			break;
		}
	}
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
void CDataScheduling::InitGPSOverturnSocket(DWORD dwAddress,int port)
{
	pRadioGPS->InitGPSOverturnSocket(dwAddress);
}
void CDataScheduling::locationIndoorConfig(int Interval, int iBeaconNumber, bool isEmergency)
{
	pRadioGPS->locationIndoorConfig(Interval,iBeaconNumber,isEmergency);
}



