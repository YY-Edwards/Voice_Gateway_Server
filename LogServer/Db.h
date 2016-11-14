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

	static std::string md5(const char* p);

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
	bool insertUser(const char* name, const char* phone, const char* username, const char* password, const char* authority, const char* type);
	bool updateUser(const char* condition, recordType& val);
	bool insertDepartment(const char* name);
	bool updateDepartment(int id, const char* name);
	int listUser(const char* condition, std::list<recordType>& records);
	int getUserIdByStaffId(int staffId);
	bool detachUser(int userId, int departmentId);
	bool assignUser(int userId, int departmentId);
	bool assignDepartmentRadio(int radioId, int departmentId);
	bool detachDepartmentRadio(int radioId, int departmentId);
	bool listDepartmentStaff(int departmentId, std::list<recordType>& records);
	bool listStaffRadio(int staffId, std::list<recordType>& records);
	bool listDepartmentRadio(int departmentId, std::list<recordType>& records);
	bool assignStaffRadio(int staffId, int radioId);
	bool detachStaffRadio(int staffId, int radioId);

	int query(const char* table, const char* condition, std::list<recordType>& records);
	int count(const char* table, const char* condition);
	bool del(const char* table, const char* condition);
};

