#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/timermodule/timer.h>
#include <hiredis/hiredis.h>

// 定时器用法
namespace
{
class TimerTest : public Timer
{
public:
	virtual void timeoutRun()
	{
		std::cout << 454545 << std::endl;
	}

};
TEST(Timer_Example, test)
{
	/*TimerTest pc;
	pc.setInterval(500);
	pc.start();
	getchar();*/
}

TEST(Redis_Example, test)
{
	timeval tv = { 1,500000 };

	redisContext* conn = redisConnectWithTimeout("192.168.43.122", 6379, tv);
	if (conn != NULL && conn->err)
	{
		printf("connection error: %s\n", conn->errstr);
		return;
	}

	redisReply* replya = NULL;
	replya = (redisReply*)redisCommand(conn, "AUTH default %s", "123456");
	if (replya->type != REDIS_REPLY_ERROR)
	{
		printf("Redis password ok\n");
	} else
	{
		printf("Redis password error\n");
	}
	freeReplyObject(replya);

	redisReply* reply = NULL;
	reply = (redisReply*)redisCommand(conn, "SET %s %s", "foo", "bar");
	freeReplyObject(reply);

	reply = (redisReply*)redisCommand(conn, "GET %s", "foo");
	printf("%s\n", reply->str);
	freeReplyObject(reply);

	redisFree(conn);
}

TEST(main_test, test)
{
	
}

}
