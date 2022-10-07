#include "dbmanager.h"
#include "postgresql/psqltables.h"
#include "dbserver.h"

#include <iosfwd>
#include <commontool/serializememory/serializememory.hpp>
#include <boostmodule/basedef.h>

DBManager::DBManager()
	: m_bQueueThreadStopFlag(true)
	, m_nQueueRigsterSize(0)
{
	initQueueThread();
}

DBManager::~DBManager()
{
	deleteInstance();
	m_bQueueThreadStopFlag = false;
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

void DBManager::registerRoleLoginInfo(const RoleLoginInfoParam& roleInfo, CallbackRigster callback)
{
	m_queueRigster.enqueue(roleInfo);
	++m_nQueueRigsterSize;
	m_cbRigster = callback;
}

void DBManager::timingSyncToCache(EnTimingSyncType type)
{
	if (!m_gloabelDBServer)
	{
		return;
	}

}

void DBManager::initQueueThread()
{
	CommonBoost::Thread t(BIND(&DBManager::onExecRigsterQueueSql, this));
	t.detach();
}

int DBManager::getQueueRigsterSize()
{
	return m_nQueueRigsterSize.load();
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
	while (m_bQueueThreadStopFlag)
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
