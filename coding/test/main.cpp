#include <gtest/gtest.h>

TEST(test,t)
{
	EXPECT_EQ(3, 3);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}