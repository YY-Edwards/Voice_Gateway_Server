#include "stdafx.h"
#include "AllCall.h"


CAllCall::CAllCall()
{
}


CAllCall::~CAllCall()
{
}
std::string CAllCall::getName()
{

	return "allCall";
}

int CAllCall::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}
