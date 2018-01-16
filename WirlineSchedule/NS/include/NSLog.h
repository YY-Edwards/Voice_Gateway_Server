#ifndef NSLOG_H
#define NSLOG_H
#include "mutex.h"
#include "linklist.h"

#define SIZE_MAX_MSG 5118

typedef enum _log_type_enum
{
	Ns_Log_Info,//一般log,仅在Debug模式下处理
	Ns_Log_Error//错误log,任何时候都要处理
}log_type_enum;

typedef struct _log_t
{
	log_type_enum type;
	char message[SIZE_MAX_MSG];
}log_t;

class NSLog
{
public:
	~NSLog();

	static NSLog* instance();
	void Stop();
	void AddLog(log_type_enum type,const char* format, ...);
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
	void handleMsg(char* pMsg, bool bPrint, log_type_enum type);
	int Size();
};


#endif