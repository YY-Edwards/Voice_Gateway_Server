#include "stdafx.h"
#include "SendGroupSms.h"


CSendGroupSms::CSendGroupSms()
{
}


CSendGroupSms::~CSendGroupSms()
{
}
std::list<std::string> CSendGroupSms::getArgNames()
{
	std::list<std::string> args;

	args.push_back("id");
	args.push_back("msg");

	return args;
}
std::string CSendGroupSms::getName()
{

	return "sendGroupSms";
}

int CSendGroupSms::run(CRemotePeer* pRemote, std::map<std::string, std::string> args, uint64_t callId)
{
	
	if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
	{
		if (args.find("id") != args.end())
		{
			int id = atoi(args["id"].c_str());
			string msg = args["msg"];
			wchar_t * text = new wchar_t[msg.size()];
			int result = m_dispatchOperate[pRemote]->sendGroupSms(pRemote, id, text, callId);
			
		}

	}
	return 0;
}