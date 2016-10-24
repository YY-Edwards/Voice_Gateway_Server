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
}

inline void push_back_task(REMOTE_TASK* task)
{
	WaitForSingleObject(g_taskLockerEvent, INFINITE);
	g_remoteCommandTaskQueue.push_back(task);
	SetEvent(g_taskLockerEvent);
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
	g_pTempClient = new TcpClient();
	g_pTempClient->addr = client->addr;
	g_pTempClient->s = client->s;
	bool isHave = false;
	for (auto i = g_onLineClients.begin(); i != g_onLineClients.end(); i++)
	{
		TcpClient *p = *i;
		if (p->s == g_pTempClient->s)
		{
			isHave = true;
			break;
		}
	}
	if (!isHave)
	{
		g_onLineClients.push_back(g_pTempClient);
	}
}

inline void configActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	g_sn = sn;
	addCRemotePeer((TcpClient*)pRemote);
	Document d;
	int errorCode = 0;
	std::map<std::string, std::string> args;
	std::string strResp = "";
	char status[64] = "0";
	std::string statusText = "";
	args["status"] = "0";//调用成功
	args["module"] = "wl";//wirelan模块
	g_pNewTask = new REMOTE_TASK;
	try
	{
		d.Parse(param.c_str());
		/*获取参数*/
		g_pNewTask->sn = sn;
		g_pNewTask->pRemote = pRemote;
		g_pNewTask->cmd = REMOTE_CMD_CONFIG;
		g_pNewTask->param.info.configParam.defaultGroup = (unsigned long)atoll(d["DEFAULT_GROUP"].GetString());
		g_pNewTask->param.info.configParam.defaultSlot = (_SlotNumber)atoi(d["DEFAULT_SLOT"].GetString());
		g_pNewTask->param.info.configParam.donglePort = atoi(d["DONGLE_PORT"].GetString());
		g_pNewTask->param.info.configParam.hangTime = atol(d["HUNG_TIME"].GetString()) * 1000;
		g_pNewTask->param.info.configParam.localPeerId = (unsigned long)atoll(d["LOCAL_PEER_ID"].GetString());
		g_pNewTask->param.info.configParam.localRadioId = (unsigned long)atoll(d["LOCAL_RADIO_ID"].GetString());
		g_pNewTask->param.info.configParam.masterHeartTime = atol(d["MASTER_HEART_TIME"].GetString())*1000;
		strcpy_s(g_pNewTask->param.info.configParam.masterIp, d["MASTER_IP"].GetString());
		g_pNewTask->param.info.configParam.masterPort = atoi(d["MASTER_PORT"].GetString());
		g_pNewTask->param.info.configParam.peerHeartTime = atol(d["PEER_HEART_TIME"].GetString())*1000;
		std::string recordType = d["RECORD_TYPE"].GetString();
		_RECORD_TYPE_VALUE temp = LCP;
		if (recordType == "IPSC")
		{
			temp = IPSC;
		}
		else if (recordType == "CPC")
		{
			temp = CPC;
		}
		g_pNewTask->param.info.configParam.recordType = temp;
		/*config配置优先级为最高*/
		push_front_task(g_pNewTask);
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
	}
	catch (...)
	{
	}
}

inline void initialCallActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	g_sn = sn;
	addCRemotePeer((TcpClient*)pRemote);
	Document d;
	int errorCode = 0;
	std::map<std::string, std::string> args;
	std::string strResp = "";
	char status[64] = "0";
	std::string statusText = "";
	args["status"] = "0";//调用成功
	args["module"] = "wl";//wirelan模块
	g_pNewTask = new REMOTE_TASK;
	try
	{
		d.Parse(param.c_str());
		/*处理参数*/
		g_pNewTask->cmd = REMOTE_CMD_CALL;
		g_pNewTask->pRemote = pRemote;
		g_pNewTask->sn = sn;
		g_pNewTask->param.info.callParam.callType = atoi(d["callType"].GetString());
		g_pNewTask->param.info.callParam.tartgetId = (unsigned long)atoll(d["tartgetId"].GetString());
		if (g_pNewTask->param.info.callParam.callType == ALL_CALL)
		{
			g_pNewTask->param.info.callParam.tartgetId = ALL_CALL_ID;
		}
		push_front_task(g_pNewTask);
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
	}
	catch (...)
	{
	}
}
inline void stopCallActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	g_sn = sn;
	addCRemotePeer((TcpClient*)pRemote);
	Document d;
	int errorCode = 0;
	std::map<std::string, std::string> args;
	std::string strResp = "";
	char status[64] = "0";
	std::string statusText = "";
	args["status"] = "0";//调用成功
	args["module"] = "wl";//wirelan模块
	g_pNewTask = new REMOTE_TASK;
	try
	{
		d.Parse(param.c_str());
		/*处理参数*/
		g_pNewTask->cmd = REMOTE_CMD_STOP_CALL;
		g_pNewTask->pRemote = pRemote;
		g_pNewTask->sn = sn;
		push_front_task(g_pNewTask);
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
	}
	catch (...)
	{
	}
}
inline void setPlayCallOfCareActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn,const std::string& type)
{
	g_sn = sn;
	addCRemotePeer((TcpClient*)pRemote);
	Document d;
	int errorCode = 0;
	std::map<std::string, std::string> args;
	std::string strResp = "";
	char status[64] = "0";
	std::string statusText = "";
	args["status"] = "0";//调用成功
	args["module"] = "wl";//wirelan模块
	g_pNewTask = new REMOTE_TASK;
	try
	{
		d.Parse(param.c_str());
		/*处理参数*/
		g_pNewTask->cmd = REMOTE_CMD_SET_PLAY_CALL;
		g_pNewTask->pRemote = pRemote;
		g_pNewTask->sn = sn;
		g_pNewTask->param.info.setCareCallParam.callType = atoi(d["callType"].GetString());
		g_pNewTask->param.info.setCareCallParam.srcId = (unsigned long)atoll(d["srcId"].GetString());
		g_pNewTask->param.info.setCareCallParam.tgtId = (unsigned long)atoll(d["tgtId"].GetString());
		push_front_task(g_pNewTask);
		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
		pRemote->sendResponse(strResp.c_str(), strResp.size());
	}
	catch (std::exception* e)
	{
	}
	catch (...)
	{
	}
}
#endif