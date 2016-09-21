#pragma once
#include "D:\newDispatch\TrboX 3.0\lib\rpc\include\AbstractAction.h"
class CGetOverTurnGPS :
	public CAbstractAction
{
public:
	CGetOverTurnGPS();
	~CGetOverTurnGPS();
public: // Derived from CAbstractAction
	virtual std::list<std::string> getArgNames();
	virtual std::string getName();
	virtual int run(CRemotePeer* pRemote, std::map<std::string, std::string> args);
};

