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

}}

