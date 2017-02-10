#pragma once
class CMonitor
{
public:
	CMonitor();
	~CMonitor();
	void startMonitor(LPCTSTR lpName, LPCTSTR logServer);
	void stopMonitor();
private:
	static DWORD WINAPI monitorThread(LPVOID lpParam);
	void monitorThreadFunc();
	bool isMonitor;
	HANDLE m_handle;
	TCHAR serverName[300];
};

