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

int CGetGps::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	if (args.find("id") != args.end() && args.find("cycle") != args.end() && args.find("querymode") != args.end())
	{
		int sn = atoi(args["sn"].c_str());
		int id = atoi(args["id"].c_str());
		int cycle = atoi(args["cycle"].c_str());
		int querymode = atoi(args["querymode"].c_str());
		int result = pDispatchOperate->getGps(id, querymode, cycle, sn);
	}

	return 0;
}