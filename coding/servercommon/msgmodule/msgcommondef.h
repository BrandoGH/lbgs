#ifndef __MSG_COMMON_DEF_H__
#define __MSG_COMMON_DEF_H__
#include "basedef.h"
#include <boost/static_assert.hpp>

#pragma pack(push,4)

namespace MsgBuffer
{
	const int g_nReadBufferSize = 1024 * 50;	// Maximum buffer per read
	const int g_nOnceMsgSize = 1024;			// A protocol maximum byte
}

struct MsgHeader 
{
	// Server Type Flag
	enum Flag
	{
		F_DEFAULT				= -1,
		F_PROXYSERVER			= 0,		// Proxy
		F_GATESERVER			= 1,		// Gate
		F_LOGICSERVER			= 2,		// Logic
		F_CACHESERVER			= 3,		// Cache
		F_DBSERVER				= 4,		// database

		F_MAX,
	};

	MsgHeader()
	{
		reset();
	}

	void reset()
	{
		m_nMsgLen = 0;
		m_nMsgType = 0;
		m_nSender = F_DEFAULT;
		m_nReceiver = F_DEFAULT;
		m_nProxyer = F_DEFAULT;
		m_nClientSrcSeq = 0;
		memset(m_nReserve, 0, sizeof(m_nReserve));
	}

	ushort m_nMsgLen;
	ushort m_nMsgType;
	ushort m_nSender;		
	ushort m_nReceiver;		
	byte m_nProxyer;		
	byte m_nReserve[3];		
	/*
	The server is used for internal communication. It stores the serial number that sends the information. 
	According to the serial number, the gateway decides to whom it is sent.
	*/
	int m_nClientSrcSeq;
};
BOOST_STATIC_ASSERT(sizeof(MsgHeader) == 16);

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

/*
	0-128		Inner server communication type
	150-65535	with client communication type
*/
enum EnMsgType
{
	// gateserver - proxyserver heart
	MSG_TYPE_GATE_PROXY_HEART_GP			= 0,
	MSG_TYPE_GATE_PROXY_HEART_PG			= 1,
	// logicserver - proxyserver heart
	MSG_TYPE_LOGIC_PROXY_HEART_LP			= 2,
	MSG_TYPE_LOGIC_PROXY_HEART_PL			= 3,
	// cacheserver - proxyserver heart
	MSG_TYPE_CACHE_PROXY_HEART_CP			= 4,
	MSG_TYPE_CACHE_PROXY_HEART_PC			= 5,
	// dbserver - proxyserver heart
	MSG_TYPE_DB_PROXY_HEART_DP				= 6,
	MSG_TYPE_DB_PROXY_HEART_PD				= 7,

	MSG_IN_TYPE_MAX,

	// client heart
	MSG_TYPE_CLIENT_START	= 150,
	MSG_TYPE_HEART_CS		= MSG_TYPE_CLIENT_START,
	MSG_TYPE_HEART_SC		= 151,
	MSG_TYPE_LOGIN_REGISTER_CS		= 152,
	MSG_TYPE_LOGIN_REGISTER_SC		= 153,

	MSG_CODE_MAX,
	MSG_TYPE_CLIENT_SIZE = MSG_CODE_MAX - MSG_TYPE_CLIENT_START,
};

#pragma pack(pop)
#endif // !__MSG_COMMON_DEF_H__
