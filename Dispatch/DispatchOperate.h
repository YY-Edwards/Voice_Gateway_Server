#include "XNLConnection.h"
#include "TextMsg.h"
#include "RadioARS.h"
#include "RadioGps.h"
#include <map>
#include "../../lib/AES/Aes.h"


#pragma once
class DispatchOperate
{
public:
	DispatchOperate();
	//DispatchOperate(CRemotePeer * pRemotePeer);
	~DispatchOperate();
	int Connect(const char* ip, const char  * pIP, int callId);
	int call(int id, int callId);
	int groupCall(int id, int callId);
	int allCall( int callId);
	int stopCall( int callId);
	int remotePowerOn(int id, int callId);
	int remotePowerOff(int id, int callId);
	int radioCheck(int id, int callId);
	int wiretap(int id, int callId);
	int sendSms(int id, wchar_t* message, int callId);
	int sendGroupSms(int id, wchar_t* message, int callId);
	int getGps(int id, int queryMode, int cycle, int callId);              //queryMode  12: 常规查询   13： 常规周期查询  14：csbk查询   15： csbk 周期查询
	int cancelPollGps(int id, int callId);
	static DWORD WINAPI TCPConnectionThread(LPVOID lpParam);
	static DWORD WINAPI TimeOutThread(LPVOID lpParam);
	//void  setCallBackFunc(void(*callBackFunc)(int, int, char *, int));
	int RadioConnect();
	void TcpConnect();
	int getOverturnGps(const char* ip,int callId );
	int getLic(const char* licPath);
	int tcpConnect(const char *ip);
	int radioUdpConnect(const char *ip);
	int mnisUdpConnect(const char* ip);
private:
	void AddAllCommand(int callId, int command);
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
};

