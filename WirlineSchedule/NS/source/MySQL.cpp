#include "StdAfx.h"
#include "../include/MySQL.h"

#include <ostream>
#include <sstream>
#include <vector>

#pragma comment(lib, "libmysql.lib")

CMySQL::CMySQL(void)
: m_pMysqlConnection(NULL)
, m_bRunning(FALSE)
, m_bIsConnected(FALSE)
{
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_mutxDbLocker = CreateMutex(NULL, FALSE, NULL);
	//m_strCurrentTableName = "";
	ZeroMemory(m_strCurrentTableName, DATA_TABLE_NAME_SIZE);
	m_report = NULL;
	ZeroMemory(m_reportMsg, 512);
}


CMySQL::~CMySQL(void)
{
	if (m_hExitEvent != INVALID_HANDLE_VALUE)
	{
		SetEvent(m_hExitEvent);
	}

	if (m_pMysqlConnection)
	{
		//mysql_close(m_pMysqlConnection);
		//m_pMysqlConnection = NULL;
	}

	ReleaseMutex(m_mutxDbLocker);
	CloseHandle(m_mutxDbLocker);
	stop();
}

BOOL CMySQL::CreateTable(LPCSTR pstrSQL)
{
	if (pstrSQL == NULL)
	{
		return FALSE;
	}

	if (WAIT_ABANDONED == WaitForSingleObject(m_mutxDbLocker, INFINITE))
	{
		return FALSE;
	}

	if (mysql_real_query(m_pMysqlConnection, pstrSQL, strlen(pstrSQL)))
	{
		LPCSTR pstrError = mysql_error(m_pMysqlConnection);
		//LOG(ERROR)<<pstrError;
		ReleaseMutex(m_mutxDbLocker);
		return FALSE;
	}

	ReleaseMutex(m_mutxDbLocker);
	return TRUE;
}

BOOL CMySQL::Open(LPCSTR pstrHost, WORD port, LPCSTR pstrUserName, LPCSTR pstrPassword, LPCSTR pstrDbName)
{
	//LOG(INFO) << "CMySQL::Open entry";
	if (pstrDbName == NULL)
	{
		return FALSE;
	}

	if (m_pMysqlConnection)
	{
		stop();
	}

	m_pMysqlConnection = mysql_init(NULL);
	if (NULL == m_pMysqlConnection)
	{
		//LOG(ERROR)<<"init mysql error";
		return FALSE;
	}
	my_bool reconnect = 1;
	mysql_options(m_pMysqlConnection, MYSQL_OPT_RECONNECT, &reconnect);

	if (!mysql_real_connect(m_pMysqlConnection, pstrHost, pstrUserName,
		pstrPassword, pstrDbName, port, NULL, 1))
	{
		if (!mysql_real_connect(m_pMysqlConnection, pstrHost, pstrUserName, pstrPassword, NULL, port, NULL, 0))
		{
			LPCSTR pstrError = mysql_error(m_pMysqlConnection);
			//LOG(ERROR)<<pstrError;
			return FALSE;
		}
		// create db
		std::string sql = "create database ";
		sql += pstrDbName;
		if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
		{
			LPCSTR pstrError = mysql_error(m_pMysqlConnection);
			//LOG(ERROR)<<pstrError;
			mysql_close(m_pMysqlConnection);
			m_pMysqlConnection = NULL;
			return FALSE;
		}
		else
		{
			if (mysql_errno(m_pMysqlConnection))
			{
				LPCSTR pstrError = mysql_error(m_pMysqlConnection);
				//LOG(ERROR)<<pstrError;
				mysql_close(m_pMysqlConnection);
				m_pMysqlConnection = NULL;
				return FALSE;
			}
		}
	}

	mysql_set_character_set(m_pMysqlConnection, "gbk");
	mysql_select_db(m_pMysqlConnection, pstrDbName);

	m_bIsConnected = TRUE;

	//char migrationTableName[DATA_TABLE_NAME_SIZE] = "migration";
	////std::map < std::string, std::string > versionRecord;
	////versionRecord["version"] = VERSION_CURRENT_APP;
	////string change_1st_version = VERSION_CHANGE_1ST_APP;
	////string change_2nd_version = VERSION_CHANGE_2ND_APP;
	///*检查是否存在表 migration*/
	////存在
	//if (CheckTableExist(migrationTableName))
	//{
	//	//std::vector<std::map<std::string, std::string> > records;
	//	//GetRows(migrationTableName, records, " order by version desc limit 0,1 ");

	//	//CStringA version;
	//	//int intVersion = 0;
	//	//for (auto i = records.begin(); i != records.end(); i++)
	//	//{
	//	//	version = (*i)["version"].c_str();
	//	//	intVersion = atoi(version);
	//	//	//未执行第一次改动
	//	//	if (intVersion < (atoi(change_1st_version.c_str())))
	//	//	{
	//	//		/*执行第一次改动*/
	//	//		addPeerIdCoulumn();
	//	//		/*执行第二次改动*/
	//	//		addSlotAndRssiCoulumn();
	//	//		/*更新迁移*/
	//	//		InsertRow(migrationTableName, versionRecord);
	//	//		break;
	//	//	}
	//	//	//未执行第二次改动
	//	//	else if (intVersion < (atoi(change_2nd_version.c_str())))
	//	//	{
	//	//		/*执行第二次改动*/
	//	//		addSlotAndRssiCoulumn();
	//	//		/*更新迁移*/
	//	//		InsertRow(migrationTableName, versionRecord);
	//	//		break;
	//	//	}
	//	//	else
	//	//	{
	//	//		break;
	//	//	}
	//	//}

	//	//if (records.size() <= 0)
	//	//{
	//	//	/*执行第一次改动*/
	//	//	addPeerIdCoulumn();
	//	//	/*执行第二次改动*/
	//	//	addSlotAndRssiCoulumn();
	//	//	/*更新迁移*/
	//	//	InsertRow(migrationTableName, versionRecord);
	//	//}

	//}
	//else
	//{
	//	//创建表migration
	//	CreateTable("CREATE TABLE IF NOT EXISTS `migration` (\
	//				`id` int(11) NOT NULL AUTO_INCREMENT,\
	//				`version` int(11) NOT NULL,\
	//				PRIMARY KEY (`id`)\
	//				) ENGINE=InnoDB  DEFAULT CHARSET=utf8"
	//				);
	//	////所有现有数据表voice_*增加src_peer_id列
	//	///*执行第一次改动*/
	//	//addPeerIdCoulumn();
	//	///*执行第二次改动*/
	//	//addSlotAndRssiCoulumn();
	//	///*更新迁移*/
	//	//InsertRow(migrationTableName, versionRecord);
	//}

	//if (CheckTableExist("tbl_user"))
	//{
	//	DeleteTable("tbl_user");
	//}

	//if (!CheckTableExist("tbl_account"))
	//{
	//	CreateTable("CREATE TABLE IF NOT EXISTS `tbl_account` (\
		//														`id` int(11) NOT NULL AUTO_INCREMENT,\
		//																													`account` varchar(64) NOT NULL,\
		//																																																	`password` varchar(64) NOT NULL,\
		//																																																																										`name` varchar(64) NOT NULL,\
		//																																																																																																								`root` int(11) NOT NULL,\
		//																																																																																																																																											`radio` int(11) NOT NULL,\
		//																																																																																																																																																																																			PRIMARY KEY (`id`)\
		//																																																																																																																																																																																																																																) ENGINE=InnoDB  DEFAULT CHARSET=utf8");
	//}

	//if (!CheckTableExist("tbl_log"))
	//{
	//	CreateTable("CREATE TABLE IF NOT EXISTS `tbl_log` (\
		//				`id` int(11) NOT NULL AUTO_INCREMENT,\
		//				`time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,\
		//				`content` varchar(64) NOT NULL,\
		//				`is_ui` int(11) NOT NULL DEFAULT '0', \
		//				PRIMARY KEY (`id`)\
		//				) ENGINE=InnoDB  DEFAULT CHARSET=utf8");
	//}

	CreateCurrentTableByYearMonth();

	m_bRunning = TRUE;
	m_hPingThread = CreateThread(NULL, 0, PingThread, this, 0, NULL);

	//LOG(INFO) << "CMySQL::Open exit";
	return TRUE;
}

void CMySQL::stop()
{
	if (m_pMysqlConnection)
	{
		SetEvent(m_hExitEvent);
		m_bRunning = FALSE;
		WaitForSingleObject(m_hPingThread, 3 * 1000);
		mysql_close(m_pMysqlConnection);
		m_pMysqlConnection = NULL;
	}
}

BOOL CMySQL::InsertRow(LPCSTR pstrTableName, std::map<std::string, std::string>& v)
{
	//LOG(INFO) << "CMySQL::InsertRow entry";
	if (pstrTableName == NULL)
	{
		return FALSE;
	}

	std::string fields = "", values = "";
	for (auto i = v.begin(); i != v.end(); i++)
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
	stream << "insert into `" << pstrTableName << "` (" << fields << ") values (" << values << ")";

	std::string sql = stream.str();

	if (WAIT_ABANDONED == WaitForSingleObject(m_mutxDbLocker, INFINITE))
	{
		return FALSE;
	}

	if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
	{
		LPCSTR pstrError = mysql_error(m_pMysqlConnection);
		//LOG(ERROR)<<pstrError<<"--sql:"<<sql;
		ReleaseMutex(m_mutxDbLocker);
		return FALSE;
	}

	if (mysql_affected_rows(m_pMysqlConnection) <= 0)
	{
		ReleaseMutex(m_mutxDbLocker);
		return FALSE;
	}

	ReleaseMutex(m_mutxDbLocker);
	//LOG(INFO) << "CMySQL::InsertRow exit";

	return TRUE;
}

BOOL CMySQL::GetRows(LPCSTR pstrTableName, std::vector<std::map<std::string, std::string> >& v, LPCSTR pstrCondition)
{
	if (pstrTableName == NULL)
	{
		return FALSE;
	}

	//LOG(INFO) << "CMySQL::GetRows entry";

	if (!m_bIsConnected)
	{
		return FALSE;
	}

	BOOL ret = FALSE;

	std::string sql = "select * from ";
	sql += pstrTableName;
	if (pstrCondition)
	{
		sql += pstrCondition;
	}

	if (WAIT_ABANDONED == WaitForSingleObject(m_mutxDbLocker, INFINITE))
	{
		return FALSE;
	}

	if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
	{
		LPCSTR pstrError = mysql_error(m_pMysqlConnection);
		//LOG(ERROR)<<pstrError;
		ReleaseMutex(m_mutxDbLocker);
		return ret;
	}

	MYSQL_RES *result = NULL;

	result = mysql_store_result(m_pMysqlConnection);
	if (result)
	{
		MYSQL_ROW row;
		//MYSQL_ROW end_row;
		int num_fields;
		MYSQL_FIELD *field;

		num_fields = mysql_num_fields(result);
		std::vector<std::string> tableFields;
		while ((field = mysql_fetch_field(result)))
		{
			// 			sprintf_s(m_reportMsg, "24");
			// 			sendLogToWindow();
			tableFields.push_back(field->name);
		}

		while ((row = mysql_fetch_row(result)))
		{
			// 			sprintf_s(m_reportMsg, "25");
			// 			sendLogToWindow();
			std::map<std::string, std::string> rowValue;
			for (int i = 0; i < num_fields; i++)
			{
				rowValue[tableFields[i]] = row[i];
			}
			v.push_back(rowValue);
		}

		ret = TRUE;
	}
	else
	{
		/*int affectRows = mysql_affected_rows(m_pMysqlConnection);*/
		//LOG(INFO)<<"no data"<<mysql_error(m_pMysqlConnection);
	}

	if (result)
	{
		mysql_free_result(result);
	}

	ReleaseMutex(m_mutxDbLocker);
	//LOG(INFO) << "CMySQL::GetRows exit";
	return ret;
}

std::map<std::string, std::string> CMySQL::GetLastRow(LPCSTR pstrTableName)
{
	std::vector<std::map<std::string, std::string> > records;

	if (GetRows(pstrTableName, records, " order by id desc limit 1"))
	{
		return records[0];
	}

	return std::map<std::string, std::string>();
}

BOOL CMySQL::RemoveRow(LPCSTR pstrTableName, LPCSTR pstrCondition)
{
	if (pstrCondition == NULL || pstrTableName)
	{
		return FALSE;
	}

	if (!m_bIsConnected)
	{
		return FALSE;
	}

	std::stringstream stream;
	stream << "delete from `" << pstrTableName << "` " << pstrCondition;
	std::string sql = stream.str();

	if (WAIT_ABANDONED == WaitForSingleObject(m_mutxDbLocker, INFINITE))
	{
		return FALSE;
	}

	if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
	{
		LPCSTR pstrError = mysql_error(m_pMysqlConnection);
		//LOG(ERROR)<<pstrError;
		ReleaseMutex(m_mutxDbLocker);
		return FALSE;
	}

	ReleaseMutex(m_mutxDbLocker);
	return TRUE;
}

BOOL CMySQL::CheckTableExist(LPCSTR pstrTableName)
{
	if (NULL == pstrTableName)
	{
		return FALSE;
	}

	char buf[100];
	ZeroMemory(buf, sizeof(buf));
	int n = sprintf_s(buf, sizeof(buf), "select 1 from %s limit 0", pstrTableName);

	if (WAIT_ABANDONED == WaitForSingleObject(m_mutxDbLocker, INFINITE))
	{
		return FALSE;
	}

	if (mysql_real_query(m_pMysqlConnection, buf, n))
	{
		ReleaseMutex(m_mutxDbLocker);
		return FALSE;
	}

	mysql_free_result(mysql_use_result(m_pMysqlConnection));

	ReleaseMutex(m_mutxDbLocker);
	return TRUE;
}

BOOL CMySQL::UpdateRow(LPCSTR pstrTableName, std::map<std::string, std::string>& v, LPCSTR pstrCondition)
{
	if (pstrCondition == NULL || pstrTableName == NULL || !m_bIsConnected)
	{
		return FALSE;
	}

	std::string strUpdate = "";
	for (auto i = v.begin(); i != v.end(); ++i)
	{
		std::stringstream s;
		s << "`" << i->first << "`='" << EscapeCharacter(i->second) << "',";

		strUpdate += s.str();
	}
	strUpdate = strUpdate.substr(0, strUpdate.size() - 1);

	std::stringstream stream;
	stream << "update `" << pstrTableName << "` set " << strUpdate << " " << pstrCondition;

	std::string sql = stream.str();

	if (WAIT_ABANDONED == WaitForSingleObject(m_mutxDbLocker, INFINITE))
	{
		return FALSE;
	}

	if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.size()))
	{
		LPCSTR pstrError = mysql_error(m_pMysqlConnection);
		//LOG(ERROR)<<pstrError;
		ReleaseMutex(m_mutxDbLocker);
		return FALSE;
	}
	unsigned long long nAffect = mysql_affected_rows(m_pMysqlConnection);
	if (nAffect <= 0)
	{
		//LOG(INFO)<<sql<<", not affect any row";
	}

	ReleaseMutex(m_mutxDbLocker);
	return TRUE;
}

DWORD WINAPI CMySQL::PingThread(LPVOID pMysql)
{
	CMySQL* pThis = reinterpret_cast<CMySQL*>(pMysql);

	while (pThis->m_bRunning)
	{
		//printf_s("26\r\n");
		int ret = WaitForSingleObject(pThis->m_hExitEvent, 500);
		if (WAIT_OBJECT_0)
		{
			CloseHandle(pThis->m_hExitEvent);
			ExitThread(0);
			return 0;
		}
		if (pThis && pThis->m_pMysqlConnection)
		{
			int r = WaitForSingleObject(pThis->m_mutxDbLocker, 200);
			if (WAIT_TIMEOUT == r)
			{
				continue;
			}

			if (WAIT_ABANDONED == r)
			{
				return 0;
			}

			if (mysql_ping(pThis->m_pMysqlConnection))
			{
				//LPCSTR pstrError = mysql_error(pThis->m_pMysqlConnection);
				//LOG(ERROR)<<pstrError;
				pThis->m_bIsConnected = FALSE;
			}
			else
			{
				pThis->m_bIsConnected = TRUE;
			}
			ReleaseMutex(pThis->m_mutxDbLocker);
		}
	}
	return 0L;
}

std::string CMySQL::EscapeCharacter(const std::string& ss)
{
	std::stringstream s;
	for (auto i = ss.begin(); i != ss.end(); ++i)
	{
		s << (*i);
		if ((*i) == '\'' || (*i) == '\\')
		{
			s << (*i);
		}
	}
	return s.str();
}

std::string CMySQL::CreateCurrentTableByYearMonth()
{
	char strTableName[DATA_TABLE_NAME_SIZE] = { 0 };
	SYSTEMTIME t = { 0 };
	GetLocalTime(&t);

	sprintf_s(strTableName, "voice_%4d", t.wYear);

	if (0 == strcmp(strTableName, m_strCurrentTableName))
	{
		return m_strCurrentTableName;
	}

	// create new table
	strcpy_s(m_strCurrentTableName, strTableName);

	if (!CheckTableExist(m_strCurrentTableName))
	{
		char strSql[1024] = { 0 };
		sprintf_s(strSql,
			"CREATE TABLE IF NOT EXISTS `%s` (\
			`id` int(11) NOT NULL AUTO_INCREMENT,\
			`src_radio` int(11) NOT NULL,\
			`target_radio` int(11) NOT NULL,\
			`time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,\
			`offset` int(11) NOT NULL,\
			`length` int(11) NOT NULL,\
			`call_type` int(11) NOT NULL,\
			`file_path` varchar(800) NOT NULL,\
			`record_type` int(11) NOT NULL, \
			`src_peer_id` int(11) NOT NULL, \
			`src_rssi` int(11) NOT NULL, \
			`src_slot` int(11) NOT NULL, \
			`call_status` int(11) NOT NULL, \
			PRIMARY KEY(`id`)\
			) ENGINE = InnoDB DEFAULT CHARSET = utf8 AUTO_INCREMENT = 1; ",
			strTableName);
		//size_t length = strlen(strSql);
		// create table
		BOOL ret = CreateTable(strSql);
		if (!ret)
		{
			//AfxMessageBox(_T("创建表失败"));
			sprintf_s(m_reportMsg, "build table %s fail", m_strCurrentTableName);
			sendLogToWindow();
		}
	}

	return m_strCurrentTableName;
}

BOOL CMySQL::InsertVoiceRecord(std::map<std::string, std::string>& v)
{
	strcpy_s(m_strCurrentTableName, CreateCurrentTableByYearMonth().c_str());
	return InsertRow(m_strCurrentTableName, v);
}
BOOL CMySQL::InsertLogRecord(std::string logRecord)
{
	std::string sql = "insert into tbl_log (content,is_ui) values (";
	sql += "'";
	sql += logRecord;
	sql += "',0);";
	if (WAIT_ABANDONED == WaitForSingleObject(m_mutxDbLocker, INFINITE))
	{
		return FALSE;
	}

	if (mysql_real_query(m_pMysqlConnection, sql.c_str(), sql.length()))
	{
		LPCSTR pstrError = mysql_error(m_pMysqlConnection);
		//LOG(ERROR)<<pstrError<<"--sql:"<<sql;
		ReleaseMutex(m_mutxDbLocker);
		return FALSE;
	}

	if (mysql_affected_rows(m_pMysqlConnection) <= 0)
	{
		ReleaseMutex(m_mutxDbLocker);
		return FALSE;
	}

	ReleaseMutex(m_mutxDbLocker);
	//LOG(INFO) << "CMySQL::InsertRow exit";

	return TRUE;
}
BOOL CMySQL::GetVoiceRows(std::vector<std::map<std::string, std::string> >& v, LPCSTR pstrCondition)
{
	std::string tableName = CreateCurrentTableByYearMonth();

	return GetRows(tableName.c_str(), v, pstrCondition);
}

BOOL CMySQL::GetVoiceRows(std::vector<std::map<std::string, std::string> >& v,
	LPCSTR pstrCondition,
	int offset,
	int nCount)
{
	strcpy_s(m_strCurrentTableName, CreateCurrentTableByYearMonth().c_str());
	char strCondition[256] = { 0 };
	sprintf_s(strCondition, (" %s order by `time` desc limit %d,%d"), pstrCondition, offset, nCount);
	return GetRows(m_strCurrentTableName, v, strCondition);
}

DWORD CMySQL::GetVoiceRowCount(LPCSTR pstrCondition)
{
	DWORD count = 0;
	//m_strCurrentTableName = CreateCurrentTableByYearMonth().c_str();
	strcpy_s(m_strCurrentTableName, CreateCurrentTableByYearMonth().c_str());
	char strSql[256];
	sprintf_s(strSql, "select count(`id`) as `count` from %s %s", m_strCurrentTableName, pstrCondition);

	if (WAIT_ABANDONED == WaitForSingleObject(m_mutxDbLocker, INFINITE))
	{
		return count;
	}

	if (mysql_real_query(m_pMysqlConnection, strSql, strlen(strSql)))
	{
		LPCSTR pstrError = mysql_error(m_pMysqlConnection);
		ReleaseMutex(m_mutxDbLocker);
		return count;
	}

	MYSQL_RES *result;
	MYSQL_ROW row;

	result = mysql_store_result(m_pMysqlConnection);
	if (result)
	{
		row = mysql_fetch_row(result);
		char strCount[256] = { 0 };
		strcpy_s(strCount, row[0]);
		count += atoi(strCount);
	}
	mysql_free_result(result);

	ReleaseMutex(m_mutxDbLocker);

	return count;
}

BOOL CMySQL::DeleteTable(LPCSTR pstrSQL)
{
	if (pstrSQL == NULL)
	{
		return FALSE;
	}

	if (WAIT_ABANDONED == WaitForSingleObject(m_mutxDbLocker, INFINITE))
	{
		return FALSE;
	}

	if (mysql_real_query(m_pMysqlConnection, pstrSQL, strlen(pstrSQL)))
	{
		LPCSTR pstrError = mysql_error(m_pMysqlConnection);
		//LOG(ERROR)<<pstrError;
		ReleaseMutex(m_mutxDbLocker);
		return FALSE;
	}

	ReleaseMutex(m_mutxDbLocker);
	return TRUE;
}

void CMySQL::SetLogPtr(PLogReport value)
{
	m_report = value;
}

void CMySQL::sendLogToWindow()
{
	if (m_report)
	{
		m_report(m_reportMsg);
	}
}

unsigned long CMySQL::GetLastVoiceRowId()
{
	std::string id = GetLastRow(m_strCurrentTableName)["id"];
	return (unsigned long)atoi(id.c_str());
}



