#pragma once
#include <functional>
#include "../lib/MySQL/MySQL.h"

typedef struct _DbMigrate{
	int ver;
	std::function<void(CMySQL* pMySQL)> func;
}DbMigrate;

class CDb
{
public:
	static CDb* instance(){
		static std::mutex locker;

		if (NULL == m_instance.get())
		{
			std::lock_guard<std::mutex> lock(locker);

			if (NULL == m_instance.get())
			{
				m_instance.reset(new CDb());
			}
		}

		return m_instance.get();
	}

private:
	CDb();
	~CDb();

private:
	CMySQL* m_pMySQLDb;
	friend class std::auto_ptr<CDb>;
	static std::auto_ptr<CDb> m_instance;
	std::string m_strLastError;

protected:
	void migration();

public:
	bool open(const char* host, u_short port, const char* user, const char* pass, const char* db);
	int checkDbVersion();
	const char* getLastError() const;
	bool auth(const char* username, const char* password);
	bool insertUser(const char* name, const char* phone, const char* username, const char* password);
};

