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
	/*ambe数据构建帧*/
	void WriteAmbe(void* src, int dataLen, pOnData fun, void* param);
	/*pcm数据构建帧*/
	void WritePcm(void* src, int dataLen, pOnData fun, void* param);
	/*返回dongle的名字*/
	char* Name();
	void StatusInfo(char* msg);
	void ReadyUse();
private:
	dongle_t m_self;//dongle信息
	HANDLE m_hReadSerialEvent;//读事件
	HANDLE m_hWriteSerialEvent;//写事件
	OVERLAPPED m_osReader;//读OVERLAPPED
	OVERLAPPED m_osWriter;//写OVERLAPPED
	unsigned __int8 m_DongleRxBuffer[INTERNALCOMBUFFSIZE];//读数据buffer
	int m_dongleReadFrame;//读取数据长度
	int m_dongleWriteFrame;//写入数据长度
	PARSERSTATE m_ParserState;//解析状态
	int m_RxMsgLength;
	int m_RxMsgIndex;
	HANDLE m_pSerialRxThread;//读线程句柄
	HANDLE m_pSerialTxThread;//写线程句柄
	bool m_bRun;//控制当前读写线程的运行
	LOCKERTYPE m_mutexRing;
	pRingBuffer m_ringBuffer;
	//CAmbe* m_parent;
	change_data_t* m_pCurHanleRing;
	unsigned long m_idleTimeTickCount;
	bool m_bIdle;
	change_data_t* m_pChangeDataPcmPools;//数据容器池
	change_data_t* m_pChangeDataAmbePools;//数据容器池
	tPCMFrame* m_pPcmPools;
	tAMBEFrame* m_pAmbePools;
	int m_sizePool;//数据容器池大小
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
	/*清除串口读写缓存*/
	bool purgeCommDongle(dongle_t* p);
	/*打开dongle*/
	bool openDongle(dongle_t* p);
	/*初始化dongle*/
	unsigned long setupDongle(dongle_t* p);
	/*清除串口错误信息和读写缓存*/
	unsigned long purgeDongle(dongle_t* p, unsigned long flags);
	/*停止dongle释放资源*/
	void stopDongle(dongle_t* p);
	/*写线程*/
	static unsigned int __stdcall SerialTxThreadProc(void* pArguments);
	/*读线程*/
	static unsigned int __stdcall SerialRxThreadProc(void* pArguments);
	/*执行读线程*/
	void SerialTxThread();
	/*执行写线程*/
	void SerialRxThread();
	/*获取AMBE空闲帧*/
	tAMBEFrame* getFreeAmbeBuffer(void);
	/*获取changedata空闲帧*/
	change_data_t* getFreeAmbeChangeDataBuffer(pOnData fun, void* param);
	/*获取changedata空闲帧*/
	change_data_t* getFreePcmChangeDataBuffer(pOnData fun, void* param);
	/*构建Ambe帧*/
	void deObfuscate(ScrambleDirection theDirection, tAMBEFrame* &pAMBEFrame);
	/*构建Ambe帧并填充*/
	void markAmbeBufferFilled(tAMBEFrame* &pFrame);
	/*Ambe帧checksum*/
	unsigned __int8 CheckSum(DVSI3000struct* pMsg);
	/*处理从dongle出来的初始pcm数据*/
	int assembleMsg(unsigned long numBytes, unsigned long * dwBytesAssembled);
	/*处理pcm数据*/
	void parseDVSImsg(DVSI3000struct* pMsg);
	/*初始化串口读*/
	void initRead();
	/*初始化串口写*/
	void initWrite();
	/*pcm数据大小端处理*/
	void handleOutPcm(unsigned __int8* pSamples);
	/*ambe数据处理*/
	void handleOutAmbe(unsigned __int8* pSamples);
	/*清除环缓冲*/
	void clearRingBuffer();
	/*清除当前操作缓冲*/
	void clearCurHandleRing();
	/*设置dongle使用状态*/
	void setUsing(bool value);
	/*判定初始化写*/
	void handleInitWrite();
	/*判定当前Dongle是否空闲下来*/
	void handleEndWork();
	/*增加环缓冲项*/
	bool addRingItem(change_data_t* p);
	/*环缓冲项数量*/
	int sizeRing();
	/*获取并删除环缓冲项*/
	void popRingItem(change_data_t* &item);
};

#endif // !CDONGLE_H
