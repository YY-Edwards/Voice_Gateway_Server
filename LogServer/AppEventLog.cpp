#include "stdafx.h"
#include "../lib/rpc/include/TcpServer.h"
#include "AppEventLog.h"


CAppEventLog::CAppEventLog()
{
}


CAppEventLog::~CAppEventLog()
{
}

std::list<std::string> CAppEventLog::getArgNames()
{
	std::list<std::string> args;

	args.push_back("name");
	args.push_back("content");

	return args;
}

std::string CAppEventLog::getName()
{

	return "appEvent";
}

int CAppEventLog::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	pRemote->sendResponse("appevent received", strlen("appevent received"));

	return 0;
}