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

int CSendGroupSms::run(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
{
	Document d;
	try{
		d.Parse(param.c_str());
		if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
		{
			std::map<std::string, std::string> args;
			args["message"] = "sendGroupSms";
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			string temp = d["id"].GetString();
			int id = atoi(temp.c_str());
			string msg = d["msg"].GetString();
			int msgSize = (int)(msg.length() + 1);
			wchar_t* text = new wchar_t[msgSize];
			MultiByteToWideChar(CP_ACP, 0, msg.c_str(), -1, text, msgSize);
			int result = m_dispatchOperate[pRemote]->sendGroupSms(pRemote, id, text, callId);
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

	return 0;
}