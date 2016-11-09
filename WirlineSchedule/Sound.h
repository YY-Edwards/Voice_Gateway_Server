#ifndef SOUND_H
#define SOUND_H


#include <dsound.h>
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")


#include <list>
#include <mutex>




/*������Ƶ��������*/
const int NUM_OUT_BUFFERS = 32;  //Software managed.
const int OUT_BYTES_PER_SAMPLE = 2;
const int OUT_SAMPLES_PER_20mS = 160;
const int OUT_BYTES_PER_20mS = OUT_SAMPLES_PER_20mS * OUT_BYTES_PER_SAMPLE;
const int OUT_SOUNDCARD_SAMPLES_PER_S = 8000;
const int OUT_BITS_PER_SAMPLE = 16;

/*¼����Ƶ��������*/
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
	void BigEndianSoundOut(unsigned __int8* pSamples, DWORD dwCurrentDecodeSize);//Dongle���ݴ�С����ת��
	DWORD StartSound(HWND hParentWnd, int uInDeviceID, int uOutDeviceID);//sound����
	void stop(void);//�ر���Ƶ����������豸
	void SetLogPtr(PLogReport value);//����log�ص�
	void StartRecord();
	void setbRecord(BOOL value);
	FILE* m_pInputFile;
	//FILE* m_pInputFile1;
	void closeFile();
	void immediatelyPlay();
	bool getbRecord();

private:
	BOOL m_bShouldStopRecording;//��ǰ�Ƿ�Ӧ��ֹͣ¼��
	BOOL m_bRecording;
	bool m_ThreadRunning;//sound thread���б�ʶ
	HWND m_hMyParentWind;//ǰ̨���ھ��
	int m_OutputDevice; //Device Index requested by ergo.
	HANDLE m_pSndThread;//sound thread���
	UINT m_pSndThreadId;//sound thread ID

	HANDLE m_pIptThread;//sound thread���
	UINT m_pIptThreadId;//sound thread ID


	/************************************************************************/
	/*ʵʱ�����м�̨��Ƶ����
	/************************************************************************/
	DSBUFFERDESC m_dscOutputbd;  //Output DS Buffer Discriptor.
	WAVEFORMATEX m_OutWFX;       //Input Wave Format.
	LPDIRECTSOUND8 m_lpOutDS;      //Pointer to Output DS Object.
	LPDIRECTSOUNDBUFFER m_lpOutputDSB1; //Pointer to DS managed Output Buffer.
	LPDIRECTSOUNDBUFFER8 m_lpOutputDSB2; //Pointer to DS managed Output Buffer.
	IDirectSoundNotify8* m_pOutDSNotify;//�豸Notify
	DSBPOSITIONNOTIFY m_pOutDSPosNotify[NUM_OUT_BUFFERS];//����λ��Notify
	HANDLE m_pOutDSPosNotifyEvents[NUM_OUT_BUFFERS];//event
	BOOL m_bOutPcmStart;//��ǰ�Ƿ�Ϊ��ʼ����pcm����
	DWORD m_dOutOffsset;//��ǰ�ڲ���buffer�е�ƫ����
	BOOL m_bStopPlay;//�Ƿ񲥷ŵı�ʶ
	HANDLE m_hPlaySongEvent;//����event
	DWORD m_dPreBufCusor;//��ǰ��Ƶ����Դ���λ��
	std::list<OUTDATA *> m_playList;//��������Ƶ����Դ
	std::mutex m_PlayTaskLocker;//��������Ƶ����Դ��
	void sendLogToWindow();//��log��ʾ��ǰ̨����
	char m_reportMsg[512];//log����
	PLogReport m_report;//ǰ̨log�ص�����ָ��
	DWORD m_dwCurPlayZone;//��ǰ����buffer�еĲ�������
	DWORD m_dwEndPlayZone;//��ǰ����buffer��Ӧ��ֹͣ������
	BOOL m_shouldStopPlay;//�Ƿ�Ӧ��ֹͣ����
	OUTDATA* m_pOutCurrentData;//��ǰ������������Դ
	OUTDATA* m_pOutPrevData;//ǰһ����������Դ
	BOOL getOutNextData();//������Դlist�л�ȡ����Դ
	void freeOutData(OUTDATA* pData);//�ͷ��ڴ�
	DWORD OpenOutput(void);//����Ƶ����豸�����������״̬
	void CloseOutput(void);//�ر���Ƶ����豸
	static UINT WINAPI SndThreadProc(LPVOID pParam);	//must be static since is thread
	void SoundThread();//��ǰ���������߳�
	void HandleSoundOutput(void);//������Ƶ���ź�����Դ���
	void SoundOutputControl();

	/************************************************************************/
	/*ʵʱ������˷���Ƶ���ݵ���ת̨
	/************************************************************************/
	LPDIRECTSOUNDCAPTURE8 m_pInputDevice;
	LPDIRECTSOUNDCAPTUREBUFFER m_pInputBuffer1;
	LPDIRECTSOUNDCAPTUREBUFFER8 m_pInputBuffer2;
	DSCBUFFERDESC m_inputDesc;
	LPDIRECTSOUNDNOTIFY m_pInputNotify;
	DSBPOSITIONNOTIFY m_pInputPosNotify[NUM_IN_BUFFERS];
	HANDLE m_pInputPosNotifyEvents[NUM_IN_BUFFERS];//event
	WAVEFORMATEX m_inputFormat;
	DWORD OpenInput(void);//����Ƶ�����豸�������¼��״̬
	void CloseInput(void);//�ر���Ƶ�����豸
	void HandleSoundInput(void);//������Ƶ���ź�����Դ���
	
	DWORD m_dwInputOffset;
	HANDLE m_hRecordEvent;//¼��event
	HANDLE m_hRecordDealEvent;
	void SoundInputControl();
	
	
	BOOL m_bStartRecording;
	//int m_dwShouldStopRecordOffset;
	DWORD m_dwCurRecordZone;//��ǰ����buffer�е�¼������
	DWORD m_dwEndRecordZone;//��ǰ����buffer��Ӧ��ֹͣ������

	BOOL m_bIsDealing;//�Ƿ��ڴ�������

	std::list<SoundCardPCM*> m_inputDataList;
	std::mutex m_inputDataLocker;
	InPCM m_ScratchPCM;
	SoundCardPCM* m_pInputPrevData;
	SoundCardPCM* m_pInputCurData;

	HANDLE m_hExitEvent;

	HANDLE m_hSoundControlEvents[3];
};


#endif