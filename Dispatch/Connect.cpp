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

int CConnect::run(CRemotePeer* pRemote, std::map<std::string, std::string> args)
{
	DispatchOperate  * pDispatchOperate = new DispatchOperate();
	string radioIP = args["radioIP"];
	string mnisIP = args["mnisIP"];
	int sn = atoi(args["sn"].c_str());
	int result = pDispatchOperate->Connect(radioIP.c_str(),mnisIP.c_str(),sn);
	switch (result)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		break;
	}
	return 0;
}