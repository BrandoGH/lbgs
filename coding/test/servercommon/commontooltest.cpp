#include <gtest/gtest.h>
#include <servercommon/commontool/commontool.h>
#include <servercommon/commontool/msgtool/msgtool.h>
#include <servercommon/basedef.h>

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


TEST(CommonTool_MsgTool, isLittleEndian)
{
	ushort num = 1;
	DEFINE_BYTE_ARRAY(bArr,sizeof(ushort));
	memmove(bArr, (char*)&num,sizeof(ushort));
	if(bArr[0] == 1)
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());
	}
	else
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, Little2Big)
{
	ushort num = 1;
	DEFINE_BYTE_ARRAY(bArr, sizeof(ushort));
	memmove(bArr, (char*)&num, sizeof(ushort));
	if(bArr[0] == 1)		// µÕ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		// 2 byte
		ushort twoByteLittle = 102;
		ushort twoByteLittle2Big = 26112;
		DEFINE_BYTE_ARRAY(ushortByteArr, sizeof(ushort));
		CommonTool::MsgTool::Little2Big(twoByteLittle, ushortByteArr);
		ushort ushortResult = *(ushort*)ushortByteArr;
		EXPECT_EQ(ushortResult, twoByteLittle2Big);

		// 4byte
		uint fourByteLittle = 102;
		uint fourByteLittle2Big = 1711276032;
		DEFINE_BYTE_ARRAY(intByteArr, sizeof(uint));
		CommonTool::MsgTool::Little2Big(fourByteLittle, intByteArr);
		uint uintResult = *(uint*)intByteArr;
		EXPECT_EQ(uintResult, fourByteLittle2Big);
	}
	else				// ∏ﬂ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}