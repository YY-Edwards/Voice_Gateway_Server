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

	args.push_back("ip");

	return args;
}
std::string CGetOverTurnGPS::getName()
{

	return "getOverTurnGps";
}

int CGetOverTurnGPS::run(CRemotePeer* pRemote, std::map<std::string, std::string> args, uint64_t callId)
{
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	if (args.find("ip")!=args.end() )
	{
		string ip = args["ip"];
		int callId = atoi(args["callId"].c_str());
		int result = pDispatchOperate->getOverturnGps(pRemote, ip.c_str(), callId);
	}
	return 0;
}
