#include "serializememory.hpp"
#include "boostmodule/basedef.h"
#include "../commontool.h"

namespace SerialzeMem
{
std::string dataToHexStr(const byte* data, uint dataSize)
{
	std::string retHex;
	for (int i = 0; i < dataSize; ++i)
	{
		char tmp[3];
		sprintf(tmp, "%02x", data[i]);
		retHex.append(tmp);
	}
	transform(retHex.begin(), retHex.end(), retHex.begin(), toupper);
	return retHex;
}

bool hexStrToData(const std::string& hexString, byte* retData, uint& retSize)
{
	std::string tmpStr;
	int strHalfSize = hexString.size() / 2;
	int hexBigNum = 0;
	int hexLittleNum = 0;
	int inDataIdx = 0;
	for (int i = 0; i < hexString.size(); i += 2)
	{
		tmpStr = hexString.substr(i, 2);
		transform(tmpStr.begin(), tmpStr.end(), tmpStr.begin(), toupper);

		if (!CommonTool::getNumByHexSymbol(tmpStr.at(0), hexBigNum) ||
			!CommonTool::getNumByHexSymbol(tmpStr.at(1), hexLittleNum))
		{
			continue;
		}
		byte hex = hexBigNum * 16 + hexLittleNum;
		retData[inDataIdx] = hex;
		++inDataIdx;
		++retSize;
	}

	if (retSize != strHalfSize)
	{
		return false;
	}

	return true;
}

}