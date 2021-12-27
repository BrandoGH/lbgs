#ifndef __MSG_TOOL_H__
#define __MSG_TOOL_H__

#include "commontool/commontool.h"
#include "basedef.h"

#include<algorithm>

namespace CommonTool 
{
namespace MsgTool
{
/*
	大小端的一些工具函数(big endian and little endian)
*/

// 当前计算机是否是低字节存储
bool isLittleEndian();

/*
	目前只支持低字节存储的服务器环境,高低字节字节存储互相转换
*/
// 低字节 -> 高字节 【目前只支持 两字节和四字节】
template<class InputNumType, class ByteArrayType>
bool littleEndian2Big(InputNumType num, ByteArrayType& outByte)
{
	int len = getArraySize(outByte);

	if (sizeof(outByte[0]) != sizeof(byte))
	{
		return false;
	}
	if (len != sizeof(InputNumType))
	{
		return false;
	}

	if(!isLittleEndian()) // 不是低字节存储的，就不用转了
	{
		byte* arr = (byte*)&num;
		memmove(outByte, arr, len);
		return true;
	}

	if (len == sizeof(ushort))
	{
		outByte[0] = (num >> 8) & 0xffff;
		outByte[1] = (num) & 0xffff;
	}
	else if (len == sizeof(uint))
	{
		outByte[0] = (num >> 24) & 0xffffffff;
		outByte[1] = (num >> 16) & 0xffffffff;
		outByte[2] = (num >> 8) & 0xffffffff;
		outByte[3] = (num) & 0xffffffff;
	}

	return true;
}

// 高字节 -> 低字节 【目前只支持 两字节和四字节】
template<class ByteArrayType, class OutputNumType>
bool bigEndian2Little(ByteArrayType& inputBytes, OutputNumType& outNum)
{
	int len = getArraySize(inputBytes);

	if(sizeof(inputBytes[0]) != sizeof(byte))
	{
		return false;
	}
	if(len != sizeof(OutputNumType))
	{
		return false;
	}

	if(isLittleEndian()) // 不是高字节存储的，就不用转了
	{
		outNum = *(OutputNumType*)inputBytes;
		return true;
	}

	DEFINE_BYTE_ARRAY(byteArray, sizeof(OutputNumType));
	memmove(byteArray, inputBytes, sizeof(OutputNumType));
	std::reverse(byteArray, byteArray + sizeof(OutputNumType) - 1);
	outNum = *(OutputNumType*)byteArray;

	return true;
}

}}

#endif // !__MSG_TOOL_H__
