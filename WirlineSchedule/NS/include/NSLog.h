#ifndef NSLOG_H
#define NSLOG_H
#include "mutex.h"
#include "linklist.h"

#define SIZE_MAX_MSG 5118

typedef enum _log_type_enum
{
	Ns_Log_Info,//һ��log,����Debugģʽ�´���
	Ns_Log_Error//����log,�κ�ʱ��Ҫ����
}log_type_enum;

typedef struct _log_t
{
	log_type_enum type;
	char message[SIZE_MAX_MSG];
}log_t;

typedef void(*pHandleLog)(char* msg, void* param);

class NSLog
{
public:
	~NSLog();

	static NSLog* instance();
	void Stop();
	void AddLog(log_type_enum type,const char* format, ...);
	void setHandleLog(pHandleLog handleLogFunc,void* param);
protected:
	NSLog();
private:
	static NSLog* _instance;
	bool m_bWork;
	HANDLE m_pLogThread;
	LOCKERTYPE m_mutexLog;
	pLinkList m_logs;
	HANDLE m_waitLogEvent;
	pHandleLog m_pHandleLog;
	void* m_handleLogParam;

	void Initialize();
	static unsigned int __stdcall LogthreadProc(void* pArguments);
	void Logthread();
	void clearLogs();
	void AddLogsItem(log_t* log);
	void handleMsg(char* pMsg, bool bPrint, log_type_enum type);
	int Size();
};


#endif