#include "basepsql.h"

#include <configmodule/dbserverconfig/dbserverconfig.h>
#include <logmodule/logdef.h>
#include <configmodule/configmanager.h>

BasePsql::BasePsql()
	: m_psqlConn(NULL)
	, m_psqlRes(NULL)
	, m_cfg(CONFIG_MGR->GetDBServerConfig())
{
	if (!m_cfg)
	{
		assert(0);
	}
}

BasePsql::~BasePsql()
{
	clearResult();
	finish();
}

void BasePsql::setStartConnectCallback(CallbackStartConnect callback)
{
	m_cbStartConn = callback;
}

void BasePsql::startConnectPostgresql()
{
	CommonBoost::Thread t(
	BIND(&BasePsql::onStartConnect, this)
	);
	t.detach();
}

bool BasePsql::query(const std::string& sql, CallbackQuery callback)
{
	CommonBoost::UniqueLock lock(m_mtx);
	bool queryOk = false;
	if (!m_psqlConn)
	{
		LOG_DBSERVER.printLog("m_psqlConn NULL");
		if (!callback.empty())
		{
			callback(queryOk, PGRES_FATAL_ERROR);
		}
		return queryOk;
	}
	clearResult();
	m_psqlRes = PQexec(m_psqlConn, sql.data());
	if (!m_psqlRes ||
		getQuerySqlStatus() == PGRES_FATAL_ERROR)
	{
		LOG_DBSERVER.printLog("fatal error!!");
		if (!callback.empty())
		{
			callback(queryOk, PGRES_FATAL_ERROR);
		}
		return queryOk;
	}
	queryOk = true;
	if (!callback.empty())
	{
		callback(queryOk, getQuerySqlStatus());
	}
	return queryOk;
}

uint BasePsql::getRows()
{
	return PQntuples(m_psqlRes);
}

uint BasePsql::getColumns()
{
	return PQnfields(m_psqlRes);
}

bool BasePsql::start(CallbackQuery callback)
{
	return query("BEGIN", callback);
}

bool BasePsql::end(CallbackQuery callback)
{
	return query("BEGIN", callback);
}

bool BasePsql::commit(CallbackQuery callback)
{
	return query("COMMIT", callback);
}

bool BasePsql::rollback(CallbackQuery callback)
{
	return query("ROLLBACK", callback);
}

bool BasePsql::savepoint(const std::string& saveName, CallbackQuery callback)
{
	std::stringstream fm;
	fm << "SAVEPOINT " << saveName;
	return query(fm.str(), callback);
}

bool BasePsql::rollbackToSavepoint(const std::string& saveName, CallbackQuery callback)
{
	std::stringstream fm;
	fm << "ROLLBACK TO SAVEPOINT " << saveName;
	return query(fm.str(), callback);
}

bool BasePsql::releaseSavepoint(const std::string& saveName, CallbackQuery callback)
{
	std::stringstream fm;
	fm << "RELEASE SAVEPOINT " << saveName;
	return query(fm.str(), callback);
}

std::string BasePsql::getValue(uint row, uint col)
{
	return std::string(PQgetvalue(m_psqlRes, row, col));
}

std::string BasePsql::getColumnName(uint col)
{
	return std::string(PQfname(m_psqlRes, col));
}

void BasePsql::finish()
{
	PQfinish(m_psqlConn);
}

void BasePsql::clearResult()
{
	PQclear(m_psqlRes);
}

ConnStatusType BasePsql::getConnectStatus()
{
	return PQstatus(m_psqlConn);
}

ExecStatusType BasePsql::getQuerySqlStatus()
{
	return PQresultStatus(m_psqlRes);
}

void BasePsql::onStartConnect()
{
	bool ok = false;
	int retCode = PCS_OK;
	do 
	{
		const DBBaseCfg* baseCfg = m_cfg->getDBBaseCfg();
		if (!baseCfg)
		{
			LOG_DBSERVER.printLog("read DBBaseCfg error");
			retCode = PCS_CONFIG_ERROR;
			break;
		}

		m_psqlConn = PQsetdbLogin(
			baseCfg->m_strIp.data(),
			(CAST_TO(std::string, baseCfg->m_nPort)).data(),
			NULL,
			NULL,
			baseCfg->m_strDBName.data(),
			baseCfg->m_strUserName.data(),
			baseCfg->m_strPassword.data()
		);

		if (!m_psqlConn)
		{
			LOG_DBSERVER.printLog("m_psqlConn NULL");
			retCode = PCS_LOGIN_NULL_POINT;
			break;
		}

		if (getConnectStatus() != CONNECTION_OK)
		{
			LOG_DBSERVER.printLog("psql connect error, status code:[%d]", getConnectStatus());
			retCode = PCS_LOGIN_STATUS_ERROR;
			break;
		}

		ok = true;
	} while (0);

	if (m_cbStartConn.empty())
	{
		LOG_DBSERVER.printLog("m_cbStartConn NULL");
		return;
	}
	m_cbStartConn(ok, retCode);
}
