#include "stdafx.h"
#include <cstdio>
#include <exception>

#include "..\include\RpcJsonParser.h"

/*
* Write SMS Log
* JSON string: {call:writeSmsLog, param:{target:22, dest:223, type:"private", message:'sms message content'}}
*/
class CWriteSmsLog : public CFunctionParser{
public:
	virtual int parse(Value& v, std::map<std::string, std::string>& args)
	{
		try
		{
			if (!v.IsObject())
			{
				throw std::exception("value is not object");
			}

			if (!v.HasMember("message"))
			{
				throw std::exception("parameter message not exist");
			}

			args["message"] = getVal(v["message"]);

			if (!v.HasMember("target"))
			{
				throw std::exception("parameter target not exist");
			}
			int x = v["target"].GetInt();
			args["target"] = getVal(v["target"]);

			if (!v.HasMember("dest"))
			{
				throw std::exception("parameter dest not exist");
			}
			args["dest"] = getVal(v["dest"]);

			if (!v.HasMember("type"))
			{
				throw std::exception("parameter type not exist");
			}
			args["type"] = getVal(v["type"]);

			return 0;
		}
		catch (std::exception e){

		}
		catch (...)
		{

		}
		return -1;
	}
};

CRpcJsonParser::CRpcJsonParser()
{
	m_mpParser["writeEventLog"] = new CWriteSmsLog();
}


CRpcJsonParser::~CRpcJsonParser()
{
	for (auto i = m_mpParser.begin(); i != m_mpParser.end(); i++)
	{
		delete i->second;
	}
}

std::string CRpcJsonParser::parseCall(std::string str, std::map<std::string, std::string>& args)
{
	std::string strCall = "";
	args.erase(args.begin(), args.end());

	Document d;
	try{
		if (d.ParseInsitu((char*)str.c_str()).HasParseError())
		{
			throw std::exception("parser error");
		}

		if (!d.IsObject()){
			throw std::exception("format error");
		}

		d.Parse(str.c_str());

		if (!d.HasMember("call"))
		{
			throw std::exception("call name not exist");
		}

		strCall = d["call"].GetString();

		if (!d.HasMember("param"))
		{
			throw std::exception("parameter not exist or parameter format error");
		}

		if (m_mpParser.find(strCall) != m_mpParser.end())
		{
			if (0 != m_mpParser[strCall]->parse(d["param"], args))
			{
				throw std::exception("parse parameters failed");
			}
		}
		else
		{
			throw std::exception("parser not found");
		}
	}
	catch (std::exception& e){
//		printf("%s\r\n",e.what());
	}
	catch (...)
	{
		//printf("unknow error\r\n");
	}
	
	return strCall;
}