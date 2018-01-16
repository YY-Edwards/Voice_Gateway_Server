#include "stdafx.h"
#include "../include/NSAmbe.h"
#include "../include/NSDongle.h"
#include "../include/NSManager.h"
#include "../include/NSLog.h"
#include "../include/NSSound.h"

NSAmbe::NSAmbe(NSManager* pManager)
:m_pManager(pManager)
, m_bWriteEnd(false)
, m_bReciveEnd(false)
, m_useDongle(NULL)
, m_pLog(NSLog::instance())
{
#if _DEBUG
	//m_outFile = fopen("D:\\out.bit", "wb");
	m_outFile = NULL;
#endif
}

NSAmbe::~NSAmbe()
{
	m_pLog->AddLog(Ns_Log_Info,"~NSAmbe");
#if _DEBUG	
	if (m_outFile)
	{
		fflush(m_outFile);
		fclose(m_outFile);
		m_outFile = NULL;
	}
#endif
	m_pManager = NULL;
	m_bWriteEnd = false;
	m_bReciveEnd = false;
	m_useDongle = NULL;
}

void NSAmbe::Ambe2Pcm(void* pBuffer, unsigned long length)
{
	if (NULL == m_useDongle) m_useDongle = m_pManager->PopIdleDonglesItem();
	if (m_useDongle)
	{
		m_useDongle->WriteAmbe(pBuffer, length, &OnDataPcmFun, this);
	}
	else
	{
		m_pLog->AddLog(Ns_Log_Error,"no useable dongle");
	}
}

void NSAmbe::OnDataPcm(void* pData, unsigned long length, unsigned long index)
{
	//LOG_INFO("Recive index:%d pcm package", index);
	if (pData)
	{
		if (g_pNSSound)
		{
			g_pNSSound->WritePcm((const char*)pData, length);
		}
#if _DEBUG
		if (m_outFile)
		{
			fwrite(pData, sizeof(char), length, m_outFile);
			fflush(m_outFile);
		}
#endif

	}
	else
	{
		m_pLog->AddLog(Ns_Log_Info, "ambe to pcm end");
		m_bReciveEnd = true;
		m_useDongle = NULL;
		canDeleteSelf();
	}
}

int NSAmbe::Pcm2Ambe(void* pBuffer, unsigned long length)
{
	int rlt = 0;
	if (NULL == m_useDongle) m_useDongle = m_pManager->PopIdleDonglesItem();
	if (m_useDongle)
	{
		m_useDongle->WritePcm(pBuffer, length, &OnDataAmbeFun, this);
	}
	else
	{
		m_pLog->AddLog(Ns_Log_Error, "no useable dongle");
		rlt = 1;
	}
	return rlt;
}

void NSAmbe::OnDataAmbe(void* pData, unsigned long length, unsigned long index)
{
	//LOG_INFO("Recive index:%d ambe package", index);
	if (pData)
	{
		if (m_pManager)
		{
			m_pManager->HandleAmbeData(pData, length);
		}
		else
		{
			m_pLog->AddLog(Ns_Log_Error, "m_pManager is null");
		}
#if _DEBUG
		if (m_outFile)
		{
			fwrite(pData, sizeof(char), length, m_outFile);
			fflush(m_outFile);
		}
#endif

	}
	else
	{
		m_pLog->AddLog(Ns_Log_Info, "pcm to ambe end");
		m_bReciveEnd = true;
		m_useDongle = NULL;
		canDeleteSelf();
	}
}

void NSAmbe::OnDataPcmFun(void* pData, unsigned long length, unsigned long index, void* param)
{
	NSAmbe* p = (NSAmbe*)param;
	if (p)
	{
		p->OnDataPcm(pData, length, index);
	}
}

void NSAmbe::OnDataAmbeFun(void* pData, unsigned long length, unsigned long index, void* param)
{
	NSAmbe* p = (NSAmbe*)param;
	if (p)
	{
		p->OnDataAmbe(pData, length, index);
	}
}

void NSAmbe::WirteEnd()
{
	m_bWriteEnd = true;
	canDeleteSelf();
}

void NSAmbe::canDeleteSelf()
{
	if (m_bWriteEnd && m_bReciveEnd)
	{
		delete this;
	}
}
void NSAmbe::AboutInfo(char* info)
{
	if (info)
	{
		if (m_useDongle)
		{
			m_useDongle->StatusInfo(info);
		}
		else
		{
			sprintf(info, "m_useDongle is null");
		}
	}
}

