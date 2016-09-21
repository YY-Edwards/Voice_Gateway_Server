#include "stdafx.h"
#include "SendGroupSms.h"


CSendGroupSms::CSendGroupSms()
{
}


CSendGroupSms::~CSendGroupSms()
{
}
std::string CSendGroupSms::getName()
{

	return "sendGroupSms";
}

int CSendGroupSms::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}