#include "msgtool.h"

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

}}

