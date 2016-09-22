#include "stdafx.h"
#include "RemotePowerOff.h"


CRemotePowerOff::CRemotePowerOff()
{
}


CRemotePowerOff::~CRemotePowerOff()
{
}
std::list<std::string> CRemotePowerOff::getArgNames()
{
	std::list<std::string> args;

	args.push_back("id");

	return args;
}
std::string CRemotePowerOff::getName()
{

	return "remotePowerOff";
}

int CRemotePowerOff::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	if (args.find("id") != args.end())
	{
		int id = atoi(args["id"].c_str());
		int callId = atoi(args["callId"].c_str());
		int result = pDispatchOperate->remotePowerOff(id, callId);
	}
	return 0;
}