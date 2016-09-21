#include "stdafx.h"
#include "Wiretap.h"


CWiretap::CWiretap()
{
}


CWiretap::~CWiretap()
{
}
std::list<std::string> CWiretap::getArgNames()
{
	std::list<std::string> args;

	args.push_back("name");
	args.push_back("content");

	return args;
}
std::string CWiretap::getName()
{

	return "wiretap";
}

int CWiretap::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	return 0;
}