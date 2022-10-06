#ifndef __DB_MANAGER_H__
#define __DB_MANAGER_H__
#include "postgresql/basepsql.h"

#include <logicserver/communicationmsg/msglogin.h>

class DBManager
{
public:
	DBManager();
	~DBManager();
	static DBManager* instance();

	void connectDB(BasePsql::CallbackStartConnect callback);
	BasePsql* getBaseSql();

	bool loginCheckRoleExists(const std::string& roleId);
	void registerRoleInfo(const RoleInfoParam& roleInfo);

private:
	static DBManager* instanceObj;

	BasePsql m_sql;
};
#define DB_MGR DBManager::instance()

#endif // !__DB_MANAGER_H__
