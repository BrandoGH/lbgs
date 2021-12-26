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

TEST(CommonTool_MsgTool, littleEndian2Big_2byte)
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
		CommonTool::MsgTool::littleEndian2Big(twoByteLittle, ushortByteArr);
		ushort ushortResult = *(ushort*)ushortByteArr;
		EXPECT_EQ(ushortResult, twoByteLittle2Big);
	}
	else				// ∏ﬂ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, littleEndian2Big_4byte)
{
	ushort num = 1;
	DEFINE_BYTE_ARRAY(bArr, sizeof(ushort));
	memmove(bArr, (char*)&num, sizeof(ushort));
	if(bArr[0] == 1)		// µÕ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		uint fourByteLittle = 102;
		uint fourByteLittle2Big = 1711276032;
		DEFINE_BYTE_ARRAY(intByteArr, sizeof(uint));
		CommonTool::MsgTool::littleEndian2Big(fourByteLittle, intByteArr);
		uint uintResult = *(uint*)intByteArr;
		EXPECT_EQ(uintResult, fourByteLittle2Big);
	}
	else				// ∏ﬂ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, littleEndian2Big_bigEndianSystem)
{
	// 10 ¥Û∂À¥Ê¥¢∂˛Ω¯÷∆÷µ≤„√Ê
	DEFINE_BYTE_ARRAY(num, sizeof(ushort));
	num[0] = '\x00';
	num[1] = '\x0A';

	DEFINE_BYTE_ARRAY(twoByteArr, sizeof(ushort));
	memmove(twoByteArr, num, sizeof(twoByteArr));

	ushort res1 = *(ushort*)num;
	ushort res2 = *(ushort*)twoByteArr;
	EXPECT_EQ(res1, res2);
}

TEST(CommonTool_MsgTool, littleEndian2Big_byteArrayMemberIsNotOneByte)
{
	ushort num = 1;
	DEFINE_BYTE_ARRAY(bArr, sizeof(ushort));
	memmove(bArr, (char*)&num, sizeof(ushort));
	if(bArr[0] == 1)		// µÕ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		uint fourByteLittle = 102;
		int intByteArr[sizeof(uint)] = {0};
		EXPECT_FALSE(CommonTool::MsgTool::littleEndian2Big(fourByteLittle, intByteArr));
	}
	else				// ∏ﬂ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, littleEndian2Big_lengthConsistency)
{
	ushort num = 1;
	DEFINE_BYTE_ARRAY(bArr, sizeof(ushort));
	memmove(bArr, (char*)&num, sizeof(ushort));
	if(bArr[0] == 1)		// µÕ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		ushort num = 10;
		DEFINE_BYTE_ARRAY(byteArr, sizeof(ushort) + 1);
		EXPECT_FALSE(CommonTool::MsgTool::littleEndian2Big(num, byteArr));

		DEFINE_BYTE_ARRAY(byteArr2, sizeof(ushort));
		EXPECT_TRUE(CommonTool::MsgTool::littleEndian2Big(num, byteArr2));
	}
	else				// ∏ﬂ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}