#include "stdafx.h"
#include "RadioCheck.h"


CRadioCheck::CRadioCheck()
{
}


CRadioCheck::~CRadioCheck()
{
}
std::list<std::string> CRadioCheck::getArgNames()
{
	std::list<std::string> args;

	args.push_back("name");
	args.push_back("content");

	return args;
}
std::string CRadioCheck::getName()
{

	return "radioCheck";
}

int CRadioCheck::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	return 0;
}