#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/timermodule/timer.h>
#include <timermodule/timer2.h>

// Timer use example
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

TEST(Timer2_Example, start_test)
{
	/*CommonBoost::IOServer ios;
	boost::shared_ptr<Timer2> t = boost::make_shared<Timer2>(ios);
	t->setInterval(1000);
	t->start(
		[&, t]()
	{
		static int count = 1;
		if (count == 5)
		{
			t->pause();
			printf_color(PRINTF_YELLOW, "pause\n");
			THREAD_SLEEP(3000);
			t->resume();
			printf_color(PRINTF_YELLOW, "resume\n");
		}
		printf_color(PRINTF_YELLOW, "time2 is timeout func, count[%d]\n", count);
		count++;
	});
	ios.run();*/
}

TEST(Timer2_Example, stop_test)
{
	//CommonBoost::IOServer ios;
	//boost::shared_ptr<Timer2> t = boost::make_shared<Timer2>(ios);
	//t->setInterval(1000);
	//t->start(
	//	[&, t]()
	//{
	//	static int count = 1;
	//	if (count == 5)
	//	{
	//		t->stop();
	//		printf_color(PRINTF_YELLOW, "stop\n");
	//		THREAD_SLEEP(3000);
	//		t->resume(); // invalid
	//		printf_color(PRINTF_YELLOW, "resume\n");
	//	}
	//	printf_color(PRINTF_YELLOW, "time2 is timeout func, count[%d]\n", count);
	//	count++;
	//});
	//ios.run();
}

TEST(Timer2_Example, shot_once_test)
{
	/*CommonBoost::IOServer ios;
	boost::shared_ptr<Timer2> t = boost::make_shared<Timer2>(ios);
	t->setInterval(3000);
	t->start(
		[&]()
	{
		printf_color(PRINTF_YELLOW, "time2 is timeout func once\n");
	}, false);
	ios.run();*/
}

}
