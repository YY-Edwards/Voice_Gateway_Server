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
	/*����AMBEתPCM*/
	void test_ambe_2_pcm(FILE* pIn, FILE* pOut);
	/*����PCMתAMBE*/
	void test_pcm_2_ambe(FILE* pIn, FILE* pOut);
	/*�յ�USB��ε�֪ͨ*/
	void OnUpdateUsb(DWORD operateType);
	/*��ȡ���п��õ�dongle*/
	NSDongle* PopIdleDonglesItem();
	void AddIdleDonglesItem(NSDongle* p);
	void HandleAmbeData(void* pData, unsigned long length);
	int SizeDongle();
	int SizeIdleDongle();
private:
	//TIMECAPS m_theTimeCaps;
	LOCKERTYPE m_mutexDongles;//dongle�ػ�����
	pLinkList m_dongles;//dongle��
	LOCKERTYPE m_mutexIdleDongles;//����dongle�ػ�����
	pLinkList m_idleDongles;//����dongle��
	NSLog* m_pLog;
	//NSNetBase** m_ppNet;
	int m_sizeDongle;

	void setSizeDongle(int value);
	/*����Usb�豸�����¼�*/
	void handleUsbAdd();
	/*����Usb�豸�γ��¼�*/
	void handleUsbDel();
	/*Ѱ��һ�µ�dongle*/
	static bool FuncFindDonglesItem(const void* dongle, const void* condition);
	NSDongle* FindDonglesItem(const void* condition, LinkMatchFunc fun);
	/*����dongle*/
	void AddDonglesItem(NSDongle* p);
	/*�������dongle*/
	void clearDongles();
	void clearIdleDongles();
	/*����ambe_2_pcm��������*/
	static unsigned int __stdcall test_ambe_2_pcmProc(void* pArguments);
	void handle_test_ambe_2_pcm(FILE* pIn,FILE* pOut);
	/*����pcm_2_ambe��������*/
	static unsigned int __stdcall test_pcm_2_ambeProc(void* pArguments);
	void handle_test_pcm_2_ambe(FILE* pIn, FILE* pOut);
};

#endif