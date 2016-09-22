#include "stdafx.h"
#include "GetGps.h"


CGetGps::CGetGps()
{
}


CGetGps::~CGetGps()
{
}
std::list<std::string> CGetGps::getArgNames()
{
	std::list<std::string> args;
	args.push_back("id");
	args.push_back("cycle");
	args.push_back("querymode");
	return args;
}
std::string CGetGps::getName()
{
	return "getGps";
}

int CGetGps::run(CRemotePeer* pRemote, std::map<std::string, std::string> args, uint64_t callId)
{
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	if (args.find("id") != args.end() && args.find("cycle") != args.end() && args.find("querymode") != args.end())
	{
		int callId = atoi(args["callId"].c_str());
		int id = atoi(args["id"].c_str());
		int cycle = atoi(args["cycle"].c_str());
		int querymode = atoi(args["querymode"].c_str());
		int result = pDispatchOperate->getGps(pRemote, id, querymode, cycle, callId);
	}

	return 0;
}