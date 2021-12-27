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
	if(bArr[0] == 1)		// 低端存储系统测试
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		// 2 byte
		ushort twoByteLittle = 102;				// 小端内存存储方式:[01100110][00000000]
		ushort twoByteLittle2Big = 26112;		// 小端内存存储方式:[00000000][01100110]
		DEFINE_BYTE_ARRAY(ushortByteArr, sizeof(ushort));
		memmove(ushortByteArr,(char*)&twoByteLittle,sizeof(ushort));
		EXPECT_EQ(ushortByteArr[0], 102);
		EXPECT_EQ(ushortByteArr[1], 0);

		CommonTool::MsgTool::littleEndian2Big(twoByteLittle, ushortByteArr);
		ushort ushortResult = *(ushort*)ushortByteArr;
		EXPECT_EQ(ushortResult, twoByteLittle2Big);
		EXPECT_EQ(ushortByteArr[0], 0);
		EXPECT_EQ(ushortByteArr[1], 102);
	}
	else				// 高端存储系统测试
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, littleEndian2Big_4byte)
{
	ushort num = 1;
	DEFINE_BYTE_ARRAY(bArr, sizeof(ushort));
	memmove(bArr, (char*)&num, sizeof(ushort));
	if(bArr[0] == 1)		// 低端存储系统测试
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		uint fourByteLittle = 102;						// 小端内存存储方式:[01100110][00000000][00000000][00000000]
		uint fourByteLittle2Big = 1711276032;			// 小端内存存储方式:[00000000][00000000][00000000][01100110]
		DEFINE_BYTE_ARRAY(intByteArr, sizeof(uint));
		memmove(intByteArr, (char*)&fourByteLittle, sizeof(uint));
		EXPECT_EQ(intByteArr[0], 102);
		EXPECT_EQ(intByteArr[1], 0);
		EXPECT_EQ(intByteArr[2], 0);
		EXPECT_EQ(intByteArr[3], 0);

		CommonTool::MsgTool::littleEndian2Big(fourByteLittle, intByteArr);
		uint uintResult = *(uint*)intByteArr;
		EXPECT_EQ(uintResult, fourByteLittle2Big);
		EXPECT_EQ(intByteArr[0], 0);
		EXPECT_EQ(intByteArr[1], 0);
		EXPECT_EQ(intByteArr[2], 0);
		EXPECT_EQ(intByteArr[3], 102);
	}
	else				// 高端存储系统测试
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, littleEndian2Big_bigEndianSystem)
{
	// 10 大端存储二进制值层面
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
	if(bArr[0] == 1)		// 低端存储系统测试
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		uint fourByteLittle = 102;
		int intByteArr[sizeof(uint)] = {0};
		EXPECT_FALSE(CommonTool::MsgTool::littleEndian2Big(fourByteLittle, intByteArr));
	}
	else				// 高端存储系统测试
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, littleEndian2Big_lengthConsistency)
{
	ushort num = 1;
	DEFINE_BYTE_ARRAY(bArr, sizeof(ushort));
	memmove(bArr, (char*)&num, sizeof(ushort));
	if(bArr[0] == 1)		// 低端存储系统测试
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		ushort num = 10;
		DEFINE_BYTE_ARRAY(byteArr, sizeof(ushort) + 1);
		EXPECT_FALSE(CommonTool::MsgTool::littleEndian2Big(num, byteArr));

		DEFINE_BYTE_ARRAY(byteArr2, sizeof(ushort));
		EXPECT_TRUE(CommonTool::MsgTool::littleEndian2Big(num, byteArr2));
	}
	else				// 高端存储系统测试
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, bigEndian2Little_ok)
{
	// 2 bytes
	DEFINE_BYTE_ARRAY(bigArr, sizeof(ushort));
	memmove(bigArr, "\x00\x0A",sizeof(bigArr));
	ushort bigNum = *(ushort*)bigArr;
	ushort bigNumRes = 0x0A00;
	EXPECT_EQ(bigNum, bigNumRes);
	ushort ushortResult = 0;
	ushort ushortResult2 = 0x000A;
	CommonTool::MsgTool::bigEndian2Little(bigArr, ushortResult);
	EXPECT_EQ(ushortResult, ushortResult2);

	// 4 bytes
	DEFINE_BYTE_ARRAY(bigArr2, sizeof(uint));
	memmove(bigArr2, "\x00\x0E\x00\x0A", sizeof(bigArr2));
	uint bigNum2 = *(uint*)bigArr2;
	uint bigNum2Res = 0x0A000E00;
	EXPECT_EQ(bigNum2, bigNum2Res);
	uint uintResult = 0;
	uint uintResult2 = 0x000E000A;
	CommonTool::MsgTool::bigEndian2Little(bigArr2, uintResult);
	EXPECT_EQ(uintResult, uintResult2);
}

TEST(CommonTool_MsgTool, bigEndian2Little_error)
{
	// array type is not byte type
	ushort arr[2] = {0,0};
	ushort result = 0;
	EXPECT_FALSE(CommonTool::MsgTool::bigEndian2Little(arr, result));
	EXPECT_EQ(result, 0);

	// out number type != bytes array type
	DEFINE_BYTE_ARRAY(arr2, sizeof(ushort));
	memmove(arr2, "\x0A\x00", sizeof(arr2));
	int result2 = 0;
	EXPECT_FALSE(CommonTool::MsgTool::bigEndian2Little(arr2, result2));
	EXPECT_EQ(result2, 0);
}