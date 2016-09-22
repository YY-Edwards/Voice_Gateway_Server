#include "stdafx.h"
#include "RmotePowerON.h"


CRmotePowerON::CRmotePowerON()
{
}


CRmotePowerON::~CRmotePowerON()
{
}
std::list<std::string> CRmotePowerON::getArgNames()
{
	std::list<std::string> args;

	args.push_back("id");

	return args;
}
std::string CRmotePowerON::getName()
{

	return "remotePowerOn";
}

int CRmotePowerON::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	if (args.find("id") != args.end())
	{
		int id = atoi(args["id"].c_str());
		int callId = atoi(args["callId"].c_str());
		int result = pDispatchOperate->remotePowerOn(id, callId);
	}
	return 0;
}