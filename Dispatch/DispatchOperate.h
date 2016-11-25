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
	static void OnData( int call, Respone data);
	static void OnTcpData( int call, TcpRespone data);
	void connect( const char * ip, const char* mIp);
	void call( int type, int op, int id);
	void control( int type, int id);
	bool getGps(int id, int querymode, double cycle);
	bool stopGps( int id, int querymode);
	bool sendMsg( std::string text, int id,  int opterateType);
	void getStatus( int type);
	void setCallBack();
	void disConnect();
private:
	std::map <int, int> gpsDic;
	int getLic(const char* licPath);
	void sendConnectStatusToClient();
	void sendRadioStatusToClient(CRemotePeer* pRemote);
	void sendCallStatusToClient();
	void send2Client(char* name, ArgumentType args);
	void addPeer(CRemotePeer * peer);
	void delPeer(CRemotePeer * peer);
	CDataScheduling* pDs;
	CTcpScheduling* pTs;
	bool isUdpConnect;
	std::mutex m_locker;
	std::string  mnisIP;
	
};

