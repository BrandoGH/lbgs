#include <gtest/gtest.h>
#include <servercommon/commontool/commontool.h>
#include <servercommon/commontool/msgtool/msgtool.h>
#include <servercommon/sysinfomodule/sysinfo.h>
#include <queuemodule/safequeue.hpp>
#include "boost/atomic/atomic.hpp"

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


TEST(SafeQueue, queue_multi_thread_example)
{
	/*
		1.多线程例子，检测是否线程安全
		2.取消注释即可测试
	*/
	//SafeQueue<int> q;
	//for (int i = 0; i < 20; ++i)
	//{
	//	q.push(i);
	//}

	//boost::atomic_int atNum = 15;
	//CommonBoost::Mutex mtx;

	//auto lambdaFun = [&](const std::string& name)
	//{
	//	while (1)
	//	{
	//		//CommonBoost::UniqueLock lock(q.getMutex());
	//		q.opStart();
	//		if (q.empty())
	//		{
	//			q.opEnd();
	//			continue;
	//		}
	//		THREAD_SLEEP(1);
	//		printf("%s---queue size[%d] get front: [%d]\n", name.data(), q.size(), q.front());
	//		q.pop();
	//		q.opEnd();

	//	}
	//};

	//auto lambdaPush = [&](const std::string& name)
	//{
	//	while (1)
	//	{
	//		//CommonBoost::UniqueLock lock(q.getMutex());
	//		q.opStart();
	//		int p = rand();
	//		q.push(p);
	//		printf("%s---queue size[%d] push[%d]\n", name.data(), q.size(), p);
	//		q.opEnd();
	//	}
	//};

	//CommonBoost::Thread t1(lambdaFun, "t1");
	//CommonBoost::Thread t2(lambdaFun, "t2");
	//CommonBoost::Thread t3(lambdaPush, "t3push");
	//t1.join();
	//t2.join();
	//t3.join();

}