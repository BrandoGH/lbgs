#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/basedef.h>

#include <libpq-fe.h>

namespace
{

TEST(Postgresql_Example, test)
{
	//PGconn* conn = PQsetdbLogin("192.168.43.122", "5432", NULL, NULL, "postgres", "postgres", "123456");
	//if (!conn)
	//{
	//	PQfinish(conn);
	//	return;
	//}
	//if (PQstatus(conn) != CONNECTION_OK)
	//{
	//	PQfinish(conn);
	//	return;
	//}
	//printf_color(PRINTF_YELLOW, "connect postgres ok!!\n");

	//PGresult* res = PQexec(conn, "SELECT * FROM role");
	//int ret = PQresultStatus(res);
	//if (ret != PGRES_TUPLES_OK)
	//{
	//	PQclear(res);
	//	PQfinish(conn);
	//	return;
	//}

	//// col name
	//int nFields = PQnfields(res);
	//for (int i = 0; i < nFields; ++i)
	//{
	//	char* str = PQfname(res, i);
	//	printf("%-15s", str);
	//}
	//printf("\n");


	//// col value
	//for (int i = 0; i < PQntuples(res); i++)
	//{
	//	for (int j = 0; j < nFields; j++)
	//	{
	//		char* str = PQgetvalue(res, i, j);
	//		printf("%-15s", str);
	//	}
	//	printf("\n");
	//}
	//PQclear(res);


	//PQfinish(conn);
}
}