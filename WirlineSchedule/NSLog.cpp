#include "stdafx.h"
#include "NSLog.h"
#include <process.h>
#include <time.h>

typedef struct _log_t
{
	char* pData;
}log_t;

#define LOG_INFO(format, ...)                                                                           \
{                                                                                                       \
	time_t t = time(0);                                                                                 \
struct tm ttt = *localtime(&t);                                                                     \
	fprintf(stdout, "[INFO] [%5d %4d-%02d-%02d %02d:%02d:%02d %03d] " format "\n", \
	GetCurrentProcessId(), ttt.tm_year + 1900, ttt.tm_mon + 1, ttt.tm_mday, ttt.tm_hour, \
	ttt.tm_min, ttt.tm_sec, GetTickCount() % 1000,##__VA_ARGS__);                            \
}

#define LOG_ERR(format, ...)                                                                            \
{                                                                                                       \
	time_t t = time(0);                                                                                 \
struct tm ttt = *localtime(&t);                                                                     \
	fprintf(stderr, "[ERRO] [%5d %4d-%02d-%02d %02d:%02d:%02d %03d] " format "\n", \
	GetCurrentProcessId(), ttt.tm_year + 1900, ttt.tm_mon + 1, ttt.tm_mday, ttt.tm_hour, \
	ttt.tm_min, ttt.tm_sec, GetTickCount() % 1000,##__VA_ARGS__);                            \
}

NSLog::NSLog()
:m_bWork(false)
, m_pLogThread(NULL)
, m_mutexLog(INITLOCKER())
, m_logs(NULL)
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
	while (m_bWork)
	{
		TRYLOCK(m_mutexLog);
		pLinkItem it = popFront(&m_logs);
		if (it)
		{
			log_t* p = (log_t*)it->data;
			if (p)
			{
				if (p->pData)
				{
#ifdef _DEBUG
					LOG_INFO("%s", p->pData);
					LOG(INFO) << p->pData;
#endif // _DEBUG
					delete p->pData;
					p->pData = NULL;
				}
				delete p;
				p = NULL;
			}
			freeList(it);
		}
		RELEASELOCK(m_mutexLog);
		Sleep(INTERVAL_LOG);
	}
}

void NSLog::Stop()
{
	m_bWork = false;
	if (NULL != m_pLogThread)
	{
		WaitForSingleObject(m_pLogThread, 1000);
		CloseHandle(m_pLogThread);
		m_pLogThread = NULL;
	}
}

void NSLog::AddLog(const char* format, ...)
{
	if (NULL == format)
	{
		return;
	}

	char buf[4096];
	va_list args;
	va_start(args, format);
	_vsnprintf(buf, sizeof(buf)-1, format, args);
	va_end(args);

	int length = strlen(buf);
	if (length == 0)
	{
		return;
	}

#ifdef _DEBUG
	LOG_INFO("%s", buf);
#else
	log_t* pInfo = new log_t;
	pInfo->pData = new char[length+1];
	memset(pInfo->pData, 0, length + 1);
	strcpy(pInfo->pData, buf);
	TRYLOCK(m_mutexLog);
	if (NULL == m_logs)
	{
		m_logs = createLinkList();
		m_logs->data = pInfo;
	}
	else if (NULL == m_logs->data)
	{
		m_logs->data = pInfo;
	}
	else
	{
		appendData(&m_logs,pInfo);
	}
	RELEASELOCK(m_mutexLog);
#endif // _DEBUG
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
			if (p->pData)
			{
				delete p->pData;
				p->pData = NULL;
			}
			delete p;
			p = NULL;
		}
		freeList(it);
		it = popFront(&m_logs);
	}
	RELEASELOCK(m_mutexLog);
}
