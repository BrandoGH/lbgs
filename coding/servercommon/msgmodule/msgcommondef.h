#ifndef __MSG_COMMON_DEF_H__
#define __MSG_COMMON_DEF_H__
#include "basedef.h"
#include <boost/static_assert.hpp>

#pragma pack(push,4)

namespace UserBuffer
{
	const int g_nReadBufferSize = 1024 * 50;	// 每次读取最大buffer	
	const int g_nOnceMsgSize = 1024;			// 一条协议最大字节
}

struct MsgHeader 
{
	MsgHeader()
	{
		reset();
	}

	void reset()
	{
		m_nMsgLen = 0;
		m_nMsgType = 0;
	}

	ushort m_nMsgLen;
	ushort m_nMsgType;
};
BOOST_STATIC_ASSERT(sizeof(MsgHeader) == 4);

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

enum EnMsgType
{
	// 心跳包
	MSG_TYPE_HEART_CS		= 0,
	MSG_TYPE_HEART_SC		= 1,

	MSG_CODE_MAX,
};

#pragma pack(pop)
#endif // !__MSG_COMMON_DEF_H__
