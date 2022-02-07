#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/timermodule/timer.h>


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

TEST(main_test, test)
{
	
}

}
