#include "stdafx.h"
#include <cstdio>
#include <exception>

#include "..\include\RpcJsonParser.h"

CRpcJsonParser::CRpcJsonParser()
{
}


CRpcJsonParser::~CRpcJsonParser()
{
}

std::string CRpcJsonParser::getCallName(const std::string str)
{
	std::string strCall = "";

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

std::string CRpcJsonParser::getVal(Value& v)
{
	if (v.IsInt())
	{
		int val = v.GetInt();
		return std::to_string(val);
	}
	else if (v.IsFloat())
	{
		return std::to_string(v.GetFloat());
	}
	else if (v.IsString())
	{
		return v.GetString();
	}

	return std::string("");
}

int CRpcJsonParser::parseArgs(Value& v, std::list<std::string> argList, std::map<std::string, std::string>& args)
{
	try
	{
		if (!v.IsObject())
		{
			throw std::exception("value is not object");
		}

		for (auto iArgName = argList.begin(); iArgName != argList.end(); iArgName++)
		{
			std::string name = *iArgName;
			if (v.HasMember(name.c_str()))
			{
				args[name] = getVal(v[name.c_str()]);
			}
			else
			{
				std::string err = "filed " + name + "not exist";
				throw std::exception(err.c_str());
			}
		}
		return 0;
	}
	catch (std::exception& e){

	}
	catch (...)
	{

	}
	return -1;
}
int CRpcJsonParser::getArgs(const std::string str, std::list<std::string> argList, std::map<std::string, std::string>& args)
{
	int ret = 0;
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

		if (!d.HasMember("param"))
		{
			throw std::exception("parameter not exist or parameter format error");
		}

		if (0 != parseArgs(d["param"], argList, args))
		{
			throw std::exception("parse argument error!");
		}
	}
	catch (std::exception& e){
		ret = -1;
	}
	catch (...)
	{
		ret = -1;
	}
	
	return ret;
}