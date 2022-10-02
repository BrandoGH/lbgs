#include "dbmanager.h"
#include "postgresql/psqltables.h"

#include <iosfwd>

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

bool DBManager::loginCheckRoleExists(const std::string& roleName)
{
	std::stringstream fm;
	fm << "SELECT role_id FROM " << DB_TABLE_ROLE 
		<< R"( WHERE role_name=')" << roleName << R"(')";
	if (!m_sql.query(fm.str(), NULL))
	{
		return false;
	}

	return m_sql.getRows() > 0;
}
