#include "stdafx.h"
#include "SendSms.h"


CSendSms::CSendSms()
{
}


CSendSms::~CSendSms()
{
}
std::list<std::string> CSendSms::getArgNames()
{
	std::list<std::string> args;

	args.push_back("id");
	args.push_back("msg");

	return args;
}
std::string CSendSms::getName()
{

	return "sendSms";
}

int CSendSms::run(CRemotePeer* pRemote, std::map<std::string, std::string> args, uint64_t callId)
{

	if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
	{
		if (args.find("id") != args.end() && args.find("msg") != args.end())
		{
			int id = atoi(args["id"].c_str());
			string msg = args["msg"];
			//string to wchar_t *
			int msgSize = (int)(msg.length() + 1);
			wchar_t* text = new wchar_t[msgSize];
			MultiByteToWideChar(CP_ACP, 0, msg.c_str(), -1, text, msgSize);
			int result = m_dispatchOperate[pRemote]->sendSms(pRemote, id, text, callId);
			delete[] text;
			
		}

	}
	return 0;
}