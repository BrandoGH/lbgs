#ifndef __BASE_PSQL_H__
#define __BASE_PSQL_H__

#include <libpq-fe.h>
#include <boost/function.hpp>
#include <boostmodule/basedef.h>
#include <servercommon/basedef.h>

class DBServerConfig;
class BasePsql
{
public:
	// callback
	typedef boost::function< void(
		bool,		/*ok*/
		int			/*PsqlConnectStatus*/
		) > CallbackStartConnect;
	typedef boost::function< void(
		bool,			/*ok*/
		ExecStatusType	/*query result type*/
		) > CallbackQuery;

public:
	enum PsqlConnectStatus
	{
		PCS_OK,

		// error code
		PCS_CONFIG_ERROR,
		PCS_LOGIN_NULL_POINT,
		PCS_LOGIN_STATUS_ERROR,
	};

public:
	BasePsql();
	~BasePsql();

	void setStartConnectCallback(CallbackStartConnect callback);
	void startConnectPostgresql();

	/*
	* common op
	* ouput a table example:
	* 
	sql->query(R"(SELECT * FROM test)",
		[=](bool ok, ExecStatusType ec)
	{
		if (!ok)
		{
			return;
		}
		BasePsql* m_sql = DB_MGR->getBaseSql();

		// col name
		int nFields = m_sql->getColumns();
		for (int i = 0; i < nFields; ++i)
		{
			std::string str = m_sql->getColumnName(i);
			printf_color(PRINTF_YELLOW ,"%-15s", str.data());
		}
		printf("\n");


		// ouput col value
		for (int i = 0; i < m_sql->getRows(); i++)
		{
			for (int j = 0; j < nFields; j++)
			{
				std::string str = m_sql->getValue(i, j);
				printf_color(PRINTF_YELLOW, "%-15s", str.data());
			}
			printf("\n");
		}
	});
	*/
	bool query(const std::string& sql, CallbackQuery callback);
	bool query(
		const std::string& sql, 
		int nParams,
		const Oid* paramTypes,
		const char* const* paramValues,
		const int* paramLengths,
		const int* paramFormats,
		int resultForma,	/*0-text  1-binary*/
		CallbackQuery callback);
	uint getRows();
	uint getColumns();

	// TRANSACTION 
	bool start(CallbackQuery callback = NULL);
	bool end(CallbackQuery callback = NULL);
	bool commit(CallbackQuery callback = NULL);
	bool rollback(CallbackQuery callback = NULL);
	bool savepoint(const std::string& saveName, CallbackQuery callback = NULL);
	bool rollbackToSavepoint(const std::string& saveName, CallbackQuery callback = NULL);
	bool releaseSavepoint(const std::string& saveName, CallbackQuery callback = NULL);

	// row and col start index from 0
	std::string getValue(uint row, uint col);
	std::string getColumnName(uint col);

HANDLER:
	void onStartConnect();

private:
	void finish();
	void clearResult();
	ConnStatusType getConnectStatus();
	ExecStatusType getQuerySqlStatus();

private:
	PGconn* m_psqlConn;
	PGresult* m_psqlRes;
	CommonBoost::Mutex m_mtx;

	// config
	DBServerConfig* m_cfg;

	// callback
	CallbackStartConnect m_cbStartConn;
};

#endif // !__BASE_PSQL_H__
