#ifndef __MSG_CREATE_ROLE_H__
#define __MSG_CREATE_ROLE_H__
#include <boost/static_assert.hpp>

#pragma pack(push,4)
// msg type: 155
struct MsgCreateRoleCS	
{
	MsgCreateRoleCS()
	{
		memset(m_nReserve, 0, sizeof(m_nReserve));
	}
	char m_nReserve[4];
};
BOOST_STATIC_ASSERT(sizeof(MsgCreateRoleCS) == 4);

// msg type: 156
struct MsgCreateRoleSC
{
	MsgCreateRoleSC()
	{
		reset();
	}

	void reset()
	{
		memset(m_strCreateRoleName, 0, sizeof(m_strCreateRoleName));
	}

	CString32 m_strCreateRoleName;

};
BOOST_STATIC_ASSERT(sizeof(MsgCreateRoleSC) == 32);

#pragma pack(pop)

#endif
