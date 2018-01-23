#ifndef CDONGLE_H
#define CDONGLE_H

#include "ringbuffer.h"
#include "mutex.h"
#include "NSStruct.h"
#include "Ambe3000.h"

const int IPSCTODONGLETABLE[49] =
//0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12. 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48
{ 0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 41, 43, 45, 47, 1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 42, 44, 46, 48, 2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35, 38 };

const int DONGLETOIPSCTABLE[49] =
//0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12. 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48
{ 0, 18, 36, 1, 19, 37, 2, 20, 38, 3, 21, 39, 4, 22, 40, 5, 23, 41, 6, 24, 42, 7, 25, 43, 8, 26, 44, 9, 27, 45, 10, 28, 46, 11, 29, 47, 12, 30, 48, 13, 31, 14, 32, 15, 33, 16, 34, 17, 35 };

class NSAmbe;
class NSManager;
class NSLog;

class NSDongle
{
public:
	NSDongle(NSManager* pManager);
	~NSDongle();

	int Initialize(dongle_t* p);
	bool IsIdle();
	/*ambe���ݹ���֡*/
	void WriteAmbe(void* src, int dataLen, pOnData fun, void* param);
	/*pcm���ݹ���֡*/
	void WritePcm(void* src, int dataLen, pOnData fun, void* param);
	/*����dongle������*/
	char* Name();
	void StatusInfo(char* msg);
	void ReadyUse();
private:
	dongle_t m_self;//dongle��Ϣ
	HANDLE m_hReadSerialEvent;//���¼�
	HANDLE m_hWriteSerialEvent;//д�¼�
	OVERLAPPED m_osReader;//��OVERLAPPED
	OVERLAPPED m_osWriter;//дOVERLAPPED
	unsigned __int8 m_DongleRxBuffer[INTERNALCOMBUFFSIZE];//������buffer
	int m_dongleReadFrame;//��ȡ���ݳ���
	int m_dongleWriteFrame;//д�����ݳ���
	PARSERSTATE m_ParserState;//����״̬
	int m_RxMsgLength;
	int m_RxMsgIndex;
	HANDLE m_pSerialRxThread;//���߳̾��
	HANDLE m_pSerialTxThread;//д�߳̾��
	bool m_bRun;//���Ƶ�ǰ��д�̵߳�����
	LOCKERTYPE m_mutexRing;
	pRingBuffer m_ringBuffer;
	//CAmbe* m_parent;
	change_data_t* m_pCurHanleRing;
	unsigned long m_idleTimeTickCount;
	bool m_bIdle;
	change_data_t* m_pChangeDataPcmPools;//����������
	change_data_t* m_pChangeDataAmbePools;//����������
	tPCMFrame* m_pPcmPools;
	tAMBEFrame* m_pAmbePools;
	int m_sizePool;//���������ش�С
	int m_idxIdle;
	out_data_pcm_t m_outPcmData;
	NSManager* m_pManager;
	NSLog* m_pLog;
	unsigned long m_prevTime;
	unsigned long m_frameCount;
	unsigned long m_totalTime;
	unsigned long m_sendCount;
	double m_curAvg;

	void alive();
	/*������ڶ�д����*/
	bool purgeCommDongle(dongle_t* p);
	/*��dongle*/
	bool openDongle(dongle_t* p);
	/*��ʼ��dongle*/
	unsigned long setupDongle(dongle_t* p);
	/*������ڴ�����Ϣ�Ͷ�д����*/
	unsigned long purgeDongle(dongle_t* p, unsigned long flags);
	/*ֹͣdongle�ͷ���Դ*/
	void stopDongle(dongle_t* p);
	/*д�߳�*/
	static unsigned int __stdcall SerialTxThreadProc(void* pArguments);
	/*���߳�*/
	static unsigned int __stdcall SerialRxThreadProc(void* pArguments);
	/*ִ�ж��߳�*/
	void SerialTxThread();
	/*ִ��д�߳�*/
	void SerialRxThread();
	/*��ȡAMBE����֡*/
	tAMBEFrame* getFreeAmbeBuffer(void);
	/*��ȡchangedata����֡*/
	change_data_t* getFreeAmbeChangeDataBuffer(pOnData fun, void* param);
	/*��ȡchangedata����֡*/
	change_data_t* getFreePcmChangeDataBuffer(pOnData fun, void* param);
	/*����Ambe֡*/
	void deObfuscate(ScrambleDirection theDirection, tAMBEFrame* &pAMBEFrame);
	/*����Ambe֡�����*/
	void markAmbeBufferFilled(tAMBEFrame* &pFrame);
	/*Ambe֡checksum*/
	unsigned __int8 CheckSum(DVSI3000struct* pMsg);
	/*�����dongle�����ĳ�ʼpcm����*/
	int assembleMsg(unsigned long numBytes, unsigned long * dwBytesAssembled);
	/*����pcm����*/
	void parseDVSImsg(DVSI3000struct* pMsg);
	/*��ʼ�����ڶ�*/
	void initRead();
	/*��ʼ������д*/
	void initWrite();
	/*pcm���ݴ�С�˴���*/
	void handleOutPcm(unsigned __int8* pSamples);
	/*ambe���ݴ���*/
	void handleOutAmbe(unsigned __int8* pSamples);
	/*���������*/
	void clearRingBuffer();
	/*�����ǰ��������*/
	void clearCurHandleRing();
	/*����dongleʹ��״̬*/
	void setUsing(bool value);
	/*�ж���ʼ��д*/
	void handleInitWrite();
	/*�ж���ǰDongle�Ƿ��������*/
	void handleEndWork();
	/*���ӻ�������*/
	bool addRingItem(change_data_t* p);
	/*������������*/
	int sizeRing();
	/*��ȡ��ɾ����������*/
	void popRingItem(change_data_t* &item);
};

#endif // !CDONGLE_H
