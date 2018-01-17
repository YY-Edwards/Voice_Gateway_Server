#include "stdafx.h"
#include "../include/NS.h"
#include "../include/NSWLNet.h"
#include "../include/NSSound.h"
#include "../include/NSManager.h"

NS::NS(const void* param, OnCall oncall, OnCallStatus onCallStatus, OnNSSystemStatusChange onSystemChange)
{
	g_pNSTool = new CTool();
	g_pNSManager = new NSManager();
	g_pNSSound = new NSSound();
	g_pNSNet = new NSWLNet(g_pNSManager);

	NS_RegCallEvent((void*)param, oncall);
	NS_RegCallStatusEvent((void*)param, onCallStatus);
	NS_RegSystemStatusChangeEvent((void*)param, onSystemChange);
}

NS::~NS()
{
	/*注销事件*/
	NS_UnregCallEvent();
	NS_UnregCallStatusEvent();
	NS_UnregSystemStatusChangeEvent();

	if (g_pNSNet)
	{
		if (WL == g_repeater_net_mode)
		{
			delete (NSWLNet*)g_pNSNet;
			g_pNSNet = NULL;
		}
	}
	if (g_pNSSound)
	{
		delete g_pNSSound;
		g_pNSSound = NULL;
	}
	if (g_pNSManager)
	{
		delete g_pNSManager;
		g_pNSManager = NULL;
	}
	if (g_pNSTool)
	{
		delete g_pNSTool;
		g_pNSTool = NULL;
	}
}

int NS::StartNet(StartNetParam* p)
{
	return g_pNSNet->StartNet(p);
}

int NS::InitializeDongleModel()
{
	return g_pNSManager->Initialize();
}

int NS::SizeDongle()
{
	return g_pNSManager->SizeDongle();
}

DWORD NS::InitSoundIn()
{
	return g_pNSSound->InitSoundIn();
}

DWORD NS::InitSoundOut()
{
	return g_pNSSound->InitSoundOut();
}

call_thread_status_enum NS::CallThreadStatus()
{
	if (WL == g_repeater_net_mode)
	{
		return ((NSWLNet*)g_pNSNet)->CallThreadStatus();
	}
	else
	{
		/*除了wirelan外尚未得到p2p模式下的通话授权*/
		return Call_Thread_Author_No;
	}
}

void NS::CallStart(make_call_param_t *p)
{
	if (WL == g_repeater_net_mode)
	{
		((NSWLNet*)g_pNSNet)->CallStart(p);
	}
}

void NS::CallStop()
{
	if (WL == g_repeater_net_mode)
	{
		((NSWLNet*)g_pNSNet)->CallStop();
	}
}

le_status_enum NS::LeStatus()
{
	if (WL == g_repeater_net_mode)
	{
		return ((NSWLNet*)g_pNSNet)->LeStatus();
	}
	else
	{
		/*暂时不考虑p2p*/
		return STARTING;
	}
}

void NS::setDb(CMySQL* value)
{
	g_pDb = value;
}

void NS::setAmbeDataPath(const wchar_t* value)
{
	wcscpy(g_ambedata_path, value);
}

mic_status_enum NS::MicStatus()
{
	return g_pNSSound->MicStatus();
}

void NS::OnUpdateUsb(DWORD operateType)
{
	g_pNSManager->OnUpdateUsb(operateType);
}

void NS::setPlayCallType(unsigned char value)
{
	g_playCalltype = value;
}

void NS::setPlayTargetId(unsigned long value)
{
	g_playTargetId = value;
}

license_status_enum NS::LicenseStatus()
{
	return g_license_status;
}

void NS::setLicenseStatus(license_status_enum value)
{
	g_license_status = value;
}
