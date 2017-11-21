#pragma once
#include "tcpCommon.h"
#include "XNLConnection.h"

#include <Dbt.h>
#include <initguid.h>
#include <Ndisguid.h>
#define CLS_NAME _T("DUMMY_CLASS")
#define HWND_MESSAGE     ((HWND)-3)

class CTcpScheduling
{
public:
	CTcpScheduling();
	~CTcpScheduling();
private:
	static DWORD WINAPI tcpConnectionThread(LPVOID lpParam);
	static DWORD WINAPI timeOutThread(LPVOID lpParam);
	static DWORD WINAPI workThread(LPVOID lpParam);
	static DWORD WINAPI radioUsbStatusThread(LPVOID lpParam);
	void workThreadFunc();
	void timeOut();
	int  tcpConnect();
	void radioUsbStatus();
	void connect();
	int  privateCall(std::string sessionId, int id);
	int  groupCall(std::string sessionId, int id);
	int  allCall(std::string sessionId);
	int  stopCall(std::string sessionId);
	int  remotePowerOn(std::string sessionId, int id);
	int  remotePowerOff(std::string sessionId, int id);
	int  radioCheck(std::string sessionId, int id);
	int  wiretap(std::string sessionId, int id);

	DWORD            dwip;
	int port;
	std::mutex   m_addCommandLocker;
	std::list <TcpCommand>  workList;
	CXNLConnection  *pXnlConnection;
	bool  m_workThread;
	bool  m_timeThread;
	bool  m_usbThread;
	bool  m_connectThread;
	HANDLE m_wMt;
	HANDLE m_tMt;
	HANDLE m_uMt;
	HANDLE m_cMt;
	bool isTcpConnect;
	radio_t m_radioCfg;
public:
	void addTcpCommand(int command, std::string radioIP, int id, int callType,std::string sessionId);
	int  radioConnect( radio_t radioCfg);
	void call(int type,int op, int id,std::string sessionId );
	void control(int type, int id, std::string sessionId);
	void  setCallBackFunc(void(*callBackFunc)( int, TcpRespone));
	void disConnect();
	std::mutex m_workLocker;
	void setUsb(bool result);
	void getSessionStatus(std::string sessionId);
	void sendSessionStatus();
	
};

