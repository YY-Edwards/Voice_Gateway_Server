#include "stdafx.h"
#include "../lib/rpc/include/TcpServer.h"
#include "AppEventLog.h"


CAppEventLog::CAppEventLog()
{
}


CAppEventLog::~CAppEventLog()
{
}

std::string CAppEventLog::getName()
{
	return "appEvent";
}

int CAppEventLog::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}