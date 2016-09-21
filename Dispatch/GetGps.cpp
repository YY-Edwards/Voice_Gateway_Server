#include "stdafx.h"
#include "GetGps.h"


CGetGps::CGetGps()
{
}


CGetGps::~CGetGps()
{
}
std::string CGetGps::getName()
{

	return "getGps";
}

int CGetGps::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}