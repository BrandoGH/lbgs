#include <gtest/gtest.h>
#include <servercommon/commontool/commontool.h>


TEST(CommonTool, getArraySize_not_view_size)
{
	int arrInt[] = { 1,2,3 };
	EXPECT_EQ(CommonTool::getArraySize(arrInt),3);

	int arrInt2[] = { 1,2,3,4,5,6 };
	EXPECT_EQ(CommonTool::getArraySize(arrInt2), 6);

	std::string strArr[] = { "string1","string2" ,"string3" ,"string4" ,"string5" };
	EXPECT_EQ(CommonTool::getArraySize(strArr), 5);

	const char* str2Arr[] = { "string1","string2" ,"string3" ,"string4" };
	EXPECT_EQ(CommonTool::getArraySize(str2Arr), 4);
}

TEST(CommonTool, getArraySize_view_size)
{
	int arrInt[5] = { 1,2,3 };
	EXPECT_EQ(CommonTool::getArraySize(arrInt), 5);

	int arrInt2[6] = { 1,2,3,4,5,6 };
	EXPECT_EQ(CommonTool::getArraySize(arrInt2), 6);

	std::string strArr[5] = { "string1","string2" ,"string3" ,"string4" ,"string5" };
	EXPECT_EQ(CommonTool::getArraySize(strArr), 5);

	const char* str2Arr[10] = { "string1","string2" ,"string3" ,"string4" };
	EXPECT_EQ(CommonTool::getArraySize(str2Arr), 10);
}