#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/timermodule/timer.h>

class TimerTest : public Timer
{
public:
	virtual void timeoutRun()
	{
		std::cout << 454545 << std::endl;
	}

};

TEST(Cpp_Example, test)
{
	/*while(true)
	{
		int data = 66;
		std::cout << "Cpp_Example: data = " << data << std::endl;
		THREAD_SLEEP(1000);
	}*/

	TimerTest pc;
	pc.setInterval(500);
	pc.start();
	

	getchar();
}