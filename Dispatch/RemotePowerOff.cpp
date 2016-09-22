#include "stdafx.h"
#include "RemotePowerOff.h"


CRemotePowerOff::CRemotePowerOff()
{
}


CRemotePowerOff::~CRemotePowerOff()
{
}
std::list<std::string> CRemotePowerOff::getArgNames()
{
	std::list<std::string> args;

	args.push_back("name");
	args.push_back("content");

	return args;
}
std::string CRemotePowerOff::getName()
{

	return "remotePowerOff";
}

int CRemotePowerOff::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	return 0;
}