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

	int getCallName(const std::string str, std::string& callName, uint64_t& callId);
	int getArgs(const std::string str, std::list<std::string> argList, std::map<std::string, std::string>& args);

public: // static memebers
	static std::string buildCall(char* pCallName, uint64_t callId, std::map<std::string, std::string> params);
	static std::string buildResponse(char* pStatus, uint64_t callId, int errCode, const char* statusText, std::map<std::string, std::string> contents = std::map<std::string, std::string>());

protected:
	int parseArgs(Value& v, std::list<std::string> argList ,std::map<std::string, std::string>& args);
	std::string getVal(Value& v);
};

