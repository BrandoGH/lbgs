#ifndef __GATESERVER_MSG_H__
#define __GATESERVER_MSG_H__

#include <servercommon/basedef.h>
#include <boost/static_assert.hpp>

#pragma pack(push,4)

struct MsgHeartCS	// 0-心跳包请求
{
	MsgHeartCS()
	{
		memset(m_bytesHeart, 0, sizeof(m_bytesHeart));
	}
	byte m_bytesHeart[4];		// 固定 LBGS = 0x4C 0x42 0x47 0x53
};
BOOST_STATIC_ASSERT(sizeof(MsgHeartCS) == 4);

struct MsgHeartSC	// 1-心跳包回应
{
	MsgHeartSC()
	{
		memset(m_bytesHeart, 0, sizeof(m_bytesHeart));
	}
	byte m_bytesHeart[4];		// 固定 SGBL = 0x53 0x47 0x42 0x4C
};
BOOST_STATIC_ASSERT(sizeof(MsgHeartSC) == 4);

#pragma pack(pop)
#endif // !__GATESERVER_MSG_H__
