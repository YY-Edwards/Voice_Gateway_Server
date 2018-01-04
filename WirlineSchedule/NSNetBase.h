#ifndef NETBASE_H
#define NETBASE_H
#include "NSStruct.h"

class NSNetBase
{
public:
	virtual int StartNet(StartNetParam* p) = 0;
	virtual void GetSerial(char* &pSerial, int &length) = 0;
	virtual int sendNetDataBase(const char* pData, int len, void* send_to) = 0;
	virtual void HandleAmbeData(void* pData, unsigned long length)=0;
private:
};

#endif