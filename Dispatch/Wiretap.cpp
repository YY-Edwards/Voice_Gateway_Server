#include "stdafx.h"
#include "Wiretap.h"


CWiretap::CWiretap()
{
}


CWiretap::~CWiretap()
{
}
std::string CWiretap::getName()
{

	return "wiretap";
}

int CWiretap::run(CBaseConnector* pConnector, std::map<std::string, std::string> args)
{
	return 0;
}