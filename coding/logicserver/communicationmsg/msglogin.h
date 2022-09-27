#ifndef __MSG_LOGIN_H__
#define __MSG_LOGIN_H__
#include <boost/static_assert.hpp>

namespace
{
const int g_nUserNameMaxSize = 12;
const int g_nUserPasswordMaxSize = 32;
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
		memset(m_strUserName, 0, g_nUserNameMaxSize);
		memset(m_strPassword, 0, g_nUserPasswordMaxSize);
		m_cLoginFlag = LF_DEFAULT;
		memset(m_nReserve, 0, sizeof(m_nReserve));
	}
	char m_strUserName[g_nUserNameMaxSize];
	char m_strPassword[g_nUserPasswordMaxSize];
	char m_cLoginFlag;
	char m_nReserve[3];
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
		ER_UNREGISTERED,
	};

	MsgLoginSC()
	{
		m_cLoginStatus = LS_DEFAULT;
		m_cErrorReason = ER_DEFAULT;
		memset(m_nReserve, 0, sizeof(m_nReserve));
	}

	char m_cLoginStatus;
	char m_cErrorReason;
	char m_nReserve[2];
};
BOOST_STATIC_ASSERT(sizeof(MsgLoginSC) == 4);

// save user info
struct RoleInfoParam
{
	RoleInfoParam()
	{
		memset(m_strUserName, 0, g_nUserNameMaxSize);
		memset(m_strPassword, 0, g_nUserPasswordMaxSize);
	}
	char m_strUserName[g_nUserNameMaxSize];
	char m_strPassword[g_nUserPasswordMaxSize];
};
namespace boost{namespace serialization
{
template<class Archive>
void serialize(Archive& ar, RoleInfoParam& p, const unsigned int version)
{
	ar& p.m_strUserName;
	ar& p.m_strPassword;
}
}}

#pragma pack(pop)

#endif