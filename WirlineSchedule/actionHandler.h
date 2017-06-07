#ifndef ACTIONHANDER_H
#define ACTIONHANDER_H
#include "stdafx.h"
#include <string>
#include "../lib/type.h"

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
			//pNewTask->callId = sn;
			//pNewTask->pRemote = pRemote;
			pNewTask->cmd = REMOTE_CMD_CONFIG;
			if (d.HasMember("IsEnable") && d["IsEnable"].IsBool())
			{
				pNewTask->param.info.configParam.IsEnable = d["IsEnable"].GetBool();
			}
			pNewTask->param.info.configParam.Type = d["Type"].GetInt();
			if (d.HasMember("Svr") && d["Svr"].IsObject())
			{
				tempJson = d["Svr"].GetObject();
				if (tempJson.HasMember("Ip") && tempJson["Ip"].IsString())
				{
					strcpy_s(pNewTask->param.info.configParam.Svr.Ip, tempJson["Ip"].GetString());
				}
				if (tempJson.HasMember("Port") && tempJson["Port"].IsInt())
				{
					pNewTask->param.info.configParam.Svr.Port = tempJson["Port"].GetInt();
				}
			}
			if (d.HasMember("DefaultGroupId") && d["DefaultGroupId"].IsInt())
			{
				pNewTask->param.info.configParam.defaultGroup = (unsigned long)d["DefaultGroupId"].GetInt();
			}
			if (d.HasMember("DefaultGroupId") && d["DefaultGroupId"].IsInt())
			{
				pNewTask->param.info.configParam.defaultSlot = (_SlotNumber)d["DefaultChannel"].GetInt();
			}

			if (d["Dongle"].IsObject())
			{
				tempJson = d["Dongle"].GetObject();
				if (tempJson.HasMember("Com") && tempJson["Com"].IsInt())
				{
					pNewTask->param.info.configParam.dongle.donglePort = tempJson["Com"].GetInt();
				}
			}

			pNewTask->param.info.configParam.hangTime = d["MinHungTime"].GetInt() * 1000;
			if (d.HasMember("LocalPeerId") && d["LocalPeerId"].IsInt())
			{
				pNewTask->param.info.configParam.localPeerId = (unsigned long)d["LocalPeerId"].GetInt();
			}
			if (d.HasMember("LocalRadioId") && d["LocalRadioId"].IsInt())
			{
				pNewTask->param.info.configParam.localRadioId = (unsigned long)d["LocalRadioId"].GetInt();
			}
			if (d.HasMember("MaxSiteAliveTime") && d["MaxSiteAliveTime"].IsInt())
			{
				pNewTask->param.info.configParam.masterHeartTime = d["MaxSiteAliveTime"].GetInt() * 1000;
			}

			if (d["Master"].IsObject())
			{
				tempJson = d["Master"].GetObject();
				if (tempJson.HasMember("Ip") && tempJson["Ip"].IsString())
				{
					strcpy_s(pNewTask->param.info.configParam.master.ip, tempJson["Ip"].GetString());
				}
				if (tempJson.HasMember("Port") && tempJson["Port"].IsInt())
				{
					pNewTask->param.info.configParam.master.port = tempJson["Port"].GetInt();
				}
			}

			if (d["Mnis"].IsObject())
			{
				tempJson = d["Mnis"].GetObject();
				if (tempJson.HasMember("Ip") && tempJson["Ip"].IsString())
				{
					strcpy_s(pNewTask->param.info.configParam.mnis.ip, tempJson["Ip"].GetString());
				}
				if (tempJson.HasMember("Port") && tempJson["Port"].IsInt())
				{
					pNewTask->param.info.configParam.mnis.port = tempJson["Port"].GetInt();
				}
			}
			if (d.HasMember("MnisId") && d["MnisId"].IsInt())
			{
				pNewTask->param.info.configParam.MnisId = d["MnisId"].GetInt();
			}
			if (d.HasMember("MaxPeerAliveTime") && d["MaxPeerAliveTime"].IsInt())
			{
				pNewTask->param.info.configParam.peerHeartTime = d["MaxPeerAliveTime"].GetInt() * 1000;
			}
			int recordType = 1;
			if (d.HasMember("Type") && d["Type"].IsInt())
			{
				recordType = d["Type"].GetInt();
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
			pNewTask->param.info.configParam.recordType = temp;
			if (d.HasMember("AudioPath") && d["AudioPath"].IsString())
			{
				strcpy_s(pNewTask->param.info.configParam.audioPath, d["AudioPath"].GetString());
			}
			//sprintf_s(pNewTask->param.info.configParam.audioPath, "%s\\Voice", pNewTask->param.info.configParam.audioPath);
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
							  //pNewTask->pRemote = pRemote;
							  //pNewTask->callId = sn;
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
// 							 pNewTask->pRemote = pRemote;
// 							 pNewTask->callId = sn;
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
			d.Parse(param.c_str());
			int getType = d["getType"].GetInt();
			/*处理参数*/
			switch (getType)
			{
			case GET_TYPE_CONN:
			{
								  pNewTask = new REMOTE_TASK;
								  memset(pNewTask, 0, sizeof(REMOTE_TASK));
								  pNewTask->cmd = REMOTE_CMD_GET_CONN_STATUS;
			}
				break;
			case GET_TYPE_ONLINE_DEVICES:
			{
											pNewTask = new REMOTE_TASK;
											memset(pNewTask, 0, sizeof(REMOTE_TASK));
											pNewTask->cmd = REMOTE_CMD_MNIS_STATUS;
											pNewTask->param.info.mnisStatusParam.getType = getType;
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
			//pNewTask->callId = sn;
			pNewTask->cmd = REMOTE_CMD_MNIS_MSG;
			//pNewTask->pRemote = pRemote;
			//swprintf_s(pNewTask->param.info.msgParam.Contents, L"%s", d["Contents"].GetString());
			//MultiByteToWideChar(CP_ACP, 0, d["Contents"].GetString(), -1, pNewTask->param.info.msgParam.Contents, 256);
			strcpy_s(pNewTask->param.info.msgParam.Contents, d["Contents"].GetString());
			pNewTask->param.info.msgParam.Source = d["Source"].GetInt();
			pNewTask->param.info.msgParam.Target = d["Target"].GetInt();
			pNewTask->param.info.msgParam.Type = d["Type"].GetInt();
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
			//pNewTask->callId = sn;
			pNewTask->cmd = REMOTE_CMD_MNIS_STATUS;
			//pNewTask->pRemote = pRemote;
			pNewTask->param.info.mnisStatusParam.getType = d["getType"].GetInt();
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