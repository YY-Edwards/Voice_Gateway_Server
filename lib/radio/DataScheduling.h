#pragma once
#include "../radio/text/TextMsg.h"
#include "../radio/ars/RadioARS.h"
#include "../radio/gps/RadioGps.h"

class CDataScheduling
{
public:
	CDataScheduling();
	~CDataScheduling();
	void radioConnect(mnis_t mnisCfg, location_t locationCfg, locationindoor_t locationIndoorCfg);
	bool radioGetGps( DWORD dwRadioID, int queryMode, double cycle,std::string sessionId,int operate);
	bool radioStopGps( DWORD dwRadioID, int	queryMode ,std::string sessionId);
	bool radioSendMsg( std::string message, DWORD dwRadioID,  int type,std::string sessionId);
	void getRadioStatus( int type,std::string sessionId);
	void radioDisConnect();
	void InitGPSOverturnSocket(DWORD dwAddress,int port);
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
	void addUdpCommand( int command, std::string radioIP, std::string gpsIP, int id, std::string text, double cycle, int querymode,std::string sessionId);
	void connect();
	void disConnect();
	void getGps(DWORD dwRadioID, int type, double cycle,int cai);
	void stopGps(DWORD dwRadioID, int	type,int cai);
	void sendMsg(std::string sessionId , std::string message, DWORD dwRadioID, int CaiNet);
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
	mnis_t m_mnisCfg;
	location_t m_locationCfg;
	locationindoor_t m_locationIndoorCfg;
	
};

