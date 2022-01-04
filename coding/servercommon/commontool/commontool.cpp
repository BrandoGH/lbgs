#include "commontool.h"

namespace
{
	const char g_hexSymbol[17] = "0123456789ABCDEF";

}

namespace CommonTool
{

bool getNumByHexSymbol(char hexSymbol, int& retNum)
{
	for(int i = 0; i < 17; ++i)
	{
		if(g_hexSymbol[i] == hexSymbol)
		{
			retNum = i;
			return true;
		}
	}

	return false;
}

}