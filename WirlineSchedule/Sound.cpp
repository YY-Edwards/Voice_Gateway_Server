#include "stdafx.h"
#include "Sound.h"
#include <io.h>
#include "SerialDongle.h"
#include "WLNet.h"

CSound::CSound()
{
	m_ThreadRunning = FALSE;
	m_hMyParentWind = NULL;
	m_bOutPcmStart = TRUE;
	m_dOutOffsset = 0;
	m_bStopPlay = TRUE;
	m_hPlaySongEvent = NULL;
	m_pOutDSNotify = NULL;
	m_dPreBufCusor = 0;
	m_dwCurPlayZone = 0;
	m_shouldStopPlay = FALSE;
	m_dwEndPlayZone = 0;
	m_pOutCurrentData = NULL;
	m_pOutPrevData = NULL;

	m_pInputDevice = NULL;
	m_pInputBuffer1 = NULL;
	m_pInputBuffer2 = NULL;
	m_pInputNotify = NULL;

	m_pInputFile = NULL;


	m_dwInputOffset = 0;

	m_hRecordEvent = NULL;
	m_bRecording = FALSE;
	m_bStartRecording = TRUE;
	m_dwCurRecordZone = 0;//当前播放buffer中的录制区域
	m_dwEndRecordZone = -1;//当前播放buffer中应该停止的区域
	m_bShouldStopRecording = FALSE;
	m_pInputPrevData = NULL;
	m_pInputCurData = NULL;
	m_hRecordDealEvent = NULL;
	m_hExitEvent = NULL;

	m_bIsDealing = FALSE;
}

CSound::~CSound()
{
	m_report = NULL;
	PleaseShutDown();
}

void CSound::BigEndianSoundOut(unsigned __int8* pSamples, DWORD dwCurrentDecodeSize)
{
	//if (dwCurrentDecodeSize < 5)
	//{
	//	int j = 0;
	//}

	int i = 0;

	//定义新的数据指针
	OUTDATA* pData = new OUTDATA;
	memset(pData, 0, sizeof(OUTDATA));

	pData->_length = OUT_SAMPLES_PER_20mS * 2;
	pData->_head = new BYTE[pData->_length + 1];
	memset(pData->_head, 0, pData->_length + 1);

	//大小端转换
	for (i = 0; i < OUT_SAMPLES_PER_20mS; i++)
	{
		pData->_head[1 + (i << 1)] = *pSamples++;
		pData->_head[+(i << 1)] = *pSamples++;
	}

	//如果这是最后一个数据，目前来说实时播放不需要此标识位。当播放音频发现播放队列为空时就停止播放。
	//if (dwCurrentDecodeSize <= 1)
	//{
	//	pData->_flag = FLAG_END;
	//}

	//将数据放入数据队列
	m_PlayTaskLocker.lock();
	m_playList.push_back(pData);
	m_PlayTaskLocker.unlock();

	//if (bEndDecode)
	//{
	//	sprintf_s(m_reportMsg, "bEndDecode:%d\r\n", bEndDecode);
	//	sendLogToWindow();
	//}

	//当前尚未开始播放任何音频并且缓存数量达到一秒或者当前已经结束解码
	if (m_bStopPlay
		&& (m_playList.size() > 50)
		)
	{
		m_bStopPlay = FALSE;
		m_bOutPcmStart = TRUE;
		SetEvent(m_hPlaySongEvent);
	}
}

DWORD CSound::StartSound(HWND hParentWnd, int uInDeviceID, int uOutDeviceID)
{
	DWORD result;

	if (m_ThreadRunning)
	{
		PleaseShutDown();
	}

	m_hMyParentWind = hParentWnd;

	if (WAVE_MAPPER == uOutDeviceID){
		m_OutputDevice = 0;
	}
	else{
		m_OutputDevice = uOutDeviceID;
	}


	m_hPlaySongEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hRecordEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_hSoundControlEvents[SOUND_PLAY] = m_hPlaySongEvent;
	m_hSoundControlEvents[SOUND_RECORD] = m_hRecordEvent;
	m_hSoundControlEvents[SOUND_EXIT] = m_hExitEvent;

	m_pSndThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		SndThreadProc,
		this,
		CREATE_SUSPENDED,
		&m_pSndThreadId
		);
	if (NULL == m_pSndThread){
		return 700;
	}
	m_ThreadRunning = TRUE;

	result = OpenOutput(); //Direct Sound has to open Output before Input.
	if (0 != result)
	{
		m_ThreadRunning = FALSE;
		m_pSndThread = NULL;

		sprintf_s(m_reportMsg, "output device open fail");
		sendLogToWindow();

		return 703;
	}

#if WXJ_DLL
#else
	result = OpenInput();
	if (0 != result)
	{
		CloseOutput();
		m_pSndThread = NULL;
		m_ThreadRunning = FALSE;

		sprintf_s(m_reportMsg, "input device open fail");
		sendLogToWindow();

		return 701;
	}
#endif // WXJ

	ResumeThread(m_pSndThread);
	return 0;
}

void CSound::PleaseShutDown(void)
{
	sprintf_s(m_reportMsg, "↓start shut down sound↓");
	sendLogToWindow();
	DWORD event;

	m_ThreadRunning = FALSE;

	sprintf_s(m_reportMsg, "close sound thread");
	sendLogToWindow();

	if (m_pSndThread != NULL)
	{
		SetEvent(m_hExitEvent);
		event = WaitForSingleObject(m_pSndThread, 1000);
		CloseHandle(m_pSndThread);
		m_pSndThread = NULL;
	}
#if WXJ_DLL
#else
	sprintf_s(m_reportMsg, "close input");
	sendLogToWindow();
	CloseInput();
#endif // WXJ_DLL
	sprintf_s(m_reportMsg, "close output");
	sendLogToWindow();
	CloseOutput();

	sprintf_s(m_reportMsg, "↑shutdown sound success↑");
	sendLogToWindow();

}

UINT WINAPI CSound::SndThreadProc(LPVOID pParam)
{
	CSound* pObject = (CSound*)pParam;
	pObject->SoundThread();
	pObject->m_ThreadRunning = FALSE;
	return 726;
}

void CSound::SoundThread()
{
	DWORD dTimeOut = INFINITE;
	DWORD rlt;
	while (m_ThreadRunning)
	{
		rlt = WaitForMultipleObjects(3, m_hSoundControlEvents, FALSE, dTimeOut);

		switch (rlt)
		{
		case WAIT_TIMEOUT:
			break;
		case SOUND_PLAY:
			ResetEvent(m_hPlaySongEvent);
			SoundOutputControl();
			break;
		case SOUND_RECORD:
			ResetEvent(m_hRecordEvent);
			SoundInputControl();
			break;
		case SOUND_EXIT:
			ResetEvent(m_hExitEvent);
			break;
		default:
			break;
		}
	}
}

DWORD CSound::OpenOutput(void)
{
	HRESULT hr;

	//获取播放设备
	hr = DirectSoundCreate8(NULL, &m_lpOutDS, NULL);
	if (DS_OK != hr)
	{
		CloseOutput();
		return hr;
	}
	//设置协作等级
	hr = m_lpOutDS->SetCooperativeLevel(m_hMyParentWind, DSSCL_PRIORITY);
	if (DS_OK != hr)
	{
		CloseOutput();
		return hr;
	}

	//创建主缓冲
	m_OutWFX.wFormatTag = WAVE_FORMAT_PCM;
	m_OutWFX.nChannels = 1;
	m_OutWFX.wBitsPerSample = OUT_BITS_PER_SAMPLE;
	m_OutWFX.nSamplesPerSec = OUT_SOUNDCARD_SAMPLES_PER_S;
	m_OutWFX.nBlockAlign = m_OutWFX.nChannels *(m_OutWFX.wBitsPerSample / 8);
	m_OutWFX.nAvgBytesPerSec = m_OutWFX.nSamplesPerSec * m_OutWFX.nBlockAlign;
	m_OutWFX.cbSize = 0;

	ZeroMemory(&m_dscOutputbd, sizeof(m_dscOutputbd));
	m_dscOutputbd.dwSize = sizeof(m_dscOutputbd);
	m_dscOutputbd.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME;
	m_dscOutputbd.dwBufferBytes = OUT_BYTES_PER_20mS * NUM_OUT_BUFFERS;
	m_dscOutputbd.lpwfxFormat = &m_OutWFX;

	hr = m_lpOutDS->CreateSoundBuffer(&m_dscOutputbd, &m_lpOutputDSB1, NULL);
	if (DS_OK != hr)
	{
		CloseOutput();
		return hr;
	}
	//关联副缓冲
	hr = m_lpOutputDSB1->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&m_lpOutputDSB2);
	if (DS_OK != hr)
	{
		CloseOutput();
		return hr;
	}

	//关联通知
	hr = m_lpOutputDSB2->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&m_pOutDSNotify);
	if (DS_OK != hr)
	{
		CloseOutput();
		return hr;
	}

	//设置消息通知位置
	for (int i = 0; i < NUM_OUT_BUFFERS; i++)
	{
		m_pOutDSPosNotify[i].dwOffset = i*OUT_BYTES_PER_20mS + OUT_BYTES_PER_20mS - 1;
		m_pOutDSPosNotifyEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_pOutDSPosNotify[i].hEventNotify = m_pOutDSPosNotifyEvents[i];
	}
	m_pOutDSNotify->SetNotificationPositions(NUM_IN_BUFFERS, m_pOutDSPosNotify);

	return 0;
}

void CSound::CloseOutput(void)
{
	DWORD result;

	//Release new DS8 buffer resources.
	if (NULL != m_lpOutputDSB2){
		result = m_lpOutputDSB2->Stop();
		result = m_lpOutputDSB2->Release();
		m_lpOutputDSB2 = NULL;
	}

	//If needed. release DS buffer resources.
	if (NULL != m_lpOutputDSB1){
		result = m_lpOutputDSB1->Release();
		m_lpOutputDSB1 = NULL;
	}

	if (NULL != m_lpOutDS){
		result = m_lpOutDS->Release();
		m_lpOutDS = NULL;
	}
}

void CSound::HandleSoundOutput(void)
{
	//m_lpOutputDSB2->SetVolume(g_volume);
	//long currentVolume = 0;
	//m_lpOutputDSB2->GetVolume(&currentVolume);
	//sprintf_s(m_reportMsg, "currentVolume:%ld", currentVolume);
	//g_volume += 1;
	//sendLogToWindow();

	LPVOID pAudioBuf1 = NULL;
	DWORD dWriteLen1 = 0;
	LPVOID pAudioBuf2 = NULL;
	DWORD dWriteLen2 = 0;
	HRESULT hr;

	if (m_bOutPcmStart)
	{

		//不存在音频数据
		if (!getOutNextData())
		{
			m_bStopPlay = TRUE;
			return;
		}

		//初始化本次播放数据的关键变量
		m_shouldStopPlay = FALSE;
		m_bOutPcmStart = FALSE;
		m_dwCurPlayZone = 0;//当前播放notify位置
		m_dwEndPlayZone = 0;//播放结束的motify位置
		m_bStopPlay = FALSE;//允许播放
		m_dOutOffsset = 0;//锁定buffer池的偏移量
		m_dPreBufCusor = 0;//当前音频数据的偏移量
		m_lpOutputDSB2->SetCurrentPosition(0);//设定播放位置
		hr = m_lpOutputDSB2->Lock(m_dOutOffsset, OUT_BYTES_PER_20mS*NUM_OUT_BUFFERS, &pAudioBuf1, &dWriteLen1, &pAudioBuf2, &dWriteLen2, NULL);
		if (DS_OK != hr)
		{
			CloseOutput();
			return;
		}

		//将buffer清空
		ZeroMemory((BYTE*)pAudioBuf1, dWriteLen1);

		//当前数据不足写入
		if (dWriteLen1 > m_pOutCurrentData->_length)
		{
			DWORD dRemainLen = dWriteLen1;
			DWORD dWritten = 0;

			memcpy(pAudioBuf1, &m_pOutCurrentData->_head[m_dPreBufCusor], m_pOutCurrentData->_length);
			dRemainLen -= m_pOutCurrentData->_length;
			dWritten += m_pOutCurrentData->_length;

			while (getOutNextData() && (dRemainLen > m_pOutCurrentData->_length))
			{
				m_dPreBufCusor = 0;
				memcpy((BYTE*)pAudioBuf1 + dWritten, &m_pOutCurrentData->_head[m_dPreBufCusor], m_pOutCurrentData->_length);
				dRemainLen -= m_pOutCurrentData->_length;
				dWritten += m_pOutCurrentData->_length;
			}

			if (NULL == m_pOutCurrentData)
			{
				ZeroMemory((BYTE*)pAudioBuf1 + dWritten, dRemainLen);
				m_shouldStopPlay = TRUE;
				m_dwEndPlayZone = (dWritten / OUT_BYTES_PER_20mS) + ((dWritten%OUT_BYTES_PER_20mS) ? (1) : (0)) - 1;
			}
			else
			{
				m_dPreBufCusor = 0;
				memcpy((BYTE*)pAudioBuf1 + dWritten, &m_pOutCurrentData->_head[m_dPreBufCusor], dRemainLen);
				m_pOutCurrentData->_length -= dRemainLen;
				m_dPreBufCusor += dRemainLen;
			}
		}
		else
		{
			memcpy(pAudioBuf1, &m_pOutCurrentData->_head[m_dPreBufCusor], dWriteLen1);
			m_pOutCurrentData->_length -= dWriteLen1;
			m_dPreBufCusor += dWriteLen1;
		}
		m_lpOutputDSB2->Unlock(pAudioBuf1, dWriteLen1, pAudioBuf2, dWriteLen2);
		//开始本轮播放
		m_lpOutputDSB2->Play(0, 0, DSBPLAY_LOOPING);

		//sprintf_s(m_reportMsg, "g_volume:%ld", g_volume);
		//sendLogToWindow();
		//m_lpOutputDSB2->SetVolume(g_volume);
		//long currentVolume = 0;
		//m_lpOutputDSB2->GetVolume(&currentVolume);
		//sprintf_s(m_reportMsg, "currentVolume:%ld", currentVolume);
		//g_volume -= 100;
		//sendLogToWindow();

	}
	else
	{
		long lockSize = 0;
		DWORD playPosition = 0;
		DWORD writePosition = 0;

		m_lpOutputDSB2->GetCurrentPosition(&playPosition, &writePosition);

		lockSize = playPosition - m_dOutOffsset;

		if (lockSize < 0)
		{
			lockSize += (OUT_BYTES_PER_20mS * NUM_OUT_BUFFERS);
			lockSize -= (lockSize%OUT_BYTES_PER_20mS);
		}
		if (lockSize == 0)
		{
			return;
		}

		hr = m_lpOutputDSB2->Lock(m_dOutOffsset, lockSize, &pAudioBuf1, &dWriteLen1, &pAudioBuf2, &dWriteLen2, NULL);
		if (DS_OK != hr)
		{
			CloseOutput();
			return;
		}

		//音频数据已经加载完毕，缓冲数据已经播放完毕
		if (m_shouldStopPlay
			&& ((m_dwCurPlayZone > m_dwEndPlayZone) || ((m_dwCurPlayZone == 0) && (m_dwEndPlayZone = (NUM_OUT_BUFFERS - 1))))
			)
		{
			m_bStopPlay = TRUE;
			//sprintf_s(m_reportMsg, "m_dwCurPlayZone:%lu,m_dwEndPlayZone:%lu", m_dwCurPlayZone, m_dwEndPlayZone);
			//sendLogToWindow();
		}
		//音频数据未加载完毕，缓冲数据未播放完毕
		else
		{
			//当前数据不足写入
			if (dWriteLen1 > m_pOutCurrentData->_length)
			{
				DWORD dRemainLen = dWriteLen1;
				DWORD dWritten = 0;

				memcpy(pAudioBuf1, &m_pOutCurrentData->_head[m_dPreBufCusor], m_pOutCurrentData->_length);
				m_dOutOffsset += m_pOutCurrentData->_length;
				dRemainLen -= m_pOutCurrentData->_length;
				dWritten += m_pOutCurrentData->_length;

				while (getOutNextData() && (dRemainLen > m_pOutCurrentData->_length))
				{
					m_dPreBufCusor = 0;
					memcpy((BYTE*)pAudioBuf1 + dWritten, &m_pOutCurrentData->_head[m_dPreBufCusor], m_pOutCurrentData->_length);
					m_dOutOffsset += m_pOutCurrentData->_length;
					dRemainLen -= m_pOutCurrentData->_length;
					dWritten += m_pOutCurrentData->_length;
				}

				if (NULL == m_pOutCurrentData)
				{
					ZeroMemory((BYTE*)pAudioBuf1 + dWritten, dRemainLen);
					m_shouldStopPlay = TRUE;
					m_dwEndPlayZone = m_dwCurPlayZone;
				}
				else
				{
					m_dPreBufCusor = 0;
					memcpy((BYTE*)pAudioBuf1 + dWritten, &m_pOutCurrentData->_head[m_dPreBufCusor], dRemainLen);
					m_dOutOffsset += dRemainLen;
					m_pOutCurrentData->_length -= dRemainLen;
					m_dPreBufCusor += dRemainLen;
				}
			}
			else
			{
				memcpy(pAudioBuf1, &m_pOutCurrentData->_head[m_dPreBufCusor], dWriteLen1);
				m_pOutCurrentData->_length -= dWriteLen1;
				m_dOutOffsset += dWriteLen1;
				m_dPreBufCusor += dWriteLen1;

				if (pAudioBuf2 != NULL)
				{
					//当前数据不足写入
					if (dWriteLen2 >= m_pOutCurrentData->_length)
					{
						DWORD dRemainLen = dWriteLen2;
						DWORD dWritten = 0;

						memcpy(pAudioBuf2, &m_pOutCurrentData->_head[m_dPreBufCusor], m_pOutCurrentData->_length);
						m_dOutOffsset += m_pOutCurrentData->_length;
						dRemainLen -= m_pOutCurrentData->_length;
						dWritten += m_pOutCurrentData->_length;

						while (getOutNextData() && (dRemainLen > m_pOutCurrentData->_length))
						{
							m_dPreBufCusor = 0;
							memcpy((BYTE*)pAudioBuf2 + dWritten, &m_pOutCurrentData->_head[m_dPreBufCusor], m_pOutCurrentData->_length);
							m_dOutOffsset += m_pOutCurrentData->_length;
							dRemainLen -= m_pOutCurrentData->_length;
							dWritten += m_pOutCurrentData->_length;
						}

						if (NULL == m_pOutCurrentData)
						{
							ZeroMemory((BYTE*)pAudioBuf1 + dWritten, dRemainLen);
							m_shouldStopPlay = TRUE;
							m_dwEndPlayZone = m_dwCurPlayZone;
						}
						else
						{
							m_dPreBufCusor = 0;
							memcpy((BYTE*)pAudioBuf2 + dWritten, &m_pOutCurrentData->_head[m_dPreBufCusor], dRemainLen);
							m_dOutOffsset += dRemainLen;
							//dWritten += dRemainLen;
							m_pOutCurrentData->_length -= dRemainLen;
							m_dPreBufCusor += dRemainLen;
							//dRemainLen = 0;
						}
					}
					else
					{
						memcpy(pAudioBuf2, &m_pOutCurrentData->_head[m_dPreBufCusor], dWriteLen2);
						m_pOutCurrentData->_length -= dWriteLen2;
						m_dOutOffsset += dWriteLen2;
						m_dPreBufCusor += dWriteLen2;
					}
				}
			}
		}
		m_lpOutputDSB2->Unlock(pAudioBuf1, dWriteLen1, pAudioBuf2, dWriteLen2);
		m_dOutOffsset = m_dOutOffsset % (OUT_BYTES_PER_20mS * NUM_OUT_BUFFERS);
	}
}

void CSound::sendLogToWindow()
{
	//SYSTEMTIME now = { 0 };
	//GetLocalTime(&now);
	//printf_s("%04u-%02u-%02u %02u:%02u:%02u %03u %s\n", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds, m_reportMsg);
	if (NULL != m_report)
	{
		m_report(m_reportMsg);
	}
}

void CSound::SetLogPtr(PLogReport value)
{
	m_report = value;
}

BOOL CSound::getOutNextData()
{
	freeOutData(m_pOutPrevData);
	m_pOutPrevData = NULL;

	if (m_playList.size() <= 0)
	{
		freeOutData(m_pOutCurrentData);
		m_pOutCurrentData = NULL;
		return FALSE;
	}
	else
	{
		m_pOutPrevData = m_pOutCurrentData;
		m_PlayTaskLocker.lock();
		m_pOutCurrentData = m_playList.front();
		m_playList.pop_front();
		m_PlayTaskLocker.unlock();
		return TRUE;
	}

}

void CSound::freeOutData(OUTDATA* pData)
{
	if (pData != NULL)
	{
		if (pData->_head != NULL)
		{
			delete pData->_head;
			pData->_head = NULL;
		}
		delete pData;
		pData = NULL;
	}
}

DWORD CSound::OpenInput(void)
{
	DWORD rlt = 0;


	m_hRecordDealEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	////开启数据处理线程
	//m_pIptThread = (HANDLE)_beginthreadex(
	//	NULL,
	//	0,
	//	IptSndThreadProc,
	//	this,
	//	CREATE_SUSPENDED,
	//	&m_pIptThreadId
	//	);
	//if (NULL == m_pSndThread){
	//	return 700;
	//}

	//创建设备
	rlt = DirectSoundCaptureCreate8(NULL, &m_pInputDevice, NULL);
	if (DS_OK != rlt)
	{
		return rlt;
	}

	ZeroMemory(&m_inputFormat, sizeof(WAVEFORMATEX));
	m_inputFormat.cbSize = 0;
	m_inputFormat.nChannels = 1;
	m_inputFormat.nSamplesPerSec = IN_SOUNDCARD_SAMPLES_PER_S;
	m_inputFormat.wBitsPerSample = IN_BITS_PER_SAMPLE;
	m_inputFormat.nBlockAlign = m_inputFormat.nChannels *(m_inputFormat.wBitsPerSample / 8);
	m_inputFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_inputFormat.nAvgBytesPerSec = (m_inputFormat.nBlockAlign)*(m_inputFormat.nSamplesPerSec);

	ZeroMemory(&m_inputDesc, sizeof(DSCBUFFERDESC));
	m_inputDesc.dwSize = sizeof(DSCBUFFERDESC);
	m_inputDesc.dwBufferBytes = NUM_IN_BUFFERS*IN_BYTES_PER_20mS;
	m_inputDesc.lpwfxFormat = &m_inputFormat;

	//创建主缓冲
	rlt = m_pInputDevice->CreateCaptureBuffer(&m_inputDesc, &m_pInputBuffer1, NULL);
	if (DS_OK != rlt)
	{
		return rlt;
	}
	//关联副缓冲
	rlt = m_pInputBuffer1->QueryInterface(IID_IDirectSoundCaptureBuffer8, (LPVOID*)&m_pInputBuffer2);
	if (DS_OK != rlt)
	{
		return rlt;
	}
	//寻找通知接口
	rlt = m_pInputBuffer2->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&m_pInputNotify);
	if (DS_OK != rlt)
	{
		return rlt;
	}
	//设置消息通知位置
	for (int i = 0; i < NUM_IN_BUFFERS; i++)
	{
		m_pInputPosNotify[i].dwOffset = i*IN_BYTES_PER_20mS + IN_BYTES_PER_20mS - 1;
		m_pInputPosNotifyEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_pInputPosNotify[i].hEventNotify = m_pInputPosNotifyEvents[i];
	}
	//关联通知
	rlt = m_pInputNotify->SetNotificationPositions(NUM_IN_BUFFERS, m_pInputPosNotify);
	if (DS_OK != rlt)
	{
		return rlt;
	}

	//ResumeThread(m_pIptThread);

	return 0;
}

void CSound::CloseInput(void)
{
	if (m_pInputBuffer2 != NULL)
	{
		m_pInputBuffer2->Stop();
		m_pInputBuffer2->Release();
		m_pInputBuffer2 = NULL;
	}
	if (m_pInputBuffer1 != NULL)
	{
		m_pInputBuffer1->Release();
		m_pInputBuffer1 = NULL;
	}
	if (m_pInputFile != NULL)
	{
		fclose(m_pInputFile);
		m_pInputFile = NULL;
	}
	//if (m_pInputFile1 != NULL)
	//{
	//	fclose(m_pInputFile1);
	//	m_pInputFile1 = NULL;
	//}
}

void CSound::HandleSoundInput(void)
{
	DWORD rlt = 0;
	long m_dwInputLockSize = 0;
	DWORD m_dwInputCapturePosition = 0;
	DWORD m_dwInputReadPosition = 0;
	LPVOID m_pAudio1 = NULL;
	DWORD m_dwReadLen1 = 0;
	LPVOID m_pAudio2 = NULL;
	DWORD m_dwReadLen2 = 0;
	DWORD m_dwWriteLen = 0;

	if (m_bStartRecording)
	{
		m_dwEndRecordZone = -1;
		m_dwInputOffset = 0;
		m_bShouldStopRecording = FALSE;
		m_bStartRecording = FALSE;
		m_pInputBuffer2->Start(DSBPLAY_LOOPING);
	}
	else
	{

		rlt = m_pInputBuffer2->GetCurrentPosition(&m_dwInputCapturePosition, &m_dwInputReadPosition);
		if (rlt != DS_OK)
		{
			sprintf_s(m_reportMsg, "GetCurrentPosition fail!");
			sendLogToWindow();
			return;
		}


		//退出录制
		if (m_bShouldStopRecording
			&& ((m_dwCurRecordZone > m_dwEndRecordZone) || ((m_dwCurRecordZone == 0) && (m_dwEndRecordZone == NUM_IN_BUFFERS - 1)))
			)
		{
			m_bRecording = FALSE;
		}

		else
		{
			if (m_bShouldStopRecording)
			{
				m_dwEndRecordZone = m_dwCurRecordZone;
			}

			m_dwInputLockSize = m_dwInputCapturePosition - m_dwInputOffset;

			if (m_dwInputLockSize < 0)
			{
				m_dwInputLockSize += (NUM_IN_BUFFERS*IN_BYTES_PER_20mS);
				m_dwInputLockSize -= (m_dwInputLockSize%IN_BYTES_PER_20mS);
			}

			if (m_dwInputLockSize == 0 
				|| (0 != (m_dwInputLockSize%IN_BYTES_PER_20mS) && !m_bShouldStopRecording)
				)
			{
				return;
			}

			UINT numOfBuf = 0;
			UINT index = 0;
			UINT offset = 0;
			SoundCardPCM* pData = NULL;
			LONG dwShouldReadLen = 0;
			LONG dwPrepareReadLen = 0;
			DWORD dwSrcOffset = 0;

			//numOfBuf = m_dwInputLockSize / IN_BYTES_PER_20mS;//根据locksize换算应该填充几个buff

			//显示当前locksize的大小和buff的数目
			//sprintf_s(m_reportMsg, "m_dwInputLockSize:%lu,numOfBuf:%u", m_dwInputLockSize, numOfBuf);
			//sendLogToWindow();


			rlt = m_pInputBuffer2->Lock(m_dwInputOffset, m_dwInputLockSize, &m_pAudio1, &m_dwReadLen1, &m_pAudio2, &m_dwReadLen2, 0);
			if (rlt != DS_OK)
			{
				sprintf_s(m_reportMsg, "HandleSoundInput Lock fail!");
				sendLogToWindow();
				return;
			}
			
			dwPrepareReadLen = m_dwReadLen1;
			dwSrcOffset = 0;
			/*向buff中填充数据*/
			while ((dwPrepareReadLen - IN_BYTES_PER_20mS) >= 0)
			{
				dwPrepareReadLen = dwPrepareReadLen - IN_BYTES_PER_20mS;
				pData = new SoundCardPCM;
				memset(pData, 0, sizeof(SoundCardPCM));
				memcpy(pData->CharBuf, (unsigned __int8*)m_pAudio1 + dwSrcOffset, IN_BYTES_PER_20mS);
				dwSrcOffset += IN_BYTES_PER_20mS;

				m_inputDataLocker.lock();
				m_inputDataList.push_back(pData);
				m_inputDataLocker.unlock();
				pData = NULL;
			}
			//填充残留数据
			if (0 != dwPrepareReadLen)
			{
				offset = dwPrepareReadLen;
				dwShouldReadLen = dwPrepareReadLen;

				pData = new SoundCardPCM;
				memset(pData, 0, sizeof(SoundCardPCM));
				memcpy(pData->CharBuf, (unsigned __int8*)m_pAudio1 + dwSrcOffset, dwShouldReadLen);
				dwSrcOffset += dwShouldReadLen;
			}
			
			//fwrite(m_pAudio1, 1, m_dwReadLen1, m_pInputFile1);


			m_dwInputOffset += m_dwReadLen1;
			//if (m_dwReadLen1 != m_dwWriteLen)
			//{
			//	sprintf_s(m_reportMsg, "HandleSoundInput warning:read is not all data!");
			//	sendLogToWindow();
			//}

			if (NULL != m_pAudio2)
			{
				dwSrcOffset = 0;
				dwPrepareReadLen = m_dwReadLen2;
				if (pData != NULL)
				{
					dwShouldReadLen = IN_BYTES_PER_20mS - offset;
					dwPrepareReadLen -= dwShouldReadLen;
					memcpy((unsigned __int8*)pData->CharBuf + offset, (unsigned __int8*)m_pAudio2 + dwSrcOffset, dwShouldReadLen);
					dwSrcOffset += dwShouldReadLen;

					m_inputDataLocker.lock();
					m_inputDataList.push_back(pData);
					m_inputDataLocker.unlock();
					pData = NULL;
				}
				while ((dwPrepareReadLen - IN_BYTES_PER_20mS) >= 0)
				{
					dwPrepareReadLen = dwPrepareReadLen - IN_BYTES_PER_20mS;
					pData = new SoundCardPCM;
					memset(pData, 0, sizeof(SoundCardPCM));

					memcpy(pData->CharBuf, (unsigned __int8*)m_pAudio2 + dwSrcOffset, IN_BYTES_PER_20mS);
					dwSrcOffset += IN_BYTES_PER_20mS;

					m_inputDataLocker.lock();
					m_inputDataList.push_back(pData);
					m_inputDataLocker.unlock();
					pData = NULL;
				}

				//m_dwWriteLen = fwrite(m_pAudio2, 1, m_dwReadLen2, m_pInputFile1);
				m_dwInputOffset += m_dwReadLen2;
				//if (m_dwReadLen2 != m_dwWriteLen)
				//{
				//	sprintf_s(m_reportMsg, "warning:read is not all data!");
				//	sendLogToWindow();
				//}
			}

			rlt = m_pInputBuffer2->Unlock(m_pAudio1, m_dwReadLen1, m_pAudio2, m_dwReadLen2);
			if (rlt != DS_OK)
			{
				sprintf_s(m_reportMsg, "HandleSoundInput Unlock fail!");
				sendLogToWindow();
				return;
			}
			m_dwInputOffset = m_dwInputOffset % (NUM_IN_BUFFERS*IN_BYTES_PER_20mS);

			//将获取到的音频数据处理为AMBE能够处理的数据
			while (m_inputDataList.size()>0)
			{
				BOOL bIsEnd = FALSE;
				m_pInputPrevData = m_pInputCurData;
				m_pInputCurData = NULL;
				if (m_pInputPrevData!= NULL)
				{
					m_inputDataLocker.lock();
					m_inputDataList.pop_front();
					m_inputDataLocker.unlock();
					delete m_pInputPrevData;
					m_pInputPrevData = NULL;
				}
				if (m_inputDataList.size()>0)
				{
					if (1 == m_inputDataList.size()
						&& m_bShouldStopRecording)
					{
						bIsEnd = TRUE;
					}
					m_inputDataLocker.lock();
					m_pInputCurData = m_inputDataList.front();
					m_inputDataLocker.unlock();
				}
				else
				{
					continue;
				}
				g_pDongle->SendDVSIPCMMsgtoDongle(m_pInputCurData->CharBuf, bIsEnd);
			}
		}
	}
}

void CSound::SoundOutputControl()
{

	DWORD dTimeOut = INFINITE;
	while (!m_bStopPlay)
	{
		if (m_bOutPcmStart)
		{
			sprintf_s(m_reportMsg, "start play");
			sendLogToWindow();
			CRecordFile *p = g_pNet->getCurrentPlayInfo();
			g_pNet->Send_CARE_CALL_STATUS(p->callType, p->srcId, p->tagetId, HAVE_CALL_START_PLAY);
			for (int i = 0; i < NUM_OUT_BUFFERS; i++)
			{
				ResetEvent(m_pOutDSPosNotifyEvents[i]);
			}
			HandleSoundOutput();
		}
		m_dwCurPlayZone = WaitForMultipleObjects(NUM_OUT_BUFFERS, m_pOutDSPosNotifyEvents, FALSE, dTimeOut);
		if (m_dwCurPlayZone == WAIT_TIMEOUT)
		{
			sprintf_s(m_reportMsg, "unknown error of Audio Device");
			sendLogToWindow();
			break;
		}
		HandleSoundOutput();
	}
	//待播放完剩余buffer池中的数据
	Sleep(PLAY_BUFFER_TIME);
	m_lpOutputDSB2->Stop();
	sprintf_s(m_reportMsg, "stop play");
	sendLogToWindow();
	CRecordFile *p = g_pNet->getCurrentPlayInfo();
	g_pNet->Send_CARE_CALL_STATUS(p->callType, p->srcId, p->tagetId, HAVE_CALL_END_PLAY);
	//本次播放完毕,初始下次播放变量
	m_bOutPcmStart = TRUE;
	freeOutData(m_pOutCurrentData);
	m_pOutCurrentData = NULL;
	g_bIsHaveAllCall = false;
	g_bIsHaveDefaultGroupCall = false;
	g_bIsHavePrivateCall = false;
	g_bIsHaveCurrentGroupCall = false;
}

void CSound::SoundInputControl()
{
	DWORD rlt = 0;
	DWORD dwWaitTimeOut = INFINITE;
	while (m_bRecording)
	{
		if (m_bStartRecording)
		{
			if (NULL == m_pInputFile)
			{
				//打开保存文件
				rlt = fopen_s(&m_pInputFile, INPUT_DATA_PATH, "wb+");

				if (NULL == m_pInputFile)
				{
					return;
				}
			}

			//if (NULL == m_pInputFile1)
			//{
			//	打开保存文件
			//		rlt = fopen_s(&m_pInputFile1, INPUT_DATA_PATH_1, "wb+");

			//	if (NULL == m_pInputFile1)
			//	{
			//		return;
			//	}
			//}

			
			for (int i = 0; i < NUM_IN_BUFFERS; i++)
			{
				ResetEvent(m_pInputPosNotifyEvents[i]);
			}

			sprintf_s(m_reportMsg, "start record");
			sendLogToWindow();

			HandleSoundInput();
		}
		m_dwCurRecordZone = WaitForMultipleObjects(NUM_IN_BUFFERS, m_pInputPosNotifyEvents, FALSE, dwWaitTimeOut);
		if (m_dwCurRecordZone == WAIT_TIMEOUT)
		{
			sprintf_s(m_reportMsg, "unknown error of Audio Device");
			sendLogToWindow();
			break;
		}
		HandleSoundInput();
	}
	m_pInputBuffer2->Stop();
	sprintf_s(m_reportMsg, "stop record");
	sendLogToWindow();

	//if (NULL != m_pInputFile1)
	//{
	//	fclose(m_pInputFile1);
	//	m_pInputFile1 = NULL;
	//}

	//本次录制完毕,初始下次录制变量
	m_bStartRecording = TRUE;
}

void CSound::StartRecord()
{
	m_bStartRecording = TRUE;
	m_bRecording = TRUE;
	SetEvent(m_hRecordEvent);
}

void CSound::StopRecord()
{
	m_bShouldStopRecording = TRUE;
}

void CSound::closeFile()
{
	if (m_pInputFile != NULL)
	{
		fclose(m_pInputFile);
		m_pInputFile = NULL;
	}
}

void CSound::immediatelyPlay()
{
	//立刻播放缓存内容
	if (m_bStopPlay
		&& (m_playList.size() > 0)
		)
	{
		sprintf_s(m_reportMsg, "immediately play");
		sendLogToWindow();

		m_bStopPlay = FALSE;
		m_bOutPcmStart = TRUE;
		SetEvent(m_hPlaySongEvent);
	}
}
