#include "stdafx.h"
#include "../lib/hash/md5.h"

#include "Db.h"

#define			CURRENT_DB_VER			100

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
																											PRIMARY KEY(`id`), \
																																		UNIQUE INDEX `id_UNIQUE` (`id` ASC)) \
																																									ENGINE = InnoDB; \
																																															");

	pMySQL->createTable("CREATE TABLE IF NOT EXISTS `staff` ( \
													`id` INT(11) NOT NULL AUTO_INCREMENT, \
																				`name` VARCHAR(45) NOT NULL, \
																											`phone` VARCHAR(45) NULL, \
																																		`user` INT NOT NULL, \
																																									`valid` INT NOT NULL DEFAULT 1 COMMENT 'staff is valid?', \
																																																PRIMARY KEY(`id`), \
																																																							INDEX `fk_employee_user_idx` (`user` ASC), \
																																																														CONSTRAINT `fk_employee_user` \
																																																																					FOREIGN KEY(`user`) \
																																																																												REFERENCES `user` (`id`) \
																																																																																			ON DELETE CASCADE \
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
	delete m_pMySQLDb;
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

bool CDb::insertUser(const char* name, const char* phone, const char* username, const char* password)
{
	m_pMySQLDb->startTransaction();
	try{
		recordType user;
		recordType staff;

		user["username"] = username;

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

int CDb::query(const char* table, const char* condition, std::list<recordType>& records)
{
	return m_pMySQLDb->find(table, condition, records);
}