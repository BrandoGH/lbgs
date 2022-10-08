#include "role.h"

Role::Role()
{
}

Role::~Role()
{
}

void Role::login()
{
}

void Role::setClientSeq(int seq)
{
	m_nClientSeq = seq;
}

int Role::getClientSeq()
{
	return m_nClientSeq;
}

void Role::setLoginParam(const RoleLoginInfoParam& param)
{
	m_paramLogin = param;
}

const std::string& Role::getRoleId()
{
	return std::string(m_paramLogin.m_strRoleId);
}

const std::string& Role::getRoleName()
{
	return std::string(m_paramLogin.m_strRoleName);
}
