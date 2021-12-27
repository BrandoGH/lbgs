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

	if(!isLittleEndian()) // 如果系统是大端存储,无需转换
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
		outByte[0] = (num >> 24) & 0xFFFFFFFF;
		outByte[1] = (num >> 16) & 0xFFFFFFFF;
		outByte[2] = (num >> 8) & 0xFFFFFFFF;
		outByte[3] = (num) & 0xFFFFFFFF;
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

	if(!isLittleEndian()) // 如果系统是大端存储,无需转换
	{
		outNum = *(OutputNumType*)inputBytes;
		return true;
	}

	if(len == sizeof(ushort))
	{
		ushort tmp = *(ushort*)inputBytes;
		outNum = ((tmp >> 8) &0x00FF) | 
				((tmp << 8) & 0xFF00);
	}
	else if(len == sizeof(uint))
	{
		uint tmp = *(uint*)inputBytes;
		outNum = (tmp >> 24 & 0x000000FF) | 
				((tmp >> 8) & 0x0000FF00) | 
				((tmp << 8) & 0x00FF0000) | 
				((tmp << 24) & 0xFF000000);
	}
	return true;
}

}}

#endif // !__MSG_TOOL_H__
