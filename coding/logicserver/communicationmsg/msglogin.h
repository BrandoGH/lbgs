#ifndef __MSG_LOGIN_H__
#define __MSG_LOGIN_H__

#include <boost/static_assert.hpp>
#include <servercommon/basedef.h>

namespace
{
// Role id fixed 36
const int g_nRoleIdMaxSize = 37;
const int g_nRoleNameMaxSize = 14;
const int g_nRoleLoginPasswordMaxSize = 34;
}

#pragma pack(push,4)
// 152-role login seq
struct MsgLoginCS	
{
	MsgLoginCS()
	{
		memset(m_strRoleName, 0, g_nRoleNameMaxSize);
		memset(m_strPassword, 0, g_nRoleLoginPasswordMaxSize);
	}
	char m_strRoleName[g_nRoleNameMaxSize];
	char m_strPassword[g_nRoleLoginPasswordMaxSize];
};
BOOST_STATIC_ASSERT(sizeof(MsgLoginCS) == 48);

// 153-role login req
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
		memset(m_strRoleName, 0, g_nRoleNameMaxSize);
		memset(m_strPassword, 0, g_nRoleLoginPasswordMaxSize);
		m_cLoginStatus = LS_DEFAULT;
		m_cErrorReason = ER_DEFAULT;
		memset(m_nReserve, 0, sizeof(m_nReserve));
	}

	char m_strRoleName[g_nRoleNameMaxSize];
	char m_strPassword[g_nRoleLoginPasswordMaxSize];
	char m_cLoginStatus;
	char m_cErrorReason;
	char m_nReserve[2];
};
BOOST_STATIC_ASSERT(sizeof(MsgLoginSC) == 52);

// save user info
struct RoleLoginInfoParam
{
	RoleLoginInfoParam()
	{
		memset(m_strRoleId, 0, g_nRoleIdMaxSize);
		memset(m_strRoleName, 0, g_nRoleNameMaxSize);
		memset(m_strPassword, 0, g_nRoleLoginPasswordMaxSize);
	}
	char m_strRoleId[g_nRoleIdMaxSize];
	char m_strRoleName[g_nRoleNameMaxSize];
	char m_strPassword[g_nRoleLoginPasswordMaxSize];
};
typedef byte RoleLoginInfoParamHex[sizeof(RoleLoginInfoParam)];

#pragma pack(pop)

#endif