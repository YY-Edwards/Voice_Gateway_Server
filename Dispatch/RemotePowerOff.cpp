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

int CRemotePowerOff::run(CRemotePeer* pRemote, std::map<std::string, std::string> args, uint64_t callId)
{
	if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
	{
		if (args.find("id") != args.end())
		{
			int id = atoi(args["id"].c_str());
			int result = m_dispatchOperate[pRemote]->remotePowerOff(pRemote, id, callId);
		}

	}
	return 0;
}