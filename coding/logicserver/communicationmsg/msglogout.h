#ifndef __MSG_LOGOUT_H__
#define __MSG_LOGOUT_H__

#include <boost/static_assert.hpp>
#include <servercommon/basedef.h>

#pragma pack(push,4)

// 154- logout req
struct MsgLogoutCS
{
	enum EnErrorCode
	{
		EN_SERVER_CHECK_ROLE_INVALID,
	};
	MsgLogoutCS()
	{
		m_nErrorCode = 0;
		memset(m_roleId, 0, sizeof(CString64));
	}

	// Under normal circumstances, the socket returns. In special cases, it is judged by the server, such as heartbeat stop.
	int m_nErrorCode;
	CString64 m_roleId;
};
BOOST_STATIC_ASSERT(sizeof(MsgLogoutCS) == 68);

// 159 MSG_TYPE_ROLE_MODEL_REMOVE
struct MsgLogoutSC
{
	MsgLogoutSC()
	{
		memset(m_strRoleName, 0, sizeof(m_strRoleName));
	}

	CString32 m_strRoleName;
};
BOOST_STATIC_ASSERT(sizeof(MsgLogoutSC) == 32);

#pragma pack(pop)
#endif // !__MSG_LOGOUT_H__
