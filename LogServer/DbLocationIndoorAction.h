#pragma once

#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "Db.h"

void locationIndoorAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
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
				//CDb::instance()->query("location", condStr.c_str(), records);
				CDb::instance()->listLocation(condStr.c_str(), records);
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
				int recordCount = CDb::instance()->count("location", condStr.c_str());
				args["count"] = FieldValue(recordCount);
			}

			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);

		}
		else if (0 == operation.compare("add"))
		{
			if (!d.HasMember("location") || !d["location"].IsObject())
			{
				throw std::exception("call parameter error, users key must be an array");
			}
			//int itemCount = d["location"].Size();

			//for (size_t i = 0; i < itemCount; i++)
			{
				int source = d["location"]["source"].GetInt();
				int major = d["location"]["major"].GetInt();
				int minor = d["location"]["minor"].GetInt();
				int timestamp = d["location"]["timestamp"].GetInt();
				int rssi = d["location"]["rssi"].GetInt();
				int txpower = d["location"]["txpower"].GetInt();
				std::string uuid="";
				for (int j = 0; j < d["location"]["uuid"].Size(); j++)
				{
					uuid += std:: to_string(d["location"]["uuid"][j].GetInt());
				}
				bool ret = CDb::instance()->insertLocationIndoor(source, major, minor, timestamp,rssi,txpower,uuid);
				if (!ret)
				{
					std::string errMsg = "add location log failed.";
					errMsg += CDb::instance()->getLastError();
					throw std::exception(errMsg.c_str());
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