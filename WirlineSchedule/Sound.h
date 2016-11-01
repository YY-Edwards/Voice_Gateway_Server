#ifndef SOUND_H
#define SOUND_H


#include <dsound.h>
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")


#include <list>
#include <mutex>




/*播放音频采样配置*/
const int NUM_OUT_BUFFERS = 32;  //Software managed.
const int OUT_BYTES_PER_SAMPLE = 2;
const int OUT_SAMPLES_PER_20mS = 160;
const int OUT_BYTES_PER_20mS = OUT_SAMPLES_PER_20mS * OUT_BYTES_PER_SAMPLE;
const int OUT_SOUNDCARD_SAMPLES_PER_S = 8000;
const int OUT_BITS_PER_SAMPLE = 16;

/*录制音频采样配置*/
const int NUM_IN_BUFFERS = 32;
const int IN_BYTES_PER_SAMPLE = 2;
const int IN_SAMPLES_PER_20mS = 160;
const int IN_BYTES_PER_20mS = IN_SAMPLES_PER_20mS * IN_BYTES_PER_SAMPLE;
const int IN_SOUNDCARD_SAMPLES_PER_S = 8000;
const int IN_BITS_PER_SAMPLE = 16;


typedef union
{
	unsigned __int8 CharBuf[IN_BYTES_PER_20mS];
	__int16 ShortBuff[IN_SAMPLES_PER_20mS];
}SoundCardPCM;

typedef union
{
	unsigned __int8   CharBuf[OUT_BYTES_PER_20mS];
	__int16  ShortBuff[OUT_SAMPLES_PER_20mS];
}InPCM;

const double decimationcoeffs[33] = {
	0.006746753,
	0.001973817,
	-0.000347508,
	-0.00423211,
	-0.009203053,
	-0.01415441,
	-0.0174333,
	-0.017108305,
	-0.011389652,
	0.000886084,
	0.019828505,
	0.044247895,
	0.07168318,
	0.098731321,
	0.12162849,
	0.136963072,
	0.142358443,
	0.136963072,
	0.12162849,
	0.098731321,
	0.07168318,
	0.044247895,
	0.019828505,
	0.000886084,
	-0.011389652,
	-0.017108305,
	-0.0174333,
	-0.01415441,
	-0.009203053,
	-0.00423211,
	-0.000347508,
	0.001973817,
	0.006746753
};

class CSerialDongle;

extern CSerialDongle* g_pDongle;

class CSound
{
public:
	CSound();
	~CSound();
	void BigEndianSoundOut(unsigned __int8* pSamples, DWORD dwCurrentDecodeSize);//Dongle数据大小端你转换
	DWORD StartSound(HWND hParentWnd, int uInDeviceID, int uOutDeviceID);//sound启动
	void stop(void);//关闭音频输出、输入设备
	void SetLogPtr(PLogReport value);//设置log回调
	void StartRecord();
	void setbRecord(BOOL value);
	FILE* m_pInputFile;
	//FILE* m_pInputFile1;
	void closeFile();
	void immediatelyPlay();
	bool getbRecord();

private:
	BOOL m_bShouldStopRecording;//当前是否应该停止录制
	BOOL m_bRecording;
	bool m_ThreadRunning;//sound thread运行标识
	HWND m_hMyParentWind;//前台窗口句柄
	int m_OutputDevice; //Device Index requested by ergo.
	HANDLE m_pSndThread;//sound thread句柄
	UINT m_pSndThreadId;//sound thread ID

	HANDLE m_pIptThread;//sound thread句柄
	UINT m_pIptThreadId;//sound thread ID


	/************************************************************************/
	/*实时播放中继台音频数据
	/************************************************************************/
	DSBUFFERDESC m_dscOutputbd;  //Output DS Buffer Discriptor.
	WAVEFORMATEX m_OutWFX;       //Input Wave Format.
	LPDIRECTSOUND8 m_lpOutDS;      //Pointer to Output DS Object.
	LPDIRECTSOUNDBUFFER m_lpOutputDSB1; //Pointer to DS managed Output Buffer.
	LPDIRECTSOUNDBUFFER8 m_lpOutputDSB2; //Pointer to DS managed Output Buffer.
	IDirectSoundNotify8* m_pOutDSNotify;//设备Notify
	DSBPOSITIONNOTIFY m_pOutDSPosNotify[NUM_OUT_BUFFERS];//缓冲位置Notify
	HANDLE m_pOutDSPosNotifyEvents[NUM_OUT_BUFFERS];//event
	BOOL m_bOutPcmStart;//当前是否为开始播放pcm数据
	DWORD m_dOutOffsset;//当前在播放buffer中的偏移量
	BOOL m_bStopPlay;//是否播放的标识
	HANDLE m_hPlaySongEvent;//播放event
	DWORD m_dPreBufCusor;//当前音频数据源填充位置
	std::list<OUTDATA *> m_playList;//待填充的音频数据源
	std::mutex m_PlayTaskLocker;//待填充的音频数据源锁
	void sendLogToWindow();//将log显示至前台窗口
	char m_reportMsg[512];//log内容
	PLogReport m_report;//前台log回调函数指针
	DWORD m_dwCurPlayZone;//当前播放buffer中的播放区域
	DWORD m_dwEndPlayZone;//当前播放buffer中应该停止的区域
	BOOL m_shouldStopPlay;//是否应该停止播放
	OUTDATA* m_pOutCurrentData;//当前正在填充的数据源
	OUTDATA* m_pOutPrevData;//前一个填充的数据源
	BOOL getOutNextData();//从数据源list中获取数据源
	void freeOutData(OUTDATA* pData);//释放内存
	DWORD OpenOutput(void);//打开音频输出设备，进入带播放状态
	void CloseOutput(void);//关闭音频输出设备
	static UINT WINAPI SndThreadProc(LPVOID pParam);	//must be static since is thread
	void SoundThread();//当前类主运行线程
	void HandleSoundOutput(void);//控制音频播放和数据源填充
	void SoundOutputControl();

	/************************************************************************/
	/*实时发送麦克风音频数据到中转台
	/************************************************************************/
	LPDIRECTSOUNDCAPTURE8 m_pInputDevice;
	LPDIRECTSOUNDCAPTUREBUFFER m_pInputBuffer1;
	LPDIRECTSOUNDCAPTUREBUFFER8 m_pInputBuffer2;
	DSCBUFFERDESC m_inputDesc;
	LPDIRECTSOUNDNOTIFY m_pInputNotify;
	DSBPOSITIONNOTIFY m_pInputPosNotify[NUM_IN_BUFFERS];
	HANDLE m_pInputPosNotifyEvents[NUM_IN_BUFFERS];//event
	WAVEFORMATEX m_inputFormat;
	DWORD OpenInput(void);//打开音频输入设备，进入待录音状态
	void CloseInput(void);//关闭音频输入设备
	void HandleSoundInput(void);//控制音频播放和数据源填充
	
	DWORD m_dwInputOffset;
	HANDLE m_hRecordEvent;//录音event
	HANDLE m_hRecordDealEvent;
	void SoundInputControl();
	
	
	BOOL m_bStartRecording;
	//int m_dwShouldStopRecordOffset;
	DWORD m_dwCurRecordZone;//当前播放buffer中的录制区域
	DWORD m_dwEndRecordZone;//当前播放buffer中应该停止的区域

	BOOL m_bIsDealing;//是否在处理数据

	std::list<SoundCardPCM*> m_inputDataList;
	std::mutex m_inputDataLocker;
	InPCM m_ScratchPCM;
	SoundCardPCM* m_pInputPrevData;
	SoundCardPCM* m_pInputCurData;

	HANDLE m_hExitEvent;

	HANDLE m_hSoundControlEvents[3];
};


#endif