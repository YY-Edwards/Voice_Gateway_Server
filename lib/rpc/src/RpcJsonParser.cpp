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
			if (d["statusText"].IsNull())
			{
				statusText = "";
			}
			else
			{
				statusText = d["statusText"].GetString();
			}
		}
		
		if (d.HasMember("errCode"))
		{
			errCode = d["errCode"].GetInt();
		}

		if (d.HasMember("contents"))
		{
			//if (!d["contents"].IsObject())
			//{
			//	throw std::exception("content is not object");
			//}

			if (!d["contents"].IsNull()){
				StringBuffer sb;
				Writer<StringBuffer> writer(sb);
				d["contents"].Accept(writer); // Accept() traverses the DOM and generates Handler events.
				content = sb.GetString();
			}
			else
			{
				content = "";
			}
			
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

rapidjson::Value CRpcJsonParser::toNode(const char* str)
{
	rapidjson::Document d;
	rapidjson::Value node(kStringType);
	node.SetString(rapidjson::StringRef(str), d.GetAllocator());
	return node;
}

rapidjson::Value CRpcJsonParser::toNode(FieldValue& v, rapidjson::Document& d)
{
	if (FieldValue::TString == v.getType())
	{
		rapidjson::Value val(kStringType);
		val.SetString(StringRef(v.getString().c_str()), d.GetAllocator());

		return val;
	} 
	else if (FieldValue::TInt == v.getType())
	{
		rapidjson::Value val(kNumberType);
		val.SetInt(v.getInt());

		return val;
	}
	else if (FieldValue::TArray == v.getType())
	{
		rapidjson::Value val(kArrayType);
		val.SetArray();

		std::list<FieldValue> ar = v.getArray();
		for (auto i = ar.begin(); i != ar.end(); i++)
		{
			val.PushBack(toNode(*i, d), d.GetAllocator());
		}
		return val;
	}
	else if (FieldValue::TObject == v.getType())
	{
		rapidjson::Value val(kObjectType);
		val.SetObject();
		std::map<std::string, FieldValue> obj = v.getObject();
		for (auto i = obj.begin(); i != obj.end(); i++){
			val.AddMember(Value(i->first.c_str(), d.GetAllocator()).Move(), toNode(i->second, d), d.GetAllocator());
		}

		return val;
	}

	return rapidjson::Value(kNullType);
}

std::string CRpcJsonParser::buildCall(char* pCallName, uint64_t callId, ArgumentType params, const char* type)
{
	std::string jsonStr = "";

	try{
		Document d;
		d.SetObject();
		Value callNameEl(kStringType);
		callNameEl.SetString(pCallName, d.GetAllocator());
		
		Value callIdEl(kNumberType);
		callIdEl.SetUint64(callId);

		if (NULL != type)
		{
			Value typeEl(kStringType);
			typeEl.SetString(type, d.GetAllocator());
			d.AddMember("type", typeEl, d.GetAllocator());
		}

		Value paramEl(kObjectType);
		for (auto p = params.begin(); p != params.end(); ++p)
		{
			paramEl.AddMember(Value(p->first.c_str(), d.GetAllocator()).Move(),
				CRpcJsonParser::toNode(p->second, d),
				d.GetAllocator());
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

std::string CRpcJsonParser::toString(rapidjson::Value& v)
{
	StringBuffer sb;
	Writer<StringBuffer> writer(sb);
	v.Accept(writer); // Accept() traverses the DOM and generates Handler events.
	return sb.GetString();
}

std::string CRpcJsonParser::buildResponse(char* pStatus, uint64_t callId, int errCode, const char* statusText, ArgumentType contents)
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
			contentEl.AddMember(Value(p->first.c_str(), d.GetAllocator()).Move(), 
				CRpcJsonParser::toNode(p->second, d),
				d.GetAllocator());
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


std::list<CCriteria> CRpcJsonParser::parseCondition(rapidjson::Value v)
{
	std::list<CCriteria> condition;

	try
	{
		if (!v.IsArray())
		{
			throw std::exception("value is not array");
		}

		for (size_t i = 0; i < v.Size(); i++)
		{
			if (v[i].IsArray() && (v[i].Size() >= 3))
			{
				CCriteria critera;

				if (v[i].Size() > 3)
				{
					critera.releation = v[i][0].GetString();
					critera.compare = v[i][1].GetString();
					critera.field = v[i][2].GetString();
					critera.val = v[i][3].GetString();
				} 
				else
				{
					critera.compare = v[i][0].GetString();
					critera.field = v[i][1].GetString();
					critera.val = v[i][2].GetString();
				}

				condition.push_back(critera);
			}
		}
	}
	catch (std::exception e)
	{

	}
	catch (...)
	{

	}
	

	return condition;
}

std::string CRpcJsonParser::listToString(std::list<std::map<std::string, std::string> >& lstVal)
{
	std::string val;

	try
	{
		rapidjson::Document d;

		d.SetArray();

		for (auto i = lstVal.begin(); i != lstVal.end(); i++)
		{
			rapidjson::Value vObj;
			vObj.SetObject();

			for (auto j = (*i).begin(); j != (*i).end(); j++)
			{
				Value vStr(kStringType);
				vStr.SetString( StringRef(j->second.c_str()), d.GetAllocator());
				vObj.AddMember(StringRef(j->first.c_str()), vStr, d.GetAllocator());
			}

			d.PushBack(vObj, d.GetAllocator());
		}

		StringBuffer sb;
		Writer<StringBuffer> writer(sb);
		d.Accept(writer); // Accept() traverses the DOM and generates Handler events.
		val = sb.GetString();
	}
	catch (std::exception e)
	{

	}
	catch (...)
	{

	}

	return val;
}

std::string CRpcJsonParser::mergeCommand(const char* command, uint64_t callId, const char* param, const char* type)
{
	std::string str;
	try
	{
		rapidjson::Document d;
		d.Parse(param);

		rapidjson::Document dOut;
		dOut.SetObject();

		rapidjson::Value vCall(kStringType);
		vCall.SetString(StringRef(command), dOut.GetAllocator());
		dOut.AddMember("call", vCall, dOut.GetAllocator());

		rapidjson::Value nCallId(kNumberType);
		nCallId.SetInt64(callId);
		dOut.AddMember("callId", nCallId, dOut.GetAllocator());

		dOut.AddMember("param", d, dOut.GetAllocator());

		if (NULL != type)
		{
			Value typeEl(kStringType);
			typeEl.SetString(StringRef(type), d.GetAllocator());
			dOut.AddMember("type", typeEl, d.GetAllocator());
		}

		StringBuffer sb;
		Writer<StringBuffer> writer(sb);
		dOut.Accept(writer); // Accept() traverses the DOM and generates Handler events.
		str = sb.GetString();
	}
	catch (std::exception e)
	{

	}
	catch (...)
	{

	}

	return str;
}