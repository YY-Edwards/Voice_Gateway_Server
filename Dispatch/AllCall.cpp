#include "stdafx.h"
#include "AllCall.h"


CAllCall::CAllCall()
{
}


CAllCall::~CAllCall()
{
}
std::list<std::string> CAllCall::getArgNames()
{
	std::list<std::string> args;

	/*args.push_back("name");
	args.push_back("content");*/

	return args;
}
std::string CAllCall::getName()
{

	return "allCall";
}

int CAllCall::run(CRemotePeer* pRemote, std::map<std::string, std::string> args, uint64_t callId)
{
	if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
	{
		int result = m_dispatchOperate[pRemote]->allCall(pRemote, callId);
	}
	return 0;
}
