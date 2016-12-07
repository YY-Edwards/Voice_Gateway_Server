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
	void monitorThreadFunc();
	bool isMonitor;
	HANDLE m_handle;
	TCHAR serverName[300];
};

