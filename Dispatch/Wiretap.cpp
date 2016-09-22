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
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	if (args.find("id") != args.end())
	{
		int id = atoi(args["id"].c_str());
		int callId = atoi(args["callId"].c_str());
		int result = pDispatchOperate->wiretap(id, callId);
	}
	return 0;
}