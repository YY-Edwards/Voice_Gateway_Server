#pragma once
#undef ERROR
#include "jsoncpp\include\json\json.h"
#include <../mutex.h>
#include "DispatchOperate.h"

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
				Json::Reader reader;
				Json::Value root;
				Json::Value root_1;
				if (reader.parse(pMessage, root_1))  // reader��Json�ַ���������root��root������Json��������Ԫ��  
				{
					string call = root_1["call"].asString();  
					string param = root_1["param"].asString();
					reader.parse(param, root);
					int sn = root["sn"].asInt();
					if (call.compare("connect") == 0)                            // ���ӳ�̨
					{
						string radioIP = root["radioIP"].asString();
						string mnisIP = root["mnisIP"].asString();
						pDispatchOperate->connect(radioIP.c_str(), mnisIP.c_str(),sn);
					}
					else if (call.compare("call") == 0)                          //����
					{
						int id = root["id"].asInt();
						pDispatchOperate->call(id, sn);
					}
					else if (call.compare("groupCall") == 0)                     //���
					{
						int id = root["id"].asInt();
						pDispatchOperate->groupCall(id, sn);
					}
					else if (call.compare("allCall") == 0)                       //ȫ��
					{
						pDispatchOperate->allCall(sn);
					}
					else if (call.compare("stopCall") == 0)                       //ֹͣ����
					{
						pDispatchOperate->stopCall(sn);
					}
					else if (call.compare("remotePowerOn") == 0)                   //ң��
					{
						int id = root["id"].asInt();
						pDispatchOperate->remotePowerOn(id, sn);
					} 
					else if (call.compare("remotePowerOff") == 0)                 //ң��
					{
						int id = root["id"].asInt();
						pDispatchOperate->remotePowerOff(id, sn);
					}

					else if (call.compare("radioCheck") == 0)                    //���߼��
					{
						int id = root["id"].asInt();
						pDispatchOperate->radioCheck(id, sn);
					}
					else if (call.compare("wiretap") == 0)                       //Զ�̼���
					{
						int id = root["id"].asInt();
						pDispatchOperate->wiretap(id, sn);
					} 
					else if (call.compare("sendSms") == 0)                       //���Ͷ���
					{
						int id = root["id"].asInt();
						string msg = root["msg"].asString();
						wchar_t * text = new wchar_t[msg.size()];
						pDispatchOperate->sendSms(id, text, sn);
					}
					else if (call.compare("sendGroupSms") == 0)                  //Ⱥ��
					{
						int id = root["id"].asInt();
						string msg = root["msg"].asString();
						wchar_t * text = new wchar_t[msg.size()];
						pDispatchOperate->sendGroupSms(id, text, sn);
					}
					else if (call.compare("getGps") == 0)                       //gps��ѯ
					{
						int id = root["id"].asInt();
						int queryMode = root["queryMode"].asInt();
						int cycle = root["cycle"].asInt();
						pDispatchOperate->getGps(id, queryMode, cycle, sn);
					}
					else if (call.compare("cancelPollGps") == 0)                     //ȡ�����ڲ�ѯ
					{
						int id = root["id"].asInt();
						pDispatchOperate->cancelPollGps(id, sn);
					}   
					else if (call.compare("getOverturnGps") == 0)                    //gps��ת
					{  
						string  ip = root["ip"].asString();
						pDispatchOperate->getOverturnGps(ip.c_str());
					}
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

