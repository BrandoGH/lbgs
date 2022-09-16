#include <gtest/gtest.h>
#include <servercommon/sysinfomodule/minidump/minidump.h>

TEST(main_test, test)
{
	// example code 
}

int main(int argc, char** argv)
{
#ifdef WIN_OS
	assert(LbgsMinisDump::autoDump() != NULL);
#endif
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}