#pragma once
#include "D:\newDispatch\TrboX 3.0\lib\rpc\include\AbstractAction.h"
class CRadioCheck :
	public CAbstractAction
{
public:
	CRadioCheck();
	~CRadioCheck();
public: // Derived from CAbstractAction
	virtual std::string getName();
	virtual int run(CBaseConnector* pConnector, std::map<std::string, std::string> args);
};

