#pragma once

#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "Db.h"

void groupAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
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
				CDb::instance()->query("department", condStr.c_str(), records);

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
				int recordCount = CDb::instance()->count("department", condStr.c_str());
				args["count"] = FieldValue(recordCount);
			}

			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);
		}
		else if (0 == operation.compare("add"))
		{
			if (!d.HasMember("departments") || !d["departments"].IsArray())
			{
				throw std::exception("call parameter error, users key must be an array");
			}
			int itemCount = d["departments"].Size();

			for (size_t i = 0; i < itemCount; i++)
			{
				std::string name = d["departments"][i]["name"].GetString();
				int gid = d["departments"][i]["gid"].GetInt();
				bool ret = CDb::instance()->insertDepartment(name.c_str(), gid);
				if (!ret)
				{
					std::string errMsg = "add department failed.";
					errMsg += CDb::instance()->getLastError();
					throw std::exception(errMsg.c_str());
				}
			}
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
		}
		else if (0 == operation.compare("update"))
		{
			if (!d.HasMember("departments") || !d["departments"].IsArray())
			{
				throw std::exception("call parameter error, users key must be an array");
			}
			int itemCount = d["departments"].Size();

			for (size_t i = 0; i < itemCount; i++)
			{
				std::string id = (rapidjson::kNumberType == d["departments"][i]["id"].GetType()) ?
					std::to_string(d["departments"][i]["id"].GetInt())
					: d["departments"][i]["id"].GetString();

				rapidjson::Value& val = d["departments"][i]["department"];

				std::string name = val.HasMember("name") ? val["name"].GetString() : "";
				int gid = val.HasMember("gid") ? val["gid"].GetInt() : -1;

				bool ret = CDb::instance()->updateDepartment(std::atoi(id.c_str()), name.c_str(), gid);
				if (!ret)
				{
					throw std::exception("update department failed");
				}
			}
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
		}
		else if (0 == operation.compare("del"))
		{
			if (!d.HasMember("departments") || !d["departments"].IsArray())
			{
				throw std::exception("call parameter error, departments key must be an array");
			}

			for (int m = 0; m < d["departments"].Size(); m++)
			{
				int id = d["departments"][m].GetInt();
				std::string condition = "where id=" + std::to_string(id);
				CDb::instance()->del("department", condition.c_str());
			}
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
		}
		else if (0 == operation.compare("assignUser"))
		{
			if (!d.HasMember("department") || !d["department"].IsInt() 
				|| !d.HasMember("user") || !d["user"].IsInt())
			{
				throw std::exception("call parameter error");
			}

			int userId = d["user"].GetInt();
			int departmentId = d["department"].GetInt();

			if (CDb::instance()->assignUser(userId, departmentId))
			{
				strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
			}
			else
			{
				throw std::exception("failed assign user");
			}
		}
		else if (0 == operation.compare("detachUser"))
		{
			if (!d.HasMember("department") || !d["department"].IsInt()
				|| !d.HasMember("user") || !d["user"].IsInt())
			{
				throw std::exception("call parameter error");
			}

			int userId = d["user"].GetInt();
			int departmentId = d["department"].GetInt();

			if (CDb::instance()->detachUser(userId, departmentId))
			{
				strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
			}
			else
			{
				throw std::exception("failed detach user");
			}
		}
		else if (0 == operation.compare("assignRadio"))
		{
			if (!d.HasMember("department") || !d["department"].IsInt()
				|| !d.HasMember("radio") || !d["radio"].IsInt())
			{
				throw std::exception("call parameter error");
			}

			int radioId = d["radio"].GetInt();
			int departmentId = d["department"].GetInt();

			if (CDb::instance()->assignDepartmentRadio(radioId, departmentId))
			{
				strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
			}
			else
			{
				throw std::exception("failed assign radio");
			}
		}
		else if (0 == operation.compare("detachRadio"))
		{
			if (!d.HasMember("department") || !d["department"].IsInt()
				|| !d.HasMember("radio") || !d["radio"].IsInt())
			{
				throw std::exception("call parameter error");
			}

			int radioId = d["radio"].GetInt();
			int departmentId = d["department"].GetInt();

			if (CDb::instance()->detachDepartmentRadio(radioId, departmentId))
			{
				strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", ArgumentType());
			}
			else
			{
				throw std::exception("failed detach radio");
			}
		}
		else if (0 == operation.compare("listUser"))
		{
			ArgumentType args;
			std::list<recordType> records;
			if (!d.HasMember("department") || !d["department"].IsInt())
			{
				throw std::exception("call parameter error");
			}
			int departmentId = d["department"].GetInt();
			CDb::instance()->listDepartmentStaff(departmentId, records);

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
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);
		}
		else if (0 == operation.compare("listRadio"))
		{
			ArgumentType args;
			std::list<recordType> records;
			if (!d.HasMember("department") || !d["department"].IsInt())
			{
				throw std::exception("call parameter error");
			}
			int departmentId = d["department"].GetInt();
			CDb::instance()->listDepartmentRadio(departmentId, records);

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
			strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);
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