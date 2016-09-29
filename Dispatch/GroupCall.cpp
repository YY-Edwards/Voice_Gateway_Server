#include "stdafx.h"
#include "GroupCall.h"


CGroupCall::CGroupCall()
{
}


CGroupCall::~CGroupCall()
{
}
std::list<std::string> CGroupCall::getArgNames()
{
	std::list<std::string> args;

	args.push_back("id");
	return args;
}
std::string CGroupCall::getName()
{

	return "groupCall";
}

int CGroupCall::run(CRemotePeer* pRemote, std::map<std::string, std::string> args, uint64_t callId)
{
	if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
	{
		if (args.find("id") != args.end())
		{
			int id = atoi(args["id"].c_str());
			int result = m_dispatchOperate[pRemote]->groupCall(pRemote, id, callId);
		}

	}
	return 0;
}