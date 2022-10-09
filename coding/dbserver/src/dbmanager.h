#ifndef __DB_MANAGER_H__
#define __DB_MANAGER_H__
#include "postgresql/basepsql.h"

#include <logicserver/communicationmsg/msglogin.h>
#include <concurrentqueue/concurrentqueue.h>
#include <boost/atomic.hpp>
#include <boost/function.hpp>

class DBServer;
class DBManager
{
	// callback
	// success will be return roleInfo
	typedef boost::function< void(const RoleLoginInfoParam&) > RoleLoginParamCallback;

	// timing sync to cache
	typedef void(DBManager::*HanlderFuncTimingSyncToCache)(void);

public:
	enum EnTimingSyncType
	{
		TST_ROLE_INFO,

		TST_MAX
	};

public:
	DBManager();
	~DBManager();
	static DBManager* instance();

	void connectDB(BasePsql::CallbackStartConnect callback);
	BasePsql* getBaseSql();
	void registerDBServer(DBServer* dbServer);
	DBServer* getGlobalDBServer();

	bool checkRoleExists(const std::string& roleId);
	void registerRoleLoginInfo(const RoleLoginInfoParam& roleInfo, RoleLoginParamCallback callback);
	void getRoleLoginInfoParamFromDB(const std::string& roleId, RoleLoginParamCallback callback);

HANDLER:
	void onExecRigsterQueueSql();
	void onExecSelectRoleParam();
	void onTimingSyncRoleInfo();

private:
	void initQueueThread();
	void initTimingSyncToCacheFunc();
	int getQueueRigsterSize();
	int getQueueSelectRoleLoginParamSize();
	void deleteInstance();

private:
	static DBManager* instanceObj;
	DBServer* m_gloabelDBServer;

	BasePsql m_sql;

	// role register
	bool m_bRigQueueThreadStopFlag;
	moodycamel::ConcurrentQueue<RoleLoginInfoParam> m_queueRigster;
	boost::atomic_int m_nQueueRigsterSize;
	RoleLoginParamCallback m_cbRigster;
	// sql select
	bool m_bSelectQueueThreadStopFlag;
	moodycamel::ConcurrentQueue<std::string> m_queueSelectSql;
	boost::atomic_int m_nQueueSelectSqlSize;
	RoleLoginParamCallback m_cbSelect;


	HanlderFuncTimingSyncToCache m_pTimingSyncFunc[TST_MAX];
};
#define DB_MGR DBManager::instance()

#endif // !__DB_MANAGER_H__
