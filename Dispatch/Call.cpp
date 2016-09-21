#include "stdafx.h"
#include "Call.h"


CCall::CCall()
{
}


CCall::~CCall()
{
}
std::list<std::string> CCall::getArgNames()
{
	std::list<std::string> args;

	args.push_back("id");
	return args;
}
std::string CCall::getName()
{

	return "call";
}

int CCall::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	if (args.find("id") != args.end())
	{
		int id = atoi(args["id"].c_str());
		int sn = atoi(args["sn"].c_str());
		int result = pDispatchOperate->call(id, sn);
	}
	
	return 0;
}