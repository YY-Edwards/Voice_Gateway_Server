#include "stdafx.h"
#include "RadioCheck.h"


CRadioCheck::CRadioCheck()
{
}


CRadioCheck::~CRadioCheck()
{
}
std::list<std::string> CRadioCheck::getArgNames()
{
	std::list<std::string> args;

	args.push_back("id");


	return args;
}
std::string CRadioCheck::getName()
{

	return "radioCheck";
}

int CRadioCheck::run(CRemotePeer* pRemote, std::map<std::string, std::string> args, uint64_t callId)
{
	if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
	{
		if (args.find("id") != args.end())
		{
			int id = atoi(args["id"].c_str());
			int callId = atoi(args["callId"].c_str());
			int result = m_dispatchOperate[pRemote]->radioCheck(pRemote, id, callId);
		}

	}
	return 0;
}