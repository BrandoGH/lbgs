#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/basedef.h>
#include <boost/coroutine2/coroutine.hpp>

namespace
{
TEST(Example_Boost_Coroutine_PULL, pull_type)
{
	//typedef boost::coroutines2::coroutine<int>   coro_t2;
	//std::cout << "start corountine" << std::endl;
	//coro_t2::pull_type source( // constructor enters coroutine-function
	//	[&](coro_t2::push_type& sink)
	//{
	//	std::cout << " sink1" << std::endl;   // start process will run here first
	//	sink(1); // push {1} back to main-context
	//	std::cout << " sink1" << std::endl;
	//	sink(1); // push {1} back to main-context
	//	std::cout << " sink2" << std::endl;
	//	sink(2); // push {2} back to main-context
	//	std::cout << " sink3" << std::endl;
	//	sink(3); // push {3} back to main-context
	//});
	//std::cout << "start while" << std::endl;
	//while (source)    // test if pull-coroutine is valid
	//{
	//	int ret = source.get(); // pushed data,that is the argument of sink()
	//	std::cout << "move to coroutine-function " << ret << std::endl;
	//	source();             // context-switch to coroutine-function
	//	std::cout << "back from coroutine-function " << std::endl;
	//}
}

TEST(Example_Boost_Coroutine_Push, push_type)
{
	//typedef boost::coroutines2::coroutine<int>   coro_t3;
	//std::cout << "start corountine" << std::endl;
	//coro_t3::push_type sink(
	//	[&](coro_t3::pull_type& source)
	//{
	//	int num;
	//	num = source.get();
	//	std::cout << " source1: " << num << std::endl;
	//	source();   //back to main-context
	//	std::cout << " source2: " << num << "   " << source.get() << std::endl;
	//	source();
	//	std::cout << " source3: " << num << "   " << source.get() << std::endl;
	//	source();
	//});
	//std::cout << "start while" << std::endl;
	//int i = 1;
	//while (sink)    // test if pull-coroutine is valid
	//{
	//	std::cout << "move to coroutine-function " << i << std::endl;
	//	sink(i);             // context-switch to coroutine-function
	//	std::cout << "back from coroutine-function " << i << std::endl;
	//	i++;
	//}
}


void cont(
	boost::coroutines2::coroutine<int>::push_type& sink
)
{
	sink(9527);
	std::cout << "a=";
	sink(123); 
	std::cout << "b=";
	sink(423);
	std::cout << "c=";
}
TEST(Example_Boost_Coroutine_Complex, test)
{
	/*boost::coroutines2::coroutine<int>::pull_type source(cont);

	std::cout << "1";
	source();

	std::cout << "2";
	source();
	std::cout << "3";

	int ret1 = source.get();
	std::cout << std::endl << "ret1: " << ret1 << std::endl;

	int ret2 = source.get();
	std::cout << std::endl << "ret2: " << ret2 << std::endl;

	source();

	int ret3 = source.get();
	std::cout << std::endl << "ret3: " << ret3 << std::endl;

	int ret4 = source.get();
	std::cout << std::endl << "ret4: " << ret4 << std::endl;

	std::cout << std::endl;*/
}
}