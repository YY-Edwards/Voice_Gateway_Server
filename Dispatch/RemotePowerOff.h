#pragma once
#include "D:\newDispatch\TrboX 3.0\lib\rpc\include\AbstractAction.h"
class CRemotePowerOff :
	public CAbstractAction
{
public:
	CRemotePowerOff();
	~CRemotePowerOff();
public: // Derived from CAbstractAction
	virtual std::list<std::string> getArgNames();
	virtual std::string getName();
	virtual int run(CRemotePeer* pRemote, const std::string& param, uint64_t callId);
};

