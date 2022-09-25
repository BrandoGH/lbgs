#include <gtest/gtest.h>
#include <servercommon/sysinfomodule/minidump/minidump.h>
#include <servercommon/basedef.h>


TEST(main_test, test)
{
	// example code 
	printf_color(PRINTF_YELLOW, "-----------------------------------------------  main_test_start ----------------------------------------------\n");
	printf_color(PRINTF_YELLOW, "// You can write your test code here(local temp) , such as \"hello world !!\" \n");


	printf_color(PRINTF_YELLOW, "-----------------------------------------------  main_test_end   ----------------------------------------------\n");

}

int main(int argc, char** argv)
{
#ifdef WIN_OS
	assert(LbgsMinisDump::autoDump() != NULL);
#endif
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}