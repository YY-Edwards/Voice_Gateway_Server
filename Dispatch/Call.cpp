#include "stdafx.h"
#include "Call.h"

std::map<CRemotePeer*, DispatchOperate*>  m_dispatchOperate;
CCall::CCall()
{
}


CCall::~CCall()
{
}
std::list<std::string> CCall::getArgNames()
{
	std::list<std::string> args;

	args.push_back("id");
	return args;
}
std::string CCall::getName()
{

	return "call";
}

int CCall::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
	{
		if (args.find("id") != args.end())
		{
			int id = atoi(args["id"].c_str());
			int callId = atoi(args["callId"].c_str());
			int result = m_dispatchOperate[pRemote]->call(id, callId);
		}

	}
	
	
	return 0;
}