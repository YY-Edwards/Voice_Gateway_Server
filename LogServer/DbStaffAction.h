#pragma once

#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "Db.h"

void staffAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
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
				CDb::instance()->query("staff", condStr.c_str(), records);

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
				int recordCount = CDb::instance()->count("staff", condStr.c_str());
				args["count"] = FieldValue(recordCount);
			}

			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);

		}
		else if (0 == operation.compare("add"))
		{
			if (!d.HasMember("staffs") || !d["staffs"].IsArray())
			{
				throw std::exception("call parameter error, users key must be an array");
			}
			int itemCount = d["staffs"].Size();

			for (size_t i = 0; i < itemCount; i++)
			{
				std::string name = d["staffs"][i]["name"].GetString();
				std::string phone = d["radios"][i]["phone"].GetString();
				bool ret = CDb::instance()->insertStaff(name.c_str(), phone.c_str());
				if (!ret)
				{
					std::string errMsg = "add staff failed.";
					errMsg += CDb::instance()->getLastError();
					throw std::exception(errMsg.c_str());
				}
			}
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
		}
		else if (0 == operation.compare("del"))
		{
			if (!d.HasMember("staffs") || !d["staffs"].IsArray())
			{
				throw std::exception("call parameter error, users key must be an array");
			}

			for (int m = 0; m < d["staffs"].Size(); m++)
			{
				int id = d["staffs"][m].GetInt();
				std::string condition = "where id=" + std::to_string(id);
				CDb::instance()->del("staff", condition.c_str());
			}
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
		}
		else if (0 == operation.compare("update"))
		{
			if (!d.HasMember("radios") || !d["radios"].IsArray())
			{
				throw std::exception("call parameter error, radio key must be an array");
			}
			int itemCount = d["radios"].Size();

			for (size_t i = 0; i < itemCount; i++)
			{
				int id = (rapidjson::kNumberType == d["staffs"][i]["id"].GetType()) ?
					d["staffs"][i]["id"].GetInt()
					: std::atoi(d["staffs"][i]["id"].GetString());

				rapidjson::Value& val = d["staffs"][i]["staff"];

				std::string name = val.HasMember("name") ? val["name"].GetString() : "";
				std::string phone = val.HasMember("phone") ? val["phone"].GetString() : "";

				recordType updateVal;
				if (name.size() > 0)
				{
					updateVal["name"] = name;
				}
				if (phone.size() > 0)
				{
					updateVal["phone"] = phone;
				}
				std::string updCond = " where `id`=" + CDb::instance()->getUserIdByStaffId(id);
				bool ret = CDb::instance()->updateStaff(updCond.c_str(), updateVal);
				if (!ret)
				{
					throw std::exception("update radio failed");
				}
			}
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
		}
	}
	catch (std::exception e)
	{
		strResp = CRpcJsonParser::buildResponse("failed", callId, 500, e.what(), ArgumentType());
	}
	catch (...)
	{
		strResp = CRpcJsonParser::buildResponse("failed", callId, 500, "unknow error", ArgumentType());
	}

	pRemote->sendResponse(strResp.c_str(), strResp.size());
}