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

inline void wlConnectActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
{
	g_sn = sn;
	addCRemotePeer((TcpClient*)pRemote);
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
	try
	{
		try
		{
			d.Parse(param.c_str());
			/*获取参数*/
			pNewTask->sn = sn;
			pNewTask->pRemote = pRemote;
			pNewTask->cmd = REMOTE_CMD_CONFIG;
			pNewTask->param.info.configParam.defaultGroup = (unsigned long)d["DefaultGroupId"].GetInt();
			pNewTask->param.info.configParam.defaultSlot = (_SlotNumber)d["DefaultChannel"].GetInt();

			if (d["Dongle"].IsObject())
			{
				tempJson = d["Dongle"].GetObject();
				pNewTask->param.info.configParam.dongle.donglePort = tempJson["Com"].GetInt();
			}

			pNewTask->param.info.configParam.hangTime = d["MinHungTime"].GetInt() * 1000;
			pNewTask->param.info.configParam.localPeerId = (unsigned long)d["LocalPeerId"].GetInt();
			pNewTask->param.info.configParam.localRadioId = (unsigned long)d["LocalRadioId"].GetInt();
			pNewTask->param.info.configParam.masterHeartTime = d["MaxSiteAliveTime"].GetInt() * 1000;

			if (d["Master"].IsObject())
			{
				tempJson = d["Master"].GetObject();
				strcpy_s(pNewTask->param.info.configParam.master.masterIp, tempJson["Ip"].GetString());
				pNewTask->param.info.configParam.master.masterPort = tempJson["Port"].GetInt();
			}

			pNewTask->param.info.configParam.peerHeartTime = d["MaxPeerAliveTime"].GetInt() * 1000;
			std::string recordType = d["Type"].GetString();
			_RECORD_TYPE_VALUE temp = LCP;
			if (recordType == "IPSC")
			{
				temp = IPSC;
			}
			else if (recordType == "CPC")
			{
				temp = CPC;
			}
			pNewTask->param.info.configParam.recordType = temp;
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
	g_sn = sn;
	addCRemotePeer((TcpClient*)pRemote);
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
							  pNewTask->cmd = REMOTE_CMD_CALL;
							  pNewTask->pRemote = pRemote;
							  pNewTask->sn = sn;
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
							 pNewTask->cmd = REMOTE_CMD_STOP_CALL;
							 pNewTask->pRemote = pRemote;
							 pNewTask->sn = sn;
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

				push_front_task(pNewTask);
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
	g_sn = sn;
	addCRemotePeer((TcpClient*)pRemote);
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
	g_sn = sn;
	addCRemotePeer((TcpClient*)pRemote);
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
			pNewTask->cmd = REMOTE_CMD_SET_PLAY_CALL;
			pNewTask->pRemote = pRemote;
			pNewTask->sn = sn;
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

				push_front_task(pNewTask);
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
	g_sn = sn;
	addCRemotePeer((TcpClient*)pRemote);
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
								  pNewTask->cmd = REMOTE_CMD_GET_CONN_STATUS;
								  pNewTask->pRemote = pRemote;
								  pNewTask->sn = sn;
			}
				break;
			default:
				break;
			}
			if (pNewTask)
			{

				push_front_task(pNewTask);
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
//inline void stopCallActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn, const std::string& type)
//{
//	g_sn = sn;
//	addCRemotePeer((TcpClient*)pRemote);
//	Document d;
//	int errorCode = 0;
//	ArgumentType args;
//	std::string strResp = "";
//	char status[64] = "0";
//	std::string statusText = "";
//	args["status"] = CLIENT_TRANSFER_OK;//调用成功
//	//args["module"] = "wl";//wirelan模块
//	REMOTE_TASK *pNewTask = new REMOTE_TASK;
//	try
//	{
//		d.Parse(param.c_str());
//		/*处理参数*/
//		pNewTask->cmd = REMOTE_CMD_STOP_CALL;
//		pNewTask->pRemote = pRemote;
//		pNewTask->sn = sn;
//		push_front_task(pNewTask);
//		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
//		pRemote->sendResponse(strResp.c_str(), strResp.size());
//	}
//	catch (std::exception* e)
//	{
//	}
//	catch (...)
//	{
//	}
//}
//inline void setPlayCallOfCareActionHandler(CRemotePeer* pRemote, const std::string& param, uint64_t sn,const std::string& type)
//{
//	g_sn = sn;
//	addCRemotePeer((TcpClient*)pRemote);
//	Document d;
//	int errorCode = 0;
//	ArgumentType args;
//	std::string strResp = "";
//	char status[64] = "0";
//	std::string statusText = "";
//	args["status"] = CLIENT_TRANSFER_OK;//调用成功
//	//args["module"] = "wl";//wirelan模块
//	REMOTE_TASK *pNewTask = new REMOTE_TASK;
//	try
//	{
//		d.Parse(param.c_str());
//		/*处理参数*/
//		pNewTask->cmd = REMOTE_CMD_SET_PLAY_CALL;
//		pNewTask->pRemote = pRemote;
//		pNewTask->sn = sn;
//		pNewTask->param.info.setCareCallParam.callType = d["callType"].GetInt();
//		pNewTask->param.info.setCareCallParam.srcId = (unsigned long)d["srcId"].GetInt();
//		pNewTask->param.info.setCareCallParam.tgtId = (unsigned long)d["tgtId"].GetInt();
//		push_front_task(pNewTask);
//		strResp = CRpcJsonParser::buildResponse(status, sn, errorCode, statusText.c_str(), args);
//		pRemote->sendResponse(strResp.c_str(), strResp.size());
//	}
//	catch (std::exception* e)
//	{
//	}
//	catch (...)
//	{
//	}
//}
#endif