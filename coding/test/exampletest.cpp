#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>

TEST(Cpp_Example, test)
{
	while(true)
	{
		int data = 66;
		std::cout << "Cpp_Example: data = " << data << std::endl;
		THREAD_SLEEP(1000);
	}

	getchar();
}