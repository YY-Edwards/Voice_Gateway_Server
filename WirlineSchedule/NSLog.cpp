#include "stdafx.h"
#include "NSLog.h"
#include <process.h>
#include <time.h>

//#define LOG_INFO(format, ...)                                                                           \
//{                                                                                                       \
//	time_t t = time(0);                                                                                 \
//struct tm ttt = *localtime(&t);                                                                     \
//	fprintf(stdout, "[INFO] [%5d %4d-%02d-%02d %02d:%02d:%02d %03d] " format "\n", \
//	GetCurrentProcessId(), ttt.tm_year + 1900, ttt.tm_mon + 1, ttt.tm_mday, ttt.tm_hour, \
//	ttt.tm_min, ttt.tm_sec, GetTickCount() % 1000,##__VA_ARGS__);                            \
//}
//
//#define LOG_ERR(format, ...)                                                                            \
//{                                                                                                       \
//	time_t t = time(0);                                                                                 \
//struct tm ttt = *localtime(&t);                                                                     \
//	fprintf(stderr, "[ERRO] [%5d %4d-%02d-%02d %02d:%02d:%02d %03d] " format "\n", \
//	GetCurrentProcessId(), ttt.tm_year + 1900, ttt.tm_mon + 1, ttt.tm_mday, ttt.tm_hour, \
//	ttt.tm_min, ttt.tm_sec, GetTickCount() % 1000,##__VA_ARGS__);                            \
//}

NSLog::NSLog()
:m_bWork(false)
, m_pLogThread(NULL)
, m_mutexLog(INITLOCKER())
, m_logs(NULL)
, m_waitLogEvent(CreateEvent(NULL,FALSE,FALSE,NULL))
{

}

NSLog::~NSLog()
{
	Stop();
	clearLogs();
	DELETELOCKER(m_mutexLog);
}

NSLog* NSLog::_instance = new NSLog();
NSLog* NSLog::instance()
{
	_instance->Initialize();
	return _instance;
}

void NSLog::Initialize()
{
	if (!m_bWork)
	{
		unsigned int m_pLogThreadId = 0;
		/*创建log处理线程*/
		m_pLogThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			LogthreadProc,
			this,
			CREATE_SUSPENDED,
			&m_pLogThreadId
			);
		if (NULL == m_pLogThread)
		{
			return;
		}
		BOOL rlt =SetThreadPriority(m_pLogThread, THREAD_PRIORITY_BELOW_NORMAL);
		m_bWork = true;
		ResumeThread(m_pLogThread);
	}
}

unsigned int __stdcall NSLog::LogthreadProc(void* pArguments)
{
	NSLog* p = (NSLog*)pArguments;
	if (p)
	{
		p->Logthread();
	}
	return 0;
}

void NSLog::Logthread()
{
	pLinkItem it = NULL;
	while (m_bWork)
	{
		TRYLOCK(m_mutexLog);
		it = popFront(&m_logs);
		while (it)
		{
			log_t* p = (log_t*)it->data;
			if (p)
			{
				handleMsg(p->message, true, p->type);
				delete p;
				p = NULL;
			}
			freeList(it);
			it = popFront(&m_logs);
		}
		RELEASELOCK(m_mutexLog);
		WaitForSingleObject(m_waitLogEvent, INFINITE);
	}
}

void NSLog::handleMsg(char* pMsg, bool bPrint, log_type_enum type)
{
	switch (type)
	{
	case Ns_Log_Error:
	{
						 if (bPrint)
						 {
							 printf("%s", pMsg);
						 }
						 LOG(INFO) << pMsg;
	}
		break;
	case Ns_Log_Info:
	{
#if _DEBUG
						if (bPrint)
						{
							printf("%s", pMsg);
						}
						LOG(INFO) << pMsg;
#endif // _DEBUG
	}
		break;
	default:
		break;
	}

}

void NSLog::Stop()
{
	m_bWork = false;
	if (NULL != m_pLogThread)
	{
		SetEvent(m_waitLogEvent);
		WaitForSingleObject(m_pLogThread, 1000);
		CloseHandle(m_pLogThread);
		m_pLogThread = NULL;
	}
}

void NSLog::AddLog(log_type_enum type,const char* format, ...)
{
	if (NULL == format)
	{
		return;
	}

	char buf[4096] = { 0 };
	va_list args;
	va_start(args, format);
	_vsnprintf(buf, sizeof(buf)-1, format, args);
	va_end(args);

	int length = strlen(buf);
	if (length == 0)
	{
		return;
	}
	log_t* pLog = new log_t;
	pLog->type = type;
	time_t t = time(0);
	struct tm ttt = *localtime(&t);
	if (Ns_Log_Error == type)
	{
		sprintf(pLog->message, "[ERRO] [%5d %4d-%02d-%02d %02d:%02d:%02d %03d] %s\n", GetCurrentProcessId(), ttt.tm_year + 1900, ttt.tm_mon + 1, ttt.tm_mday, ttt.tm_hour, ttt.tm_min, ttt.tm_sec, GetTickCount() % 1000, buf);
	}
	else
	{
		sprintf(pLog->message, "[INFO] [%5d %4d-%02d-%02d %02d:%02d:%02d %03d] %s\n", GetCurrentProcessId(), ttt.tm_year + 1900, ttt.tm_mon + 1, ttt.tm_mday, ttt.tm_hour, ttt.tm_min, ttt.tm_sec, GetTickCount() % 1000, buf);
	}
	AddLogsItem(pLog);
}

void NSLog::clearLogs()
{
	TRYLOCK(m_mutexLog);
	pLinkItem it = popFront(&m_logs);
	while (it)
	{
		log_t* p = (log_t*)it->data;
		if (p)
		{
			handleMsg(p->message, false, p->type);
			delete p;
			p = NULL;
		}
		freeList(it);
		it = popFront(&m_logs);
	}
	RELEASELOCK(m_mutexLog);
}

void NSLog::AddLogsItem(log_t* log)
{
	TRYLOCK(m_mutexLog);
	appendData(&m_logs, log);
	RELEASELOCK(m_mutexLog);
	SetEvent(m_waitLogEvent);
}
