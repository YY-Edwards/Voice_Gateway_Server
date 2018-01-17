#include "stdafx.h"
#include "../include/NSSound.h"
#include "../include/NSLog.h"
#include <process.h>
#include "../include/NSAmbe.h"
#include "../include/NSWLNet.h"

#define RING_SIZE_OUT 64

NSSound::NSSound()
:m_bOutWork(false)
, m_bInWork(false)
, m_pLog(NSLog::instance())
, m_pThreadOut(NULL)
, m_pThreadIn(NULL)
, m_mutexOut(INITLOCKER())
, m_ringOut(createRingBuffer(RING_SIZE_OUT + 1, sizeof(out_data_pcm_t)))
, m_pWaveOutBlocks(NULL)
, m_waveOutFreeBlockCount(0)
, m_waveOutCurrentBlock(0)
, m_hWaveOut(NULL)
, m_hWaveIn(NULL)
, m_pAmbe(new NSAmbe(g_pNSManager))
, m_bWaveInReset(false)
, m_bufflag(0)
, m_micStatus(Mic_Error)
, m_inThreadTimer(NULL)
{
	memset(&m_outFormat, 0, sizeof(WAVEFORMATEX));
	memset(&m_inputFormat, 0, sizeof(WAVEFORMATEX));
}

NSSound::~NSSound()
{
	/*Sound*/
	m_bOutWork = false;
	if (NULL != m_pThreadOut)
	{
		WaitForSingleObject(m_pThreadOut, 1000);
		CloseHandle(m_pThreadOut);
		m_pThreadOut = NULL;
	}
	clearRingOut();

	/*Mic*/
	if (m_hWaveIn)
	{
		m_bWaveInReset = true;
		waveInStop(m_hWaveIn);
		waveInReset(m_hWaveIn);
		waveInClose(m_hWaveIn);
		while (m_hWaveIn)
		{
			/*等待麦克风正常关闭*/
			Sleep(20);
		}
	}
	m_bInWork = false;
	if (NULL != m_pThreadIn)
	{
		ContinueInThread();
		WaitForSingleObject(m_pThreadIn, 1000);
		CloseHandle(m_pThreadIn);
		m_pThreadIn = NULL;
	}
	if (m_pAmbe)
	{
		delete m_pAmbe;
		m_pAmbe = NULL;
	}
	DELETELOCKER(m_mutexOut);
}

DWORD NSSound::InitSoundOut()
{
	if (m_bOutWork)
	{
		return 0;
	}
	unsigned int pSndThreadId = 0;
	/*参数配置*/
	m_outFormat.nSamplesPerSec = 8000;
	m_outFormat.wBitsPerSample = 16;
	m_outFormat.nChannels = 1;
	m_outFormat.cbSize = 0;
	m_outFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_outFormat.nBlockAlign = (m_outFormat.wBitsPerSample * m_outFormat.nChannels) >> 3;
	m_outFormat.nAvgBytesPerSec = m_outFormat.nBlockAlign * m_outFormat.nSamplesPerSec;
	/*检测音频输出设备是否正常*/
	if (waveOutOpen(NULL, WAVE_MAPPER, &m_outFormat, 0, 0, WAVE_FORMAT_QUERY) != MMSYSERR_NOERROR)
	{
		m_pLog->AddLog(Ns_Log_Error, "waveOutOpen fail");
		return 1;
	}
	m_pThreadOut = (HANDLE)_beginthreadex(
		NULL,
		0,
		SoundOutThredProc,
		this,
		CREATE_SUSPENDED,
		&pSndThreadId
		);
	if (NULL == m_pThreadOut)
	{
		m_pLog->AddLog(Ns_Log_Error, "SoundOutThredProc fail");
		return 1;
	}
	m_bOutWork = true;
	ResumeThread(m_pThreadOut);
	return 0;
}

UINT WINAPI NSSound::SoundOutThredProc(LPVOID pParam)
{
	NSSound* p = (NSSound*)pParam;
	if (p)
	{
		p->SoundOutThread();
	}
	return 0;
}

void NSSound::SoundOutThread()
{
	m_pLog->AddLog(Ns_Log_Info, "SoundOutThred start");
	out_data_pcm_t pcm = { 0 };
	int unit_size = sizeof(pcm._head);
	/*内存申请*/
	InitializeCriticalSection(&m_waveCriticalSection);
	m_pWaveOutBlocks = allocateBlocks(BLOCK_SIZE, BLOCK_COUNT);
	if (NULL == m_pWaveOutBlocks)
	{
		DeleteCriticalSection(&m_waveCriticalSection);
		m_pLog->AddLog(Ns_Log_Error, "allocateBlocks fail");
		goto SoundOutThread_End;
		return;
	}
	m_waveOutFreeBlockCount = BLOCK_COUNT;
	m_waveOutCurrentBlock = 0;
	/*打开扬声器*/
	if (waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &m_outFormat, (DWORD_PTR)waveOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
	{
		DeleteCriticalSection(&m_waveCriticalSection);
		freeBlocks(m_pWaveOutBlocks);
		m_pLog->AddLog(Ns_Log_Error, "waveOutOpen fail");
		goto SoundOutThread_End;
		return;
	}
	while (m_bOutWork)
	{
		int size = sizeRingOut();
		if (0 == size)
		{
			Sleep(SLEEP_SOUNDOUT_THREAD);
			continue;
		}
		//m_pLog->AddLog("before del sizeRingOut:%d", size);
		popRingOutItem(&pcm);
		writeAudio(m_hWaveOut, (char*)pcm._head, unit_size);
	}
	///*等待内存释放*/
	while (m_waveOutFreeBlockCount < BLOCK_COUNT)
		Sleep(10);
	for (int i = 0; i < m_waveOutFreeBlockCount; i++)
	{
		/*对已经释放的内存重置FLAG*/
		if (m_pWaveOutBlocks[i].dwFlags &WHDR_PREPARED)
		{
			waveOutUnprepareHeader(m_hWaveOut, &m_pWaveOutBlocks[i], sizeof(WAVEHDR));
		}
	}
	/*释放资源*/
	DeleteCriticalSection(&m_waveCriticalSection);
	freeBlocks(m_pWaveOutBlocks);
	waveOutClose(m_hWaveOut);
SoundOutThread_End:
	m_bOutWork = false;
	m_pLog->AddLog(Ns_Log_Info, "SoundOutThred End");
}

void NSSound::WritePcm(const char* pData, int length)
{
	if (!m_bOutWork)
	{
		return;
	}
	int index = 0;
	out_data_pcm_t pcm = { 0 };
	int unit_size = sizeof(pcm._head);
	while (length >= (unit_size*(index + 1)))
	{
		int size = sizeRingOut();
		//m_pLog->AddLog("before add sizeRingOut:%d", size);
		memcpy(pcm._head, pData + (index*unit_size), unit_size);
		if (size < RING_SIZE_OUT)
		{
			addRingOutItem(&pcm);
		}
		else
		{
			m_pLog->AddLog(Ns_Log_Info, "sizeFreeRingOut is 0 will drop");
		}
		index++;
	}
}

void NSSound::clearRingOut()
{
	TRYLOCK(m_mutexOut);
	freeRingBuffer(m_ringOut);
	m_ringOut = NULL;
	RELEASELOCK(m_mutexOut);
}

int NSSound::sizeRingOut()
{
	int rlt = 0;
	TRYLOCK(m_mutexOut);
	rlt = getRingBufferSize(m_ringOut);
	RELEASELOCK(m_mutexOut);
	return rlt;
}

int NSSound::popRingOutItem(out_data_pcm_t* item)
{
	int rlt = 0;
	TRYLOCK(m_mutexOut);
	rlt = pop(m_ringOut, item);
	RELEASELOCK(m_mutexOut);
	return rlt;
}

void NSSound::addRingOutItem(out_data_pcm_t* item)
{
	TRYLOCK(m_mutexOut);
	push(m_ringOut, item);
	RELEASELOCK(m_mutexOut);
}

WAVEHDR* NSSound::allocateBlocks(int size, int count)
{
	unsigned char* buffer;
	int i;
	WAVEHDR* blocks;
	DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;
	if ((buffer = (unsigned char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBufferSize)) == NULL)
	{
		m_pLog->AddLog(Ns_Log_Error,"Memory allocationerror");
		return NULL;
	}
	blocks = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR)* count;
	for (i = 0; i < count; i++) {
		blocks[i].dwBufferLength = size;
		blocks[i].lpData = (LPSTR)buffer;
		buffer += size;
	}
	return blocks;
}

void NSSound::freeBlocks(WAVEHDR* blockArray)
{
	HeapFree(GetProcessHeap(), 0, blockArray);
}

void NSSound::writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size)
{
	WAVEHDR* current;
	int remain;
	current = &m_pWaveOutBlocks[m_waveOutCurrentBlock];
	while (size > 0)
	{
		if (current->dwFlags & WHDR_PREPARED)
		{
			waveOutUnprepareHeader(hWaveOut, current, sizeof(WAVEHDR));
		}
		if (size < (int)(BLOCK_SIZE - current->dwUser))
		{
			memcpy(current->lpData + current->dwUser, data, size);
			current->dwUser += size;
			break;
		}
		remain = BLOCK_SIZE - current->dwUser;
		memcpy(current->lpData + current->dwUser, data, remain);
		size -= remain;
		data += remain;
		current->dwBufferLength = BLOCK_SIZE;
		waveOutPrepareHeader(hWaveOut, current, sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, current, sizeof(WAVEHDR));
		EnterCriticalSection(&m_waveCriticalSection);
		m_waveOutFreeBlockCount--;
		LeaveCriticalSection(&m_waveCriticalSection);
		while (!m_waveOutFreeBlockCount)
		{
			Sleep(10);
		}
		m_waveOutCurrentBlock++;
		m_waveOutCurrentBlock %= BLOCK_COUNT;
		current = &m_pWaveOutBlocks[m_waveOutCurrentBlock];
		current->dwUser = 0;
	}
}

void CALLBACK NSSound::waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	NSSound *p = (NSSound *)dwInstance;
	if (p)
	{
		p->handleWaveOutProc(uMsg);
	}
}

void NSSound::handleWaveOutProc(UINT uMsg)
{
	switch (uMsg)
	{
	case WOM_DONE:
	{
					 /*一个BLOCK播放完毕并释放*/
					 EnterCriticalSection(&m_waveCriticalSection);
					 m_waveOutFreeBlockCount++;
					 LeaveCriticalSection(&m_waveCriticalSection);
	}
		break;
	case WOM_OPEN:
	{

	}
		break;
	case WOM_CLOSE:
	{
	}
		break;
	default:
		break;
	}
}

void NSSound::temp(char* pdata, int length)
{
	/*内存申请*/
	InitializeCriticalSection(&m_waveCriticalSection);
	m_pWaveOutBlocks = allocateBlocks(BLOCK_SIZE, BLOCK_COUNT);
	m_waveOutFreeBlockCount = BLOCK_COUNT;
	m_waveOutCurrentBlock = 0;
	/*打开扬声器*/
	if (waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &m_outFormat, (DWORD_PTR)waveOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
	{
		//sprintf_s(m_reportMsg, "waveOutOpen fail");
		//sendLogToWindow();
		m_pLog->AddLog(Ns_Log_Error,"waveOutOpen fail");
		return;
	}

	/*填充音频数据*/

	writeAudio(m_hWaveOut, pdata, length);

	///*等待内存释放*/
	while (m_waveOutFreeBlockCount < BLOCK_COUNT)
		Sleep(10);
	for (int i = 0; i < m_waveOutFreeBlockCount; i++)
	{
		/*对已经释放的内存重置FLAG*/
		if (m_pWaveOutBlocks[i].dwFlags &WHDR_PREPARED)
		{
			waveOutUnprepareHeader(m_hWaveOut, &m_pWaveOutBlocks[i], sizeof(WAVEHDR));
		}
	}
	/*释放资源*/
	DeleteCriticalSection(&m_waveCriticalSection);
	freeBlocks(m_pWaveOutBlocks);
	waveOutClose(m_hWaveOut);
}

DWORD NSSound::InitSoundIn()
{
	if (m_bInWork)
	{
		return 0;
	}
	unsigned int pSndThreadId = 0;
	/*参数配置*/
	m_inputFormat.nSamplesPerSec = 8000;
	m_inputFormat.wBitsPerSample = 16;
	m_inputFormat.nChannels = 1;
	m_inputFormat.cbSize = 0;
	m_inputFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_inputFormat.nBlockAlign = (m_inputFormat.wBitsPerSample * m_inputFormat.nChannels) >> 3;
	m_inputFormat.nAvgBytesPerSec = m_inputFormat.nBlockAlign * m_inputFormat.nSamplesPerSec;
	/*检测音频输入设备是否正常*/
	if (waveInOpen(NULL, WAVE_MAPPER, &m_inputFormat, 0, 0, WAVE_FORMAT_QUERY) != MMSYSERR_NOERROR)
	{
		return 1;
	}
	m_pThreadIn = (HANDLE)_beginthreadex(
		NULL,
		0,
		SoundInThredProc,
		this,
		CREATE_SUSPENDED,
		&pSndThreadId
		);
	if (NULL == m_pThreadIn)
	{
		m_pLog->AddLog(Ns_Log_Error, "SoundInThredProc fail");
		return 1;
	}
	m_bInWork = true;
	setMicStatus(Mic_Idle);
	ResumeThread(m_pThreadIn);
	return 0;
}

UINT WINAPI NSSound::SoundInThredProc(LPVOID pParam)
{
	NSSound* p = (NSSound*)pParam;
	if (p)
	{
		p->SoundInThread();
	}
	return 0;
}

void NSSound::SoundInThread()
{
	m_pLog->AddLog(Ns_Log_Info, "SoundInThread Start");
	m_inThreadTimer = CreateEvent(NULL, FALSE, FALSE, NULL);
	while (m_bInWork)
	{
		mic_status_enum status = MicStatus();
		switch (status)
		{
		case Mic_Idle:
		case Mic_Ready_Work:
		case Mic_Work:
		case Mic_Wait_End:
		{
							 /*do nothing*/
		}
			break;
		case Mic_Start:
		{
						  setMicStatus(Mic_Ready_Work);
						  m_bufflag = 0;
						  m_bWaveInReset = false;
						  /*打开扬声器*/
						  if (waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_inputFormat, (DWORD_PTR)waveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
						  {
							  m_pLog->AddLog(Ns_Log_Error, "waveInOpen fail");
							  setMicStatus(Mic_Error);
							  goto SoundInThread_End;
						  }
						  for (int i = 0; i < BUFFER_NUM; i++)
						  {
							  m_whis[i].lpData = (LPSTR)m_cbBuffer[i];
							  m_whis[i].dwBufferLength = BUFFER_SIZE;
							  m_whis[i].dwBytesRecorded = 0;
							  m_whis[i].dwUser = 0;
							  m_whis[i].dwFlags = 0;
							  m_whis[i].dwLoops = 1;
							  m_whis[i].lpNext = NULL;
							  m_whis[i].reserved = 0;
						  }
						  //起始缓冲
						  waveInPrepareHeader(m_hWaveIn, &m_whis[m_bufflag], sizeof(WAVEHDR));
						  waveInAddBuffer(m_hWaveIn, &m_whis[m_bufflag], sizeof(WAVEHDR));
						  /*开始录音*/
						  if (MMSYSERR_NOERROR != waveInStart(m_hWaveIn))
						  {
							  m_pLog->AddLog(Ns_Log_Error, "waveInOpen fail");
							  setMicStatus(Mic_Error);
							  goto SoundInThread_End;
						  }
		}
			break;
		case Mic_Stop:
		{
						 setMicStatus(Mic_Wait_End);
						 if (m_hWaveIn)
						 {
							 m_bWaveInReset = true;
							 waveInStop(m_hWaveIn);
							 waveInReset(m_hWaveIn);
							 waveInClose(m_hWaveIn);
						 }
						 setMicStatus(Mic_Idle);
		}
			break;
		default:
			break;
		}
		WaitForSingleObject(m_inThreadTimer, INFINITE);

	}
SoundInThread_End:
	m_bInWork = false;
	m_inThreadTimer = NULL;
	m_pLog->AddLog(Ns_Log_Info, "SoundInThread End");
}

void CALLBACK NSSound::waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	NSSound* p = (NSSound*)dwInstance;
	if (p)
	{
		p->handleWaveInProc(hwi, uMsg, dwParam1, dwParam2);
	}
}

void NSSound::handleWaveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	bool bError = false;
	switch (uMsg)
	{
	case WIM_CLOSE:
	{
					  for (int i = 0; i < BUFFER_NUM; i++)
					  {
						  if (m_whis[i].dwFlags &WHDR_PREPARED)
						  {
							  waveInUnprepareHeader(m_hWaveIn, &m_whis[i], sizeof (WAVEHDR));
						  }
					  }
					  m_hWaveIn = NULL;
	}
		break;
	case WIM_DATA:
	{
					 if (!m_bWaveInReset)
					 {
						 WAVEHDR *pwhi = &m_whis[m_bufflag];
						 waveInUnprepareHeader(hwi, pwhi, sizeof(WAVEHDR));
						 pwhi = &m_whis[BUFFER_NUM - 1 - m_bufflag];
						 pwhi->dwFlags = 0;
						 pwhi->dwLoops = 0;
						 waveInPrepareHeader(hwi, pwhi, sizeof(WAVEHDR));
						 waveInAddBuffer(hwi, pwhi, sizeof(WAVEHDR));
						 DWORD dwWrite = 0;
						 /*处理音频数据*/
						 if (m_pAmbe)
						 {
							 int rlt = m_pAmbe->Pcm2Ambe((unsigned char*)m_cbBuffer[m_bufflag], BUFFER_SIZE);
							 bError = (0 != rlt);
						 }
						 else
						 {
							 m_pLog->AddLog(Ns_Log_Error, "m_pAmbe is null");
							 bError = true;
						 }
						 m_bufflag = (m_bufflag + 1) % BUFFER_NUM;
					 }
					 else
					 {

						 WAVEHDR *pwhi = (WAVEHDR*)dwParam1;
						 //m_pLog->AddLog("WIM_DATA pwhi->dwBytesRecorded:%lu", pwhi->dwBytesRecorded);
						 if (pwhi->dwBytesRecorded > 0)
						 {
							 /*处理音频数据*/
							 if (m_pAmbe)
							 {
								 int rlt = m_pAmbe->Pcm2Ambe((unsigned char*)pwhi->lpData, pwhi->dwBytesRecorded);
								 bError = (0 != rlt);
							 }
							 else
							 {
								 m_pLog->AddLog(Ns_Log_Error, "m_pAmbe is null");
								 bError = true;
							 }
						 }
					 }
	}
		break;
	case WIM_OPEN:
	{
					 setMicStatus(Mic_Work);
	}
		break;
	default:
		break;
	}
	if (bError)
	{
		NSWLNet* pNet = (NSWLNet*)g_pNSNet;
		pNet->CallStopUnnormal();
	}
}

void NSSound::setMicStatus(mic_status_enum value)
{
	if (m_micStatus != value)
	{
		m_pLog->AddLog(Ns_Log_Info, "====Mic Status From %d To %d====", m_micStatus, value);
		m_micStatus = value;
		ContinueInThread();
	}
}

void NSSound::DongleInfo(char* info)
{
	if (info)
	{
		if (m_pAmbe)
		{
			m_pAmbe->AboutInfo(info);
		}
		else
		{
			sprintf(info, "m_pAmbe is null");
		}
	}
}

mic_status_enum NSSound::MicStatus()
{
	return m_micStatus;
}

void NSSound::ContinueInThread()
{
	if (NULL != m_inThreadTimer)
	{
		SetEvent(m_inThreadTimer);
	}
}
