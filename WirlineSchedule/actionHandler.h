#ifndef ACTIONHANDER_H
#define ACTIONHANDER_H
#include "stdafx.h"
#include <string>
#include "../lib/type.h"
#include "Manager.h"


/************************************************************************/
/*参考文档: http://120.26.88.11/w/dispatcher/trbox3.0/rpc/
/************************************************************************/

inline void push_front_task(REMOTE_TASK* task)
{
	WaitForSingleObject(g_taskLockerEvent, INFINITE);
	g_remoteCommandTaskQueue.push_front(task);
	SetEvent(g_taskLockerEvent);
	SetEvent(g_waitHandleRemoteTask);
}

inline void push_back_task(REMOTE_TASK* task)
{
	WaitForSingleObject(g_taskLockerEvent, INFINITE);
	g_remoteCommandTaskQueue.push_back(task);
	SetEvent(g_taskLockerEvent);
	SetEvent(g_waitHandleRemoteTask);
}

inline void erase_front_task()
{
	WaitForSingleObject(g_taskLockerEvent, INFINITE);
	REMOTE_TASK *p = g_remoteCommandTaskQueue.front();
	if (p)
	{
		g_remoteCommandTaskQueue.pop_front();
		delete p;
		p = NULL;
	}
	SetEvent(g_taskLockerEvent);
}

inline void get_front_task(REMOTE_TASK &target)
{
	WaitForSingleObject(g_taskLockerEvent, INFINITE);
	target = *g_remoteCommandTaskQueue.front();
	SetEvent(g_taskLockerEvent);
}

inline void requestRemoteTaskLock()
{
	WaitForSingleObject(g_taskLockerEvent, INFINITE);
}
inline void releaseRemoteTaskLock()
{
	SetEvent(g_taskLockerEvent);
}

inline void addCRemotePeer(TcpClient* client)
{
	TcpClient *pTempClient = new TcpClient();
	pTempClient->addr = client->addr;
	pTempClient->s = client->s;
	bool isHave = false;
	for (auto i = g_onLineClients.begin(); i != g_onLineClients.end(); i++)
	{
		TcpClient *p = *i;
		if (p->s == pTempClient->s)
		{
			isHave = true;
			break;
		}
	}
	if (!isHave)
	{
		g_onLineClients.push_back(pTempClient);
	}
	else
	{
		delete pTempClient;
		pTempClient = NULL;
	}
}

inline void removeCRemotePeer(TcpClient* client)
{
	TcpClient *pTempClient = new TcpClient();
	pTempClient->addr = client->addr;
	pTempClient->s = client->s;
	for (auto i = g_onLineClients.begin(); i != g_onLineClients.end(); i++)
	{
		TcpClient *p = *i;
		if (p->s == pTempClient->s)
		{
			delete (*i);
			g_onLineClients.erase(i);
			break;
		}
	}
	delete pTempClient;
	pTempClient = NULL;
}

inline bool wlScheduleIsEnable()
{
	return CONFIG_SCHDULE_ISENABLE;
}

inline void parseRepeaterCfg(repeater_t &cfg, Value json)
{
	Value tempJson;
	if (json.HasMember("IsEnable") && json["IsEnable"].IsBool())
	{
		cfg.IsEnable = json["IsEnable"].GetBool();
	}
	if (json.HasMember("TomeoutSeconds") && json["TomeoutSeconds"].IsInt())
	{
		cfg.TomeoutSeconds = json["TomeoutSeconds"].GetInt();
	}
	if (json.HasMember("Svr") && json["Svr"].IsObject())
	{
		tempJson = json["Svr"].GetObject();
		if (tempJson.HasMember("Ip") && tempJson["Ip"].IsString())
		{
			strcpy_s(cfg.Svr.Ip, tempJson["Ip"].GetString());
		}
		if (tempJson.HasMember("Port") && tempJson["Port"].IsInt())
		{
			cfg.Svr.Port = tempJson["Port"].GetInt();
		}
	}
	if (json.HasMember("DefaultGroupId") && json["DefaultGroupId"].IsInt())
	{
		cfg.DefaultGroupId = (unsigned long)json["DefaultGroupId"].GetInt();
	}
	if (json.HasMember("DefaultGroupId") && json["DefaultGroupId"].IsInt())
	{
		cfg.DefaultChannel = (SlotNumber_e)json["DefaultChannel"].GetInt();
	}

	if (json["Dongle"].IsObject())
	{
		tempJson = json["Dongle"].GetObject();
		if (tempJson.HasMember("Com") && tempJson["Com"].IsInt())
		{
			cfg.Dongle.donglePort = tempJson["Com"].GetInt();
		}
	}

	cfg.MinHungTime = json["MinHungTime"].GetInt() * 1000;
	if (json.HasMember("LocalPeerId") && json["LocalPeerId"].IsInt())
	{
		cfg.LocalPeerId = (unsigned long)json["LocalPeerId"].GetInt();
	}
	if (json.HasMember("LocalRadioId") && json["LocalRadioId"].IsInt())
	{
		cfg.LocalRadioId = (unsigned long)json["LocalRadioId"].GetInt();
	}
	if (json.HasMember("MaxSiteAliveTime") && json["MaxSiteAliveTime"].IsInt())
	{
		cfg.MaxSiteAliveTime = json["MaxSiteAliveTime"].GetInt() * 1000;
	}

	if (json["Master"].IsObject())
	{
		tempJson = json["Master"].GetObject();
		if (tempJson.HasMember("Ip") && tempJson["Ip"].IsString())
		{
			strcpy_s(cfg.Master.ip, tempJson["Ip"].GetString());
		}
		if (tempJson.HasMember("Port") && tempJson["Port"].IsInt())
		{
			cfg.Master.port = tempJson["Port"].GetInt();
		}
	}

	if (json.HasMember("MaxPeerAliveTime") && json["MaxPeerAliveTime"].IsInt())
	{
		cfg.MaxPeerAliveTime = json["MaxPeerAliveTime"].GetInt() * 1000;
	}
	int recordType = 1;
	if (json.HasMember("Type") && json["Type"].IsInt())
	{
		cfg.Type = json["Type"].GetInt();
		recordType = cfg.Type;
	}
	_RECORD_TYPE_VALUE temp = LCP;
	if (recordType == 0)
	{
		temp = IPSC;
	}
	else if (recordType == 1)
	{
		temp = CPC;
	}
	cfg.recordType = temp;
	if (json.HasMember("AudioPath") && json["AudioPath"].IsString())
	{
		strcpy_s(cfg.AudioPath, json["AudioPath"].GetString());
	}
}
inline void parseMnisCfg(mnis_t &cfg, Value json)
{
	if (json.HasMember("IsEnable") && json["IsEnable"].IsBool())
	{
		cfg.IsEnable = json["IsEnable"].GetBool();
	}
	if (json.HasMember("TomeoutSeconds") && json["TomeoutSeconds"].IsInt())
	{
		cfg.TomeoutSeconds = json["TomeoutSeconds"].GetInt();
	}
	if (json.HasMember("ID") && json["ID"].IsInt())
	{
		cfg.ID = json["ID"].GetInt();
	}
	if (json.HasMember("Host") && json["Host"].IsString())
	{
		strcpy_s(cfg.Host, json["Host"].GetString());
	}
	if (json.HasMember("MessagePort") && json["MessagePort"].IsInt())
	{
		cfg.MessagePort = json["MessagePort"].GetInt();
	}
	if (json.HasMember("ArsPort") && json["ArsPort"].IsInt())
	{
		cfg.ArsPort = json["ArsPort"].GetInt();
	}
	if (json.HasMember("GpsPort") && json["GpsPort"].IsInt())
	{
		cfg.GpsPort = json["GpsPort"].GetInt();
	}
	if (json.HasMember("XnlPort") && json["XnlPort"].IsInt())
	{
		cfg.XnlPort = json["XnlPort"].GetInt();
	}
	if (json.HasMember("CAI") && json["CAI"].IsInt())
	{
		cfg.CAI = json["CAI"].GetInt();
	}
	if (json.HasMember("GroupCAI") && json["GroupCAI"].IsInt())
	{
		cfg.GroupCAI = json["GroupCAI"].GetInt();
	}
	if (json.HasMember("LocationType") && json["LocationType"].IsInt())
	{
		cfg.LocationType = json["LocationType"].GetInt();
	}
}
inline void parseLocationCfg(location_t &cfg, Value json)
{
	Value tempJson;
	if (json.HasMember("IsEnable") && json["IsEnable"].IsBool())
	{
		cfg.IsEnable = json["IsEnable"].GetBool();
	}
	if (json.HasMember("Interval") && json["Interval"].IsDouble())
	{
		cfg.Interval = json["Interval"].GetDouble();
	}
	if (json.HasMember("IsEnableGpsC") && json["IsEnableGpsC"].IsBool())
	{
		cfg.IsEnableGpsC = json["IsEnableGpsC"].GetBool();
	}
	if (json["GpsC"].IsObject())
	{
		tempJson = json["GpsC"].GetObject();
		if (tempJson.HasMember("Ip") && tempJson["Ip"].IsString())
		{
			strcpy_s(cfg.GpsC.Ip, tempJson["Ip"].GetString());
		}
		if (tempJson.HasMember("Port") && tempJson["Port"].IsInt())
		{
			cfg.GpsC.Port = tempJson["Port"].GetInt();
		}
	}
}
inline void parseLocationIndoorCfg(locationindoor_t &cfg, Value json)
{
	if (json.HasMember("IsEnable") && json["IsEnable"].IsBool())
	{
		cfg.IsEnable = json["IsEnable"].GetBool();
	}
	if (json.HasMember("Interval") && json["Interval"].IsDouble())
	{
		cfg.Interval = json["Interval"].GetDouble();
	}
	if (json.HasMember("iBeaconNumber") && json["iBeaconNumber"].IsInt())
	{
		cfg.iBeaconNumber = json["iBeaconNumber"].GetInt();
	}
	if (json.HasMember("IsEmergency") && json["IsEmergency"].IsBool())
	{
		cfg.IsEmergency = json["IsEmergency"].GetBool();
	}
}

inline void wlConnectActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	g_sn = sn;
	Document d;
	Value tempJson;
	int errorCode = 0;
	ArgumentType args;
	std::string strResp = "";
	char status[64] = {0};
	strcpy_s(status, CLIENT_TRANSFER_OK);
	std::string statusText = "";
	//args["module"] = "wl";//wirelan模块
	REMOTE_TASK *pNewTask = new REMOTE_TASK;
	memset(pNewTask, 0, sizeof(REMOTE_TASK));
	try
	{
		try
		{
			d.Parse(param.c_str());
			/*获取参数*/
			pNewTask->cmd = REMOTE_CMD_CONFIG;

			if (d.HasMember("repeater") && d["repeater"].IsObject())
			{
				parseRepeaterCfg(pNewTask->param.info.configParam.reapeater, d["repeater"].GetObject());
			}
			else
			{
				pNewTask->param.info.configParam.reapeater.IsEnable = false;
			}

			if (d.HasMember("mnis") && d["mnis"].IsObject())
			{
				parseMnisCfg(pNewTask->param.info.configParam.mnis, d["mnis"].GetObject());
			}
			else
			{
				pNewTask->param.info.configParam.mnis.IsEnable = false;
			}

			if (d.HasMember("location") && d["location"].IsObject())
			{
				parseLocationCfg(pNewTask->param.info.configParam.location, d["location"].GetObject());
			}
			else
			{
				pNewTask->param.info.configParam.location.IsEnable = false;
			}

			if (d.HasMember("locationIndoor") && d["locationIndoor"].IsObject())
			{
				parseLocationIndoorCfg(pNewTask->param.info.configParam.locationindoor, d["locationIndoor"].GetObject());
			}
			else
			{
				pNewTask->param.info.configParam.locationindoor.IsEnable = false;
			}

			/*config配置优先级为最高*/
			push_front_task(pNewTask);
		}
		catch (...)
		{
			strcpy_s(status, CLIENT_TRANSFER_FAIL);
		}
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
		printf_s("conect response send error");
	}
	catch (...)
	{
		printf_s("conect response send error");
	}
}

inline void wlCallActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	if (!wlScheduleIsEnable())
	{
		return;
	}
	g_sn = sn;
	//addCRemotePeer((TcpClient*)pRemote);
	Document d;
	int errorCode = 0;
	ArgumentType args;
	std::string strResp = "";
	char status[64] = { 0 };
	strcpy_s(status, CLIENT_TRANSFER_OK);
	std::string statusText = "";
	//args["module"] = "wl";//wirelan模块
	REMOTE_TASK *pNewTask = NULL;
	try
	{
		try{
			d.Parse(param.c_str());
			int operate = d["operate"].GetInt();
			/*处理参数*/
			switch (operate)
			{
			case StartCall:
			{
							  pNewTask = new REMOTE_TASK;
							  memset(pNewTask, 0, sizeof(REMOTE_TASK));
							  pNewTask->cmd = REMOTE_CMD_CALL;
							  pNewTask->timeOutTickCout = GetTickCount() + (CONFIG_TIMEOUT_SECONDS * 1000);
							  pNewTask->param.info.callParam.operateInfo.source = CONFIG_LOCAL_RADIO_ID;//本机发起的呼叫则来源一定是本机的RadioId
							  if (d.HasMember("SessionId") && d["SessionId"].IsString())
							  {
								  strcpy_s(pNewTask->param.info.callParam.operateInfo.SessionId, d["SessionId"].GetString());
								  std::string sessionid = d["SessionId"].GetString();
								  if (g_manager->isRepeat(sessionid))
								  {
									  delete pNewTask;
									  pNewTask = NULL;
									  break;
								  }
							  }
							  pNewTask->param.info.callParam.operateInfo.operate = StartCall;

							  int callType = d["type"].GetInt();
							  if (callType == CLIENT_CALL_TYPE_All)
							  {
								  callType = ALL_CALL;
							  }
							  else if (callType == CLIENT_CALL_TYPE_Group)
							  {
								  callType = GROUP_CALL;
							  }
							  else
							  {
								  callType = PRIVATE_CALL;
							  }
							  pNewTask->param.info.callParam.operateInfo.callType = callType;

							  pNewTask->param.info.callParam.operateInfo.tartgetId = (unsigned long)d["target"].GetInt();
							  if (pNewTask->param.info.callParam.operateInfo.callType == ALL_CALL)
							  {
								  pNewTask->param.info.callParam.operateInfo.tartgetId = ALL_CALL_ID;
							  }
			}
				break;
			case StopCall:
			{
							 pNewTask = new REMOTE_TASK;
							 memset(pNewTask, 0, sizeof(REMOTE_TASK));
							 pNewTask->cmd = REMOTE_CMD_STOP_CALL;
							 pNewTask->timeOutTickCout = GetTickCount() + (CONFIG_TIMEOUT_SECONDS * 1000);
							 pNewTask->param.info.callParam.operateInfo.source = CONFIG_LOCAL_RADIO_ID;//本机发起的呼叫则来源一定是本机的RadioId
							 if (d.HasMember("SessionId") && d["SessionId"].IsString())
							 {
								 strcpy_s(pNewTask->param.info.callParam.operateInfo.SessionId, d["SessionId"].GetString());
								 std::string sessionid = d["SessionId"].GetString();
								 if (g_manager->isRepeat(sessionid))
								 {
									 delete pNewTask;
									 pNewTask = NULL;
									 break;
								 }

							 }
							 pNewTask->param.info.callParam.operateInfo.operate = StopCall;

							 int callType = d["type"].GetInt();
							 if (callType == CLIENT_CALL_TYPE_All)
							 {
								 callType = ALL_CALL;
							 }
							 else if (callType == CLIENT_CALL_TYPE_Group)
							 {
								 callType = GROUP_CALL;
							 }
							 else
							 {
								 callType = PRIVATE_CALL;
							 }
							 pNewTask->param.info.callParam.operateInfo.callType = callType;

							 pNewTask->param.info.callParam.operateInfo.tartgetId = (unsigned long)d["target"].GetInt();
							 if (pNewTask->param.info.callParam.operateInfo.callType == ALL_CALL)
							 {
								 pNewTask->param.info.callParam.operateInfo.tartgetId = ALL_CALL_ID;
							 }
			}
				break;
			default:
				break;
			}
			if (pNewTask)
			{

				push_back_task(pNewTask);
			}
			else
			{
				strcpy_s(status, CLIENT_TRANSFER_FAIL);
			}
		}
		catch (...)
		{
			strcpy_s(status, CLIENT_TRANSFER_FAIL);
		}
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
		printf_s("call response send error");
	}
	catch (...)
	{
		printf_s("call response send error");
	}
}
inline void wlCallStatusActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	if (!wlScheduleIsEnable())
	{
		return;
	}
	g_sn = sn;
	//addCRemotePeer((TcpClient*)pRemote);
	Document d;
	int errorCode = 0;
	ArgumentType args;
	std::string strResp = "";
	char status[64] = { 0 };
	strcpy_s(status, CLIENT_TRANSFER_OK);
	std::string statusText = "";
	REMOTE_TASK *pNewTask = NULL;
	try
	{
		//do nothing
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
		printf_s("call response send error");
	}
	catch (...)
	{
		printf_s("call response send error");
	}
}

inline void wlPlayActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	if (!wlScheduleIsEnable())
	{
		return;
	}
	g_sn = sn;
	//addCRemotePeer((TcpClient*)pRemote);
	Document d;
	int errorCode = 0;
	ArgumentType args;
	std::string strResp = "";
	char status[64] = { 0 };
	strcpy_s(status, CLIENT_TRANSFER_OK);
	std::string statusText = "";
	REMOTE_TASK *pNewTask = NULL;
	try
	{
		try{
			d.Parse(param.c_str());
			int target = d["target"].GetInt();
			/*处理参数*/
			pNewTask = new REMOTE_TASK;
			memset(pNewTask, 0, sizeof(REMOTE_TASK));
			pNewTask->cmd = REMOTE_CMD_SET_PLAY_CALL;
// 			pNewTask->pRemote = pRemote;
// 			pNewTask->callId = sn;
			pNewTask->param.info.setCareCallParam.playParam.targetId = target;
			if (PRIVATE_CALL_TAGET == target)
			{
				pNewTask->param.info.setCareCallParam.playParam.callType = PRIVATE_CALL;
			}
			else if (ALL_CALL_TAGET == target)
			{
				pNewTask->param.info.setCareCallParam.playParam.callType = ALL_CALL;
			}
			else
			{
				pNewTask->param.info.setCareCallParam.playParam.callType = GROUP_CALL;
			}
			if (pNewTask)
			{

				push_back_task(pNewTask);
			}
			else
			{
				strcpy_s(status, CLIENT_TRANSFER_FAIL);
			}
		}
		catch (...)
		{
			strcpy_s(status, CLIENT_TRANSFER_FAIL);
		}
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
		printf_s("call response send error");
	}
	catch (...)
	{
		printf_s("call response send error");
	}
}
inline void wlInfoActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	if (!wlScheduleIsEnable())
	{
		return;
	}
	g_sn = sn;
	//addCRemotePeer((TcpClient*)pRemote);
	Document d;
	int errorCode = 0;
	ArgumentType args;
	std::string strResp = "";
	char status[64] = { 0 };
	strcpy_s(status, CLIENT_TRANSFER_OK);
	std::string statusText = "";
	REMOTE_TASK *pNewTask = NULL;
	try
	{
		try{
			int getType = -1;
			d.Parse(param.c_str());
			/*处理参数*/
			if (d.HasMember("getType") && d["getType"].IsInt())
			{
				getType = d["getType"].GetInt();
			}
			switch (getType)
			{
			case GET_TYPE_CONN:
			{
								  pNewTask = new REMOTE_TASK;
								  memset(pNewTask, 0, sizeof(REMOTE_TASK));
								  pNewTask->cmd = REMOTE_CMD_GET_CONN_STATUS;
								  if (d.HasMember("SessionId") && d["SessionId"].IsString())
								  {
									  strcpy_s(pNewTask->param.info.getInfoParam.getInfo.SessionId, d["SessionId"].GetString());
									  std::string sessionid = d["SessionId"].GetString();
									  if (g_manager->isRepeat(sessionid))
									  {
										  delete pNewTask;
										  pNewTask = NULL;
										  break;
									  }
								  }
			}
				break;
			case GET_TYPE_SESSION_STATUS:
			{
											pNewTask = new REMOTE_TASK;
											memset(pNewTask, 0, sizeof(REMOTE_TASK));
											pNewTask->cmd = REMOTE_CMD_SESSION_STATUS;
											pNewTask->param.info.getInfoParam.getInfo.getType = getType;
											if (d.HasMember("SessionId") && d["SessionId"].IsString())
											{
												strcpy_s(pNewTask->param.info.getInfoParam.getInfo.SessionId, d["SessionId"].GetString());
												std::string sessionid = d["SessionId"].GetString();
												if (g_manager->isRepeat(sessionid))
												{
													delete pNewTask;
													pNewTask = NULL;
													break;
												}
											}
			}
				break;
			case GET_TYPE_SYSTEM_STATUS:
			{
										   pNewTask = new REMOTE_TASK;
										   memset(pNewTask, 0, sizeof(REMOTE_TASK));
										   pNewTask->cmd = REMOTE_CMD_SYSTEM_STATUS;
										   pNewTask->param.info.getInfoParam.getInfo.getType = getType;
										   if (d.HasMember("SessionId") && d["SessionId"].IsString())
										   {
											   strcpy_s(pNewTask->param.info.getInfoParam.getInfo.SessionId, d["SessionId"].GetString());
											   std::string sessionid = d["SessionId"].GetString();
											   if (g_manager->isRepeat(sessionid))
											   {
												   delete pNewTask;
												   pNewTask = NULL;
												   break;
											   }
										   }
			}
				break;
			case GET_TYPE_ONLINE_DEVICES:
			{
											pNewTask = new REMOTE_TASK;
											memset(pNewTask, 0, sizeof(REMOTE_TASK));
											pNewTask->cmd = REMOTE_CMD_MNIS_STATUS;
											pNewTask->param.info.mnisStatusParam.getType = getType;
											if (d.HasMember("SessionId") && d["SessionId"].IsString())
											{
												strcpy_s(pNewTask->param.info.mnisStatusParam.SessionId, d["SessionId"].GetString());
												std::string sessionid = d["SessionId"].GetString();
												if (g_manager->isRepeat(sessionid))
												{
													delete pNewTask;
													pNewTask = NULL;
													break;
												}
											}
			}
				break;
			default:
				break;
			}
			if (pNewTask)
			{

				push_back_task(pNewTask);
			}
			else
			{
				strcpy_s(status, CLIENT_TRANSFER_FAIL);
			}
		}
		catch (...)
		{
			strcpy_s(status, CLIENT_TRANSFER_FAIL);
		}
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
		printf_s("call response send error");
	}
	catch (...)
	{
		printf_s("call response send error");
	}
}

inline void wlMnisQueryGpsActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	if (!wlScheduleIsEnable())
	{
		return;
	}
	g_sn = sn;
	Document d;
	int errorCode = 0;
	ArgumentType args;
	std::string strResp = "";
	char status[64] = { 0 };
	strcpy_s(status, CLIENT_TRANSFER_OK);
	std::string statusText = "";
	REMOTE_TASK *pNewTask = NULL;
	try
	{
		try
		{
			pNewTask = new REMOTE_TASK;
			memset(pNewTask, 0, sizeof(REMOTE_TASK));
			d.Parse(param.c_str());
			//pNewTask->callId = sn;
			pNewTask->cmd = REMOTE_CMD_MNIS_QUERY_GPS;
			//pNewTask->pRemote = pRemote;
			pNewTask->param.info.queryGpsParam.Cycle = d["Cycle"].GetDouble();
			pNewTask->param.info.queryGpsParam.Operate = d["Operate"].GetInt();
			pNewTask->param.info.queryGpsParam.Target = d["Target"].GetInt();
			pNewTask->param.info.queryGpsParam.Type = d["Type"].GetInt();
			if (d.HasMember("SessionId") && d["SessionId"].IsString())
			{
				strcpy_s(pNewTask->param.info.queryGpsParam.SessionId, d["SessionId"].GetString());
				std::string sessionid = d["SessionId"].GetString();
				if (g_manager->isRepeat(sessionid))
				{
					delete pNewTask;
					pNewTask = NULL;
				}
			}
			if (pNewTask)
			{

				push_back_task(pNewTask);
			}
			else
			{
				strcpy_s(status, CLIENT_TRANSFER_FAIL);
			}
		}
		catch (...)
		{
			strcpy_s(status, CLIENT_TRANSFER_FAIL);
		}
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
		printf_s("call response send error");
	}
	catch (...)
	{
		printf_s("call response send error");
	}
}

inline void wlMnisMessageHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	if (!wlScheduleIsEnable())
	{
		return;
	}
	g_sn = sn;
	Document d;
	int errorCode = 0;
	ArgumentType args;
	std::string strResp = "";
	char status[64] = { 0 };
	strcpy_s(status, CLIENT_TRANSFER_OK);
	std::string statusText = "";
	REMOTE_TASK *pNewTask = NULL;
	try
	{
		try
		{
			pNewTask = new REMOTE_TASK;
			memset(pNewTask, 0, sizeof(REMOTE_TASK));
			d.Parse(param.c_str());
			pNewTask->cmd = REMOTE_CMD_MNIS_MSG;
			strcpy_s(pNewTask->param.info.msgParam.Contents, d["Contents"].GetString());
			pNewTask->param.info.msgParam.Source = d["Source"].GetInt();
			pNewTask->param.info.msgParam.Target = d["Target"].GetInt();
			pNewTask->param.info.msgParam.Type = d["Type"].GetInt();
			if (d.HasMember("SessionId") && d["SessionId"].IsString())
			{
				strcpy_s(pNewTask->param.info.msgParam.SessionId, d["SessionId"].GetString());
				std::string sessionid = d["SessionId"].GetString();
				if (g_manager->isRepeat(sessionid))
				{
					delete pNewTask;
					pNewTask = NULL;
				}
			}
			if (pNewTask)
			{

				push_back_task(pNewTask);
			}
			else
			{
				strcpy_s(status, CLIENT_TRANSFER_FAIL);
			}
		}
		catch (...)
		{
			strcpy_s(status, CLIENT_TRANSFER_FAIL);
		}
		//do nothing
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
		printf_s("call response send error");
	}
	catch (...)
	{
		printf_s("call response send error");
	}
}

inline void wlMnisStatusHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	if (!wlScheduleIsEnable())
	{
		return;
	}
	g_sn = sn;
	Document d;
	int errorCode = 0;
	ArgumentType args;
	std::string strResp = "";
	char status[64] = { 0 };
	strcpy_s(status, CLIENT_TRANSFER_OK);
	std::string statusText = "";
	REMOTE_TASK *pNewTask = NULL;
	try
	{
		try
		{
			pNewTask = new REMOTE_TASK;
			memset(pNewTask, 0, sizeof(REMOTE_TASK));
			d.Parse(param.c_str());
			pNewTask->cmd = REMOTE_CMD_MNIS_STATUS;
			pNewTask->param.info.mnisStatusParam.getType = d["getType"].GetInt();
			if (d.HasMember("SessionId") && d["SessionId"].IsString())
			{
				strcpy_s(pNewTask->param.info.mnisStatusParam.SessionId, d["SessionId"].GetString());
				std::string sessionid = d["SessionId"].GetString();
				if (g_manager->isRepeat(sessionid))
				{
					delete pNewTask;
					pNewTask = NULL;
				}
			}
			if (pNewTask)
			{
				push_back_task(pNewTask);
			}
			else
			{
				strcpy_s(status, CLIENT_TRANSFER_FAIL);
			}
		}
		catch (...)
		{
			strcpy_s(status, CLIENT_TRANSFER_FAIL);
		}
		//do nothing
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
		printf_s("call response send error");
	}
	catch (...)
	{
		printf_s("call response send error");
	}
}

/*未使用的方法*/
inline void wlMnisLocationIndoorHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	if (!wlScheduleIsEnable())
	{
		return;
	}
	g_sn = sn;
	Document d;
	int errorCode = 0;
	ArgumentType args;
	std::string strResp = "";
	char status[64] = { 0 };
	strcpy_s(status, CLIENT_TRANSFER_OK);
	std::string statusText = "";
	REMOTE_TASK *pNewTask = NULL;
	try
	{
		try
		{
			pNewTask = new REMOTE_TASK;
			memset(pNewTask, 0, sizeof(REMOTE_TASK));
			d.Parse(param.c_str());
			//pNewTask->callId = sn;
			pNewTask->cmd = REMOTE_CMD_MNIS_LOCATION_INDOOR_CONFIG;
			//pNewTask->pRemote = pRemote;
			pNewTask->param.info.locationParam.isEnable = d["IsEnable"].GetBool();
			pNewTask->param.info.locationParam.internal = d["Interval"].GetInt();
			pNewTask->param.info.locationParam.ibconNum = d["iBeaconNumber"].GetInt();
			pNewTask->param.info.locationParam.isEmergency = d["IsEmergency"].GetBool();
			if (pNewTask)
			{

				push_back_task(pNewTask);
			}
			else
			{
				strcpy_s(status, CLIENT_TRANSFER_FAIL);
			}
		}
		catch (...)
		{
			strcpy_s(status, CLIENT_TRANSFER_FAIL);
		}
		//do nothing
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
		printf_s("call response send error");
	}
	catch (...)
	{
		printf_s("call response send error");
	}
}
#endif