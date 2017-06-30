#pragma once
class CMonitorServer
{
public:
	CMonitorServer();
	~CMonitorServer();
public :
	void startMonitor(LPCTSTR lpName);
	void stopMonitor();
private:
	static DWORD WINAPI monitorThread(LPVOID lpParam);
	static DWORD WINAPI logServerThread(LPVOID lpParam);
	void monitorThreadFunc();
	void logServerThreadFunc();
	void stopServer(LPCTSTR lpName);
	bool isMonitor;
	HANDLE m_handle;
	HANDLE m_logServerHandle;
	TCHAR serverName[300];
	TCHAR logServerName[300];
	bool isStart;
	std::mutex m_workLocker;
};

