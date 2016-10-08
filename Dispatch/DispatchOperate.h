#include "XNLConnection.h"
#include "TextMsg.h"
#include "RadioARS.h"
#include "RadioGps.h"
#include <map>
#include "../../lib/AES/Aes.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#pragma once
class DispatchOperate
{
public:
	DispatchOperate();
	~DispatchOperate();
	int Connect(CRemotePeer* pRemote,const char* ip, const char  * pIP, int callId);
	int call(CRemotePeer* pRemote, int id, int callId);
	int groupCall(CRemotePeer* pRemote, int id, int callId);
	int allCall(CRemotePeer* pRemote, int callId);
	int stopCall(CRemotePeer* pRemote, int callId);
	int remotePowerOn(CRemotePeer* pRemote, int id, int callId);
	int remotePowerOff(CRemotePeer* pRemote, int id, int callId);
	int radioCheck(CRemotePeer* pRemote, int id, int callId);
	int wiretap(CRemotePeer* pRemote, int id, int callId);
	int sendSms(CRemotePeer* pRemote, int id, wchar_t* message, int callId);
	int sendGroupSms(CRemotePeer* pRemote, int id, wchar_t* message, int callId);
	int getGps(CRemotePeer* pRemote, int id, int queryMode, int cycle, int callId);              //queryMode  12: 常规查询   13： 常规周期查询  14：csbk查询   15： csbk 周期查询
	int cancelPollGps(CRemotePeer* pRemote, int id, int callId);
	static DWORD WINAPI TCPConnectionThread(LPVOID lpParam);
	static DWORD WINAPI TimeOutThread(LPVOID lpParam);
	int RadioConnect();
	void TcpConnect();
	int getOverturnGps(CRemotePeer* pRemote, const char* ip, int callId);
	int getLic(const char* licPath);
	int tcpConnect(const char *ip);
	int radioUdpConnect(const char *ip);
	int mnisUdpConnect(const char* ip);
	
private:
	void AddAllCommand(CRemotePeer* pRemote,int callId, int command);
	void TimeOut();
	CXNLConnection  *pXnlConnection;
	CTextMsg        pTextMsg;
	CRadioARS       pRadioARS;
	CRadioGps       pRadioGPS;
	int             m_queryMode;
	DWORD            dwIP;
	DWORD            dwip;
	bool             textConnectResult;
	bool             ARSConnectResult;
	bool            GPSConnectResult;
	map <int, int> gpsDic;
	CRemotePeer* pRemotePeer;
	int callID;
};

