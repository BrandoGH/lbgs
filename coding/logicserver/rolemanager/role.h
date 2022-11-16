#ifndef __ROLE_H__
#define __ROLE_H__
#include "communicationmsg/msglogin.h"
#include "communicationmsg/msgcreaterole.h"

#include <servercommon/boostmodule/basedef.h>
#include <servercommon/basedef.h>
#include <eigen3/Eigen/Core>

struct MsgRoleInfoUpdateCS;
class Role : public boost::enable_shared_from_this<Role>
{
public:
	Role();
	~Role();

	void login();
	void logout();

	void setClientSeq(ullong seq);
	ullong getClientSeq();
	void setLoginParam(const RoleLoginInfoParam& param);
	std::string getRoleId();
	std::string getRoleName();
	void setLogoutErrorCode(int ec);
	int getLogoutErrorCode();

	void updateInfoWhenRoleOperation(MsgRoleInfoUpdateCS* csData);

	const Eigen::Vector3d& getCurrentLocation();
	ushort getMoveVelocity() { return m_velocity; }
	bool isJumping() { return m_bJumping; }

private:
	void sendDeleteLoginCacheInfo();

private:
	ullong m_nClientSeq;
	RoleLoginInfoParam m_paramLogin;

	int m_nLogoutErrorCode;
	Eigen::Vector3d m_vecLocation;
	ushort m_velocity;
	bool m_bJumping;
};

#endif // !__ROLE_H__
