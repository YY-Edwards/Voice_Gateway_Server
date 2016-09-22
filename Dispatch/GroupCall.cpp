#include "stdafx.h"
#include "GroupCall.h"


CGroupCall::CGroupCall()
{
}


CGroupCall::~CGroupCall()
{
}
std::list<std::string> CGroupCall::getArgNames()
{
	std::list<std::string> args;

	args.push_back("name");
	args.push_back("content");

	return args;
}
std::string CGroupCall::getName()
{

	return "groupCall";
}

int CGroupCall::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	return 0;
}