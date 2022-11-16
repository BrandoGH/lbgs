#include "role.h"
#include "src/globallogicserver.h"
#include "src/logicserver.h"
#include "communicationmsg/msglogout.h"

#include <msgmodule/msgcommondef.h>
#include <logmodule/logdef.h>
#include "communicationmsg/msgroleinfoupdate.h"

Role::Role()
	: m_nLogoutErrorCode(0)
	, m_nClientSeq(0)
	, m_velocity(0)
{
	m_vecLocation.setZero();
}

Role::~Role()
{
}

void Role::login()
{
	LOG_ROLE.printLog("role [%s] has login!!", getRoleName().data());
	// TODO login do something
}

void Role::logout()
{
	LOG_ROLE.printLog("role [%s]  has logout!!",getRoleName().data());
	sendDeleteLoginCacheInfo();
}

void Role::setClientSeq(ullong seq)
{
	m_nClientSeq = seq;
}

ullong Role::getClientSeq()
{
	return m_nClientSeq;
}

void Role::setLoginParam(const RoleLoginInfoParam& param)
{
	m_paramLogin = param;
}

std::string Role::getRoleId()
{
	return std::string(m_paramLogin.m_strRoleId);
}

std::string Role::getRoleName()
{
	return std::string(m_paramLogin.m_strRoleName);
}

void Role::setLogoutErrorCode(int ec)
{
	m_nLogoutErrorCode = ec;
}

int Role::getLogoutErrorCode()
{
	return m_nLogoutErrorCode;
}

void Role::updateInfoWhenRoleOperation(MsgRoleInfoUpdateCS* csData)
{
	if (!csData)
	{
		return;
	}

	m_vecLocation <<
		csData->m_roleX.m_double,
		csData->m_roleY.m_double, 
		csData->m_roleZ.m_double;

	m_velocity = csData->m_velocity;
	m_bJumping = (csData->m_jumpFlag == MsgRoleInfoUpdateCS::EJF_JUMPING) ? true : false;

	/*printf_color(PRINTF_YELLOW, "[%s]: move X[%lf] Y[%lf] Z[%lf] m_velocity[%d] m_jumpFlag[%d]\n", 
		getRoleName().data(), csData->m_roleX.m_double, csData->m_roleY.m_double, csData->m_roleZ.m_double, csData->m_velocity, int(csData->m_jumpFlag));*/
}

const Eigen::Vector3d& Role::getCurrentLocation()
{
	return m_vecLocation;
}

void Role::sendDeleteLoginCacheInfo()
{
	DEFINE_BYTE_ARRAY(sendData, sizeof(MsgHeader) + sizeof(MsgLogoutCS));

	MsgLogoutCS cs;
	memmove(cs.m_roleId, getRoleId().data(), sizeof(cs.m_roleId));
	cs.m_nErrorCode = getLogoutErrorCode();

	MsgHeader header;
	header.m_nMsgLen = sizeof(MsgHeader) + sizeof(MsgLogoutCS);
	header.m_nMsgType = MSG_TYPE_LOGOUT_CS;

	memmove(sendData, (const char*)&header, sizeof(MsgHeader));
	memmove(sendData + sizeof(MsgHeader), (const char*)&cs, sizeof(MsgLogoutCS));

	if (GLOBAL_LOGIC->getLogicServer())
	{
		GLOBAL_LOGIC->getLogicServer()->sendToCache(sendData, sizeof(sendData));
	}
}
