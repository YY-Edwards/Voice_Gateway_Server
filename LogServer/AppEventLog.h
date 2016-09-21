#pragma once
#include "..\lib\rpc\include\AbstractAction.h"
class CAppEventLog :
	public CAbstractAction
{
public:
	CAppEventLog();
	~CAppEventLog();

public: // Derived from CAbstractAction
	virtual std::string getName();
	virtual int run(CRemotePeer* pRemote, std::map<std::string, std::string> args);
};

