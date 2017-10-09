#pragma once
#include "../radio/text/TextMsg.h"
#include "../radio/ars/RadioARS.h"
#include "../radio/gps/RadioGps.h"

class CDataScheduling
{
public:
	CDataScheduling();
	~CDataScheduling();
	bool radioConnect( const char* ip);
	bool radioGetGps( DWORD dwRadioID, int queryMode, double cycle);
	bool radioStopGps( DWORD dwRadioID, int	queryMode );
	bool radioSendMsg( std::string message, DWORD dwRadioID,  int type);
	void getRadioStatus( int type);
	void radioDisConnect();
	void InitGPSOverturnSocket(DWORD dwAddress);
	void  setCallBackFunc(void(*callBackFunc)(int, Respone));
	static DWORD WINAPI timeOutThread(LPVOID lpParam);
	static DWORD WINAPI workThread(LPVOID lpParam);
	void locationIndoorConfig(int Interval, int iBeaconNumber, bool isEmergency);
	/************************************************************************/
	/* 更新在线设备列表
	/************************************************************************/
	void updateOnLineRadioInfo(int radioId, int status, int gpsQueryMode = -1);
private:
	CTextMsg        *pRadioMsg;
	CRadioARS       *pRadioARS;
	CRadioGps       *pRadioGPS;
	bool            isUdpConnect;
	std::string       lastIP;
	std::list <Command> workList;
	void timeOut();
	void workThreadFunc();
	void addUdpCommand( int command, std::string radioIP, std::string gpsIP, int id, std::string text, double cycle, int querymode);
	void connect(const char* ip);
	void disConnect();
	void getGps(DWORD dwRadioID, int queryMode, double cycle);
	void stopGps(DWORD dwRadioID, int	queryMode);
	void sendMsg(int callId, std::string message, DWORD dwRadioID, int CaiNet);
	void initGPSOverturnSocket(DWORD dwAddress);
	void sendAck(int call, int callId, int id);
	void sendRadioStatusToClient();
	void sendConnectStatusToClient();
	std::function<void( int, Respone)> m_serverFunHandler;
	void sendToClient(int callFuncId, Respone response);
	bool m_workThread;
	bool m_timeoutThread;
	HANDLE m_hWthd;
	HANDLE m_hTthd;
};

