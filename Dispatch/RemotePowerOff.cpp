#include "stdafx.h"
#include "RemotePowerOff.h"


CRemotePowerOff::CRemotePowerOff()
{
}


CRemotePowerOff::~CRemotePowerOff()
{
}
std::string CRemotePowerOff::getName()
{

	return "remotePowerOff";
}

int CRemotePowerOff::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}