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
	//����Ǳ��ط���ĺ��У�����Ҫ����
	if (bLocalRequest)
	{
		goto RECORD_VOICE_DATA;
	}
	if ((callType == GROUPCALL_TYPE && tagetId == CONFIG_DEFAULT_GROUP)
		|| (callType == PRIVATE_CALL && tagetId == CONFIG_LOCAL_RADIO_ID)
		|| (callType == ALL_CALL && tagetId == ALL_CALL_ID))
	{
		//��ǰ�����
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
					/*��֪�������һ����ǰ�������*/
					g_pNet->thereIsCallOfCare(this);
					bReport = true;
				}
				goto RECORD_VOICE_DATA;
			}
		}
		//���������
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
					/*��֪�������һ�������������*/
					g_pNet->thereIsCallOfCare(this);
					bReport = true;
				}
				goto RECORD_VOICE_DATA;
			}
		}
		//����
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
					/*��֪�������һ����������*/
					g_pNet->thereIsCallOfCare(this);
					bReport = true;
				}
				goto RECORD_VOICE_DATA;
			}
		}
		//ȫ��
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
					/*��֪�������һ��ȫ������*/
					g_pNet->thereIsCallOfCare(this);
					bReport = true;
				}
				goto RECORD_VOICE_DATA;
			}
		}
		//����ʵʱ���ŵ�buffer
		tAMBEFrame* pAMBEFrame = NULL;
		pAMBEFrame = g_pDongle->GetFreeAMBEBuffer();
		memcpy(pAMBEFrame->fld.ChannelBits, pFrame, len);
		g_pDongle->deObfuscate(IPSCTODONGLE, pAMBEFrame);
		g_pDongle->MarkAMBEBufferFilled();
		//������������
		g_pDongle->releaseWaitNextNetDataEvent();
		g_pDongle->DecodeBuffers();
		g_pNet->setCurrentPlayInfo(this);
	}
RECORD_VOICE_DATA:
	prevTimestamp = GetTickCount();
	/*����¼���̣߳���AMBE����д�뵽����*/
	memcpy(buffer + lenght, pFrame, len);
	lenght += len;
}


