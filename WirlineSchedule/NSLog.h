#ifndef NSLOG_H
#define NSLOG_H
#include "NS\mutex.h"
#include "linklist.h"

class NSLog
{
public:
	~NSLog();

	static NSLog* instance();
	void Stop();
	void AddLog(const char* format, ...);
protected:
	NSLog();
private:
	static NSLog* _instance;
	bool m_bWork;
	HANDLE m_pLogThread;
	LOCKERTYPE m_mutexLog;
	pLinkItem m_logs;

	void Initialize();
	static unsigned int __stdcall LogthreadProc(void* pArguments);
	void Logthread();
	void clearLogs();
};


#endif