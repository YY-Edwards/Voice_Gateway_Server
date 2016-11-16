#include "../lib/radio/text/TextMsg.h"
#include "../lib/radio/ars/RadioARS.h"
#include "../lib/radio/gps/RadioGps.h"
#include <Dbt.h>
#include <initguid.h>
#include <Ndisguid.h>


#define CLS_NAME _T("DUMMY_CLASS")
#define HWND_MESSAGE     ((HWND)-3)
#pragma once


class CDataScheduling
{
public:
	CDataScheduling();
	~CDataScheduling();
	bool radioConnect(TcpClient* tp  ,const char* ip, int callId);
	bool radioGetGps(TcpClient* tp,DWORD dwRadioID, int queryMode, double cycle, int callId);
	bool radioStopGps(TcpClient* tp,DWORD dwRadioID, int	queryMode, int callId);
	bool radioSendMsg(TcpClient* tp, LPTSTR message, DWORD dwRadioID, int callId, int type);
	//bool InitGPSOverturnSocket(DWORD dwAddress);
	void  setCallBackFunc(void(*callBackFunc)(TcpClient*, int, int, Respone));
	static DWORD WINAPI TimeOutThread(LPVOID lpParam);
	static DWORD WINAPI WorkThread(LPVOID lpParam);
private:
	CTextMsg        pRadioMsg;
	CRadioARS       pRadioARS;
	CRadioGps       pRadioGPS;
	
	std::list <Command> workList;
	void timeOut();
	void WorkThreadFunc();
	void AddAllCommand(TcpClient*  tp, int command, std::string radioIP, std::string gpsIP, int id, wchar_t* text, double cycle, int querymode, int callId);
	void connect(const char* ip, int callId);
	void getGps(DWORD dwRadioID, int queryMode, double cycle);
	void stopGps(DWORD dwRadioID, int	queryMode);
	void sendMsg(int callId, LPTSTR message, DWORD dwRadioID, int CaiNet);
	void InitGPSOverturnSocket(DWORD dwAddress);
	void sendAck(int call,int callId, int id);
	void sendRadioStatusToClient(CRemotePeer* pRemote);
};

