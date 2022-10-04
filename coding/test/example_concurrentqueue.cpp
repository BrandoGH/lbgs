#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/basedef.h>

#include <concurrentqueue/concurrentqueue.h>

namespace
{

TEST(Example_Concurrentqueue_Base, test)
{
	/*moodycamel::ConcurrentQueue<int> q;

	for (int i = 0; i != 123; ++i)
	{
		q.enqueue(i);
		printf_color(PRINTF_YELLOW, "enqueue: %d\n", i);
	}

	int item;
	for (int i = 0; i != 123; ++i)
	{
		q.try_dequeue(item);
		printf_color(PRINTF_YELLOW, "try_dequeue: %d\n", i);
		assert(item == i);
	}
	return;*/
}

TEST(Example_Concurrentqueue_Producers_Consumers, test)
{
	//moodycamel::ConcurrentQueue<int> q;
	//int dequeued[100] = { 0 };
	//std::thread threads[20];

	//// Producers
	//for (int i = 0; i != 10; ++i)
	//{
	//	threads[i] = std::thread([&](int i)
	//	{
	//		for (int j = 0; j != 10; ++j)
	//		{
	//			q.enqueue(i * 10 + j);
	//			printf_color(PRINTF_YELLOW, "enqueue: %d\n", i * 10 + j);
	//		}
	//	}, i);
	//}

	//// Consumers
	//for (int i = 10; i != 20; ++i)
	//{
	//	threads[i] = std::thread([&]()
	//	{
	//		int item;
	//		for (int j = 0; j != 20; ++j)
	//		{
	//			if (q.try_dequeue(item))
	//			{
	//				++dequeued[item];
	//				printf_color(PRINTF_YELLOW, "try_dequeue: %d\n", item);
	//			}
	//		}
	//	});
	//}

	//// Wait for all threads
	//for (int i = 0; i != 20; ++i)
	//{
	//	threads[i].join();
	//}

	//// Collect any leftovers (could be some if e.g. consumers finish before producers)
	//int item;
	//while (q.try_dequeue(item))
	//{
	//	++dequeued[item];
	//}

	//// Make sure everything went in and came back out!
	//for (int i = 0; i != 100; ++i)
	//{
	//	assert(dequeued[i] == 1);
	//}
}
}