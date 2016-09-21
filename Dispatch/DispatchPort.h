
#undef ERROR
#include <../mutex.h>
#include "DispatchOperate.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#pragma once
class CClientConnection
{
public:
	CClientConnection(SOCKET s, const SOCKADDR_IN& addr)
		:m_socketClient(s)
	{
		memcpy(&m_addr, &addr, sizeof(addr));
	}

	CClientConnection& operator = (const CClientConnection& c){
		m_socketClient = c.m_socketClient;
		memcpy(&m_addr, &c.m_addr, sizeof(m_addr));
		return *this;
	}

	void HandleIncomingData(LPCSTR pMessage)
	{
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		int len = 0;
		int ret = 0;
		try{
			std::string strMsg;
			for (int i = 0; i < strlen(pMessage); i++)
			{
				rapidjson::Document doc;
				doc.Parse(pMessage);
				
				string call = doc["call"].GetString();
					//string param = doc["param"].GetString();
					
					int sn = doc["param"]["sn"].GetInt();
					if (call.compare("connect") == 0)                            // 连接车台
					{
						string radioIP = doc["param"]["radioIP"].GetString();
						string mnisIP = doc["param"]["mnisIP"].GetString();
						pDispatchOperate->Connect(radioIP.c_str(), mnisIP.c_str(),sn);
					}
					else if (call.compare("call") == 0)                          //单呼
					{
						int id = doc["param"]["id"].GetInt();
						pDispatchOperate->call(id, sn);
					}
					else if (call.compare("groupCall") == 0)                     //组呼
					{
						int id = doc["param"]["id"].GetInt();
						pDispatchOperate->groupCall(id, sn);
					}
					else if (call.compare("allCall") == 0)                       //全呼
					{
						pDispatchOperate->allCall(sn);
					}
					else if (call.compare("stopCall") == 0)                       //停止呼叫
					{
						pDispatchOperate->stopCall(sn);
					}
					else if (call.compare("remotePowerOn") == 0)                   //遥开
					{
						int id = doc["param"]["id"].GetInt();
						pDispatchOperate->remotePowerOn(id, sn);
					} 
					else if (call.compare("remotePowerOff") == 0)                 //遥闭
					{
						int id = doc["param"]["id"].GetInt();
						pDispatchOperate->remotePowerOff(id, sn);
					}

					else if (call.compare("radioCheck") == 0)                    //在线检测
					{
						int id = doc["param"]["id"].GetInt();
						pDispatchOperate->radioCheck(id, sn);
					}
					else if (call.compare("wiretap") == 0)                       //远程监听
					{
						int id = doc["param"]["id"].GetInt();
						pDispatchOperate->wiretap(id, sn);
					} 
					else if (call.compare("sendSms") == 0)                       //发送短信
					{
						int id = doc["param"]["id"].GetInt();
						string msg = doc["param"]["msg"].GetString();
						wchar_t * text = new wchar_t[msg.size()];
						pDispatchOperate->sendSms(id, text, sn);
					}
					else if (call.compare("sendGroupSms") == 0)                  //群发
					{
						int id = doc["param"]["id"].GetInt();
						string msg = doc["param"]["msg"].GetString();
						wchar_t * text = new wchar_t[msg.size()];
						pDispatchOperate->sendGroupSms(id, text, sn);
					}
					else if (call.compare("getGps") == 0)                       //gps查询
					{
						int id = doc["param"]["id"].GetInt();
						int queryMode = doc["param"]["queryMode"].GetInt();
						int cycle = doc["param"]["cycle"].GetInt();
						pDispatchOperate->getGps(id, queryMode, cycle, sn);
					}
					else if (call.compare("cancelPollGps") == 0)                     //取消周期查询
					{
						int id = doc["param"]["id"].GetInt();
						pDispatchOperate->cancelPollGps(id, sn);
					}   
					else if (call.compare("getOverturnGps") == 0)                    //gps翻转
					{  
						string  ip = doc["param"]["ip"].GetString();
						pDispatchOperate->getOverturnGps(ip.c_str());
					}
				}
			
		}
		catch (std::exception e)
		{
			//LOG(ERROR) << "handle incoming data error:" << e.what();
		}
	}
	void Close()
	{
		closesocket(m_socketClient);
	}
public:
	SOCKET						m_socketClient;
	SOCKADDR_IN					m_addr;
	DispatchOperate *pDispatchOperate = new DispatchOperate();
		


};

class CDispatchPort
{
public:
	CDispatchPort();
	~CDispatchPort();
private:
	SOCKET						m_serverSocket;
	BOOL						m_bServerIsRunning;

	//CFcsServerInterface			*m_pDataHandler;

	// work thread
	HANDLE							m_hAccetpThread;
	XyLib::CMutex					m_connectionsLocker;
	std::deque<CClientConnection>		m_clientConnections;

public:
	static DWORD WINAPI AcceptThread(LPVOID pVoid);

public:
	int Start(DWORD dwIpAddr, WORD port);
	void Close();
	void HandleClientMessage(SOCKET s, LPCSTR pMessage, DWORD dwSize);
	void CloseConnection(SOCKET s);
	void AddConnection(SOCKET s, const SOCKADDR_IN& addr);
	bool sendResultToClient(string jsonResult);
};

