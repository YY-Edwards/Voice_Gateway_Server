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
struct SessionStatus{
	std::string sessionId;
	int status = -1;
};

class DispatchOperate
{
public:
	DispatchOperate();
	~DispatchOperate();
	static void OnConnect(CRemotePeer* pRemotePeer);
	static void OnDisConnect(CRemotePeer* pRemotePeer);
	static void OnData( int call, Respone data);
	static void OnTcpData( int call, TcpRespone data);
	static void OnRadioUsb(bool isConnected );
	void connect(radio_t radioCfg, mnis_t mnisCfg, location_t locationCfg, locationindoor_t locationIndoorCfg);
	void locationIndoorConfig(int Interval, int iBeaconNumber, bool isEmergency);
	void call( int type, int op, int id,std::string sessionId);
	void control(int type, int id, std::string sessionId);
	bool getGps(int id, int querymode, double cycle, std::string sessionId,int operate);
	bool stopGps( int id, int querymode,std::string sessionId);
	bool sendMsg(std::string text, int id, int opterateType, std::string sessionId);
	void getStatus( int type,std::string sessionId);
	void setCallBack();
	void disConnect();
	bool isTcpConnect;

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
	std::mutex m_sessionlocker;
	std::string  mnisIP;
	std::list <Command> udpTimeoutList;
	std::list<SessionStatus>  sessionStatusList;
	
	void sendSessIonStatus(std::string sessionId);
	std::string serial;
	radio_t m_radioCfg;
	mnis_t m_mnisCfg;
};

