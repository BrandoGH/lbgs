#ifndef __SERIALIZE_MEMORY_H__
#define __SERIALIZE_MEMORY_H__

#include "basedef.h"

namespace SerialzeMem
{
std::string dataToHexStr(const byte* data, uint dataSize);
bool hexStrToData(const std::string& hexString, byte* retData, uint& retSize);


template<class SeriStruct>
std::string serializationToHexString(SeriStruct* inSt)
{
	std::string retString;
	if (!inSt)
	{
		return retString;
	}
	byte structHex[sizeof(SeriStruct)];
	memset(structHex, 0, sizeof(structHex));
	memmove(structHex, (const byte*)inSt, sizeof(structHex));
	retString = dataToHexStr(structHex, sizeof(SeriStruct));
	return retString;
}

template<class SeriStruct>
bool unSerializationFromHexString(const std::string& inHexString, SeriStruct& outSt)
{
	byte structHex[sizeof(SeriStruct)];
	memset(structHex, 0, sizeof(structHex));

	uint retSize = 0;
	bool retBool = hexStrToData(inHexString, structHex, retSize);
	outSt = *(SeriStruct*)(structHex);
	return retBool;
}

// dataSize : Two more bytes should be defined to store '\x'
template<class SeriStruct, class OutByteDataType>
bool serializationToMemData(SeriStruct* inSt, OutByteDataType& outData, int dataSize)
{
	if (!inSt || !outData)
	{
		return false;
	}

	memset(outData, 0, dataSize);
	memmove(outData, (const char*)inSt, dataSize);

	return true;
}

template<class SeriStruct, class InByteDataType>
void unSerializationFromMemData(SeriStruct& outStruct, InByteDataType& inData)
{
	outStruct = *((SeriStruct*)(inData));
}

}

#endif // !__SERIALIZE_MEMORY_H__
