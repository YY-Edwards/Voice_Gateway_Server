#include "stdafx.h"
#include "SendSms.h"


CSendSms::CSendSms()
{
}


CSendSms::~CSendSms()
{
}
std::list<std::string> CSendSms::getArgNames()
{
	std::list<std::string> args;

	args.push_back("name");
	args.push_back("content");

	return args;
}
std::string CSendSms::getName()
{

	return "sendSms";
}

int CSendSms::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	return 0;
}