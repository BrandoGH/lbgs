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
		m_msgSeq = 0;
		m_msgFlag = 0;
		memset(m_msgLen, 0, sizeof(m_msgLen));
		memset(m_msgType, 0, sizeof(m_msgType));
		memset(m_msgSender, 0, sizeof(m_msgSender));
		memset(m_msgReceive, 0, sizeof(m_msgReceive));
		memset(m_msgForward, 0, sizeof(m_msgForward));
		memset(m_reserve, 0, sizeof(m_reserve));
	}

	byte m_msgLen[2];
	byte m_msgType[2];
	byte m_msgSender[4];
	byte m_msgReceive[4];
	byte m_msgForward[4];
	byte m_msgSeq;			// 包序列号
	byte m_msgFlag;			// 0-请求 1-应答 2-后续还有包 3-包结束
	byte m_reserve[2];
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
		memset(m_md5, 0, sizeof(m_md5));
	}

	byte m_md5[16];
};
BOOST_STATIC_ASSERT(sizeof(MsgEnder) == 16);

#pragma pack(pop)
#endif // !__MSG_COMMON_DEF_H__
