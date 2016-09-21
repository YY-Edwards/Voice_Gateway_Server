#pragma once
#include "D:\newDispatch\TrboX 3.0\lib\rpc\include\AbstractAction.h"
class CGroupCall :
	public CAbstractAction
{
public:
	CGroupCall();
	~CGroupCall();
public: // Derived from CAbstractAction
	virtual std::list<std::string> getArgNames();
	virtual std::string getName();
	virtual int run(CRemotePeer* pRemote, std::map<std::string, std::string> args);
};

