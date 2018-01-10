#ifndef NSLOG_H
#define NSLOG_H
#include "NS\mutex.h"
#include "linklist.h"

#define SIZE_MAX_MSG 5118

typedef struct _log_t
{
	char message[SIZE_MAX_MSG];
}log_t;

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
	pLinkList m_logs;
	HANDLE m_waitLogEvent;

	void Initialize();
	static unsigned int __stdcall LogthreadProc(void* pArguments);
	void Logthread();
	void clearLogs();
	void AddLogsItem(log_t* log);
	void handleMsg(char* pMsg,bool bPrint);
};


#endif