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

int CSendSms::run(CRemotePeer* pRemote, const std::string& param, uint64_t callId)
{
	Document d;
	try{
		d.Parse(param.c_str());
		if (m_dispatchOperate.find(pRemote) != m_dispatchOperate.end())
		{
			std::map<std::string, std::string> args;
			args["message"] = "sendSms";
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", args);
			pRemote->sendResponse(strResp.c_str(), strResp.size());
			string temp = d["id"].GetString();
			int id = atoi(temp.c_str());
			string msg = d["msg"].GetString();
			int msgSize = (int)(msg.length() + 1);
			wchar_t* text = new wchar_t[msgSize];
			MultiByteToWideChar(CP_ACP, 0, msg.c_str(), -1, text, msgSize);
			int result = m_dispatchOperate[pRemote]->sendSms(pRemote, id, text, callId);
		}
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

	return 0;
}