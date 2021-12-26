#ifndef __MSG_TOOL_H__
#define __MSG_TOOL_H__

#include "commontool/commontool.h"

namespace CommonTool 
{
namespace MsgTool
{
/*
	大小端的一些工具函数(big endian and little endian)
*/

// 当前计算机是否是低字节存储
bool isLittleEndian();

// 低字节 -> 高字节 【目前只支持 两字节和四字节】
template<class InputNumType, class ByteArrayType>
bool Little2Big(InputNumType num, ByteArrayType& outByte)
{
	int len = getArraySize(outByte);
	if (sizeof(outByte[0]) != 1)
	{
		return false;
	}
	if (len != sizeof(InputNumType))
	{
		return false;
	}

	if (!isLittleEndian()) // 不是低字节存储的，就不用转了
	{
		char* arr = (char*)&num;
		memmove(outByte, arr, len);
		return true;
	}

	if (len == 2)
	{
		outByte[0] = (num >> 8) & 0xffff;
		outByte[1] = (num) & 0xffff;
	}
	else if (len == 4)
	{
		outByte[0] = (num >> 24) & 0xffffffff;
		outByte[1] = (num >> 16) & 0xffffffff;
		outByte[2] = (num >> 8) & 0xffffffff;
		outByte[3] = (num) & 0xffffffff;
	}

	return true;
}

}}

#endif // !__MSG_TOOL_H__
