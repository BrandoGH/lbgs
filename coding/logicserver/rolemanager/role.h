#ifndef __ROLE_H__
#define __ROLE_H__
#include "communicationmsg/msglogin.h"

#include <servercommon/boostmodule/basedef.h>
#include <servercommon/basedef.h>

class Role : public boost::enable_shared_from_this<Role>
{
public:
	Role();
	~Role();

	void login();

	void setClientSeq(int seq);
	int getClientSeq();
	void setLoginParam(const RoleLoginInfoParam& param);
	const std::string& getRoleId();
	const std::string& getRoleName();

private:
	int m_nClientSeq;
	RoleLoginInfoParam m_paramLogin;
};

#endif // !__ROLE_H__
