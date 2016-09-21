#pragma once

#include <string>
#include <map>
#include <list>

#include "../../rapidjson/document.h"     // rapidjson's DOM-style API
#include "../../rapidjson/prettywriter.h" // for stringify JSON
using namespace rapidjson;

class CRpcJsonParser
{
public:
	CRpcJsonParser();
	~CRpcJsonParser();

	std::string getCallName(const std::string str);
	int getArgs(const std::string str, std::list<std::string> argList, std::map<std::string, std::string>& args);

protected:
	int parseArgs(Value& v, std::list<std::string> argList ,std::map<std::string, std::string>& args);
	std::string getVal(Value& v);
};

