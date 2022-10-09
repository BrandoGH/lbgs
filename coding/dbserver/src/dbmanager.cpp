#include "dbmanager.h"
#include "postgresql/psqltables.h"
#include "dbserver.h"

#include <iosfwd>
#include <commontool/serializememory/serializememory.hpp>
#include <boostmodule/basedef.h>

DBManager::DBManager()
	: m_bRigQueueThreadStopFlag(true)
	, m_nQueueRigsterSize(0)
	, m_bSelectQueueThreadStopFlag(true)
	, m_nQueueSelectSqlSize(0)
{
	initQueueThread();
	initTimingSyncToCacheFunc();
}

DBManager::~DBManager()
{
	deleteInstance();
	m_bRigQueueThreadStopFlag = false;
	m_bSelectQueueThreadStopFlag = false;
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

void DBManager::registerDBServer(DBServer* dbServer)
{
	m_gloabelDBServer = dbServer;
}

DBServer* DBManager::getGlobalDBServer()
{
	return m_gloabelDBServer;
}

bool DBManager::checkRoleExists(const std::string& roleId)
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

void DBManager::registerRoleLoginInfo(const RoleLoginInfoParam& roleInfo, RoleLoginParamCallback callback)
{
	m_cbRigster = callback;
	++m_nQueueRigsterSize;
	m_queueRigster.enqueue(roleInfo);
}

void DBManager::getRoleLoginInfoParamFromDB(const std::string& roleId, RoleLoginParamCallback callback)
{
	m_cbSelect = callback;
	++m_nQueueSelectSqlSize;

	std::stringstream fm;
	fm << "SELECT " << DB_ROLE_FIELD_ROLE_PARAM << " FROM " << DB_TABLE_ROLE
		<< " WHERE " << DB_ROLE_FIELD_ROLE_ID << "=\'" << roleId << "\'";

	std::string sql = fm.str();
	m_queueSelectSql.enqueue(sql);
}

void DBManager::onExecSelectRoleParam()
{
	while (m_bSelectQueueThreadStopFlag)
	{
		if (getQueueSelectRoleLoginParamSize() <= 0)
		{
			continue;
		}

		std::string sql;
		if (!m_queueSelectSql.try_dequeue(sql))
		{
			continue;
		}

		if (!m_sql.query(sql,NULL))
		{
			continue;
		}

		std::string hexStr = m_sql.getValue(0, 0).data();
		RoleLoginInfoParam param;
		SerialzeMem::unSerializationFromHexString<RoleLoginInfoParam>(hexStr, param);
		if (!m_cbSelect.empty())
		{
			m_cbSelect(param);
		}

	}
}

void DBManager::onTimingSyncRoleInfo()
{
	// TODO
}

void DBManager::initQueueThread()
{
	CommonBoost::Thread tRigRole(BIND(&DBManager::onExecRigsterQueueSql, this));
	CommonBoost::Thread tSelectRoleParam(BIND(&DBManager::onExecSelectRoleParam, this));

	tRigRole.detach();
	tSelectRoleParam.detach();
}

void DBManager::initTimingSyncToCacheFunc()
{
	m_pTimingSyncFunc[TST_ROLE_INFO] = &DBManager::onTimingSyncRoleInfo;
}

int DBManager::getQueueRigsterSize()
{
	return m_nQueueRigsterSize.load();
}

int DBManager::getQueueSelectRoleLoginParamSize()
{
	return m_nQueueSelectSqlSize.load();
}

void DBManager::deleteInstance()
{
	if (instanceObj)
	{
		delete instanceObj;
		instanceObj = NULL;
	}
}

void DBManager::onExecRigsterQueueSql()
{
	while (m_bRigQueueThreadStopFlag)
	{
		if (getQueueRigsterSize() <= 0)
		{
			continue;
		}

		RoleLoginInfoParam roleInfo;
		if (!m_queueRigster.try_dequeue(roleInfo))
		{
			continue;
		}

		// serialize
		RoleLoginInfoParamHex hexStr;
		SerialzeMem::serializationToMemData<RoleLoginInfoParam>(const_cast<RoleLoginInfoParam*>(&roleInfo), hexStr, sizeof(hexStr));

		// sql
		const char* valueData[1] = { (const char*)hexStr };
		int paramLengths[1] = { sizeof(RoleLoginInfoParamHex) };
		int paramFormats[1] = { 1 };
		std::stringstream fm;
		fm << "INSERT INTO " << DB_TABLE_ROLE
			<< " (" << DB_ROLE_FIELD_ROLE_ID << "," << DB_ROLE_FIELD_ROLE_NAME << "," << DB_ROLE_FIELD_ROLE_PARAM << ") VALUES"
			<< " ('" << roleInfo.m_strRoleId << "','" << roleInfo.m_strRoleName << "'," << "$1::bytea" << ")";
		if (m_sql.query(fm.str(), 1, NULL, valueData, paramLengths, paramFormats, 1, NULL) &&
			!m_cbRigster.empty())
		{
			m_cbRigster(roleInfo);
		}
		

	}
}
