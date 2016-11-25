#ifndef WLSOCKETLOG_H
#define WLSOCKETLOG_H
class WLSocketLog
{
public:
	WLSocketLog();
	~WLSocketLog();
	void sendLog(std::string log);
private:
	SOCKET m_serverSocket;
	HANDLE m_hWorkThreadId;
	static DWORD WINAPI workThread(LPVOID lpParam);
	void workThreadProc();
	std::list<std::string> m_logs;
	bool m_bSendLog;
	HANDLE m_waitSendLog;
	std::mutex m_logLstLocker;
};
#endif