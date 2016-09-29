#pragma once

#include <map>
#include <string>
#include <vector>
#include "mysql-connector-c/include/mysql.h"

#undef ERROR

class CMySQL
{
public:
	CMySQL(void);
	~CMySQL(void);

private:
	MYSQL *m_pMysqlConnection;
	HANDLE m_hExitEvent;
	BOOL m_bRunning;
	BOOL m_bIsConnected;
	HANDLE m_hPingThread;
	HANDLE m_mutxDbLocker;
	char m_strCurrentTableName[DATA_TABLE_NAME_SIZE];

	PLogReport m_report;
	char m_reportMsg[512];

private:
	std::string CreateCurrentTableByYearMonth();

	void sendLogToWindow();//将log显示至前台窗口

public:
	static DWORD WINAPI PingThread(LPVOID pMysql);

	BOOL InsertVoiceRecord(std::map<std::string, std::string>& v);
	BOOL GetVoiceRows(std::vector<std::map<std::string, std::string> >& v, LPCSTR pstrCondition);
	BOOL GetVoiceRows(std::vector<std::map<std::string, std::string> >& v, LPCSTR pstrCondition, int offset, int nCount);
	DWORD GetVoiceRowCount(LPCSTR pstrCondition);

public: // derived from CDbInterface

	std::string EscapeCharacter(const std::string& ss);

	virtual BOOL CreateTable(LPCSTR pstrSQL);

	virtual BOOL Open(LPCSTR pstrHost = NULL, WORD port = 3306,
		LPCSTR pstrUserName = NULL, LPCSTR pstrPassword = NULL, LPCSTR pstrDbName = NULL);

	virtual void Close();

	virtual BOOL InsertRow(LPCSTR pstrTableName, std::map<std::string, std::string>& v);

	virtual BOOL GetRows(LPCSTR pstrTableName, std::vector<std::map<std::string, std::string> >& v, LPCSTR pstrCondition);

	virtual std::map<std::string, std::string> GetLastRow(LPCSTR pstrTableName);

	virtual BOOL RemoveRow(LPCSTR pstrTableName, LPCSTR pstrCondition);

	virtual BOOL CheckTableExist(LPCSTR pstrTableName);

	virtual BOOL DeleteTable(LPCSTR pstrSQL);

	virtual BOOL UpdateRow(LPCSTR pstrTableName, std::map<std::string, std::string>& v, LPCSTR pstrCondition);

	BOOL InsertLogRecord(std::string logRecord);

	void SetLogPtr(PLogReport value);
};

