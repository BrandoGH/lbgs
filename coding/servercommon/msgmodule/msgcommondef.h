#ifndef __MSG_COMMON_DEF_H__
#define __MSG_COMMON_DEF_H__
#include "basedef.h"
#include <boost/static_assert.hpp>

#pragma pack(push,4)

namespace MsgBuffer
{
	const int g_nReadBufferSize = 1024 * 50;	// 每次读取最大buffer	
	const int g_nOnceMsgSize = 1024;			// 一条协议最大字节
}

struct MsgHeader 
{
	enum Flag
	{
		F_DEFAULT = -1,
		F_PROXYSERVER,		// 代理服
		F_GATESERVER,		// 网关服
		F_LOGICSERVER,		// 逻辑服

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
		m_nReserve = 0;
	}

	ushort m_nMsgLen;
	ushort m_nMsgType;
	ushort m_nSender;		// 发送方
	ushort m_nReceiver;		// 接收方
	byte m_nProxyer;		// 转发方 固定代理服
	byte m_nReserve;		// 保留
};
BOOST_STATIC_ASSERT(sizeof(MsgHeader) == 10);

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
	0-128		内部服务器通信类型
	150-65535	客户端通信类型
*/
enum EnMsgType
{
	// 网关-转发 心跳
	MSG_TYPE_GATE_PROXY_HEART_GP = 0,
	MSG_TYPE_GATE_PROXY_HEART_PG = 1,
	// 逻辑-转发 心跳
	MSG_TYPE_GATE_PROXY_HEART_LP = 2,
	MSG_TYPE_GATE_PROXY_HEART_PL = 3,

	MSG_IN_TYPE_MAX,

	// 客户端心跳包
	MSG_TYPE_CLIENT_START	= 150,
	MSG_TYPE_HEART_CS		= MSG_TYPE_CLIENT_START,
	MSG_TYPE_HEART_SC		= 151,
	MSG_CODE_MAX,
	MSG_TYPE_CLIENT_SIZE = MSG_CODE_MAX - MSG_TYPE_CLIENT_START,
};

#pragma pack(pop)
#endif // !__MSG_COMMON_DEF_H__
