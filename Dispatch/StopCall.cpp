#include "stdafx.h"
#include "StopCall.h"


CStopCall::CStopCall()
{
}


CStopCall::~CStopCall()
{
}
std::list<std::string> CStopCall::getArgNames()
{
	std::list<std::string> args;

	args.push_back("name");
	args.push_back("content");

	return args;
}
std::string CStopCall::getName()
{

	return "stopCall";
}

int CStopCall::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	return 0;
}
