#include "dbmanager.h"
#include "postgresql/psqltables.h"

#include <iosfwd>
#include <commontool/serializememory/serializememory.hpp>

DBManager::DBManager()
{
}

DBManager::~DBManager()
{
}

DBManager* DBManager::instanceObj = new(std::nothrow) DBManager;
DBManager* DBManager::instance()
{
	return instanceObj;
}

void DBManager::connectDB(BasePsql::CallbackStartConnect callback)
{
	m_sql.setStartConnectCallback(callback);
	m_sql.startConnectPostgresql();
}

BasePsql* DBManager::getBaseSql()
{
	return &m_sql;
}

bool DBManager::loginCheckRoleExists(const std::string& roleId)
{
	std::stringstream fm;
	fm << "SELECT role_id FROM " << DB_TABLE_ROLE 
		<< R"( WHERE role_id=')" << roleId << R"(')";
	if (!m_sql.query(fm.str(), NULL))
	{
		assert(0);
	}

	return m_sql.getRows() == 1;
}

void DBManager::registerRoleInfo(const RoleInfoParam& roleInfo)
{
	// serialize
	RoleInfoParamHex hexStr;
	SerialzeMem::serializationToMemData<RoleInfoParam>(const_cast<RoleInfoParam*>(&roleInfo), hexStr, sizeof(hexStr));

	// sql
	const char* valueData[1] = { (const char*)hexStr };
	int paramLengths[1] = { sizeof(RoleInfoParamHex)};
	int paramFormats[1] = {1};
	std::stringstream fm;
	fm << "INSERT INTO " << DB_TABLE_ROLE
		<< " (" << DB_ROLE_FIELD_ROLE_ID << "," << DB_ROLE_FIELD_ROLE_NAME << "," << DB_ROLE_FIELD_ROLE_PARAM << ") VALUES"
		<< " ('" << roleInfo.m_strRoleId << "','" << roleInfo.m_strRoleName << "'," << "$1::bytea" << ")";
	if (!m_sql.query(fm.str(), 1, NULL, valueData, paramLengths, paramFormats, 1, NULL))
	{
		assert(0);
	}
}
