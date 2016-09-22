#include "stdafx.h"
#include "RmotePowerON.h"


CRmotePowerON::CRmotePowerON()
{
}


CRmotePowerON::~CRmotePowerON()
{
}
std::list<std::string> CRmotePowerON::getArgNames()
{
	std::list<std::string> args;

	args.push_back("name");
	args.push_back("content");

	return args;
}
std::string CRmotePowerON::getName()
{

	return "remotePowerOn";
}

int CRmotePowerON::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	return 0;
}