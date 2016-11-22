#pragma once
#include "XNLConnection.h"
#include <map>
#include <mutex>
#include<list>
#include "../lib/AES/Aes.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "../lib/radio/DataScheduling.h"
#include "tcpScheduling.h"


class DispatchOperate
{
public:
	DispatchOperate();
	~DispatchOperate();
	static void OnConnect(CRemotePeer* pRemotePeer);
	static void OnDisConnect(CRemotePeer* pRemotePeer);
	static void OnData(TcpClient* peer, int callId, int call, Respone data);
	static void OnTcpData(TcpClient* peer, int callId, int call, TcpRespone data);
	void connect(TcpClient * tp, const char * ip, const char* mIp, int callId);
	void call(TcpClient * tp, int type, int op, int id, int callId);
	void control(TcpClient * tp, int type, int id, int callId);
	bool getGps(TcpClient * tp, int id, int querymode, double cycle, int callId);
	bool stopGps(TcpClient * tp, int id, int querymode, int callId);
	bool sendMsg(TcpClient * tp, wchar_t * text, int id, int callId, int opterateType);
	void getStatus(TcpClient * tp, int type, int callId);
	void setCallBack();
private:
	std::map <int, int> gpsDic;
	int getLic(const char* licPath);
	void sendConnectStatusToClient(CRemotePeer* pRemote);
	void sendRadioStatusToClient(CRemotePeer* pRemote);
	void sendCallStatusToClient();
	void send2Client(char* name, ArgumentType args, TcpClient * tp);
	void addPeer(CRemotePeer * peer);
	void delPeer(CRemotePeer * peer);
	CTcpScheduling ts;
	CDataScheduling ds;
	bool isUdpConnect;
	std::mutex m_locker;
};

