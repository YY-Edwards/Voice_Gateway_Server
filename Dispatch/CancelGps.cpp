#include "stdafx.h"
#include "CancelGps.h"


CCancelGps::CCancelGps()
{
}


CCancelGps::~CCancelGps()
{
}
std::string CCancelGps::getName()
{

	return "cancelGps";
}

int CCancelGps::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}
