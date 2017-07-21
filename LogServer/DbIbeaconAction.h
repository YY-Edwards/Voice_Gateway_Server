#pragma once

#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "Db.h"

void ibeaconAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
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
				CDb::instance()->query("ibeacons", condStr.c_str(), records);
				//CDb::instance()->listRadio(condStr.c_str(), records);

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
				int recordCount = CDb::instance()->count("ibeacons", condStr.c_str());
				args["count"] = FieldValue(recordCount);
			}

			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);

		}
		else if (0 == operation.compare("add"))
		{
			if (!d.HasMember("ibeacons") || !d["ibeacons"].IsArray())
			{
				throw std::exception("call parameter error, users key must be an array");
			}
			int itemCount = d["ibeacons"].Size();

			for (size_t i = 0; i < itemCount; i++)
			{
				rapidjson::Value& val = d["ibeacons"][i];

				std::string name = val.HasMember("name") ? val["name"].GetString() : "";
				std::string uuid = val.HasMember("uuid") ? val["uuid"].GetString() : "";
				int major = val.HasMember("major") ? val["major"].GetInt() : -1;
				int minor = val.HasMember("minor") ? val["minor"].GetInt() : -1;
				int tx_power = val.HasMember("tx_power") ? val["tx_power"].GetInt() : -1;
				int rssi = val.HasMember("rssi") ? val["rssi"].GetInt() : -1;
				int time_stamp = val.HasMember("time_stamp") ? val["time_stamp"].GetInt() : -1;
				int valid = val.HasMember("valid") ? val["valid"].GetInt() : -1;
				std::string area = val.HasMember("area") ? val["area"].GetString() : "";

				std::string pointx = val.HasMember("pointx") ? val["pointx"].GetString() : "";
				std::string pointy = val.HasMember("pointy") ? val["pointy"].GetString() : "";


				bool ret = CDb::instance()->insertIBeacon(
					name.c_str(),
					uuid.c_str(),
					major, minor, tx_power, rssi, time_stamp, valid, area.c_str(),
					pointx.c_str(),
					pointy.c_str()
					);
				if (!ret)
				{
					std::string errMsg = "add ibeacon failed.";
					errMsg += CDb::instance()->getLastError();
					throw std::exception(errMsg.c_str());
				}
			}
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
		}
		else if (0 == operation.compare("del"))
		{
			if (!d.HasMember("ibeacons") || !d["ibeacons"].IsArray())
			{
				throw std::exception("call parameter error, users key must be an array");
			}

			for (int m = 0; m < d["ibeacons"].Size(); m++)
			{
				int id = d["ibeacons"][m].GetInt();
				std::string condition = "where id=" + std::to_string(id);
				CDb::instance()->del("ibeacons", condition.c_str());
			}
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
		}
		else if (0 == operation.compare("update"))
		{
			if (!d.HasMember("ibeacons") || !d["ibeacons"].IsArray())
			{
				throw std::exception("call parameter error, radio key must be an array");
			}
			int itemCount = d["ibeacons"].Size();

			for (size_t i = 0; i < itemCount; i++)
			{
				int id = (rapidjson::kNumberType == d["ibeacons"][i]["id"].GetType()) ?
					d["ibeacons"][i]["id"].GetInt()
					: std::atoi(d["ibeacons"][i]["id"].GetString());

				rapidjson::Value& val = d["ibeacons"][i]["ibeacon"];

				std::string name = val.HasMember("name") ? val["name"].GetString() : "";
				std::string uuid = val.HasMember("uuid") ? val["uuid"].GetString() : "";
				int major = val.HasMember("major") ? val["major"].GetInt() : -1;
				int minor = val.HasMember("minor") ? val["minor"].GetInt() : -1;
				int tx_power = val.HasMember("tx_power") ? val["tx_power"].GetInt() : -1;
				int rssi = val.HasMember("rssi") ? val["rssi"].GetInt() : -1;
				int time_stamp = val.HasMember("time_stamp") ? val["time_stamp"].GetInt() : -1;
				int valid = val.HasMember("valid") ? val["valid"].GetInt() : -1;
				std::string area = val.HasMember("area") ? val["area"].GetString() : "";

				std::string pointx = val.HasMember("pointx") ? val["pointx"].GetString() : "";
				std::string pointy = val.HasMember("pointy") ? val["pointy"].GetString() : "";

				recordType updateVal;

				if (name.size() > 0){ updateVal["name"] = name; }
				if (uuid.size() > 0){ updateVal["uuid"] = uuid; }
				if (major >= 0){ updateVal["major"] = std::to_string(major); }
				if (minor >= 0){ updateVal["minor"] = std::to_string(minor); }
				if (tx_power >= 0){ updateVal["tx_power"] = std::to_string(tx_power); }
				if (rssi >= 0){ updateVal["rssi"] = std::to_string(rssi); }
				if (time_stamp >= 0){ updateVal["time_stamp"] = std::to_string(time_stamp); }
				if (valid >= 0){ updateVal["valid"] = std::to_string(valid); }
				if (area.size() >= 0){ updateVal["area"] = area; }

				if (pointx.size() > 0){ updateVal["pointx"] = pointx; }
				if (pointy.size() > 0){ updateVal["pointy"] = pointy; }


				std::string updCond = " where `id`='" + std::to_string(id) + "'";
				bool ret = CDb::instance()->updateIBeacon(updCond.c_str(), updateVal);
				if (!ret)
				{
					throw std::exception("update ibeacon failed");
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