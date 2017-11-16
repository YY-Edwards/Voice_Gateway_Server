#include "stdafx.h"
#include "../lib/hash/md5.h"

#include "Db.h"

#define			CURRENT_DB_VER			100


void createMigration(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `migration` (\
						`id` INT(11) NOT NULL AUTO_INCREMENT, \
						`ver` VARCHAR(64) NOT NULL, \
						`created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, \
						PRIMARY KEY(`id`))\
						ENGINE = InnoDB;"
						);
}

void createUser(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `user` ( \
						`id` BIGINT NOT NULL, \
						`username` VARCHAR(64) BINARY NOT NULL, \
						`password` VARCHAR(300) NOT NULL, \
						`type` VARCHAR(64) NOT NULL DEFAULT 'radio', \
						`authority` TinyText NOT NULL, \
						`created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, \
						PRIMARY KEY(`id`), \
						UNIQUE INDEX `id_UNIQUE` (`id` ASC), \
						UNIQUE INDEX `username_UNIQUE` (`username` ASC)) \
						ENGINE = InnoDB; \
						");
}

void createDepartment(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `department` ( \
						`id` BIGINT NOT NULL, \
						`name` VARCHAR(64) NOT NULL, \
						`gid` INT NOT NULL COMMENT 'radio group id', \
						PRIMARY KEY(`id`), \
						UNIQUE INDEX `id_UNIQUE` (`id` ASC),\
						UNIQUE INDEX `gid_UNIQUE` (`gid` ASC))\
						ENGINE = InnoDB; \
						");
}

void createStaff(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `staff` ( \
						`id` BIGINT NOT NULL, \
						`name` VARCHAR(45) NOT NULL, \
						`phone` VARCHAR(45) NULL, \
						`user` BIGINT, \
						`valid` INT NOT NULL DEFAULT 1 COMMENT 'staff is valid?', \
						PRIMARY KEY(`id`)) \
						ENGINE = InnoDB;"
						);
}

void createOrganize(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `organize` ( \
						`staff` BIGINT NOT NULL, \
						`department` BIGINT NOT NULL, \
						INDEX `fk_organize_staff_idx` (`staff` ASC), \
						INDEX `fk_organize_department_idx` (`department` ASC), \
						CONSTRAINT `fk_organize_staff` \
						FOREIGN KEY(`staff`) \
						REFERENCES `staff` (`id`) \
						ON DELETE CASCADE \
						ON UPDATE NO ACTION, \
						CONSTRAINT `fk_organize_department` \
						FOREIGN KEY(`department`) \
						REFERENCES `department` (`id`) \
						ON DELETE CASCADE \
						ON UPDATE NO ACTION) \
						ENGINE = InnoDB; \
					");
}

void createRadios(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `radios` ( \
						`id` BIGINT NOT NULL ,	\
						`radio_id` MEDIUMTEXT NOT NULL, \
						`type` INT NOT NULL DEFAULT 0 COMMENT '0:手台;1:车台;2:第三方应用', \
						`screen` INT NOT NULL DEFAULT 0, \
						`gps` INT NOT NULL DEFAULT 0, \
						`keyboard` INT NOT NULL DEFAULT 0, \
						`sn` VARCHAR(45) NULL, \
						`valid` INT NOT NULL DEFAULT 1 COMMENT 'radio is valid?', \
						PRIMARY KEY(`id`), \
						UNIQUE INDEX `id_UNIQUE` (`id` ASC)) \
						ENGINE = InnoDB; \
						");
}

void createRadioBelong(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `radio_belong` ( \
						`staff` BIGINT NULL, \
						`department` BIGINT NULL, \
						`radio` BIGINT NOT NULL, \
						INDEX `fk_radio_staff_idx` (`staff` ASC), \
						INDEX `fk_radio_department_idx` (`department` ASC), \
						INDEX `fk_radio_self_idx` (`radio` ASC), \
						CONSTRAINT `fk_radio_staff` \
						FOREIGN KEY(`staff`) \
						REFERENCES `staff` (`id`) \
						ON DELETE CASCADE \
						ON UPDATE NO ACTION, \
						CONSTRAINT `fk_radio_department` \
						FOREIGN KEY(`department`) \
						REFERENCES `department` (`id`) \
						ON DELETE CASCADE \
						ON UPDATE NO ACTION, \
						CONSTRAINT `fk_radio_self` \
						FOREIGN KEY(`radio`) \
						REFERENCES `radios` (`id`) \
						ON DELETE CASCADE \
						ON UPDATE NO ACTION) \
						ENGINE = InnoDB; \
						");
}

void createAreas(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `areas` (\
						`id` BIGINT NOT NULL ,\
						`name` VARCHAR(210) NOT NULL,\
						`map` VARCHAR(210) NOT NULL,\
						`width` VARCHAR(210) NOT NULL,\
						`height` VARCHAR(210) NOT NULL,\
						PRIMARY KEY(`id`))\
						ENGINE = InnoDB;"
						);
}

void createBeacons(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `ibeacons` (\
						`id` BIGINT NOT NULL,\
						`name` VARCHAR(210) NOT NULL,\
						`uuid` VARCHAR(210) NOT NULL,\
						`major` INT NOT NULL DEFAULT 0, \
						`minor` INT NOT NULL DEFAULT 0, \
						`tx_power` INT NOT NULL DEFAULT 0, \
						`rssi` INT NOT NULL DEFAULT 0, \
						`time_stamp` INT NOT NULL DEFAULT 0, \
						`valid` INT NOT NULL DEFAULT 0, \
						`area` BIGINT NOT NULL DEFAULT 0, \
						`pointx` VARCHAR(210) NOT NULL,\
						`pointy` VARCHAR(210) NOT NULL,\
						PRIMARY KEY(`id`))\
						ENGINE = InnoDB; "
						);
}

void createSmsReport(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `sms` (\
						`id` INT(11) NOT NULL AUTO_INCREMENT,\
						`source` MEDIUMTEXT NOT NULL,\
						`destination` MEDIUMTEXT NOT NULL,\
						`message` VARCHAR(210) NOT NULL,\
						`is_ticket` INT NOT NULL DEFAULT 0 COMMENT '0: it is normal sms; 1: is ticket',\
						`createdf_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
						PRIMARY KEY(`id`))\
						ENGINE = InnoDB; "
						);
}

void createGpsReport(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `gps` (\
						`id` INT(11) NOT NULL AUTO_INCREMENT,\
						`latitude` FLOAT NOT NULL,\
						`logitude` FLOAT NOT NULL,\
						`velocity` FLOAT NOT NULL,\
						`created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
						`altitude` FLOAT NULL,\
						PRIMARY KEY(`id`),\
						UNIQUE INDEX `id_UNIQUE` (`id` ASC))\
						ENGINE = InnoDB; "
						);
}

void createBeaconReport(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `location` ( \
						`id` INT(11) NOT NULL AUTO_INCREMENT,	\
						`source` MEDIUMTEXT NOT NULL, \
						`major` INT NOT NULL DEFAULT 0, \
						`minor` INT NOT NULL DEFAULT 0 , \
						`timestamp` INT NOT NULL DEFAULT 0, \
						`uuid` VARCHAR(210) NOT NULL,\
						`txpwr` VARCHAR(210) NOT NULL,\
						`rssi` VARCHAR(210) NOT NULL,\
						`created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, \
						PRIMARY KEY(`id`)) \
						ENGINE = InnoDB;"
						);
						
}


void migrate_v100(CMySQL* pMySQL)
{
	createMigration(pMySQL);
	createUser(pMySQL);
	createDepartment(pMySQL);
	createStaff(pMySQL);
	createOrganize(pMySQL);
	createRadios(pMySQL);
	createRadioBelong(pMySQL);
	createAreas(pMySQL);
	createBeacons(pMySQL);
	createSmsReport(pMySQL);
	createGpsReport(pMySQL);
	createBeaconReport(pMySQL);
}

DbMigrate migrateTable[] = {
	{ 100, migrate_v100 },
};

std::auto_ptr<CDb> CDb::m_instance;

CDb::CDb()
{
	m_pMySQLDb = new CMySQL();
}


CDb::~CDb()
{
	close();
}

void CDb::migration()
{
	int dbVer = checkDbVersion();
	if (CURRENT_DB_VER <= dbVer)
	{
		return;
	}

	for (size_t i = 0; i < sizeof(migrateTable) / sizeof(DbMigrate); i++)
	{
		if (CURRENT_DB_VER < migrateTable[i].ver)
		{
			continue;
		}

		migrateTable[i].func(m_pMySQLDb);

		recordType record;
		record["ver"] = std::to_string(migrateTable[i].ver);
		m_pMySQLDb->insert("migration", record);
	}

}

int CDb::checkDbVersion()
{
	int ver = 0;
	recordType record;

	int ret = m_pMySQLDb->findLastOne("migration", record);
	if (0 == ret)
	{
		return ver;
	}

	try
	{
		ver = std::atoi(record["ver"].c_str());
	}
	catch (std::exception e)
	{
		ver = 0;
	}
	catch (...)
	{
		ver = 0;
	}
	return ver;
}

void CDb::close()
{
	if (m_pMySQLDb)
	{
		m_pMySQLDb->close();
		delete m_pMySQLDb;
		m_pMySQLDb = NULL;
	}
}

bool CDb::open(const char* host, u_short port, const char* user, const char* pass, const char* db)
{
	bool ret = m_pMySQLDb->open(host, port, user, pass, db);
	if (ret)
	{
		migration();
		return true;
	}

	return false;
}

const char* CDb::getLastError() const
{
	return m_strLastError.c_str();
}

bool CDb::insertUser(long long id,  const char* username, const char* password, const char* authority, const char* type)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType user;
		user["id"] = std::to_string(id);
		user["username"] = username;
		user["authority"] = authority;
		user["type"] = type;

		if (m_pMySQLDb->recordExist("user", user))
		{
			throw std::exception("username exist");
		}
		
		MD5 md5;
		md5.add(password, strlen(password));
		user["password"] = md5.getHash();
		
		m_pMySQLDb->insert("user", user);

		m_pMySQLDb->commit();
	}
	catch (std::exception e)
	{
		m_pMySQLDb->rollback();
		return false;
	}
	catch (...)
	{
		m_pMySQLDb->rollback();
		return false;
	}

	return true;
}

std::string CDb::md5(const char* p)
{
	std::string md5Str;
	if (NULL == p)
	{
		return md5Str;
	}
	MD5 md5;
	md5.add(p, strlen(p));
	md5Str = md5.getHash();

	return md5Str;
}



bool CDb::auth(const char* username, const char* password)
{
	try{
		recordType user;
		MD5 md5;
		md5.add(password, strlen(password));
		user["password"] = md5.getHash();
		user["username"] = username;

		if (m_pMySQLDb->recordExist("user", user))
		{
			throw std::exception("username or password error");
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

int CDb::listUser(const char* condition, std::list<recordType>& records)
{
	std::string sql = "select user.id, user.username as username, user.type as type, user.authority as authority  from user";
	//std::string sql = "select staff.*, user.username as username, user.type as type, user.authority as authority from staff left join user on staff.user=user.id";
	if (NULL != condition && strlen(condition) > 0)
	{
		sql += " ";
		sql += condition;
	}

	return m_pMySQLDb->query(sql.c_str(), records);
}


int CDb::listStaff(const char* condition, std::list<recordType>& records)
{
	std::string sql = "SELECT staff.*, organize.department FROM staff LEFT JOIN organize ON staff.id=organize.staff";
	if (NULL != condition && strlen(condition) > 0)
	{
		sql += " ";
		sql += condition;
	}

	return m_pMySQLDb->query(sql.c_str(), records);
}

int CDb::listRadio(const char* condition, std::list<recordType>& records)
{
	std::string sql = "SELECT radios.*, radio_belong.department, radio_belong.staff FROM radios LEFT JOIN radio_belong ON radios.id = radio_belong.radio";
	if (NULL != condition && strlen(condition) > 0)
	{
		sql += " ";
		sql += condition;
	}

	return m_pMySQLDb->query(sql.c_str(), records);
}




int CDb::query(const char* table, const char* condition, std::list<recordType>& records)
{
	return m_pMySQLDb->find(table, condition, records);
}

int CDb::count(const char* table, const char* condition)
{
	return m_pMySQLDb->count(table, condition);
}

int CDb::getUserIdByStaffId(long long staffId)
{
	std::list<recordType> records;
	std::string condition = " where id=" + std::to_string(staffId);
	m_pMySQLDb->find("staff", condition.c_str(), records);

	if (records.size() <= 0)
	{
		return 0;
	}

	std::string userId = records.front()["user"];
	return std::atoi(userId.c_str());
}

bool CDb::updateUser(const char* condition, recordType& val)
{
	return m_pMySQLDb->update("user", val, condition);
}

bool CDb::updateRadio(const char* condition, recordType& val)
{
	return m_pMySQLDb->update("radios", val, condition);
}

bool CDb::updateStaff(const char* condition, recordType& val)
{
	return m_pMySQLDb->update("staff", val, condition);
}

bool CDb::del(const char* table, const char* condition)
{
	return (m_pMySQLDb->del(table, condition) > 0);
}

bool CDb::insertDepartment(long long id, const char* name, int gid)
{
	try{
		recordType department;
		department["id"] = std::to_string(id);
		department["name"] = name;
		department["gid"] = std::to_string(gid);

		if (m_pMySQLDb->recordExist("department", department))
		{
			throw std::exception("department exist");
		}

		m_pMySQLDb->insert("department", department);
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
bool CDb::updateDepartment(long long id, const char* name, int gid)
{
	recordType val;
	if (strlen(name) > 0){
		val["name"] = name;
	}
	
	if (gid >= 0)
	{
		val["gid"] = std::to_string(gid);
	}
	char condition[200];
	memset(condition, 0, sizeof(condition));
	sprintf_s(condition, " where id=%I64d", id);

	return m_pMySQLDb->update("department", val, condition);
}

bool CDb::assignUser(long long userId, long long departmentId)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType rec;
		rec["staff"] = std::to_string(userId);
		rec["department"] = std::to_string(departmentId);
		if (!m_pMySQLDb->recordExist("organize", rec))
		{
			std::string constr = " where staff=" + rec["staff"];
			if (m_pMySQLDb->count("organize", constr.c_str()) > 0)
			{
				m_pMySQLDb->update("organize", rec, constr.c_str());
			}
			else
			{
				m_pMySQLDb->insert("organize", rec);
			}
		}
		m_pMySQLDb->commit();
		return true;
	}
	catch (std::exception e)
	{
		m_pMySQLDb->rollback();
	}
	catch (...)
	{
		m_pMySQLDb->rollback();
	}

	return false;
}
bool CDb::detachUser(long long userId, long long departmentId)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType rec;
		rec["staff"] = std::to_string(userId);
		rec["department"] = std::to_string(departmentId);
		if (m_pMySQLDb->recordExist("organize", rec))
		{
			std::string sql = "delete from organize where staff=" + rec["staff"];
			sql += " and department=" + rec["department"];
			std::list<recordType> records;
			m_pMySQLDb->query(sql.c_str(), records);
		}
		m_pMySQLDb->commit();
		return true;
	}
	catch (std::exception e)
	{
		m_pMySQLDb->rollback();
	}
	catch (...)
	{
		m_pMySQLDb->rollback();
	}

	return false;
}

bool CDb::assignDepartmentRadio(long long radioId, long long departmentId)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType rec;		
		rec["radio"] = std::to_string(radioId);
		rec["department"] = std::to_string(departmentId);
		if (!m_pMySQLDb->recordExist("radio_belong", rec))
		{
			std::string constr = " where radio=" + rec["radio"];
			if (m_pMySQLDb->count("radio_belong", constr.c_str()) > 0)
			{
				m_pMySQLDb->update("radio_belong", rec, constr.c_str());
			}
			else
			{
				m_pMySQLDb->insert("radio_belong", rec);
			}
		}
		m_pMySQLDb->commit();
		return true;
	}
	catch (std::exception e)
	{
		m_pMySQLDb->rollback();
	}
	catch (...)
	{
		m_pMySQLDb->rollback();
	}

	return false;
}

bool CDb::detachDepartmentRadio(long long radioId, long long departmentId)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType rec;
		rec["radio"] = std::to_string(radioId);

		std::string sql = "select from radio_belong where radio=" + rec["radio"];
		std::list<recordType> records;
		m_pMySQLDb->query(sql.c_str(), records);

		if (records.size() > 0)
		{
			for (auto i = records.begin(); i != records.end(); i++)
			{
				rec["staff"] = (*i)["staff"];
				break;
			}
		}

		sql = "delete from radio_belong where radio=" + rec["radio"];
		m_pMySQLDb->query(sql.c_str(), records);

		if (!rec["staff"].compare("null"))m_pMySQLDb->insert("radio_belong", rec);

		m_pMySQLDb->commit();
		return true;
	}
	catch (std::exception e)
	{
		m_pMySQLDb->rollback();
	}
	catch (...)
	{
		m_pMySQLDb->rollback();
	}
	return false;
}

bool CDb::listDepartmentStaff(long long departmentId, std::list<recordType>& records)
{
	try{
		records.erase(records.begin(), records.end());

		//std::string sql = "select staff.id, staff.name, staff.phone, user.* from organize left join staff on organize.staff=staff.id left join user on staff.user=user.id where organize.department=" + std::to_string(departmentId);
		std::string sql = "select staff.* from organize left join staff on organize.staff=staff.id  where organize.department=" + std::to_string(departmentId);
		m_pMySQLDb->query(sql.c_str(), records);
		if (records.size() <= 0)
		{
			throw std::exception("no records");
		}
		return true;
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}
	return false;
}

bool CDb::listDepartmentRadio(long long departmentId, std::list<recordType>& records)
{
	try{
		records.erase(records.begin(), records.end());

		std::string sql = "select radios.* from radio_belong left join radios on radio_belong.radio=radios.id where radio_belong.department=" + std::to_string(departmentId);
		m_pMySQLDb->query(sql.c_str(), records);
		if (records.size() <= 0)
		{
			throw std::exception("no records");
		}
		return true;
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}
	return false;
}

bool CDb::listStaffRadio(long long staffId, std::list<recordType>& records)
{
	try{
		records.erase(records.begin(), records.end());

		std::string sql = "select radios.* from radio_belong left join radios on radio_belong.radio=radios.id where radio_belong.staff=" + std::to_string(staffId);
		m_pMySQLDb->query(sql.c_str(), records);
		if (records.size() <= 0)
		{
			throw std::exception("no records");
		}
		return true;
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}
	return false;
}

bool CDb::assignStaffRadio(long long staffId, long long radioId)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType rec;
		rec["staff"] = std::to_string(staffId);
		rec["radio"] = std::to_string(radioId);
		if (!m_pMySQLDb->recordExist("radio_belong", rec))
		{
			std::string constr = " where radio=" + rec["radio"];
			if (m_pMySQLDb->count("radio_belong", constr.c_str()) > 0)
			{
				m_pMySQLDb->update("radio_belong", rec, constr.c_str());
			}
			else
			{
				m_pMySQLDb->insert("radio_belong", rec);
			}			
		}
		m_pMySQLDb->commit();
		return true;
	}
	catch (std::exception e)
	{
		m_pMySQLDb->rollback();
	}
	catch (...)
	{
		m_pMySQLDb->rollback();
	}

	return false;
}

bool CDb::detachStaffRadio(long long staffId, long long radioId)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType rec;
		rec["radio"] = std::to_string(radioId);

		std::string sql = "select from radio_belong where radio=" + rec["radio"];
		std::list<recordType> records;
		m_pMySQLDb->query(sql.c_str(), records);

		if (records.size() > 0)
		{
			for (auto i = records.begin(); i != records.end(); i++)
			{
				rec["department"] = (*i)["department"];
				break;
			}			
		}

		sql = "delete from radio_belong where radio=" + rec["radio"];
		m_pMySQLDb->query(sql.c_str(), records);

		if (!rec["department"].compare("null"))m_pMySQLDb->insert("radio_belong", rec);
		
		m_pMySQLDb->commit();
		return true;
	}
	catch (std::exception e)
	{
		m_pMySQLDb->rollback();
	}
	catch (...)
	{
		m_pMySQLDb->rollback();
	}
	return false;
}

bool CDb::insertRadio(long long id, const char* radioId, int type, const char* sn, int screen, int gps, int keyboard)
{
	try{
		recordType radio;
		radio["id"] = std::to_string(id);
		radio["radio_id"] = radioId;
		radio["type"] = std::to_string(type);
		radio["screen"] = std::to_string(screen);
		radio["gps"] = std::to_string(gps);
		radio["valid"] = "1";
		radio["keyboard"] = std::to_string(keyboard);
		radio["sn"] = sn;

		if (m_pMySQLDb->recordExist("radios", radio))
		{
			throw std::exception("radio exist");
		}

		m_pMySQLDb->insert("radios", radio);
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

bool CDb::insertGpsLog(int radio, float latitude, float longitude, float velocity, float altitude)
{
	try{
		recordType gpsLog;

		gpsLog["latitude"] = std::to_string(latitude);
		gpsLog["lognitude"] = std::to_string(longitude);;
		gpsLog["radio"] = std::to_string(radio);
		gpsLog["altitude"] = std::to_string(altitude);
		gpsLog["velocity"] = std::to_string(velocity);

		m_pMySQLDb->insert("gps", gpsLog);
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

bool CDb::insertSmsLog(int source, int destination, const char* message, int is_ticket)
{
	try{
		recordType smsLog;

		smsLog["source"] = std::to_string(source);
		smsLog["destination"] = std::to_string(destination);;
		smsLog["message"] = message;
		smsLog["is_ticket"] = std::to_string(is_ticket);

		m_pMySQLDb->insert("sms", smsLog);
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

bool CDb::insertStaff(long long id, const char* name, const char* phone, bool isStaff)
{
	try{
		recordType staff;

		staff["id"] = std::to_string(id);
		staff["name"] = name;
		staff["phone"] = phone;
		staff["valid"] = isStaff ? "1" : "0";

		if (m_pMySQLDb->recordExist("staff", staff))
		{
			throw std::exception("radio exist");
		}

		m_pMySQLDb->insert("staff", staff);
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







bool CDb::insertArea(long long id, const char* name, const char* map, const char* width, const char* height)
{
	try{
		recordType area;
		area["id"] = std::to_string(id);
		area["name"] = name;
		area["map"] = map;
		area["width"] = width;
		area["height"] = height;

		std::string constr = " where name=" + area["name"] ;
		if (m_pMySQLDb->count("areas", constr.c_str()) > 0)
		{
			throw std::exception("area exist");
		}
		else
		{
			m_pMySQLDb->insert("areas", area);
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

bool CDb::updateArea(const char* condition, recordType& val)
{
	return m_pMySQLDb->update("areas", val, condition);
}


bool CDb::insertIBeacon(
	long long id,
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
	)
{
	try{
		recordType ibeacon;
		ibeacon["id"] = std::to_string(id);
		ibeacon["name"] = name;
		ibeacon["uuid"] = uuid;
		ibeacon["major"] = std::to_string(major);
		ibeacon["minor"] = std::to_string(minor);
		ibeacon["tx_power"] = std::to_string(tx_power);
		ibeacon["rssi"] = std::to_string(rssi);
		ibeacon["time_stamp"] = std::to_string(time_stamp);
		ibeacon["valid"] = std::to_string(valid);
		ibeacon["area"] = std::to_string(area);
		ibeacon["pointx"] = pointx;
		ibeacon["pointy"] = pointy;


		std::string constr = " where major=" + ibeacon["major"] + " and minor=" + ibeacon["minor"];
		if (m_pMySQLDb->count("ibeacons", constr.c_str()) > 0)
		{
			throw std::exception("ibeacon exist");
		}
		else
		{
			m_pMySQLDb->insert("ibeacons", ibeacon);
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

bool CDb::updateIBeacon(const char* condition, recordType& val)
{
	return m_pMySQLDb->update("ibeacons", val, condition);
}
bool CDb::insertLocationIndoor(int source, int major, int minor, int timestamp, int rssi, int txpower ,std::string uuid)
{
	try{
		recordType locationIndoor;

		locationIndoor["source"] = std::to_string(source);
		locationIndoor["major"] = std::to_string(major);;
		locationIndoor["minor"] = std::to_string(minor);
		locationIndoor["timestamp"] = std::to_string(timestamp);
		locationIndoor["rssi"] = std::to_string(rssi);
		locationIndoor["txpwr"] = std::to_string(txpower);
		locationIndoor["uuid"] = uuid;

		m_pMySQLDb->insert("location", locationIndoor);
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
int CDb::listLocation(const char* condition, std::list<recordType>& records)
{
	std::string sql = "select location.*, ibeacons.pointx, ibeacons.pointy, areas.name as areaname from (ibeacons left join areas on areas.id = ibeacons.area) right join location on ibeacons.major = location.major and ibeacons.minor = location.minor ";
	if (NULL != condition && strlen(condition) > 0)
	{
		sql += " ";
		sql += condition;
	}

	return m_pMySQLDb->query(sql.c_str(), records);
}