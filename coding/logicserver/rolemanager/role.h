#ifndef __ROLE_H__
#define __ROLE_H__
#include "communicationmsg/msglogin.h"
#include "communicationmsg/msgcreaterole.h"

#include <servercommon/boostmodule/basedef.h>
#include <servercommon/basedef.h>

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

private:
	void sendDeleteLoginCacheInfo();

private:
	ullong m_nClientSeq;
	RoleLoginInfoParam m_paramLogin;

	int m_nLogoutErrorCode;
};

#endif // !__ROLE_H__
