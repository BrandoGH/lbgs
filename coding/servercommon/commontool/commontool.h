#ifndef __COMMON_TOOL_H__
#define __COMMON_TOOL_H__

namespace CommonTool
{

template<class ArrayType, int len>
int getArraySize(ArrayType(&arr)[len]) { return len; }

}

#endif // !__COMMON_TOOL_H__
