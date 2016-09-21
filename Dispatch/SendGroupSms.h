#pragma once
#include "D:\newDispatch\TrboX 3.0\lib\rpc\include\AbstractAction.h"
class CSendGroupSms :
	public CAbstractAction
{
public:
	CSendGroupSms();
	~CSendGroupSms();
public: // Derived from CAbstractAction
	virtual std::list<std::string> getArgNames();
	virtual std::string getName();
	virtual int run(CRemotePeer* pRemote, std::map<std::string, std::string> args);
};

