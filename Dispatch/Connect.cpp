#include "stdafx.h"
#include "Connect.h"
#include "DispatchOperate.h"

CConnect::CConnect()
{
}


CConnect::~CConnect()
{
}
std::list<std::string> CConnect::getArgNames()
{
	std::list<std::string> args;

	args.push_back("radioIP");
	args.push_back("mnisIP");

	return args;
}
std::string CConnect::getName()
{

	return "connect";
}

int CConnect::run(CRemotePeer* pRemote, std::map<std::string, std::string> args, uint64_t callId)
{
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	if (args.find("radioIP") != args.end() && args.find("mnisIP") != args.end())
	{
		string radioIP = args["radioIP"];
		string mnisIP = args["mnisIP"];
		pDispatchOperate->Connect( pRemote,radioIP.c_str(), mnisIP.c_str(), callId);
		if (m_dispatchOperate.find(pRemote) == m_dispatchOperate.end())
		{
			m_dispatchOperate[pRemote] = pDispatchOperate;
		}
	}
	
	
	return 0;
}