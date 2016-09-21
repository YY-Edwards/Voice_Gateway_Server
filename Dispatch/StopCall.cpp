#include "stdafx.h"
#include "StopCall.h"


CStopCall::CStopCall()
{
}


CStopCall::~CStopCall()
{
}
std::string CStopCall::getName()
{

	return "stopCall";
}

int CStopCall::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}
