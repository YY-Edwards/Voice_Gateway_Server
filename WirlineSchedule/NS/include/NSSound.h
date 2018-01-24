#ifndef NSSOUND_H
#define NSSOUND_H

#include "mutex.h"
#include "ringbuffer.h"
#include "NSStruct.h"

#define BLOCK_SIZE 5120
#define BLOCK_COUNT 20

#define BUFFER_NUM 2
#define BUFFER_SIZE 320

class NSLog;
class NSAmbe;

class NSSound
{
public:
	NSSound();
	~NSSound();

	/*初始化扬声器*/
	DWORD InitSoundOut();
	/*初始化麦克风*/
	DWORD InitSoundIn();
	/*写入需要播放的PCM数据*/
	void WritePcm(const char* pData, int length);
	mic_status_enum MicStatus();
	void setMicStatus(mic_status_enum value);
	void DongleInfo(char* info);
private:
	bool m_bOutWork;
	bool m_bInWork;
	WAVEFORMATEX m_outFormat;       //Input Wave Format.
	WAVEFORMATEX m_inputFormat;
	NSLog* m_pLog;
	HANDLE m_pThreadOut;
	HANDLE m_pThreadIn;
	LOCKERTYPE m_mutexOut;
	pRingBuffer m_ringOut;
	WAVEHDR* m_pWaveOutBlocks;
	CRITICAL_SECTION m_waveCriticalSection;
	volatile int m_waveOutFreeBlockCount;
	int m_waveOutCurrentBlock;
	HWAVEOUT m_hWaveOut;
	WAVEHDR m_whis[BUFFER_NUM];
	HWAVEIN m_hWaveIn;//音频输入句柄
	NSAmbe* m_pAmbe;
	char m_cbBuffer[BUFFER_NUM][BUFFER_SIZE];    //声音临时缓存
	bool m_bWaveInReset;
	DWORD m_bufflag;
	mic_status_enum m_micStatus;
	HANDLE m_inThreadTimer;

	static UINT WINAPI SoundOutThredProc(LPVOID pParam);
	void SoundOutThread();
	static UINT WINAPI SoundInThredProc(LPVOID pParam);
	void SoundInThread();
	void clearRingOut();
	int sizeRingOut();
	int popRingOutItem(out_data_pcm_t* item);
	void addRingOutItem(out_data_pcm_t* item);
	WAVEHDR* allocateBlocks(int size, int count);
	void freeBlocks(WAVEHDR* blockArray);
	void writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size);
	static void CALLBACK waveOutProc(
		HWAVEOUT  hwo,
		UINT      uMsg,
		DWORD_PTR dwInstance,
		DWORD_PTR dwParam1,
		DWORD_PTR dwParam2
		);
	void handleWaveOutProc(UINT uMsg);
	void temp(char* pdata, int length);
	static void CALLBACK waveInProc(
		HWAVEIN   hwi,
		UINT      uMsg,
		DWORD_PTR dwInstance,
		DWORD_PTR dwParam1,
		DWORD_PTR dwParam2
		);
	void handleWaveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
	void ContinueInThread();
};

#endif