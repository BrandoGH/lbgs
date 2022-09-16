#ifndef __INNER_SERVER_MSG_H__
#define __INNER_SERVER_MSG_H__

#include <servercommon/basedef.h>
#include <boost/static_assert.hpp>

#pragma pack(push,4)

// heart to proxy
struct MsgInHeartCS	// 0-to proxy heart req
{
	MsgInHeartCS()
	{
		memset(m_bytesHeart, 0, sizeof(m_bytesHeart));
	}
	byte m_bytesHeart[4];		// LBGS = 0x4C 0x42 0x47 0x53
};
BOOST_STATIC_ASSERT(sizeof(MsgInHeartCS) == 4);

struct MsgInHeartSC	// 1-proxy rep
{
	MsgInHeartSC()
	{
		memset(m_bytesHeart, 0, sizeof(m_bytesHeart));
	}
	byte m_bytesHeart[4];		// SGBL = 0x53 0x47 0x42 0x4C
};
BOOST_STATIC_ASSERT(sizeof(MsgInHeartSC) == 4);

#pragma pack(pop)
#endif // !__INNER_SERVER_MSG_H__
