#include "stdafx.h"
#include "../lib/hash/md5.h"

#include "Db.h"

#define			CURRENT_DB_VER			101

void migrate_v100(CMySQL* pMySQL){
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `migration` (\
													`id` int(11) NOT NULL AUTO_INCREMENT, \
																				`ver` varchar(64) NOT NULL, \
																											`created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, \
																																				PRIMARY KEY(`id`)\
																																											) ENGINE = InnoDB  DEFAULT CHARSET = utf8");

	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `user` ( \
													`id` INT(11) NOT NULL AUTO_INCREMENT, \
																				`username` VARCHAR(45) NOT NULL, \
																											`password` VARCHAR(300) NOT NULL, \
																																		`type` VARCHAR(45) NOT NULL DEFAULT 'radio', \
																																									`authority` TinyText NOT NULL, \
																																																`created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, \
																																																							PRIMARY KEY(`id`), \
																																																														UNIQUE INDEX `id_UNIQUE` (`id` ASC), \
																																																																					UNIQUE INDEX `username_UNIQUE` (`username` ASC)) \
																																																																												ENGINE = InnoDB; \
																																																																																		");

	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `department` ( \
													`id` INT(11) NOT NULL AUTO_INCREMENT, \
													`name` VARCHAR(300) NOT NULL, \
													`gid` INT NOT NULL COMMENT 'radio group id', \
													PRIMARY KEY(`id`), \
													UNIQUE INDEX `id_UNIQUE` (`id` ASC),\
													UNIQUE INDEX `gid_UNIQUE` (`gid` ASC))\
												    ENGINE = InnoDB; \
													");

	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `staff` ( \
													`id` INT(11) NOT NULL AUTO_INCREMENT, \
													`name` VARCHAR(45) NOT NULL, \
													`phone` VARCHAR(45) NULL, \
													`user` INT, \
													`valid` INT NOT NULL DEFAULT 1 COMMENT 'staff is valid?', \
													PRIMARY KEY(`id`), \
													INDEX `fk_employee_user_idx` (`user` ASC), \
													CONSTRAINT `fk_employee_user` \
													FOREIGN KEY(`user`) \
													REFERENCES `user` (`id`) \
													ON DELETE SET NULL \
													ON UPDATE NO ACTION) \
																																																																																																	ENGINE = InnoDB; \
																																																																																																							");

	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `organize` ( \
					`staff` INT NOT NULL AUTO_INCREMENT, \
					`department` INT NOT NULL, \
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

	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `sms` ( \
													`id` INT(11) NOT NULL AUTO_INCREMENT,	\
																				`source` MEDIUMTEXT NOT NULL, \
																											`destination` MEDIUMTEXT NOT NULL, \
																																		`message` VARCHAR(210) NOT NULL, \
																																									`is_ticket` INT NOT NULL DEFAULT 0 COMMENT '0: it\'s normal sms; 1: is ticket', \
																																																`createdf_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, \
																																																							PRIMARY KEY(`id`)) \
																																																														ENGINE = InnoDB; \
																																																																				");

	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `radios` ( \
													`id` INT(11) NOT NULL AUTO_INCREMENT,	\
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

	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `radio_belong` ( \
													`staff` INT NULL, \
																				`department` INT NULL, \
																											`radio` INT NOT NULL, \
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

	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `gps` ( \
						`id` INT(11) NOT NULL AUTO_INCREMENT, \
						`latitude` FLOAT NOT NULL, \
																											`logitude` FLOAT NOT NULL, \
																																		`velocity` FLOAT NOT NULL, \
																																									`radio` MEDIUMTEXT NULL, \
																																																`created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, \
																																																							`altitude` FLOAT NULL, \
																																																														PRIMARY KEY(`id`), \
																																																																					UNIQUE INDEX `id_UNIQUE` (`id` ASC)) \
																																																																												ENGINE = InnoDB;  \
																																																																																		");
}

void migrate_v101(CMySQL* pMySQL)
{
	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `sms` (\
		`id` INT(11) NOT NULL,\
		`source` MEDIUMTEXT NOT NULL,\
		`destination` MEDIUMTEXT NOT NULL,\
		`message` VARCHAR(210) NOT NULL,\
		`is_ticket` INT NOT NULL DEFAULT 0 COMMENT '0: it is normal sms; 1: is ticket',\
		`createdf_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
		PRIMARY KEY(`id`))\
		ENGINE = InnoDB; ");


	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `gps` (\
		`id` INT(11) NOT NULL AUTO_INCREMENT,\
		`latitude` FLOAT NOT NULL,\
		`logitude` FLOAT NOT NULL,\
		`velocity` FLOAT NOT NULL,\
		`radio` MEDIUMTEXT NULL,\
		`created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
		`altitude` FLOAT NULL,\
		PRIMARY KEY(`id`),\
		UNIQUE INDEX `id_UNIQUE` (`id` ASC))\
		ENGINE = InnoDB; ");


}

DbMigrate migrateTable[] = {
	{ 100, migrate_v100 },
	{ 101, migrate_v101 },
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
		if (CURRENT_DB_VER > migrateTable[i].ver)
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

bool CDb::insertUser(const char* name, const char* phone, const char* username, const char* password, const char* authority, const char* type)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType user;
		recordType staff;

		user["username"] = username;
		user["authority"] = authority;
		user["type"] = type;

		if (m_pMySQLDb->recordExist("user", user))
		{
			throw std::exception("username exist");
		}

		staff["name"] = name;
		if (m_pMySQLDb->recordExist("staff", staff))
		{
			throw std::exception("staff exist");
		}
		MD5 md5;
		md5.add(password, strlen(password));
		user["password"] = md5.getHash();
		staff["phone"] = phone;

		m_pMySQLDb->insert("user", user);

		recordType lastUser;
		m_pMySQLDb->findLastOne("user", lastUser);
		staff["user"] = lastUser["id"];

		m_pMySQLDb->insert("staff", staff);

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
	std::string sql = "select staff.*, user.username as username, user.type as type, user.authority as authority from staff left join user on staff.user=user.id";
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

int CDb::getUserIdByStaffId(int staffId)
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

bool CDb::insertDepartment(const char* name, int gid)
{
	try{
		recordType department;

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
bool CDb::updateDepartment(int id, const char* name, int gid)
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
	sprintf_s(condition, " where id=%d", id);

	return m_pMySQLDb->update("department", val, condition);
}

bool CDb::assignUser(int userId, int departmentId)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType rec;
		rec["staff"] = std::to_string(userId);
		rec["department"] = std::to_string(departmentId);
		if (!m_pMySQLDb->recordExist("organize", rec))
		{
			m_pMySQLDb->insert("organize", rec);
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
bool CDb::detachUser(int userId, int departmentId)
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

bool CDb::assignDepartmentRadio(int radioId, int departmentId)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType rec;
		rec["radio"] = std::to_string(radioId);
		rec["department"] = std::to_string(departmentId);
		if (!m_pMySQLDb->recordExist("radio_belong", rec))
		{
			m_pMySQLDb->insert("radio_belong", rec);
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

bool CDb::detachDepartmentRadio(int radioId, int departmentId)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType rec;
		rec["radio"] = std::to_string(radioId);
		rec["department"] = std::to_string(departmentId);
		if (m_pMySQLDb->recordExist("radio_belong", rec))
		{
			std::string sql = "delete from radio_belong where radio=" + rec["radio"];
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

bool CDb::listDepartmentStaff(int departmentId, std::list<recordType>& records)
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

bool CDb::listDepartmentRadio(int departmentId, std::list<recordType>& records)
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

bool CDb::listStaffRadio(int staffId, std::list<recordType>& records)
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

bool CDb::assignStaffRadio(int staffId, int radioId)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType rec;
		rec["staff"] = std::to_string(staffId);
		rec["radio"] = std::to_string(radioId);
		if (!m_pMySQLDb->recordExist("radio_belong", rec))
		{
			m_pMySQLDb->insert("radio_belong", rec);
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

bool CDb::detachStaffRadio(int staffId, int radioId)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType rec;
		rec["radio"] = std::to_string(radioId);
		rec["staff"] = std::to_string(staffId);
		if (m_pMySQLDb->recordExist("radio_belong", rec))
		{
			std::string sql = "delete from radio_belong where radio=" + rec["radio"];
			sql += " and staff=" + rec["staff"];
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

bool CDb::insertRadio(const char* radioId, int type, const char* sn, int screen, int gps, int keyboard)
{
	try{
		recordType radio;

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

bool CDb::insertStaff(const char* name, const char* phone, bool isStaff)
{
	try{
		recordType staff;

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