#pragma once

#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "Db.h"

void radioAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
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
				//CDb::instance()->query("radios", condStr.c_str(), records);
				CDb::instance()->listRadio(condStr.c_str(), records);

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
				int recordCount = CDb::instance()->count("radios", condStr.c_str());
				args["count"] = FieldValue(recordCount);
			}

			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);

		}
		else if (0 == operation.compare("add"))
		{
			if (!d.HasMember("radios") || !d["radios"].IsArray())
			{
				throw std::exception("call parameter error, users key must be an array");
			}
			int itemCount = d["radios"].Size();

			for (size_t i = 0; i < itemCount; i++)
			{
				long long id = d["radios"][i]["id"].GetInt64();
				std::string radioId = std::to_string(d["radios"][i]["radio_id"].GetInt());
				std::string sn = d["radios"][i]["sn"].GetString();
				int type = d["radios"][i]["type"].GetInt();
				int screen = d["radios"][i]["screen"].GetInt();
				int gps = d["radios"][i]["gps"].GetInt();
				int keyboard = d["radios"][i]["keyboard"].GetInt();
				bool ret = CDb::instance()->insertRadio(id, radioId.c_str(), type, sn.c_str(), screen, gps, keyboard);
				if (!ret)
				{
					std::string errMsg = "add radio failed.";
					errMsg += CDb::instance()->getLastError();
					throw std::exception(errMsg.c_str());
				}
			}
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
		}
		else if (0 == operation.compare("del"))
		{
			if (!d.HasMember("radios") || !d["radios"].IsArray())
			{
				throw std::exception("call parameter error, users key must be an array");
			}

			for (int m = 0; m < d["radios"].Size(); m++)
			{
				long long id = d["radios"][m].GetInt64();
				std::string condition = "where id=" + std::to_string(id);
				CDb::instance()->del("radios", condition.c_str());
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
				long long id = d["radios"][i]["id"].GetInt64();

				rapidjson::Value& val = d["radios"][i]["radio"];

				std::string radioId = val.HasMember("radio_id") ? std::to_string(val["radio_id"].GetInt()) : "0";
				std::string sn = val.HasMember("sn") ? val["sn"].GetString() : "";
				int type = val.HasMember("type") ? val["type"].GetInt(): -1;
				int gps = val.HasMember("gps") ? val["gps"].GetInt() : -1;
				int screen = val.HasMember("screen") ? val["screen"].GetInt() : -1;
				int keyboard = val.HasMember("keyboard") ? val["keyboard"].GetInt() : -1;

				recordType updateVal;
				if (radioId.size() > 0)
				{
					updateVal["radio_id"] = radioId;
				}
				if (sn.size() > 0)
				{
					updateVal["sn"] = sn;
				}
				if (type >= 0)
				{
					updateVal["type"] = std::to_string(type);
				}
				if (screen >= 0)
				{
					updateVal["screen"] = std::to_string(screen);
				}
				if (gps >= 0)
				{
					updateVal["gps"] = std::to_string(gps);
				}
				if (keyboard >= 0)
				{
					updateVal["keyboard"] = std::to_string(keyboard);
				}

				std::string updCond = " where `id`='" + std::to_string(id) +"'";
				bool ret = CDb::instance()->updateRadio(updCond.c_str(), updateVal);
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