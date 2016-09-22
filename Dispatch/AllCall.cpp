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

int CAllCall::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	int callId = atoi(args["callId"].c_str());
	int result = pDispatchOperate->allCall(callId);
	return 0;
}
