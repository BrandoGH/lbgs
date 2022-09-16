#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/timermodule/timer.h>
#include <boost/random.hpp>

extern "C"
{
#include <hiredis/hiredis.h>
}

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

// link redis and set/get value
TEST(Redis_Example, test)
{
	/*timeval tv = { 1,500000 };

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

	redisFree(conn);*/
}

// redis verification code study -- test
TEST(Redis_Example_2, test)
{
	/*boost::random::mt19937_64 gen(time(NULL));
	boost::random::uniform_int_distribution<> dist(100000,999999);
	int r = dist(gen);
	
	timeval tv = { 1,500000 };

	redisContext* conn = redisConnectWithTimeout("192.168.43.122", 6379, tv);
	if (conn != NULL && conn->err)
	{
		printf("connection error: %s\n", conn->errstr);
		return;
	}

	redisReply* reply = NULL;
	reply = (redisReply*)redisCommand(conn, "AUTH default %s", "123456");
	if (reply->type != REDIS_REPLY_ERROR)
	{
		printf("Redis password ok\n");
	} else
	{
		printf("Redis password error\n");
	}
	freeReplyObject(reply);

	reply = (redisReply*)redisCommand(conn, "setnx check_code %d", r);
	freeReplyObject(reply);

	reply = (redisReply*)redisCommand(conn, "setnx input_count 0");
	freeReplyObject(reply);

	reply = (redisReply*)redisCommand(conn, "expire check_code 120");
	freeReplyObject(reply);

	int intpu_count = 0;
	reply = (redisReply*)redisCommand(conn, "get input_count");
	intpu_count = CAST_TO(int, reply->str);
	freeReplyObject(reply);
	if (intpu_count == 3)
	{
		printf("input caount == 3\n");
		return;
	}

	int retCode = 0;
	reply = (redisReply*)redisCommand(conn, "get check_code");
	retCode = CAST_TO(int, reply->str);
	freeReplyObject(reply);

	printf("please input check code: [%d]\n", retCode);

	int inputCneck = 0;
	scanf("%d",&inputCneck);
	if (inputCneck == retCode)
	{
		printf("check code ok\n");
	}
	else
	{
		printf("check code error\n");
	}

	reply = (redisReply*)redisCommand(conn, "incr input_count");
	freeReplyObject(reply);

	redisFree(conn);*/

}

}
