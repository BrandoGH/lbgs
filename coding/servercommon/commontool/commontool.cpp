#include "commontool.h"
#include "servercommon/boostmodule/basedef.h"

#include <boost/algorithm/string.hpp>

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

bool isIpFormat(const std::string& ip)
{
	std::vector<std::string> vec;
	split(vec, ip, boost::algorithm::is_any_of("."));

	for(std::string str : vec)
	{
		ushort ipSeg = -1;
		try
		{
			ipSeg = CAST_TO(ushort, str);
		}
		catch(...)
		{
			return false;
		}
		if(ipSeg < 0 || ipSeg > 0xFF)
		{
			return false;
		}
	}

	return true;
}

int getRandom(int left, int right)
{
	srand(time(NULL));
	return (left +(rand() % (right - left + 1)));
}

std::string genRoleIdByUserName(const std::string& userName)
{
	boost::uuids::string_generator sGen;
	boost::uuids::uuid idFm = sGen("{0123456789abcdef0123456789abcdef}");
	boost::uuids::name_generator ng(idFm);
	return boost::uuids::to_string(ng(userName));
}

}