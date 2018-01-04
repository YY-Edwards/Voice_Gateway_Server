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

	/*����AMBEתPCM����PCM�ļ�д��ָ���ļ�*/
	void Ambe2Pcm(void* pBuffer, unsigned long length);
	/*����PCMתAMBE����AMBE�ļ�д��ָ���ļ�*/
	void Pcm2Ambe(void* pBuffer, unsigned long length);
	/*PCM���ݻص�*/
	void OnDataPcm(void* pData, unsigned long length, unsigned long index);
	/*AMBE���ݻص�*/
	void OnDataAmbe(void* pData, unsigned long length, unsigned long index);
	///*��⵽USB�豸���*/
	//void OnUpdateUsb(DWORD operateType);
	/*PCM���ݻص�*/
	static void OnDataPcmFun(void* pData, unsigned long length, unsigned long index, void* param);
	/*AMBE���ݻص�*/
	static void OnDataAmbeFun(void* pData, unsigned long length, unsigned long index, void* param);
	/*��ʾд�����*/
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