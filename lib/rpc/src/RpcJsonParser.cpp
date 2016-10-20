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

int CRpcJsonParser::getRequest(const std::string str, std::string& callName, uint64_t& callId, std::string& args, std::string& type)
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

		if (d.HasMember("type"))
		{
			type = d["type"].GetString();
		}

		callName = d["call"].GetString();
		if (rapidjson::kNumberType == d["callId"].GetType())
		{
			callId = d["callId"].GetUint64();
		}
		else if (rapidjson::kStringType == d["callId"].GetType()) {
			callId = std::atoll(d["callId"].GetString());
		}

		if (d.HasMember("param"))
		{
			if (!d["param"].IsObject())
			{
				throw std::exception("parameter invalid");
			}

			StringBuffer sb;
			Writer<StringBuffer> writer(sb);
			d["param"].Accept(writer); // Accept() traverses the DOM and generates Handler events.
			args = sb.GetString();
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

int CRpcJsonParser::getResponse(const std::string str, 
						std::string& status, 
						std::string& statusText, 
						int& errCode, 
						uint64_t& callId, 
						std::string& content)
{
	int ret = 0;
	content.erase(content.begin(), content.end());

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

		if (d.HasMember("status"))
		{
			status = d["status"].GetString();
		}
		else
		{
			throw std::exception("response is invalid");
		}

		if (d.HasMember("callId"))
		{
			if (rapidjson::kNumberType == d["callId"].GetType())
			{
				callId = d["callId"].GetUint64();
			} 
			else if (rapidjson::kStringType == d["callId"].GetType())
			{
				callId = std::atoll(d["callId"].GetString());
			} 
			else
			{
				throw std::exception("call id is invalid");
			}
		}
		else
		{
			throw std::exception("response is not valid");
		}
		
		if (d.HasMember("statusText"))
		{
			statusText = d["statusText"].GetString();
		}
		
		if (d.HasMember("errCode"))
		{
			errCode = d["errCode"].GetInt();
		}

		if (d.HasMember("contents"))
		{
			if (!d["contents"].IsObject())
			{
				throw std::exception("content is not object");
			}

			StringBuffer sb;
			Writer<StringBuffer> writer(sb);
			d["contents"].Accept(writer); // Accept() traverses the DOM and generates Handler events.
			content = sb.GetString();
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
		Writer<StringBuffer> writer(sb);
		d.Accept(writer); // Accept() traverses the DOM and generates Handler events.
		jsonStr = sb.GetString();
		d.RemoveAllMembers();
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
		Writer<StringBuffer> writer(sb);
		d.Accept(writer); // Accept() traverses the DOM and generates Handler events.
		jsonStr = sb.GetString();
		d.RemoveAllMembers();
	}
	catch (std::exception& e)
	{

	}
	catch (...)
	{

	}

	return jsonStr;
}