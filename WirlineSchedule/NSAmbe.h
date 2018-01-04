#ifndef CAMBE_H
#define CAMBE_H

#include "mutex.h"
#include "linklist.h"

class NSManager;
class NSDongle;
class NSLog;

class NSAmbe
{
public:
	NSAmbe(NSManager* pManager);
	~NSAmbe();

	/*测试AMBE转PCM并把PCM文件写入指定文件*/
	void Ambe2Pcm(void* pBuffer, unsigned long length);
	/*测试PCM转AMBE并把AMBE文件写入指定文件*/
	void Pcm2Ambe(void* pBuffer, unsigned long length);
	/*PCM数据回调*/
	void OnDataPcm(void* pData, unsigned long length, unsigned long index);
	/*AMBE数据回调*/
	void OnDataAmbe(void* pData, unsigned long length, unsigned long index);
	///*监测到USB设备插拔*/
	//void OnUpdateUsb(DWORD operateType);
	/*PCM数据回调*/
	static void OnDataPcmFun(void* pData, unsigned long length, unsigned long index, void* param);
	/*AMBE数据回调*/
	static void OnDataAmbeFun(void* pData, unsigned long length, unsigned long index, void* param);
	/*表示写入完毕*/
	void WirteEnd();
private:
	NSManager* m_pManager;
	bool m_bWriteEnd;
	bool m_bReciveEnd;
	NSDongle* m_useDongle;
	NSLog* m_pLog;

	void canDeleteSelf();

#if _DEBUG
	FILE* m_outFile;
#endif
};

#endif