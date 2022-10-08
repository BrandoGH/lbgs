#ifndef __MSG_LOGOUT_H__
#define __MSG_LOGOUT_H__

#include <boost/static_assert.hpp>
#include <servercommon/basedef.h>

#pragma pack(push,4)

// 154- logout req
struct MsgLogoutCS
{
	MsgLogoutCS()
	{
		m_nErrorCode = 0;
		memset(m_roleId, 0, sizeof(CString64));
	}
	int m_nErrorCode;
	CString64 m_roleId;
};
BOOST_STATIC_ASSERT(sizeof(MsgLogoutCS) == 68);

#pragma pack(pop)
#endif // !__MSG_LOGOUT_H__
