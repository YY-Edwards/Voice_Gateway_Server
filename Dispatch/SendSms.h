#pragma once
#include "D:\newDispatch\TrboX 3.0\lib\rpc\include\AbstractAction.h"
class CSendSms :
	public CAbstractAction
{
public:
	CSendSms();
	~CSendSms();
public: // Derived from CAbstractAction
	virtual std::string getName();
	virtual int run(CBaseConnector* pConnector, std::map<std::string, std::string> args);
};

