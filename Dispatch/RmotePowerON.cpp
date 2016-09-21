#include "stdafx.h"
#include "RmotePowerON.h"


CRmotePowerON::CRmotePowerON()
{
}


CRmotePowerON::~CRmotePowerON()
{
}
std::string CRmotePowerON::getName()
{

	return "remotePowerOn";
}

int CRmotePowerON::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}