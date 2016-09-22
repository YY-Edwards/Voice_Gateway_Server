#include "stdafx.h"
#include "CancelGps.h"


CCancelGps::CCancelGps()
{
}


CCancelGps::~CCancelGps()
{
}
std::list<std::string> CCancelGps::getArgNames()
{
	std::list<std::string> args;
	args.push_back("id");
	return args;
}
std::string CCancelGps::getName()
{
	return "cancelGps";
}

int CCancelGps::run(CRemotePeer* pRemote, std::map<std::string, std::string> args, uint64_t callId)
{
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	if (args.find("id") != args.end())
	{
		int callId = atoi(args["callId"].c_str());
		int id = atoi(args["id"].c_str());
		int result = pDispatchOperate->cancelPollGps(pRemote, id, callId);
	}
	return 0;
}
