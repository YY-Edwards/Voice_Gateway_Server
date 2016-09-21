#pragma once

#include <string>
#include <map>

#include "../../rapidjson/document.h"     // rapidjson's DOM-style API
#include "../../rapidjson/prettywriter.h" // for stringify JSON
using namespace rapidjson;

class CFunctionParser{
protected:
	std::string getVal(Value& v){
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
public:
	virtual int parse(Value& v, std::map<std::string, std::string>& args) = 0;
};

class CRpcJsonParser
{
public:
	CRpcJsonParser();
	~CRpcJsonParser();

	std::string parseCall(std::string str, std::map<std::string, std::string>& args);

protected:
	std::map<std::string, CFunctionParser*>  m_mpParser;
};

