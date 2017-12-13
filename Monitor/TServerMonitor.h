#pragma once
class CTServerMonitor
{
public:
	CTServerMonitor();
	~CTServerMonitor();
	void StartMonitor();
	void stopMonitor();
	void stopServer(LPCTSTR lpName);
	static DWORD WINAPI logServerThread(LPVOID lpParam);
private:
	void logServerThreadFunc();
	static DWORD WINAPI monitorThread(LPVOID lpParam);
	void monitorThreadFunc();
	bool isMonitor;
	HANDLE m_handle;
	HANDLE m_logHandle;
	TCHAR serverName[300];
	TCHAR logServerName[300];
	bool isStart;
};

