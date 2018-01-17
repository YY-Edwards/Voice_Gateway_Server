#ifndef CMANAGER_H
#define CMANAGER_H
#include "mutex.h"
#include "linklist.h"

class NSAmbe;
class NSDongle;
class NSLog;

class NSManager
{
public:
	NSManager();
	~NSManager();

	int Initialize();
	/*测试AMBE转PCM*/
	void test_ambe_2_pcm(FILE* pIn, FILE* pOut);
	/*测试PCM转AMBE*/
	void test_pcm_2_ambe(FILE* pIn, FILE* pOut);
	/*收到USB插拔的通知*/
	void OnUpdateUsb(DWORD operateType);
	/*获取空闲可用的dongle*/
	NSDongle* PopIdleDonglesItem();
	void AddIdleDonglesItem(NSDongle* p);
	void HandleAmbeData(void* pData, unsigned long length);
	int SizeDongle();
	int SizeIdleDongle();
private:
	//TIMECAPS m_theTimeCaps;
	LOCKERTYPE m_mutexDongles;//dongle池互斥锁
	pLinkList m_dongles;//dongle池
	LOCKERTYPE m_mutexIdleDongles;//空闲dongle池互斥锁
	pLinkList m_idleDongles;//空闲dongle池
	NSLog* m_pLog;
	//NSNetBase** m_ppNet;
	int m_sizeDongle;

	void setSizeDongle(int value);
	/*处理Usb设备增加事件*/
	void handleUsbAdd();
	/*处理Usb设备拔出事件*/
	void handleUsbDel();
	/*寻找一致的dongle*/
	static bool FuncFindDonglesItem(const void* dongle, const void* condition);
	NSDongle* FindDonglesItem(const void* condition, LinkMatchFunc fun);
	/*增加dongle*/
	void AddDonglesItem(NSDongle* p);
	/*清除所有dongle*/
	void clearDongles();
	void clearIdleDongles();
	/*运行ambe_2_pcm测试任务*/
	static unsigned int __stdcall test_ambe_2_pcmProc(void* pArguments);
	void handle_test_ambe_2_pcm(FILE* pIn,FILE* pOut);
	/*运行pcm_2_ambe测试任务*/
	static unsigned int __stdcall test_pcm_2_ambeProc(void* pArguments);
	void handle_test_pcm_2_ambe(FILE* pIn, FILE* pOut);
};

#endif