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
, bReport(false)
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
		goto RECORD_VOICE_DATA;
	}
	if ((callType == GROUPCALL_TYPE && tagetId == CONFIG_DEFAULT_GROUP)
		|| (callType == PRIVATE_CALL && tagetId == CONFIG_LOCAL_RADIO_ID)
		|| (callType == ALL_CALL && tagetId == ALL_CALL_ID))
	{
		//当前组组呼
		if (callType == GROUPCALL_TYPE && tagetId == g_targetId && g_targetId != CONFIG_DEFAULT_GROUP)
		{
			if (!g_bIsHaveAllCall && !g_bIsHaveDefaultGroupCall && !g_bIsHavePrivateCall)
			{
				g_bIsHaveCurrentGroupCall = true;
			}
			else
			{
				if (!bReport)
				{
					/*告知界面存在一个当前组呼呼入*/
					g_pNet->thereIsCallOfCare(this);
					bReport = true;
				}
				goto RECORD_VOICE_DATA;
			}
		}
		//调度组组呼
		else if (callType == GROUPCALL_TYPE && tagetId == CONFIG_DEFAULT_GROUP)
		{
			if (!g_bIsHaveAllCall && !g_bIsHavePrivateCall && !g_bIsHaveCurrentGroupCall)
			{
				g_bIsHaveDefaultGroupCall = true;
			}
			else
			{
				if (!bReport)
				{
					/*告知界面存在一个调度组呼呼入*/
					g_pNet->thereIsCallOfCare(this);
					bReport = true;
				}
				goto RECORD_VOICE_DATA;
			}
		}
		//个呼
		else if (callType == PRIVATE_CALL && tagetId == CONFIG_LOCAL_RADIO_ID)
		{
			if (!g_bIsHaveAllCall && !g_bIsHaveDefaultGroupCall && !g_bIsHaveCurrentGroupCall)
			{
				g_bIsHavePrivateCall = true;
			}
			else
			{
				if (!bReport)
				{
					/*告知界面存在一个个呼呼入*/
					g_pNet->thereIsCallOfCare(this);
					bReport = true;
				}
				goto RECORD_VOICE_DATA;
			}
		}
		//全呼
		else if (callType == ALL_CALL && tagetId == ALL_CALL_ID)
		{
			if (!g_bIsHaveDefaultGroupCall && !g_bIsHavePrivateCall && !g_bIsHaveCurrentGroupCall)
			{
				g_bIsHaveAllCall = true;
			}
			else
			{
				if (!bReport)
				{
					/*告知界面存在一个全呼呼入*/
					g_pNet->thereIsCallOfCare(this);
					bReport = true;
				}
				goto RECORD_VOICE_DATA;
			}
		}
		//加入实时播放的buffer
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


