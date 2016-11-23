#include "stdafx.h"
#include "WLRecordFile.h"
#include "Sound.h"
#include "SerialDongle.h"
#include "WLNet.h"

extern CSerialDongle *g_pDongle;
extern CSound *g_pSound;
extern CWLNet *g_pNet;

CRecordFile::CRecordFile()
:lenght(0)
, srcRssi(0)
, m_bReport(false)
, m_bPlay(false)
, m_bLogicProcess(false)
{

}

CRecordFile::~CRecordFile()
{

}

void CRecordFile::WriteVoiceFrame(char* pFrame, int len /*= 7*/, bool bLocalRequest /*= false*/)
{
	//如果是本地发起的呼叫，则不需要播放
	if (bLocalRequest)
	{
		if (callType == GROUP_CALL && tagetId == CONFIG_CURRENT_TAGET && tagetId != CONFIG_DEFAULT_GROUP)
		{
			g_pNet->updateChangeToCurrentTick();
		}
		goto RECORD_VOICE_DATA;
	}
	if (getBoolPlay() && 
		Env_DongleIsOk &&
		Env_SoundIsOk)
	{
		tAMBEFrame* pAMBEFrame = NULL;
		pAMBEFrame = g_pDongle->GetFreeAMBEBuffer();
		memcpy(pAMBEFrame->fld.ChannelBits, pFrame, len);
		g_pDongle->deObfuscate(IPSCTODONGLE, pAMBEFrame);
		g_pDongle->MarkAMBEBufferFilled();
		//继续解码数据
		g_pDongle->releaseWaitNextNetDataEvent();
		g_pDongle->DecodeBuffers();
		g_pNet->setCurrentPlayInfo(this);
	}
RECORD_VOICE_DATA:
	prevTimestamp = GetTickCount();
	/*交互录音线程，将AMBE数据写入到本地*/
	memcpy(buffer + lenght, pFrame, len);
	lenght += len;
}

void CRecordFile::setBoolPlay(bool value)
{
	bool oldValue = m_bPlay;
	m_bPlay = value;
	if (!oldValue && value)
	{
		int targetId = 0;
		if (callType == GROUP_CALL)
		{
			targetId = tagetId;
		}
		else if (callType == ALL_CALL)
		{
			targetId = ALL_CALL_TAGET;
		}
		else
		{
			targetId = PRIVATE_CALL_TAGET;
		}
		g_pNet->wlPlayStatus(CMD_SUCCESS, targetId);
	}
}

void CRecordFile::setBoolReport(bool value)
{
	m_bReport = value;
	g_pNet->updateOnLineRadioInfo(srcId, RADIO_STATUS_ONLINE);
}

bool CRecordFile::getBoolPlay()
{
	return m_bPlay;
}

bool CRecordFile::getBoolReport()
{
	return m_bReport;
}

void CRecordFile::setBoolLogicProcess(bool value)
{
	m_bLogicProcess = value;
}

bool CRecordFile::getBoolLogicProcess()
{
	return m_bLogicProcess;
}


