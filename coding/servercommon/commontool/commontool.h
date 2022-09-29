#ifndef __COMMON_TOOL_H__
#define __COMMON_TOOL_H__

#include"servercommon/basedef.h"

namespace CommonTool
{

template<class ArrayType, int len>
int getArraySize(ArrayType(&arr)[len]) { return len; }

// hexSymbol = 0-F
bool getNumByHexSymbol(char hexSymbol, int& retNum);

bool isIpFormat(const std::string& ip);

// [left ,right]
int getRandom(int left,int right);

std::string genRoleIdByUserName(const std::string& userName);

}

#endif // !__COMMON_TOOL_H__
