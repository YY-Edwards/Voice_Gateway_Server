#pragma once
class CStartMysql
{
public:
	CStartMysql();
	~CStartMysql();
	void startMysql();
private:
	static DWORD WINAPI monitorMysqlStatusThread(LPVOID lpParam);
	void monitorMysqlStatusThreadFunc();
	bool isStartMonitor;
};

