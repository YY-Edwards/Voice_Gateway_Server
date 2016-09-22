#include "stdafx.h"
#include <algorithm>
#include <cstdio>
#include <exception>

#include "..\include\RpcJsonParser.h"

CRpcJsonParser::CRpcJsonParser()
{
}


CRpcJsonParser::~CRpcJsonParser()
{
}

int CRpcJsonParser::getCallName(const std::string str, std::string& callName, uint64_t& callId)
{
	int ret = 0;

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

		if (!d.HasMember("callId"))
		{
			throw std::exception("call id not exist");
		}

		callName = d["call"].GetString();
		if (rapidjson::kNumberType == d["callId"].GetType())
		{
			callId = d["callId"].GetUint64();
		}
		else if (rapidjson::kStringType == d["callId"].GetType()) {
			callId = std::atoll(d["callId"].GetString());
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

std::string CRpcJsonParser::buildCall(char* pCallName, uint64_t callId, std::map<std::string, std::string> params)
{
	std::string jsonStr = "";

	try{
		Document d;
		d.SetObject();
		Value callNameEl(kStringType);
		callNameEl.SetString(pCallName, d.GetAllocator());
		
		Value callIdEl(kNumberType);
		callIdEl.SetUint64(callId);

		Value paramEl(kObjectType);
		for (auto p = params.begin(); p != params.end(); ++p)
		{
			Value el(kStringType);
			el.SetString(p->second.c_str(), d.GetAllocator());
			paramEl.AddMember( StringRef( p->first.c_str()), el, d.GetAllocator());
		}
		
		d.AddMember("call", callNameEl, d.GetAllocator());
		d.AddMember("callId", callIdEl, d.GetAllocator());
		d.AddMember("param", paramEl, d.GetAllocator());

		StringBuffer sb;
		PrettyWriter<StringBuffer> writer(sb);
		d.Accept(writer); // Accept() traverses the DOM and generates Handler events.
		jsonStr = sb.GetString();
		//std::string tmpStr = sb.GetString();
		//for (auto ich = tmpStr.begin(); ich != tmpStr.end(); ++ich)
		//{
		//	if (*ich != '\n' && *ich!=' ')
		//	{
		//		jsonStr.push_back(*ich);
		//	}
		//}
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}

	return jsonStr;
}

std::string CRpcJsonParser::buildResponse(char* pStatus, uint64_t callId, int errCode, const char* statusText, std::map<std::string, std::string> contents)
{
	std::string jsonStr = "";

	try{
		Document d;
		d.SetObject();
		Value statusEl(kStringType);
		statusEl.SetString(pStatus, d.GetAllocator());

		Value callIdEl(kNumberType);
		callIdEl.SetUint64(callId);

		Value errCodeEl(kNumberType);
		errCodeEl.SetInt(errCode);

		Value statusTextEl(kStringType);
		statusTextEl.SetString(statusText, d.GetAllocator());

		Value contentEl(kObjectType);
		for (auto p = contents.begin(); p != contents.end(); ++p)
		{
			Value el(kStringType);
			el.SetString(p->second.c_str(), d.GetAllocator());
			contentEl.AddMember(StringRef(p->first.c_str()), el, d.GetAllocator());
		}

		d.AddMember("status", statusEl, d.GetAllocator());
		d.AddMember("statusText", statusTextEl, d.GetAllocator());
		d.AddMember("callId", callIdEl, d.GetAllocator());
		d.AddMember("errCode", errCodeEl, d.GetAllocator());
		if (contents.size() > 0)
		{
			d.AddMember("contents", contentEl, d.GetAllocator());
		}

		StringBuffer sb;
		PrettyWriter<StringBuffer> writer(sb);
		d.Accept(writer); // Accept() traverses the DOM and generates Handler events.
		jsonStr = sb.GetString();
		//std::string tmpStr = sb.GetString();
		//for (auto ich = tmpStr.begin(); ich != tmpStr.end(); ++ich)
		//{
		//	if (*ich != '\n' && *ich != ' ')
		//	{
		//		jsonStr.push_back(*ich);
		//	}
		//}
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}

	return jsonStr;
}