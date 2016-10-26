#include "stdafx.h"
#include "MySQL.h"
#include <ostream>
#include <chrono>
#include <vector>
#include <sstream>
#include <tchar.h>

#pragma comment(lib, "libmysql.lib")

CMySQL::CMySQL(void)
	: m_pMysqlConnection(NULL)
	, m_bStoped(false)
	, m_bIsConnected(false)
{
}


CMySQL::~CMySQL(void)
{
	m_bStoped = true;
	m_hExitEvent.notify_all();
	//m_pingThread.join();
}

bool CMySQL::open(const char* host, unsigned short port, const char* user, const char* pass, const char* db)
{
	if (NULL == host || 0 == port || NULL == user || NULL == pass || NULL == db)
	{
		return false;
	}

	if (m_pMysqlConnection)
	{
		close();
	}

	try{
		m_pMysqlConnection = mysql_init(NULL);
		if (NULL == m_pMysqlConnection)
		{
			throw std::exception("init mysql failed");
		}

		my_bool reconnect = 1;
		mysql_options(m_pMysqlConnection, MYSQL_OPT_RECONNECT, &reconnect);

		if (!mysql_real_connect(m_pMysqlConnection, host, user, pass, db, port, NULL, 1))
		{
			const char * pstrError = mysql_error(m_pMysqlConnection);
			if (!mysql_real_connect(m_pMysqlConnection, host, user, pass, NULL, port, NULL, 0))
			{
				const char * pstrError = mysql_error(m_pMysqlConnection);
				throw std::exception(pstrError);
			}

			// create db
			std::string sql = "create database ";
			sql += db;
			if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
			{
				const char * pstrError = mysql_error(m_pMysqlConnection);
				throw std::exception(pstrError);
			}
			else
			{
				if (mysql_errno(m_pMysqlConnection))
				{
					const char * pstrError = mysql_error(m_pMysqlConnection);
					throw std::exception(pstrError);
				}
			}
		}

		my_bool autocommit = 0;
		mysql_autocommit(m_pMysqlConnection, autocommit);

		mysql_set_character_set(m_pMysqlConnection, "utf8");
		mysql_select_db(m_pMysqlConnection, db);

		m_bIsConnected = true;

		// start ping thread
		m_pingThread = std::thread([&](){
			while (!m_bStoped)
			{
				if (m_bIsConnected && m_pMysqlConnection)
				{
					std::lock_guard<std::mutex> locker(m_tcpChannelUseLocker);

					if (mysql_ping(m_pMysqlConnection))
					{
						const char* pstrError = mysql_error(m_pMysqlConnection);
						m_bIsConnected = false;
					}
				}

				// wait quit event and timeout
				std::unique_lock<std::mutex> lk(this->m_mtxQuit);
				this->m_hExitEvent.wait_for(lk, std::chrono::seconds(1));
			}
		});
	}
	catch (std::exception e){
		return false;
	}
	catch (...){
		return false;
	}

	return true;
}

bool CMySQL::createTable(const char* tbSql)
{
	try
	{
		if (!m_bIsConnected)
		{
			throw std::exception("MySQL is not connected");
		}

		if (NULL == tbSql)
		{
			throw std::exception("table name is null");
		}

		std::lock_guard<std::mutex> locker(m_tcpChannelUseLocker);

		if (mysql_real_query(m_pMysqlConnection, tbSql, strlen(tbSql)))
		{
			const char* pstrError = mysql_error(m_pMysqlConnection);
			throw std::exception(pstrError);
		}
	}
	catch (std::exception e)
	{
		return false;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

int CMySQL::insert(const char* table, recordType &record)
{
	try{
		if (!m_bIsConnected)
		{
			throw std::exception("MySQL is not connected");
		}

		std::string fields = "", values = "";
		for (auto i = record.begin(); i != record.end(); i++)
		{
			fields += "`";
			fields += i->first;
			fields += "`,";

			values += "'";
			values += EscapeCharacter(i->second);
			values += "',";
		}

		fields.erase(fields.begin() + (fields.size() - 1));
		values.erase(values.begin() + (values.size() - 1));

		std::stringstream stream;
		stream << "insert into `" << table << "` (" << fields << ") values (" << values << ")";

		std::string sql = stream.str();

		std::lock_guard<std::mutex> locker(m_tcpChannelUseLocker);

		if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
		{
			const char* pstrError = mysql_error(m_pMysqlConnection);
			throw std::exception(pstrError);
		}

		if (mysql_affected_rows(m_pMysqlConnection) <= 0)
		{
			throw std::exception("affect 0 row for insert operation");
		}
	}
	catch (std::exception e){
		return 0;
	}
	catch (...){
		return 0;
	}
	return 1;
}


int CMySQL::del(const char* table, const char* condition)
{
	try{
		if (!m_bIsConnected)
		{
			throw std::exception("MySQL is not connected");
		}

		if (NULL == table || NULL == condition)
		{
			throw std::exception("table or condition is null");
		}


		std::stringstream stream;
		stream << "delete from `" << table << "` " << condition;
		std::string sql = stream.str();

		std::lock_guard<std::mutex> lock(m_tcpChannelUseLocker);

		if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
		{
			const char* pstrError = mysql_error(m_pMysqlConnection);
			throw std::exception(pstrError);
		}

		if (mysql_affected_rows(m_pMysqlConnection) <= 0)
		{
			throw std::exception("affect 0 row for del operation");
		}
	}
	catch (std::exception e){
		return 0;
	}
	catch (...){
		return 0;
	}
	return 1;
}


void CMySQL::close()
{
	m_bStoped = true;
	m_hExitEvent.notify_all();
	m_pingThread.join();

	if (m_pMysqlConnection)
	{
		mysql_close(m_pMysqlConnection);
		m_pMysqlConnection = NULL;
	}
}

int CMySQL::update(const char* table, recordType& val, const char* condition)
{
	try{
		if (NULL == table || NULL == condition)
		{
			throw std::exception("table or condition is null");
		}
		if (!m_bIsConnected)
		{
			throw std::exception("MySQL is not connected");
		}

		std::string strUpdate = "";
		for (auto i = val.begin(); i != val.end(); ++i)
		{
			std::stringstream s;
			s << "`" << i->first << "`='" << EscapeCharacter(i->second) << "',";

			strUpdate += s.str();
		}
		strUpdate = strUpdate.substr(0, strUpdate.size() - 1);

		std::stringstream stream;
		stream << "update `" << table << "` set " << strUpdate << " " << condition;

		std::string sql = stream.str();

		std::lock_guard<std::mutex> lock(m_tcpChannelUseLocker);

		if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
		{
			const char* pstrError = mysql_error(m_pMysqlConnection);
			throw std::exception(pstrError);
		}

		commit();

		//if (mysql_affected_rows(m_pMysqlConnection) <= 0)
		//{
		//	throw std::exception("affect 0 row");
		//}
	}
	catch (std::exception e)
	{
		return 0;
	}
	catch (...)
	{
		return 0;
	}

	return 1;
}
int CMySQL::find(const char* table, const char* condition, std::list<recordType> &records)
{
	records.erase(records.begin(), records.end());

	try{
		if (NULL == table)
		{
			throw std::exception("table not found");
		}

		std::string sql = "select * from ";
		sql += table;
		if (condition)
		{
			sql += " ";
			sql += condition;
		}

		std::lock_guard<std::mutex> locker(m_tcpChannelUseLocker);
		if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
		{
			const char* pstrError = mysql_error(m_pMysqlConnection);
			throw std::exception(pstrError);
		}

		MYSQL_RES *result = NULL;

		result = mysql_store_result(m_pMysqlConnection);
		if (result)
		{
			MYSQL_ROW	row;
			int num_fields;
			MYSQL_FIELD *field;

			num_fields = mysql_num_fields(result);
			std::vector<std::string> tableFields;
			while ((field = mysql_fetch_field(result)))
			{
				tableFields.push_back(field->name);
			}

			while ((row = mysql_fetch_row(result)))
			{
				std::map<std::string, std::string> rowValue;
				for (int i = 0; i < num_fields; i++)
				{
					rowValue[tableFields[i]] = row[i];
				}
				records.push_back(rowValue);
			}
		}
		else
		{
			/*int affectRows = mysql_affected_rows(m_pMysqlConnection);*/
			//LOG(INFO) << "no data" << mysql_error(m_pMysqlConnection);
			throw std::exception(mysql_error(m_pMysqlConnection));
		}

		if (result)
		{
			mysql_free_result(result);
		}
	}
	catch (std::exception e)
	{
		return 0;
	}
	catch (...)
	{
		return 0;
	}
	return records.size();
}
int CMySQL::findLastOne(const char* table, recordType& record)
{
	try
	{
		if (NULL == table)
		{
			throw std::exception("table not found");
		}

		record.erase(record.begin(), record.end());

		std::string sql = "select * from ";
		sql += table;
		sql += " order by id desc limit 1";

		std::lock_guard<std::mutex> locker(m_tcpChannelUseLocker);
		if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
		{
			const char* pstrError = mysql_error(m_pMysqlConnection);
			throw std::exception(pstrError);
		}

		MYSQL_RES *result = NULL;

		result = mysql_store_result(m_pMysqlConnection);
		if (result)
		{
			MYSQL_ROW	row;
			int num_fields;
			MYSQL_FIELD *field;

			num_fields = mysql_num_fields(result);
			std::vector<std::string> tableFields;
			while ((field = mysql_fetch_field(result)))
			{
				tableFields.push_back(field->name);
			}

			while ((row = mysql_fetch_row(result)))
			{
				for (int i = 0; i < num_fields; i++)
				{
					record[tableFields[i]] = row[i];
				}
			}
		}
		else{
			throw std::exception("no record");
		}
	}
	catch (std::exception e)
	{
		return 0;
	}
	catch (...)
	{
		return 0;
	}

	return 1;
}

std::string CMySQL::EscapeCharacter( const std::string& ss )
{
	std::stringstream s;
	for (auto i = ss.begin(); i != ss.end(); ++i)
	{
		s << (*i);
		if ((*i) == '\'')
		{
			s << (*i);
		}
	}
	return s.str();
}

void CMySQL::startTransaction()
{
	std::string strSql = "START TRANSACTION";
	int ret = mysql_real_query(m_pMysqlConnection, strSql.c_str(), strSql.size());
	printf(mysql_error(m_pMysqlConnection));
}

void CMySQL::commit()
{
	std::string strSql = "COMMIT";
	int ret = mysql_real_query(m_pMysqlConnection, strSql.c_str(), strSql.size());
}

void CMySQL::rollback()
{
	std::string strSql = "ROLLBACK";
	int ret = mysql_real_query(m_pMysqlConnection, strSql.c_str(), strSql.size());
}

bool CMySQL::recordExist(const char* table, recordType record)
{
	std::list<recordType> records;

	std::string condition = "";
	for (auto i = record.begin(); i != record.end(); i++)
	{
		condition += "`" + i->first;
		condition += "`='";
		condition += i->second;
		condition += "' and ";
	}

	condition = condition.substr(0, condition.size() - 4);
	condition = "where " + condition;

	int ret = find(table, condition.c_str(), records);
	return (ret > 0);
}

int CMySQL::count(const char* table, const char* condition)
{
	int total = 0;
	try{
		if (NULL == table)
		{
			throw std::exception("table not found");
		}

		std::string sql = "select count(*) as total from ";
		sql += table;
		if (condition)
		{
			sql += " ";
			sql += condition;
		}

		std::lock_guard<std::mutex> locker(m_tcpChannelUseLocker);
		if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
		{
			const char* pstrError = mysql_error(m_pMysqlConnection);
			throw std::exception(pstrError);
		}

		MYSQL_RES *result = NULL;

		result = mysql_store_result(m_pMysqlConnection);
		if (result)
		{
			MYSQL_ROW	row;
			if ((row = mysql_fetch_row(result)))
			{
				total = std::atoi(row[0]);
			}
		}
		else
		{
			/*int affectRows = mysql_affected_rows(m_pMysqlConnection);*/
			//LOG(INFO) << "no data" << mysql_error(m_pMysqlConnection);
			throw std::exception(mysql_error(m_pMysqlConnection));
		}

		if (result)
		{
			mysql_free_result(result);
		}
	}
	catch (std::exception e)
	{
		return 0;
	}
	catch (...)
	{
		return 0;
	}

	return total;
}