#include "stdafx.h"
#include "StopCall.h"


CStopCall::CStopCall()
{
}


CStopCall::~CStopCall()
{
}
std::list<std::string> CStopCall::getArgNames()
{
	std::list<std::string> args;

	args.push_back("id");


	return args;
}
std::string CStopCall::getName()
{

	return "stopCall";
}

int CStopCall::run(CRemotePeer* pRemote, std::map<std::string, std::string> args, uint64_t callId)
{
	if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
	{
		if (args.find("id") != args.end())
		{
			int id = atoi(args["id"].c_str());
			int result = m_dispatchOperate[pRemote]->stopCall(pRemote,callId);
		}

	}
		
	return 0;
}
