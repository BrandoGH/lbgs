#ifndef __MSG_ROLE_INFO_UPDATE_H__
#define __MSG_ROLE_INFO_UPDATE_H__

#include <boost/static_assert.hpp>
#include <servercommon/basedef.h>

#pragma pack(push,4)

// 157 MSG_TYPE_ROLE_INFO_UPDATE_CS
struct MsgRoleInfoUpdateCS
{
	enum EnJumpFlag
	{
		EJF_NORMAL,
		EJF_JUMPING,
	};

	MsgRoleInfoUpdateCS()
	{
		m_roleX = 0.0;
		m_roleY = 0.0;
		m_roleZ = 0.0;
		m_jumpFlag = EJF_NORMAL;
		m_velocity = 0;
		m_reserve = 0;
	}

	MsgDouble m_roleX;
	MsgDouble m_roleY;
	MsgDouble m_roleZ;
	char m_jumpFlag;
	char m_reserve;
	ushort m_velocity; // max speed
};
BOOST_STATIC_ASSERT(sizeof(MsgRoleInfoUpdateCS) == 28);

// 158 MSG_TYPE_ROLE_INFO_UPDATE_SC
struct MsgRoleInfoUpdateSC
{
	MsgRoleInfoUpdateSC()
	{
		memset(m_targetRoleName, 0, sizeof(CString32));
		m_roleX = 0.0;
		m_roleY = 0.0;
		m_roleZ = 0.0;
		m_jumpFlag = MsgRoleInfoUpdateCS::EJF_NORMAL;
		m_velocity = 0;
		m_reserve = 0;
	}

	CString32 m_targetRoleName;	// myself
	MsgDouble m_roleX;
	MsgDouble m_roleY;
	MsgDouble m_roleZ;
	char m_jumpFlag;
	char m_reserve;
	ushort m_velocity; // max speed
};
BOOST_STATIC_ASSERT(sizeof(MsgRoleInfoUpdateSC) == 60);


#pragma pack(pop)
#endif // !__MSG_ROLE_INFO_UPDATE_H__
