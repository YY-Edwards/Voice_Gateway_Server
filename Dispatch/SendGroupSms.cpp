#include "stdafx.h"
#include "SendGroupSms.h"


CSendGroupSms::CSendGroupSms()
{
}


CSendGroupSms::~CSendGroupSms()
{
}
std::list<std::string> CSendGroupSms::getArgNames()
{
	std::list<std::string> args;

	args.push_back("name");
	args.push_back("content");

	return args;
}
std::string CSendGroupSms::getName()
{

	return "sendGroupSms";
}

int CSendGroupSms::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	return 0;
}