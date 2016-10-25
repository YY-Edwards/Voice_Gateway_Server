#pragma once

#include <string>
#include <map>
#include <list>

#include "FieldValue.h"
#include "../../rapidjson/document.h"     // rapidjson's DOM-style API
#include "../../rapidjson/prettywriter.h" // for stringify JSON
#include "../../MySQL/criteria.h"
using namespace rapidjson;

#ifndef ArgumentType
typedef std::map<std::string, FieldValue> ArgumentType;
#endif

class CRpcJsonParser
{
public:
	CRpcJsonParser();
	~CRpcJsonParser();
public: // static memebers
	static rapidjson::Value toNode(const char* str);
	static rapidjson::Value toNode(FieldValue& v, rapidjson::Document& d);
	static std::string buildCall(char* pCallName, uint64_t callId, ArgumentType params);
	static std::string buildResponse(char* pStatus, uint64_t callId, int errCode, const char* statusText, ArgumentType contents = ArgumentType());
	static int getRequest(const std::string str, std::string& callName, uint64_t& callId, std::string& args, std::string& type);
	static int getResponse(const std::string str, std::string& status, std::string& statusText, int& errCode, uint64_t& callId, std::string& content);

	static std::list<CCriteria> parseCondition(rapidjson::Value v);
	static std::string listToString(std::list<std::map<std::string, std::string> >& lstVal);
	static std::string toString(rapidjson::Value& v);

};

