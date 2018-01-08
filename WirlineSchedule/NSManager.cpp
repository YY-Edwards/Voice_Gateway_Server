#include "stdafx.h"
#include "NSManager.h"
#include "NSAmbe.h"
#include <process.h>
#include "NSDongle.h"
#include "WDK_VidPidQuery.h"
#include "Tool.h"
#include "NSLog.h"
#include <Dbt.h>
#pragma comment(lib, "winmm.lib")
#include "NSNetBase.h"

NSManager::NSManager()
:m_mutexDongles(INITLOCKER())
, m_dongles(NULL)
, m_mutexIdleDongles(INITLOCKER())
, m_idleDongles(NULL)
, m_pLog(NSLog::instance())
, m_ppNet(NULL)
, m_sizeDongle(0)
{
}

NSManager::~NSManager()
{
	//if (m_theTimeCaps.wPeriodMin != 0)
	//{
	//	timeEndPeriod(m_theTimeCaps.wPeriodMin);//清除最小定时器精度
	//}
	m_ppNet = NULL;
	clearIdleDongles();
	clearDongles();
	DELETELOCKER(m_mutexDongles);
	DELETELOCKER(m_mutexIdleDongles);
}

int NSManager::Initialize(NSNetBase* &pNet)
{
	///*建立最小定时器精度*/
	//memset(&m_theTimeCaps, 0, sizeof(TIMECAPS));
	//if (!FAILED(timeGetDevCaps(&m_theTimeCaps, sizeof(TIMECAPS))))
	//{
	//	timeBeginPeriod(m_theTimeCaps.wPeriodMin);//建立最小定时器精度
	//}
	m_ppNet = &pNet;
	com_use_t result = { 0 };
	WDK_WhoAllVidPid(VID_PID, &result);
	setSizeDongle(result.num);
	for (int i = 0; i < result.num; i++)
	{
		/*加载dongle并初始化*/
		dongle_t dongle = { 0 };
		dongle.hcom = NULL;
		dongle.isusing = false;

		swprintf_s(dongle.createfile, SIZE_COM_NAME, L"\\\\.\\%s", g_pNSTool->ANSIToUnicode(result.coms[i]).c_str());
		strcpy(dongle.strname, result.coms[i]);
		NSDongle* p = new NSDongle(this);
		if (WL_OK != p->Initialize(&dongle))
		{
			delete p;
		}
		else
		{
			AddDonglesItem(p);
			AddIdleDonglesItem(p);
		}
	}
	if (0 == result.num)
	{
		//LOG_ERR("AMBE DONGLE ZERO");
		m_pLog->AddLog("AMBE DONGLE ZERO,will not play and call");
		return WL_FAIL_AMBE_DONGLE_ZERO;
	}
	else
	{
		return WL_OK;
	}
}

void NSManager::test_ambe_2_pcm(FILE* pIn,FILE* pOut)
{
	test_param_t* param = new test_param_t;
	param->pHandler = this;
	param->pIn = pIn;
	param->pOut = pOut;

	/*建立测试线程*/
	unsigned int m_pThreadId = 0;
	HANDLE m_pHandleThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		test_ambe_2_pcmProc,
		param,
		CREATE_SUSPENDED,
		&m_pThreadId
		);
	if (NULL == m_pHandleThread)
	{
		//LOG_ERR("create test_ambe_2_pcmProc fail");
		m_pLog->AddLog("create test_ambe_2_pcmProc fail");
	}
	else
	{
		ResumeThread(m_pHandleThread);
	}
}

void NSManager::test_pcm_2_ambe(FILE* pIn, FILE* pOut)
{
	test_param_t* param = new test_param_t;
	param->pHandler = this;
	param->pIn = pIn;
	param->pOut = pOut;

	/*建立测试线程*/
	unsigned int m_pThreadId = 0;
	HANDLE m_pHandleThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		test_pcm_2_ambeProc,
		param,
		CREATE_SUSPENDED,
		&m_pThreadId
		);
	if (NULL == m_pHandleThread)
	{
		m_pLog->AddLog("create test_pcm_2_ambeProc fail");
	}
	else
	{
		ResumeThread(m_pHandleThread);
	}
}

void NSManager::OnUpdateUsb(DWORD operateType)
{
	//LOG_INFO("OnUpdateUsb");
	m_pLog->AddLog("OnUpdateUsb");
	switch (operateType)
	{
	case USB_ADD:
	{
					//LOG_INFO("USB_ADD");
					m_pLog->AddLog("USB_ADD");
					handleUsbAdd();
	}
		break;
	case USB_DEL:
	{
					//LOG_INFO("USB_DEL");
					m_pLog->AddLog("USB_DEL");
					handleUsbDel();
	}
		break;
	default:
		break;
	}
}

void NSManager::handle_test_ambe_2_pcm(FILE* pIn, FILE* pOut)
{
	//LOG_INFO("handle_test_ambe_2_pcm Start");
	m_pLog->AddLog("handle_test_ambe_2_pcm Start");
	/*指定文件传AMBE*/
	FILE *fp = pIn;
	if (NULL == fp)
	{
		//LOG_ERR("Error on open pIn!", );
		m_pLog->AddLog("Error on open pIn!");
		return;
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	char* pBuffer = new char[size];
	fseek(fp, 0, SEEK_SET);
	size_t readLen = fread(pBuffer, sizeof(char), size, fp);
	fclose(fp);
	fp = NULL;
	fp = pOut;
	//m_ambeSchdule->Ambe2Pcm(pBuffer, readLen, fp);
	NSAmbe* ambe = new NSAmbe(this);
	ambe->Ambe2Pcm(pBuffer, readLen);
	ambe->WirteEnd();
	if (pBuffer)
	{
		delete pBuffer;
		pBuffer = NULL;
	}
	//LOG_INFO("handle_test_ambe_2_pcm End");
	m_pLog->AddLog("handle_test_ambe_2_pcm End");
}

void NSManager::handle_test_pcm_2_ambe(FILE* pIn, FILE* pOut)
{
	//LOG_INFO("handle_test_pcm_2_ambe Start");
	m_pLog->AddLog("handle_test_pcm_2_ambe Start");
	/*指定文件传PCM*/
	FILE *fp = pIn;
	if (NULL == fp)
	{
		//LOG_ERR("Error on open pIn!", );
		m_pLog->AddLog("Error on open pIn!");
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	char* pBuffer = new char[size];
	fseek(fp, 0, SEEK_SET);
	size_t readLen = fread(pBuffer, sizeof(char), size, fp);
	fclose(fp);
	fp = NULL;
	fp = pOut;
	NSAmbe* ambe = new NSAmbe(this);
	ambe->Pcm2Ambe(pBuffer, readLen);
	ambe->WirteEnd();
	if (pBuffer)
	{
		delete pBuffer;
		pBuffer = NULL;
	}
	//LOG_INFO("handle_test_pcm_2_ambe End");
	m_pLog->AddLog("handle_test_pcm_2_ambe End");
}

unsigned int __stdcall NSManager::test_ambe_2_pcmProc(void* pArguments)
{
	test_param_t* p = (test_param_t*)pArguments;
	if (p)
	{
		if (p->pHandler)
		{
			p->pHandler->handle_test_ambe_2_pcm(p->pIn, p->pOut);
		}
	}
	delete p;
	p = NULL;
	return 0;
}

unsigned int __stdcall NSManager::test_pcm_2_ambeProc(void* pArguments)
{
	test_param_t* p = (test_param_t*)pArguments;
	if (p)
	{
		if (p->pHandler)
		{
			p->pHandler->handle_test_pcm_2_ambe(p->pIn, p->pOut);
		}
	}
	delete p;
	p = NULL;
	return 0;
}

void NSManager::clearDongles()
{
	TRYLOCK(m_mutexDongles);
	int count = 0;
	pLinkItem it = popFront(&m_dongles);
	while (it)
	{
		NSDongle* p = (NSDongle*)it->data;
		if (p)
		{
			delete p;
			p = NULL;
		}
		freeList(it);
		it = popFront(&m_dongles);
	}
	m_mutexDongles = NULL;
	RELEASELOCK(m_mutexDongles);
}

NSDongle* NSManager::PopIdleDonglesItem()
{
	NSDongle* p = NULL;
	TRYLOCK(m_mutexIdleDongles);
	pLinkItem item = popFront(&m_idleDongles);
	if (item)
	{
		if (item->data)
		{
			p = (NSDongle*)item->data;
		}
		freeList(item);
	}
	RELEASELOCK(m_mutexIdleDongles);
	return p;
}

void NSManager::AddDonglesItem(NSDongle* p)
{
	TRYLOCK(m_mutexDongles);
	appendData(&m_dongles, p);
	RELEASELOCK(m_mutexDongles);
}

bool NSManager::FuncFindDonglesItem(const void* dongle, const void* condition)
{
	NSDongle* p = (NSDongle*)dongle;
	if (NULL != p && NULL != condition)
	{
		char* pName = (char*)condition;
		if (0 == strcmp(pName, p->Name()))
		{
			return true;
		}
	}
	return false;
}

void NSManager::handleUsbAdd()
{
	com_use_t result = { 0 };
	WDK_WhoAllVidPid(VID_PID, &result);
	setSizeDongle(result.num);
	for (int i = 0; i < result.num; i++)
	{
		NSDongle* rlt = FindDonglesItem(result.coms[i], &FuncFindDonglesItem);
		if (NULL == rlt)
		{
			/*增加此dongle*/
			/*加载dongle并初始化*/
			dongle_t dongle = { 0 };
			dongle.hcom = NULL;
			dongle.isusing = false;

			swprintf_s(dongle.createfile, SIZE_COM_NAME, L"\\\\.\\%s", g_pNSTool->ANSIToUnicode(result.coms[i]).c_str());
			strcpy(dongle.strname, result.coms[i]);
			NSDongle* p = new NSDongle(this);
			if (WL_OK != p->Initialize(&dongle))
			{
				delete p;
			}
			else
			{
				AddDonglesItem(p);
				AddIdleDonglesItem(p);
			}
		}
	}
}

void NSManager::handleUsbDel()
{
	com_use_t result = { 0 };
	WDK_WhoAllVidPid(VID_PID, &result);
	setSizeDongle(result.num);
	TRYLOCK(m_mutexDongles);
	pLinkItem header = m_dongles;
	pLinkItem item = m_dongles;
	NSDongle* it = NULL;
	bool isHave = false;
	while (header)
	{
		it = (NSDongle*)item->data;
		isHave = false;
		for (int i = 0; i < result.num; i++)
		{
			if (0 == strcmp(it->Name(), result.coms[i]))
			{
				isHave = true;
				break;
			}
		}
		if (!isHave)
		{
			/*删除空闲选中的Dongle*/
			pLinkItem idleItem = removeItem(&m_idleDongles, it);
			if (idleItem)
			{
				idleItem->pNext = NULL;
				freeList(idleItem);
			}
			pLinkItem dongleItem = removeItem(&m_dongles, it);
			NSDongle* rItem = (NSDongle*)(dongleItem->data);
			if (NULL != rItem)
			{
				if (!rItem->IsIdle())
				{
					m_pLog->AddLog("%s is remove，work will stop", rItem->Name());
				}
				delete rItem;
			}
			else
			{
				m_pLog->AddLog("%s is remove，but don't found", it->Name());
			}
			header = item->pNext;
			item->pNext = NULL;
			freeList(item);
			item = header;
		}
		else
		{
			header = item->pNext;
			item = header;
		}
	}
	RELEASELOCK(m_mutexDongles);
}

void NSManager::clearIdleDongles()
{
	TRYLOCK(m_mutexIdleDongles);
	freeList(m_idleDongles);
	RELEASELOCK(m_mutexIdleDongles);
}

NSDongle* NSManager::FindDonglesItem(const void* condition, LinkMatchFunc fun)
{
	NSDongle* p = NULL;
	TRYLOCK(m_mutexDongles);
	pLinkItem item = findItem(m_dongles, condition, fun);
	if (item)
	{
		if (item->data)
		{
			p = (NSDongle*)item->data;
		}
	}
	RELEASELOCK(m_mutexDongles);
	return p;
}

void NSManager::AddIdleDonglesItem(NSDongle* p)
{
	TRYLOCK(m_mutexIdleDongles);
	appendData(&m_idleDongles, p);
	RELEASELOCK(m_mutexIdleDongles);
}

void NSManager::HandleAmbeData(void* pData, unsigned long length)
{
	if (m_ppNet)
	{
		if (*m_ppNet)
		{
			(*m_ppNet)->HandleAmbeData(pData, length);
		}
		else
		{
			m_pLog->AddLog("m_pNet is null");
		}
	}
	else
	{
		m_pLog->AddLog("m_ppNet is null");
	}
}

int NSManager::SizeDongle()
{
	return m_sizeDongle;
}

void NSManager::setSizeDongle(int value)
{
	m_sizeDongle = value;
}

int NSManager::SizeIdleDongle()
{
	int rlt = 0;
	TRYLOCK(m_mutexIdleDongles);
	rlt = listSize(m_idleDongles);
	RELEASELOCK(m_mutexIdleDongles);
	return rlt;
}
