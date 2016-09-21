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
	~DispatchOperate();
	int Connect(const char* ip, const char  * pIP, int sn);
	int call(int id, int sn);
	int groupCall(int id, int sn);
	int allCall( int sn);
	int stopCall( int sn);
	int remotePowerOn(int id, int sn);
	int remotePowerOff(int id, int sn);
	int radioCheck(int id, int sn);
	int wiretap(int id, int sn);
	int sendSms(int id, wchar_t* message, int sn);
	int sendGroupSms(int id, wchar_t* message, int sn);
	int getGps(int id, int queryMode, int cycle, int sn);              //queryMode  12: 常规查询   13： 常规周期查询  14：csbk查询   15： csbk 周期查询
	int cancelPollGps(int id, int sn);
	static DWORD WINAPI TCPConnectionThread(LPVOID lpParam);
	static DWORD WINAPI TimeOutThread(LPVOID lpParam);
	//void  setCallBackFunc(void(*callBackFunc)(int, int, char *, int));
	void RadioConnect();
	void TcpConnect();
	int getOverturnGps(const char* ip);
	int getLic(const char* licPath);
	int tcpConnect(const char *ip);
	int radioUdpConnect(const char *ip);
	int mnisUdpConnect(const char* ip);
private:
	void AddAllCommand(int sn, int command);
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

