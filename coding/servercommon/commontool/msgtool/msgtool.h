#ifndef __MSG_TOOL_H__
#define __MSG_TOOL_H__

#include "commontool/commontool.h"
#include "servercommon/basedef.h"
#include "boostmodule/basedef.h"
#include "basedef.h"

#include <openssl/md5.h>
#include <algorithm>


namespace CommonTool 
{
namespace MsgTool
{
/*
	��С�˵�һЩ���ߺ���(big endian and little endian)
*/

// ��ǰ������Ƿ��ǵ��ֽڴ洢
bool isLittleEndian();
bool isBytesDataEQ(const byte* data1, const byte* data2, uint len);

/*
	Ŀǰֻ֧�ֵ��ֽڴ洢�ķ���������,�ߵ��ֽ��ֽڴ洢����ת�� number to bytes
*/
// ���ֽ� -> ���ֽ� ��Ŀǰֻ֧�� ���ֽں����ֽڡ�������Ǵ��ϵͳ���򱾺���Ϊ���ֽ� ����> ���ֽڡ�
template<class InputNumType, class ByteArrayType>
bool byteSeqTransformN2B(InputNumType num, ByteArrayType& outByte)
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

	if (len == sizeof(ushort))
	{
		outByte[0] = (num >> 8) & 0XFFFF;
		outByte[1] = (num) & 0XFFFF;
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

// ���ֽ� -> �ֽ� ��Ŀǰֻ֧�� ���ֽں����ֽڡ�������Ǵ��ϵͳ���򱾺���Ϊ���ֽ� ����> ���ֽڡ�
template<class ByteArrayType, class OutputNumType>
bool byteSeqTransformB2N(ByteArrayType& inputBytes, OutputNumType& outNum)
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

// ���� -> MD5  Ĭ��32λ
template<class OutMd5BytesType>
bool data2Md5(
	const byte* data,	// ������֤�Ƿ�ΪNULL,���Ƕ�����
	uint dataSize,
	OutMd5BytesType& outBytes, 
	std::string* outMd5String = NULL)
{
	if(CommonTool::getArraySize(outBytes) != MD5_DIGEST_LENGTH ||
		(sizeof(outBytes[0]) != sizeof(byte)))
	{
		return false;
	}

	MD5_CTX ctx;
	DEFINE_BYTE_ARRAY(md5, MD5_DIGEST_LENGTH);

	MD5_Init(&ctx);
	MD5_Update(&ctx, data, dataSize);
	MD5_Final(md5, &ctx);

	memmove(outBytes, md5, MD5_DIGEST_LENGTH);

	// ת��32λ�ַ���
	if(outMd5String != NULL)
	{
		outMd5String->clear();
		for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
		{
			char tmp[3];
			sprintf(tmp, "%02x", outBytes[i]);
			outMd5String->append(tmp);
		}
		transform(outMd5String->begin(), outMd5String->end(), outMd5String->begin(), toupper);
	}

	return true;
}

// 32MD5�ַ��� -> 16�ֽ�����
template<class OutMd5BytesType>
bool Md5Str2Bytes(const std::string& str32, OutMd5BytesType& outBytes)
{
	if(str32.size() != 32)
	{
		return false;
	}

	if(CommonTool::getArraySize(outBytes) != MD5_DIGEST_LENGTH ||
		(sizeof(outBytes[0]) != sizeof(byte)))
	{
		return false;
	}

	for(int i = 0; i < str32.size(); i += 2)
	{
		str32.substr(i,2);
	}

	int inStringIdx = 0;
	std::string tmpStr;
	int hexBigNum = 0;
	int hexLittleNum = 0;
	for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
	{
		tmpStr = str32.substr(inStringIdx, 2);
		transform(tmpStr.begin(), tmpStr.end(), tmpStr.begin(), toupper);

		if(!CommonTool::getNumByHexSymbol(tmpStr.at(0), hexBigNum) || 
			!CommonTool::getNumByHexSymbol(tmpStr.at(1), hexLittleNum))
		{
			continue;
		}
		byte hex = hexBigNum * 16 + hexLittleNum;
		outBytes[i] = hex;

		inStringIdx += 2;
	}

	return true;
}

template<class BytesArrayMd51, class BytesArrayMd512>
bool isBytesMd5EQ(BytesArrayMd51& data1, BytesArrayMd512& data2)
{
	if(CommonTool::getArraySize(data1) != MD5_DIGEST_LENGTH ||
		CommonTool::getArraySize(data2) != MD5_DIGEST_LENGTH)
	{
		return false;
	}

	if(sizeof(data1[0]) != sizeof(byte) || sizeof(data2[0]) != sizeof(byte))
	{
		return false;
	}

	for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
	{
		if(data1[i] != data2[i])
		{
			return false;
		}
	}

	return true;
}

}}

#endif // !__MSG_TOOL_H__
