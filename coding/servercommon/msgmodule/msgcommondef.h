#ifndef __MSG_COMMON_DEF_H__
#define __MSG_COMMON_DEF_H__
#include "basedef.h"
#include <boost/static_assert.hpp>

#pragma pack(push,4)

struct MsgHeader 
{
	enum EnPackageFlag
	{
		FLAG_INVALID = -1,
		FLAG_REQ,
		FLAG_REP,
		FLAG_MORE_PKG,
		FLAG_END,

		FLAG_MAX,
	};

	MsgHeader()
	{
		reset();
	}

	void reset()
	{
		m_bytesSeq = 0;
		m_bytesFlag = 0;
		memset(m_bytesLen, 0, sizeof(m_bytesLen));
		memset(m_bytesType, 0, sizeof(m_bytesType));
		memset(m_bytesSender, 0, sizeof(m_bytesSender));
		memset(m_bytesReceive, 0, sizeof(m_bytesReceive));
		memset(m_bytesForward, 0, sizeof(m_bytesForward));
		memset(m_bytesReserve, 0, sizeof(m_bytesReserve));
	}

	byte m_bytesLen[2];
	byte m_bytesType[2];
	byte m_bytesSender[4];
	byte m_bytesReceive[4];
	byte m_bytesForward[4];
	byte m_bytesSeq;			// 包序列号
	byte m_bytesFlag;			// 0-请求 1-应答 2-后续还有包 3-包结束
	byte m_bytesReserve[2];
};
BOOST_STATIC_ASSERT(sizeof(MsgHeader) == 20);

struct MsgEnder
{
	MsgEnder()
	{
		reset();
	}

	void reset()
	{
		memset(m_bytesMD5, 0, sizeof(m_bytesMD5));
	}

	byte m_bytesMD5[16];
};
BOOST_STATIC_ASSERT(sizeof(MsgEnder) == 16);

#pragma pack(pop)
#endif // !__MSG_COMMON_DEF_H__
