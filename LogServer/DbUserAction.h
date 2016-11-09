#pragma once

#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "Db.h"

void userAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	Document d;
	std::string strResp;
	
	try{
		d.Parse(param.c_str());

		if (!d.HasMember("operation") || rapidjson::kStringType != d["operation"].GetType())
		{
			throw std::exception("call format error, need operation key, or key type is not string");
		}

		std::string operation = d["operation"].GetString();
		
		if (0 == operation.compare("list") || 0 == operation.compare("count"))
		{
			if (!d.HasMember("critera") || rapidjson::kObjectType != d["critera"].GetType())
			{
				throw std::exception("call format error, critera must be an object");
			}

			std::list<CCriteria> condition;
			rapidjson::Value critera = d["critera"].GetObjectW();

			if (critera.HasMember("condition") && critera["condition"].IsArray())
			{
				condition = CRpcJsonParser::parseCondition(critera["condition"].GetArray());
			}

			int offset = 0;
			int count = 10;

			if (critera.HasMember("offset"))
			{
				offset = critera["offset"].GetInt();
			}
			if (critera.HasMember("count"))
			{
				count = critera["count"].GetInt();
			}

			std::list<recordType> records;
			std::string condStr;

			if (condition.size() > 0)
			{
				condStr = "where ";

				for (auto i = condition.begin(); i != condition.end(); i++)
				{
					if ((*i).releation.size() > 0)
					{
						condStr += (*i).releation + " ";
					}
					condStr += "`" + (*i).field + "`";
					condStr += " " + (*i).compare + " ";
					condStr += "'" + (*i).val + "'";

					condStr += " ";
				}
			}

			condStr += " limit " + std::to_string(offset) + "," + std::to_string(count);
			ArgumentType args;

			if (0 == operation.compare("list"))
			{
				CDb::instance()->query("user", condStr.c_str(), records);

				FieldValue fvRecords(FieldValue::TArray);
				for (auto i = records.begin(); i != records.end(); i++)
				{
					FieldValue r(FieldValue::TObject);

					for (auto j = (*i).begin(); j != (*i).end(); j++){
						r.setKeyVal(j->first.c_str(), FieldValue(j->second.c_str()));
					}
					fvRecords.push(r);
				}
				args["records"] = fvRecords;
			}
			else
			{
				int recordCount = CDb::instance()->count("user", condStr.c_str());
				args["count"] = FieldValue(recordCount);
			}

			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);

		}
		else if (0 == operation.compare("add"))
		{
			if (!d.HasMember("users") || !d["users"].IsArray())
			{
				throw std::exception("call parameter error, users key must be an array");
			}
			int itemCount = d["users"].Size();

			for (size_t i = 0; i < itemCount; i++)
			{
				std::string name = d["users"][i]["username"].GetString();
				std::string password = d["users"][i]["password"].GetString();
				std::string authority = d["users"][i]["authority"].GetString();
				std::string type = d["users"][i]["type"].GetString();

				bool ret = CDb::instance()->insertUser(name.c_str(), 
													   "", 
													   name.c_str(), 
													   password.c_str(), 
													   authority.c_str(), 
													   type.c_str());
				if (!ret)
				{
					std::string errMsg = "add user failed.";
					errMsg += CDb::instance()->getLastError();
					throw std::exception(errMsg.c_str());
				}
				strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
			}
		}
		else if (0 == operation.compare("update"))
		{
			if (!d.HasMember("users") || !d["users"].IsArray())
			{
				throw std::exception("call parameter error, users key must be an array");
			}
			int itemCount = d["users"].Size();

			for (size_t i = 0; i < itemCount; i++)
			{
				std::string id = (rapidjson::kNumberType == d["users"][i]["id"].GetType()) ? 
											std::to_string( d["users"][i]["id"].GetInt())
											: d["users"][i]["id"].GetString();

				rapidjson::Value& val = d["users"][i]["user"];

				std::string name = val.HasMember("username")?val["username"].GetString():"";
				std::string password = val.HasMember("password") ? val["password"].GetString():"";
				std::string authority = val.HasMember("authority") ? val["authority"].GetString():"";
				std::string type = val.HasMember("type") ? val["type"].GetString():"";

				recordType updateVal;
				if (name.size() > 0)
				{
					updateVal["username"] = name;
				}
				if (password.size() > 0)
				{
					updateVal["password"] = CDb::md5(password.c_str());
				}
				if (authority.size() > 0)
				{
					updateVal["authority"] = authority;
				}
				if (type.size() > 0)
				{
					updateVal["type"] = type;
				}

				std::string updCond = " where `id`=" + id;
				bool ret = CDb::instance()->updateUser(updCond.c_str(), updateVal);
				if (!ret)
				{
					throw std::exception("update user failed");
				}
				strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
			}
		}
		else if (0 == operation.compare("del"))
		{
			if (!d.HasMember("users") || !d["users"].IsArray())
			{
				throw std::exception("call parameter error, users key must be an array");
			}

			for (int m = 0; m < d["users"].Size(); m++)
			{
				int id = d["users"][m].GetInt();
				std::string condition = "where id=" + std::to_string(id);
				CDb::instance()->del("user", condition.c_str());
			}
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
		}
	}
	catch (std::exception e){
		strResp = CRpcJsonParser::buildResponse("failed", callId, 500, e.what(), ArgumentType());
	}
	catch (...)
	{
		strResp = CRpcJsonParser::buildResponse("failed", callId, 500, "unknow error", ArgumentType());
	}

	pRemote->sendResponse(strResp.c_str(), strResp.size());
}