#include "stdafx.h"
#include "SendSms.h"


CSendSms::CSendSms()
{
}


CSendSms::~CSendSms()
{
}
std::string CSendSms::getName()
{

	return "sendSms";
}

int CSendSms::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}