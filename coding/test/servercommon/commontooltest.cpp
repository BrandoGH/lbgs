#include <gtest/gtest.h>
#include <servercommon/commontool/commontool.h>
#include <servercommon/commontool/msgtool/msgtool.h>
#include <servercommon/basedef.h>
#include <servercommon/sysinfomodule/sysinfo.h>

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

TEST(CommonTool_MsgTool, byteSeqTransformN2B_2byte)
{
	ushort num = 1;
	DEFINE_BYTE_ARRAY(bArr, sizeof(ushort));
	memmove(bArr, (char*)&num, sizeof(ushort));
	if(bArr[0] == 1)		// µÕ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		// 2 byte
		ushort twoByteLittle = 102;				// –°∂Àƒ⁄¥Ê¥Ê¥¢∑Ω Ω:[01100110][00000000]
		ushort twoByteLittle2Big = 26112;		// –°∂Àƒ⁄¥Ê¥Ê¥¢∑Ω Ω:[00000000][01100110]
		DEFINE_BYTE_ARRAY(ushortByteArr, sizeof(ushort));
		memmove(ushortByteArr,(char*)&twoByteLittle,sizeof(ushort));
		EXPECT_EQ(ushortByteArr[0], 102);
		EXPECT_EQ(ushortByteArr[1], 0);

		CommonTool::MsgTool::byteSeqTransformN2B(twoByteLittle, ushortByteArr);
		ushort ushortResult = *(ushort*)ushortByteArr;
		EXPECT_EQ(ushortResult, twoByteLittle2Big);
		EXPECT_EQ(ushortByteArr[0], 0);
		EXPECT_EQ(ushortByteArr[1], 102);
	}
	else				// ∏ﬂ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, byteSeqTransformN2B_4byte)
{
	ushort num = 1;
	DEFINE_BYTE_ARRAY(bArr, sizeof(ushort));
	memmove(bArr, (char*)&num, sizeof(ushort));
	if(bArr[0] == 1)		// µÕ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		uint fourByteLittle = 102;						// –°∂Àƒ⁄¥Ê¥Ê¥¢∑Ω Ω:[01100110][00000000][00000000][00000000]
		uint fourByteLittle2Big = 1711276032;			// –°∂Àƒ⁄¥Ê¥Ê¥¢∑Ω Ω:[00000000][00000000][00000000][01100110]
		DEFINE_BYTE_ARRAY(intByteArr, sizeof(uint));
		memmove(intByteArr, (char*)&fourByteLittle, sizeof(uint));
		EXPECT_EQ(intByteArr[0], 102);
		EXPECT_EQ(intByteArr[1], 0);
		EXPECT_EQ(intByteArr[2], 0);
		EXPECT_EQ(intByteArr[3], 0);

		CommonTool::MsgTool::byteSeqTransformN2B(fourByteLittle, intByteArr);
		uint uintResult = *(uint*)intByteArr;
		EXPECT_EQ(uintResult, fourByteLittle2Big);
		EXPECT_EQ(intByteArr[0], 0);
		EXPECT_EQ(intByteArr[1], 0);
		EXPECT_EQ(intByteArr[2], 0);
		EXPECT_EQ(intByteArr[3], 102);
	}
	else				// ∏ﬂ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, byteSeqTransformN2B_byteArrayMemberIsNotOneByte)
{
	ushort num = 1;
	DEFINE_BYTE_ARRAY(bArr, sizeof(ushort));
	memmove(bArr, (char*)&num, sizeof(ushort));
	if(bArr[0] == 1)		// µÕ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		uint fourByteLittle = 102;
		int intByteArr[sizeof(uint)] = {0};
		EXPECT_FALSE(CommonTool::MsgTool::byteSeqTransformN2B(fourByteLittle, intByteArr));
	}
	else				// ∏ﬂ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, byteSeqTransformN2B_lengthConsistency)
{
	ushort num = 1;
	DEFINE_BYTE_ARRAY(bArr, sizeof(ushort));
	memmove(bArr, (char*)&num, sizeof(ushort));
	if(bArr[0] == 1)		// µÕ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_TRUE(CommonTool::MsgTool::isLittleEndian());

		ushort num = 10;
		DEFINE_BYTE_ARRAY(byteArr, sizeof(ushort) + 1);
		EXPECT_FALSE(CommonTool::MsgTool::byteSeqTransformN2B(num, byteArr));

		DEFINE_BYTE_ARRAY(byteArr2, sizeof(ushort));
		EXPECT_TRUE(CommonTool::MsgTool::byteSeqTransformN2B(num, byteArr2));
	}
	else				// ∏ﬂ∂À¥Ê¥¢œµÕ≥≤‚ ‘
	{
		EXPECT_FALSE(CommonTool::MsgTool::isLittleEndian());
	}
}

TEST(CommonTool_MsgTool, byteSeqTransformB2N_ok)
{
	// 2 bytes
	DEFINE_BYTE_ARRAY(bigArr, sizeof(ushort));
	memmove(bigArr, "\x00\x0A",sizeof(bigArr));
	ushort bigNum = *(ushort*)bigArr;
	ushort bigNumRes = 0x0A00;
	EXPECT_EQ(bigNum, bigNumRes);
	ushort ushortResult = 0;
	ushort ushortResult2 = 0x000A;
	CommonTool::MsgTool::byteSeqTransformB2N(bigArr, ushortResult);
	EXPECT_EQ(ushortResult, ushortResult2);

	// 4 bytes
	DEFINE_BYTE_ARRAY(bigArr2, sizeof(uint));
	memmove(bigArr2, "\x00\x0E\x00\x0A", sizeof(bigArr2));
	uint bigNum2 = *(uint*)bigArr2;
	uint bigNum2Res = 0x0A000E00;
	EXPECT_EQ(bigNum2, bigNum2Res);
	uint uintResult = 0;
	uint uintResult2 = 0x000E000A;
	CommonTool::MsgTool::byteSeqTransformB2N(bigArr2, uintResult);
	EXPECT_EQ(uintResult, uintResult2);
}

TEST(CommonTool_MsgTool, byteSeqTransformB2N_error)
{
	// array type is not byte type
	ushort arr[2] = {0,0};
	ushort result = 0;
	EXPECT_FALSE(CommonTool::MsgTool::byteSeqTransformB2N(arr, result));
	EXPECT_EQ(result, 0);

	// out number type != bytes array type
	DEFINE_BYTE_ARRAY(arr2, sizeof(ushort));
	memmove(arr2, "\x0A\x00", sizeof(arr2));
	int result2 = 0;
	EXPECT_FALSE(CommonTool::MsgTool::byteSeqTransformB2N(arr2, result2));
	EXPECT_EQ(result2, 0);
}



TEST(CommonTool_MsgTool, data2Md5_inputDataNull)
{
	DEFINE_BYTE_ARRAY(bytesRes, 16);
	EXPECT_TRUE(CommonTool::MsgTool::data2Md5(NULL, 0, bytesRes));
}

TEST(CommonTool_MsgTool, data2Md5_outBytesError)
{
	std::string inputString = "test";
	DEFINE_BYTE_ARRAY(bytesRes, 10);
	int outBytes[16];
	EXPECT_FALSE(CommonTool::MsgTool::data2Md5((const byte*)inputString.data(), inputString.size(), bytesRes));
	EXPECT_FALSE(CommonTool::MsgTool::data2Md5((const byte*)inputString.data(), inputString.size(), outBytes));
}

TEST(CommonTool_MsgTool, data2Md5_ok_inputString1)
{
	std::string inputString = "test";
	const char* cInputString = "test";
	const byte bInputString[5] = {'t','e','s','t'};
	std::string resultString = "098F6BCD4621D373CADE4E832627B4F6";
	DEFINE_BYTE_ARRAY(resultCString, 16);
	memmove(resultCString, "\x09\x8F\x6B\xCD\x46\x21\xD3\x73\xCA\xDE\x4E\x83\x26\x27\xB4\xF6", 16);
	DEFINE_BYTE_ARRAY(outBytes, 16);
	std::string outMd5String;
	EXPECT_TRUE(CommonTool::MsgTool::data2Md5((const byte*)inputString.data(), inputString.size(), outBytes, &outMd5String));
	EXPECT_TRUE(CommonTool::MsgTool::data2Md5((const byte*)cInputString, strlen(cInputString), outBytes, &outMd5String));
	EXPECT_TRUE(CommonTool::MsgTool::data2Md5(bInputString, strlen((const char*)bInputString), outBytes,&outMd5String));
	for(int i = 0; i < 16; ++i)
	{
		EXPECT_EQ(resultCString[i], outBytes[i]);
	}
	EXPECT_STREQ(outMd5String.data(), resultString.data());
}

TEST(CommonTool_MsgTool, data2Md5_ok_inputString2)
{
	std::string inputString = "we are fml hah";
	const char* cInputString = "we are fml hah";
	std::string resultString = "300B22248A9A679B92FADE20363E56C7";
	DEFINE_BYTE_ARRAY(resultCString, 16);
	memmove(resultCString, "\x30\x0B\x22\x24\x8A\x9A\x67\x9B\x92\xFA\xDE\x20\x36\x3E\x56\xC7", 16);
	DEFINE_BYTE_ARRAY(outBytes, 16);
	std::string outMd5String;
	EXPECT_TRUE(CommonTool::MsgTool::data2Md5((const byte*)inputString.data(), inputString.size(), outBytes, &outMd5String));
	EXPECT_TRUE(CommonTool::MsgTool::data2Md5((const byte*)cInputString, strlen(cInputString), outBytes, &outMd5String));
	for(int i = 0; i < 16; ++i)
	{
		EXPECT_EQ(resultCString[i], outBytes[i]);
	}
	EXPECT_STREQ(outMd5String.data(), resultString.data());
}

TEST(CommonTool_MsgTool, Md5Str2Bytes_ok)
{
	std::string inString = "300B22248A9A679B92FADE20363E56C7";
	DEFINE_BYTE_ARRAY(outBytes, 16);
	DEFINE_BYTE_ARRAY(resultCString, 16);
	memmove(resultCString, "\x30\x0B\x22\x24\x8A\x9A\x67\x9B\x92\xFA\xDE\x20\x36\x3E\x56\xC7", 16);
	EXPECT_TRUE(CommonTool::MsgTool::Md5Str2Bytes(inString, outBytes));
	for(int i = 0; i < 16; ++i)
	{
		EXPECT_EQ(resultCString[i], outBytes[i]);
	}
}

TEST(CommonTool_MsgTool, Md5Str2Bytes_error)
{
	std::string inString = "300B22248A9A679B92FADE20363E56C7FF";
	DEFINE_BYTE_ARRAY(outBytes, 16);
	DEFINE_BYTE_ARRAY(outBytesErr, 15);
	DEFINE_BYTE_ARRAY(outBytesErr2, 17);
	short outBytesErr3[16];
	short outBytesErr4[14];
	EXPECT_FALSE(CommonTool::MsgTool::Md5Str2Bytes(inString, outBytes));
	EXPECT_FALSE(CommonTool::MsgTool::Md5Str2Bytes(inString, outBytesErr));
	EXPECT_FALSE(CommonTool::MsgTool::Md5Str2Bytes(inString, outBytesErr2));
	EXPECT_FALSE(CommonTool::MsgTool::Md5Str2Bytes(inString, outBytesErr3));
	EXPECT_FALSE(CommonTool::MsgTool::Md5Str2Bytes(inString, outBytesErr4));
}

TEST(CommonTool_MsgTool, isBytesMd5EQ_ok)
{
	DEFINE_BYTE_ARRAY(data1, 16);
	DEFINE_BYTE_ARRAY(data2, 16);
	memmove(data1, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
	memmove(data2, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
	EXPECT_TRUE(CommonTool::MsgTool::isBytesMd5EQ(data1, data2));
}

TEST(CommonTool_MsgTool, isBytesMd5EQ_data1SizeError)
{
	DEFINE_BYTE_ARRAY(data1, 15);
	DEFINE_BYTE_ARRAY(data2, 16);
	memmove(data1, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E", 15);
	memmove(data2, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
	EXPECT_FALSE(CommonTool::MsgTool::isBytesMd5EQ(data1, data2));
}

TEST(CommonTool_MsgTool, isBytesMd5EQ_data2SizeError)
{
	DEFINE_BYTE_ARRAY(data1, 16);
	DEFINE_BYTE_ARRAY(data2, 15);
	memmove(data1, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
	memmove(data2, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E", 15);
	EXPECT_FALSE(CommonTool::MsgTool::isBytesMd5EQ(data1, data2));
}

TEST(CommonTool_MsgTool, isBytesMd5EQ_data1TypeError)
{
	DEFINE_BYTE_ARRAY(data1, 16);
	int data2[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	memmove(data1, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
	EXPECT_FALSE(CommonTool::MsgTool::isBytesMd5EQ(data1, data2));
}

TEST(CommonTool_MsgTool, isBytesMd5EQ_data2TypeError)
{
	int data1[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	DEFINE_BYTE_ARRAY(data2, 16);
	memmove(data2, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
	EXPECT_FALSE(CommonTool::MsgTool::isBytesMd5EQ(data1, data2));
}

TEST(CommonTool_MsgTool, isBytesMd5EQ_md5Error)
{
	DEFINE_BYTE_ARRAY(data1, 16);
	DEFINE_BYTE_ARRAY(data2, 16);
	memmove(data1, "\x01\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
	memmove(data2, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
	EXPECT_FALSE(CommonTool::MsgTool::isBytesMd5EQ(data1, data2));
}

TEST(SystemInfo, isProcessRuning)
{
#ifdef WIN_OS
	EXPECT_TRUE(SystemInfo::isProcessRuning("test.exe"));
	EXPECT_FALSE(SystemInfo::isProcessRuning("liubinniubi.exe"));
#elif LINUX_OS
	EXPECT_TRUE(SystemInfo::isProcessRuning("test"));
	EXPECT_FALSE(SystemInfo::isProcessRuning("liubinniubi.exe"));
	EXPECT_FALSE(SystemInfo::isProcessRuning("liubinniubi"));
#endif
}

TEST(CommonTool, isIpFormat)
{
	std::string ipOk = "255.255.255.6";
	std::string ipOk2 = "127.0.0.1";
	std::string ipErr = "127.0.0.-1";
	std::string ipErr2 = "127.0.0..";
	std::string ipErr3 = "127.0.0.test";

	EXPECT_TRUE(CommonTool::isIpFormat(ipOk));
	EXPECT_TRUE(CommonTool::isIpFormat(ipOk2));
	EXPECT_FALSE(CommonTool::isIpFormat(ipErr));
	EXPECT_FALSE(CommonTool::isIpFormat(ipErr2));
	EXPECT_FALSE(CommonTool::isIpFormat(ipErr3));
}