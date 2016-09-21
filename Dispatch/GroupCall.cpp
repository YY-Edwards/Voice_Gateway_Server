#include "stdafx.h"
#include "GroupCall.h"


CGroupCall::CGroupCall()
{
}


CGroupCall::~CGroupCall()
{
}
std::string CGroupCall::getName()
{

	return "groupCall";
}

int CGroupCall::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}