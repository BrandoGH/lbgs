#ifndef __MSG_LOGIN_H__
#define __MSG_LOGIN_H__

#include <boost/static_assert.hpp>
#include <servercommon/basedef.h>

namespace
{

}

#pragma pack(push,4)
// 152-role login seq
struct MsgLoginCS	
{
	MsgLoginCS()
	{
		memset(m_strRoleName, 0, sizeof(CString32));
		memset(m_strPassword, 0, sizeof(CString64));
	}
	CString32 m_strRoleName;
	CString64 m_strPassword;
};
BOOST_STATIC_ASSERT(sizeof(MsgLoginCS) == 96);

// 153-role login rep
struct MsgLoginSC
{
	enum EnLoginStatus
	{
		LS_DEFAULT = -1,

		LS_LOGIN_OK,
		LS_LOGIN_ERROR,
	};

	enum EnErrorReason
	{
		ER_DEFAULT = -1,

		ER_NO_ERROR,
		ER_PASSWORD_ERROR,
		ER_HAS_LOGIN_ERROR,
	};

	MsgLoginSC()
	{
		memset(m_strRoleName, 0, sizeof(CString32));
		memset(m_strPassword, 0, sizeof(CString64));
		m_cLoginStatus = LS_DEFAULT;
		m_cErrorReason = ER_DEFAULT;
		memset(m_nReserve, 0, sizeof(m_nReserve));
	}

	CString32 m_strRoleName;
	CString64 m_strPassword;
	char m_cLoginStatus;
	char m_cErrorReason;
	char m_nReserve[2];
};
BOOST_STATIC_ASSERT(sizeof(MsgLoginSC) == 100);

// save user info
struct RoleLoginInfoParam
{
	RoleLoginInfoParam()
	{
		memset(m_strRoleId, 0, sizeof(CString32));
		memset(m_strRoleName, 0, sizeof(CString32));
		memset(m_strPassword, 0, sizeof(CString64));
	}
	CString64 m_strRoleId;
	CString32 m_strRoleName;
	CString64 m_strPassword;
};
typedef byte RoleLoginInfoParamHex[sizeof(RoleLoginInfoParam)];

#pragma pack(pop)

#endif