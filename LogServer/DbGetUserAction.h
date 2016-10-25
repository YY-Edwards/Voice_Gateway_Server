#pragma once

#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#include "Db.h"

void getUserAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	Document d;
	std::string strResp;
	
	try{
		d.Parse(param.c_str());
		std::list<CCriteria> condition;

		if (d.HasMember("condition") && d["condition"].IsArray())
		{
			condition = CRpcJsonParser::parseCondition(d["condition"].GetArray());
		}

		int offset = 0;
		int count = 10;

		if (d.HasMember("offset"))
		{
			offset = d["offset"].GetInt();
		}
		if (d.HasMember("count"))
		{
			count = d["count"].GetInt();
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

		CDb::instance()->query("user", condStr.c_str(), records);
		ArgumentType args;

		FieldValue fvRecords(FieldValue::TArray);
		for (auto i = records.begin(); i != records.end(); i++)
		{
			FieldValue r(FieldValue::TObject);

			for (auto j = (*i).begin(); j != (*i).end(); j++){
				FieldValue fv(FieldValue::TString);
				fv.setString(j->second.c_str());
				r.setKeyVal(j->first.c_str(), fv);
			}
			fvRecords.push(r);
		}
		
		args["records"] = fvRecords;
		strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);

	}
	catch (std::exception e){
		strResp = CRpcJsonParser::buildResponse("failed", callId, 500, "", ArgumentType());
	}
	catch (...)
	{
		strResp = CRpcJsonParser::buildResponse("failed", callId, 500, "", ArgumentType());
	}

	pRemote->sendResponse(strResp.c_str(), strResp.size());
}