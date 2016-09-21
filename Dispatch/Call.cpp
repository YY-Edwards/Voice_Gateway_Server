#include "stdafx.h"
#include "Call.h"


CCall::CCall()
{
}


CCall::~CCall()
{
}
std::string CCall::getName()
{

	return "call";
}

int CCall::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}