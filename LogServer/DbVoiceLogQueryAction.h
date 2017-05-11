#pragma once

#include <string>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "../lib/rpc/include/TcpServer.h"

#include "Db.h"

typedef struct
{
	char condition[4][64];
}Condition;
typedef struct
{
	Condition conditions[64];
	char sort[2][64];
	int offset;
	int count;
}Critera;
typedef struct
{
	char operation[64];
	Critera critera;
}VoiceQureyParam;
typedef struct
{
	//char call[64];
	//int callId;
	VoiceQureyParam param;
}VOICE_LOG;

#define CLIENT_TRANSFER_OK "success"
#define CLIENT_TRANSFER_FAIL "fail"

inline void voiceLogQueryAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	Document d;
	int errorCode = 0;
	ArgumentType args;
	std::string strResp = "";
	char status[64] = { 0 };
	strcpy_s(status, CLIENT_TRANSFER_OK);
	std::string statusText = "";
	VOICE_LOG voiceLog = { 0 };
	Value tempJsonSub, sortArray, conditionsArray, conditionArray;
	try
	{
		d.Parse(param.c_str());
		VoiceQureyParam *pVoiceQureyParam = &(voiceLog.param);
		strcpy_s(pVoiceQureyParam->operation, d["operation"].GetString());
		if (!d["critera"].IsObject())
		{
			strcpy_s(status, CLIENT_TRANSFER_FAIL);
		}
		else
		{
			tempJsonSub = d["critera"].GetObject();
			pVoiceQureyParam->critera.count = tempJsonSub["count"].GetInt();
			pVoiceQureyParam->critera.offset = tempJsonSub["offset"].GetInt();
			if (!tempJsonSub["sort"].IsArray() || !tempJsonSub["condition"].IsArray())
			{
				strcpy_s(status, CLIENT_TRANSFER_FAIL);
			}
			else
			{
				sortArray = tempJsonSub["sort"].GetArray();
				int size = sortArray.Size();
				for (int i = 0; i < size; i++)
				{
					strcpy_s(pVoiceQureyParam->critera.sort[i], sortArray[i].GetString());
				}
				conditionsArray = tempJsonSub["condition"].GetArray();
				size = conditionsArray.Size();
				bool invalid = false;
				for (int i = 0; i < size; i++)
				{
					if (conditionsArray[i].IsArray())
					{
						conditionArray = conditionsArray[i].GetArray();
						int sizeSub = conditionArray.Size();
						for (int j = 0; j < sizeSub; j++)
						{
							strcpy_s(pVoiceQureyParam->critera.conditions[i].condition[j], conditionArray[j].GetString());
						}
					}
					else
					{
						invalid = true;
						break;
					}
				}
				if (!invalid)
				{
					strcpy_s(status, CLIENT_TRANSFER_FAIL);
				}
				else
				{
					if ("list" == voiceLog.param.operation)
					{
						std::list<recordType> templList, list;
						std::string condition = NULL;
						std::string tempCondition = NULL;
						std::string tableName = NULL;
						CDb* m_pDb = CDb::instance();
						tableName = "voice";
						tempCondition += "where ";
						int which = 0;
						std::map<int, int> years;
						std::map<int, int>::iterator it;
						while (NULL != voiceLog.param.critera.conditions[which++].condition[0])
						{
							std::string andOr, name, operators, value;
							int index = which - 1;
							if (index == 0)
							{
								name = voiceLog.param.critera.conditions[index].condition[1];
								operators = voiceLog.param.critera.conditions[index].condition[0];
								value = voiceLog.param.critera.conditions[index].condition[2];
								tempCondition += name;
								tempCondition += " ";
								tempCondition += operators;
								tempCondition += " ";
								tempCondition += value;
								tempCondition += " ";
								if (name == "time")
								{
									int year, month, day, hour, minute, second;
									sscanf_s(value.c_str(), "%04d-%02d-%02d %02d:%02d:%02d", &year, &month, &day, &hour, &minute, &second);
									years[year] = 0;
								}
							}
							else
							{
								andOr = voiceLog.param.critera.conditions[index].condition[0];
								name = voiceLog.param.critera.conditions[index].condition[2];
								operators = voiceLog.param.critera.conditions[index].condition[1];
								value = voiceLog.param.critera.conditions[index].condition[3];
								tempCondition += andOr;
								tempCondition += " ";
								tempCondition += name;
								tempCondition += " ";
								tempCondition += operators;
								tempCondition += " ";
								tempCondition += value;
								tempCondition += " ";
								if (name == "time")
								{
									int year, month, day, hour, minute, second;
									sscanf_s(value.c_str(), "%04d-%02d-%02d %02d:%02d:%02d", &year, &month, &day, &hour, &minute, &second);
									years[year] = 0;
								}
							}
						}

						std::list<int> lst;
						for (auto i = years.begin(); i != years.end(); i++)
						{
							lst.push_back(i->first);
						}
						lst.sort();

						int count = voiceLog.param.critera.count;
						int offset = voiceLog.param.critera.offset;
						std::string orderByName = voiceLog.param.critera.sort[0];
						std::string orderByOrder = voiceLog.param.critera.sort[1];
						for each (int var in lst)
						{
							char temp[1024] = { 0 };
							sprintf_s(temp, "%s_%d", tableName.c_str(), var);
							tableName = temp;
							if (count <= 0)
							{
								break;
							}
							else
							{
								sprintf_s(temp, "%s order by %s %s limit %d,%d", tempCondition.c_str()
									, orderByName.c_str()
									, orderByOrder.c_str()
									, offset
									, count);
								condition = temp;
								m_pDb->query(tableName.c_str(), condition.c_str(), templList);
								count -= (templList.size());
								offset = 0;
								list.insert(list.end(), templList.begin(), templList.end());
							}
						}
						//ArgumentType args;
						FieldValue contents(FieldValue::TArray);
						FieldValue records(FieldValue::TArray);

						for (auto i = list.begin(); i != list.end(); i++)
						{
							FieldValue r(FieldValue::TObject);

							for (auto j = (*i).begin(); j != (*i).end(); j++){
								r.setKeyVal(j->first.c_str(), FieldValue(j->second.c_str()));
							}
							records.push(r);
						}
						contents.push(records);
						args["contents"] = contents;
					}
				}
			}
		}
	}
	catch (...)
	{
		strcpy_s(status, CLIENT_TRANSFER_FAIL);
	}
	/*·¢ËÍµ½Client*/
	std::string strResp = CRpcJsonParser::buildResponse(status, callId, 200, "", args);
	pRemote->sendResponse(strResp.c_str(), strResp.size());
}