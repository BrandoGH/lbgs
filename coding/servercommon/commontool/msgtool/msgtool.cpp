#include "msgtool.h"
#include "msgmodule/msgcommondef.h"

#include <memory>

namespace CommonTool
{
namespace MsgTool
{

bool isLittleEndian()
{
	union Endian
	{
		int num;
		char c[4];
	};
	Endian e;
	e.num = 1;

	return (e.c[0] == '\x01');
}

bool isBytesDataEQ(const byte* data1, const byte* data2, uint len)
{
	if(len <= 0 || !data1 || !data2)
	{
		return false;
	}
	for(int i = 0; i < len; ++i)
	{
		if(data1[i] != data2[i])
		{
			return false;
		}
	}

	return true;
}

const std::string getMsgHeaderFlagString(int flag)
{
	std::string retString;
	switch (flag)
	{
	case MsgHeader::F_DEFAULT:
		retString = "Default";
		break;
	case MsgHeader::F_PROXYSERVER:
		retString = "ProxyServer";
		break;
	case MsgHeader::F_GATESERVER:
		retString = "GateServer";
		break;
	case MsgHeader::F_LOGICSERVER:
		retString = "LogicServer";
		break;
	case MsgHeader::F_CACHESERVER:
		retString = "CacheServer";
		break;
	default:
		retString = "NULL";
		break;
	}
	return retString;
}

}}

