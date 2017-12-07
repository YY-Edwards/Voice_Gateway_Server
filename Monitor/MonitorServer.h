#pragma once
class CMonitorServer
{
public:
	CMonitorServer();
	~CMonitorServer();
public :
	void startMonitor(LPCTSTR lpName);
	void stopMonitor();
	void stopServer(LPCTSTR lpName);
private:
	static DWORD WINAPI monitorThread(LPVOID lpParam);
	
	void monitorThreadFunc();
	
	
	bool isMonitor;
	HANDLE m_handle;
	HANDLE m_logServerHandle;
	TCHAR serverName[300];
	//TCHAR logServerName[300];
	bool isStart;
	std::mutex m_workLocker;
};

