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
	typedef boost::function< void(const RoleLoginInfoParam&) > CallbackRigster;

public:
	enum EnTimingSyncType
	{
		TST_ROLE_INFO,
	};

public:
	DBManager();
	~DBManager();
	static DBManager* instance();

	void connectDB(BasePsql::CallbackStartConnect callback);
	BasePsql* getBaseSql();
	void registerDBServer(DBServer* dbServer);

	bool checkRoleExists(const std::string& roleId);
	void registerRoleLoginInfo(const RoleLoginInfoParam& roleInfo, CallbackRigster callback);

	void timingSyncToCache(EnTimingSyncType type);

HANDLER:
	void onExecRigsterQueueSql();

private:
	void initQueueThread();
	int getQueueRigsterSize();
	void deleteInstance();

private:
	static DBManager* instanceObj;
	DBServer* m_gloabelDBServer;

	BasePsql m_sql;

	bool m_bQueueThreadStopFlag;
	moodycamel::ConcurrentQueue<RoleLoginInfoParam> m_queueRigster;
	boost::atomic_int m_nQueueRigsterSize;
	CallbackRigster m_cbRigster;
};
#define DB_MGR DBManager::instance()

#endif // !__DB_MANAGER_H__
