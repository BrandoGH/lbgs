#include <gtest/gtest.h>
#include <servercommon/commontool/commontool.h>
#include <servercommon/commontool/msgtool/msgtool.h>
#include <servercommon/basedef.h>
#include <servercommon/sysinfomodule/sysinfo.h>
#include <queuemodule/safequeue.hpp>

TEST(SafeQueue, queue_size)
{
	SafeQueue<int> q;
	q.push(100);
	q.push(200);
	q.push(300);
	EXPECT_EQ(q.front(), 100);
	EXPECT_EQ(q.size(), 3);
	q.pop();
	EXPECT_EQ(q.front(), 200);
	EXPECT_EQ(q.size(), 2);
}

TEST(SafeQueue, queue_type)
{
	SafeQueue<int>			q_int;
	SafeQueue<char>			q_char;
	SafeQueue<std::string>	q_string;

	q_int.push(12);
	q_char.push(0xfc);
	q_string.push("string test");
	EXPECT_EQ(typeid(q_int.front()), typeid(int));
	EXPECT_EQ(typeid(q_char.front()), typeid(char));
	EXPECT_EQ(typeid(q_string.front()), typeid(std::string));
}