#pragma once
class CTServerMonitor
{
public:
	CTServerMonitor();
	~CTServerMonitor();
	void StartMonitor();
	void stopMonitor();
	void stopServer(LPCTSTR lpName);
private:
	
	static DWORD WINAPI monitorThread(LPVOID lpParam);
	void monitorThreadFunc();
	bool isMonitor;
	HANDLE m_handle;
	TCHAR serverName[300];
	bool isStart;
};

