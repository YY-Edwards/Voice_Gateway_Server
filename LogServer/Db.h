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
	void close();
	int checkDbVersion();
	const char* getLastError() const;
	bool auth(const char* username, const char* password);
	bool insertUser(const char* name, const char* phone, const char* username, const char* password, const char* authority, const char* type);
	bool updateRadio(const char* condition, recordType& val);
	bool updateStaff(const char* condition, recordType& val);
	bool updateUser(const char* condition, recordType& val);
	bool insertDepartment(const char* name, int gid);
	bool updateDepartment(int id, const char* name, int gid);
	int listUser(const char* condition, std::list<recordType>& records);
	int listStaff(const char* condition, std::list<recordType>& records);
	int listRadio(const char* condition, std::list<recordType>& records);
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
	bool insertRadio(const char* radioId, int type, const char* sn, int screen = 0, int gps = 0, int keyboard=0 );
	bool insertStaff(const char* name, const char* phone, bool isStaff = true);
	bool insertSmsLog(int source, int destination, const char* message, int is_ticket = 0);
	bool insertGpsLog(int radio, float latitude, float longitude, float velocity, float altitude = 0.0f);

	int query(const char* table, const char* condition, std::list<recordType>& records);
	int count(const char* table, const char* condition);
	bool del(const char* table, const char* condition);

	bool insertArea(const char* name, const char* map, const char* width, const char* height);
	bool updateArea(const char* condition, recordType& val);

	bool insertIBeacon(
		const char* name,
		const char* uuid,
		int major,
		int minor,
		int tx_power,
		int rssi,
		int time_stamp,
		int valid,
		int area,
		const char* pointx,
		const char* pointy
		);
	bool updateIBeacon(const char* condition, recordType& val);
	bool insertLocationIndoor(int source, int major, int minor, int timestamp, int rssi, int txpower, std::string uuid);
	int listLocation(const char* condition, std::list<recordType>& records);
};

