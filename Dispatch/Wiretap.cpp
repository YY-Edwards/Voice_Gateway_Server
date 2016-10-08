#include "stdafx.h"
#include "Wiretap.h"


CWiretap::CWiretap()
{
}


CWiretap::~CWiretap()
{
}
std::list<std::string> CWiretap::getArgNames()
{
	std::list<std::string> args;

	args.push_back("id");


	return args;
}
std::string CWiretap::getName()
{

	return "wiretap";
}

int CWiretap::run(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
{
	Document d;
	try{
		d.Parse(param.c_str());
		if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
		{
			std::map<std::string, std::string> args;
			args["message"] = "wiretap";
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			string temp = d["id"].GetString();
			int id = atoi(temp.c_str());
			int result = m_dispatchOperate[pRemote]->wiretap(pRemote, id, callId);
		}

	}
	catch (std::exception e){

	}
	catch (...)
	{

	}
	
	

	return 0;
}