#pragma once

#include <atomic>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <map>
#include <string>
#include <list>

#include "mysql-connector-c/include/mysql.h"

#ifndef recordType
typedef std::map<std::string, std::string> recordType;
#endif

class CMySQL 
{
public:
	CMySQL(void);
	~CMySQL(void);

private:
	MYSQL						*m_pMysqlConnection;
	std::condition_variable		m_hExitEvent;
	std::mutex					m_mtxQuit;
	std::atomic<bool>			m_bStoped;
	std::atomic<bool>			m_bIsConnected;
	std::mutex					m_tcpChannelUseLocker;
	std::thread					m_pingThread;

public: 
	bool open(const char* host, unsigned short port, const char* user, const char* pass, const char* db);
	void close();
	std::string EscapeCharacter(const std::string& ss);
	bool createTable(const char* tbSql);
	int insert(const char* table, recordType &record);
	int del(const char* table, const char* condition);
	int update(const char* table, recordType& val, const char* condition);
	int find(const char* table, const char* condition, std::list<recordType> &records);
	int findLastOne(const char* table, recordType& record);
	void startTransaction();
	void commit();
	void rollback();
	bool recordExist(const char* table, recordType record);
	int count(const char* table, const char* condition);
	int query(const char* sql, std::list<recordType> &records);
};

