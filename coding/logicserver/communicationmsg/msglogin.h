#ifndef __MSG_LOGIN_H__
#define __MSG_LOGIN_H__

#include <boost/static_assert.hpp>
#include <servercommon/basedef.h>

namespace
{
// Role id fixed 36
const int g_nRoleIdMaxSize = 37;
const int g_nRoleNameMaxSize = 13;
const int g_nRoleLoginPasswordMaxSize = 32;
}

#pragma pack(push,4)
// 152-role login seq
struct MsgLoginCS	
{
	enum EnLoginFlag
	{
		LF_DEFAULT = -1,
		LF_LOGIN,
		LF_REGISTER,
	};

	MsgLoginCS()
	{
		memset(m_strRoleName, 0, g_nRoleNameMaxSize);
		memset(m_strPassword, 0, g_nRoleLoginPasswordMaxSize);
		m_cLoginFlag = LF_DEFAULT;
		memset(m_nReserve, 0, sizeof(m_nReserve));
	}
	char m_strRoleName[g_nRoleNameMaxSize];
	char m_strPassword[g_nRoleLoginPasswordMaxSize];
	char m_cLoginFlag;
	char m_nReserve[2];
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

		LS_REGISTER_OK,
		LS_REGISTER_ERROR,
	};

	enum EnErrorReason
	{
		ER_DEFAULT = -1,

		ER_NO_ERROR,
		ER_UNREGISTERED,
	};

	MsgLoginSC()
	{
		memset(m_strRoleName, 0, g_nRoleNameMaxSize);
		memset(m_strPassword, 0, g_nRoleLoginPasswordMaxSize);
		m_cLoginStatus = LS_DEFAULT;
		m_cErrorReason = ER_DEFAULT;
		m_nReserve = 0;
	}

	char m_strRoleName[g_nRoleNameMaxSize];
	char m_strPassword[g_nRoleLoginPasswordMaxSize];
	char m_cLoginStatus;
	char m_cErrorReason;
	char m_nReserve;
};
BOOST_STATIC_ASSERT(sizeof(MsgLoginSC) == 48);

// save user info
struct RoleInfoParam
{
	RoleInfoParam()
	{
		memset(m_strRoleId, 0, g_nRoleIdMaxSize);
		memset(m_strRoleName, 0, g_nRoleNameMaxSize);
		memset(m_strPassword, 0, g_nRoleLoginPasswordMaxSize);
	}
	char m_strRoleId[g_nRoleIdMaxSize];
	char m_strRoleName[g_nRoleNameMaxSize];
	char m_strPassword[g_nRoleLoginPasswordMaxSize];
};
typedef byte RoleInfoParamHex[sizeof(RoleInfoParam)];

#pragma pack(pop)

#endif