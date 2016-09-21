#include "stdafx.h"
#include "GetOverTurnGPS.h"


CGetOverTurnGPS::CGetOverTurnGPS()
{
}


CGetOverTurnGPS::~CGetOverTurnGPS()
{
}
std::list<std::string> CGetOverTurnGPS::getArgNames()
{
	std::list<std::string> args;

	args.push_back("name");
	args.push_back("content");

	return args;
}
std::string CGetOverTurnGPS::getName()
{

	return "getOverTurnGps";
}

int CGetOverTurnGPS::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	if (args.find("ip")!=args.end() )
	{
		string ip = args["ip"];
		int sn = atoi(args["sn"].c_str());
		int result = pDispatchOperate->getOverturnGps(ip.c_str(), sn);
	}
	return 0;
}
