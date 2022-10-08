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
	typedef boost::function< void(const RoleLoginInfoParam&) > CallbackRoleParam;

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
	void registerRoleLoginInfo(const RoleLoginInfoParam& roleInfo, CallbackRoleParam callback);


	void timingSyncToCache(EnTimingSyncType type);

HANDLER:
	void onExecRigsterQueueSql();

	void onTimingSyncRoleInfo();

private:
	void initQueueThread();
	void initTimingSyncToCacheFunc();
	int getQueueRigsterSize();
	void deleteInstance();

private:
	static DBManager* instanceObj;
	DBServer* m_gloabelDBServer;

	BasePsql m_sql;

	bool m_bQueueThreadStopFlag;
	moodycamel::ConcurrentQueue<RoleLoginInfoParam> m_queueRigster;
	boost::atomic_int m_nQueueRigsterSize;
	CallbackRoleParam m_cbRigster;

	HanlderFuncTimingSyncToCache m_pTimingSyncFunc[TST_MAX];
};
#define DB_MGR DBManager::instance()

#endif // !__DB_MANAGER_H__
