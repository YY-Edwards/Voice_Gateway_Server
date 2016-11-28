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
	int  privateCall( int id);
	int  groupCall( int id);
	int  allCall( );
	int  stopCall();
	int  remotePowerOn( int id);
	int  remotePowerOff( int id);
	int  radioCheck( int id);
	int  wiretap( int id);

	DWORD            dwip;
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
	
public:
	void addTcpCommand(int command, std::string radioIP, int id, int callType);
	int  radioConnect( const char* ip);
	void call(int type,int op, int id );
	void control( int type,  int id);
	void  setCallBackFunc(void(*callBackFunc)( int, TcpRespone));
	void disConnect();
	std::mutex m_workLocker;
	
};

