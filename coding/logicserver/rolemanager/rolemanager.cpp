#include "rolemanager.h"
#include "src/globallogicserver.h"
#include "src/logicserver.h"
#include "role.h"
#include "communicationmsg/msgcreaterole.h"
#include "communicationmsg/msgroleinfoupdate.h"

#include <logmodule/logdef.h>
#include <msgmodule/msgcommondef.h>
#include <eigen3/Eigen/Core>


RoleManager::RoleManager()
{
}

RoleManager::~RoleManager()
{
	deleteInstance();
}

RoleManager* RoleManager::instanceObj = new(std::nothrow) RoleManager;
RoleManager* RoleManager::instance()
{
	return instanceObj;
}

void RoleManager::createRole(const CreateRoleInput& input)
{
	boost::shared_ptr<Role> newRole = boost::make_shared<Role>();
	newRole->setClientSeq(input.m_nClientSeq);
	newRole->setLoginParam(input.m_param);

	// insert to map
	CommonBoost::UniqueLock lock(m_mtxMap);
	m_mapIdToRole[input.m_param.m_strRoleId] = newRole;
	lock.unlock();

	newRole->login();

}

void RoleManager::removeRole(const std::string& roleId, int errCode)
{
	if (m_mapIdToRole.empty())
	{
		LOG_ROLE.printLog("m_mapIdToRole empty, roleId[%s]", roleId.data());
		return;
	}

	typedef std::map<std::string, boost::shared_ptr<Role>>::const_iterator MapRoleCIT;
	CommonBoost::UniqueLock lock(m_mtxMap);
	
	// delete from m_mapIdToRole
	MapRoleCIT cit = m_mapIdToRole.find(roleId);
	if (cit == m_mapIdToRole.cend())
	{
		LOG_ROLE.printLog("RoleManager is not have roleId[%s]", roleId.data());
		return;
	}

	boost::shared_ptr<Role> onceRole = m_mapIdToRole[roleId];

	m_mapIdToRole.erase(cit);
	LOG_ROLE.printLog("roleId[%s] has remove from RoleManager", roleId.data());

	if (onceRole)
	{
		onceRole->setLogoutErrorCode(errCode);
		onceRole->logout();
	}

}

void RoleManager::tryRemoveRole(ullong roleSeq, int errCode)
{
	if (m_mapIdToRole.empty())
	{
		LOG_ROLE.printLog("m_mapIdToRole empty, roleSeq[%lld] is not exists", roleSeq);
		return;
	}

	typedef std::map<std::string, boost::shared_ptr<Role>>::const_iterator MapRoleCIT;

	CommonBoost::UniqueLock lock(m_mtxMap);

	// delete from m_mapIdToRole
	boost::shared_ptr<Role> onceRole;
	ullong seq = 0;
	for (MapRoleCIT cit = m_mapIdToRole.cbegin(); cit != m_mapIdToRole.cend(); ++cit)
	{
		onceRole = cit->second;

		if (!onceRole)
		{
			continue;
		}

		seq = onceRole->getClientSeq();
		if (seq != roleSeq)
		{
			continue;
		}

		onceRole->setLogoutErrorCode(errCode);
		onceRole->logout();
		m_mapIdToRole.erase(cit);
		break;
	}

	LOG_ROLE.printLog("has remove roleSeq[%lld]", roleSeq);
}

bool RoleManager::isRoleExists(const std::string& roleId)
{
	std::map<std::string, boost::shared_ptr<Role>>::const_iterator cit =
		m_mapIdToRole.find(roleId);

	return (cit != m_mapIdToRole.end());
}

bool RoleManager::tryToDetermineIfExists(ullong roleSeq)
{
	std::map<std::string, boost::shared_ptr<Role>>::const_iterator cit =
		m_mapIdToRole.cbegin();

	for (; cit != m_mapIdToRole.cend(); ++cit)
	{
		if (!cit->second)
		{
			continue;
		}

		if (cit->second->getClientSeq() == roleSeq)
		{
			return true;
		}
	}

	return false;
}

boost::shared_ptr<Role> RoleManager::findRoleByClientSeq(ullong clientSeq)
{
	std::map<std::string, boost::shared_ptr<Role>>::const_iterator cit =
		m_mapIdToRole.cbegin();

	for (; cit != m_mapIdToRole.cend(); ++cit)
	{
		if (!cit->second)
		{
			continue;
		}

		if (cit->second->getClientSeq() == clientSeq)
		{
			return cit->second;
		}
	}

	return NULL;
}

void RoleManager::createRoleModel(boost::shared_ptr<Role> myself)
{
	if (!myself)
	{
		return;
	}

	// 1. tell all role, i will create model
	DEFINE_BYTE_ARRAY(sendData, sizeof(MsgHeader) + sizeof(MsgCreateRoleSC));
	MsgHeader* header = (MsgHeader*)sendData;
	if (!header)
	{
		return;
	}
	header->m_nMsgLen = sizeof(sendData);
	header->m_nMsgType = MSG_TYPE_CREATE_ROLE_SC;

	MsgCreateRoleSC sc;
	memmove(sc.m_strCreateRoleName, myself->getRoleName().data(), myself->getRoleName().size());

	boost::shared_ptr<Role> targetRole;
	std::map<std::string, boost::shared_ptr<Role>>::const_iterator cit = m_mapIdToRole.cbegin();
	for (;cit != m_mapIdToRole.cend();++cit)
	{
		targetRole = cit->second;
		if (!targetRole)
		{
			continue;
		}

		MsgHeader* header = (MsgHeader*)sendData;
		if (!header)
		{
			continue;
		}
		header->m_nClientSrcSeq = targetRole->getClientSeq();

		memmove(sendData, (const char*)header, sizeof(MsgHeader));
		memmove(sendData + sizeof(MsgHeader), (const char*)&sc, sizeof(MsgCreateRoleSC));

		if (GLOBAL_LOGIC->getLogicServer())
		{
			// printf_color(PRINTF_YELLOW, "tell [%s][%lld]  will create [%s][%lld] \n", targetRole->getRoleName().data(), targetRole->getClientSeq(), myself->getRoleName().data(), myself->getClientSeq());
			GLOBAL_LOGIC->getLogicServer()->sendToClient(sendData, sizeof(sendData));
		}
	}
	
	// 2. create other role on my client
	header->m_nClientSrcSeq = myself->getClientSeq();
	std::map<std::string, boost::shared_ptr<Role>>::const_iterator cit2 = m_mapIdToRole.cbegin();
	for (; cit2 != m_mapIdToRole.cend(); ++cit2)
	{
		targetRole = cit2->second;
		if (!targetRole || targetRole->getRoleId() == myself->getRoleId())
		{
			continue;
		}

		sc.reset();
		memmove(sc.m_strCreateRoleName, targetRole->getRoleName().data(), targetRole->getRoleName().size());

		memmove(sendData, (const char*)header, sizeof(MsgHeader));
		memmove(sendData + sizeof(MsgHeader), (const char*)&sc, sizeof(MsgCreateRoleSC));

		if (GLOBAL_LOGIC->getLogicServer())
		{
			// printf_color(PRINTF_GREEN, "tell [%s][%lld] will create [%s][%lld]\n",  myself->getRoleName().data(), header->m_nClientSrcSeq, targetRole->getRoleName().data(), targetRole->getClientSeq());
			GLOBAL_LOGIC->getLogicServer()->sendToClient(sendData, sizeof(sendData));
		}
	}

}

void RoleManager::notifyRoleInfoChange(boost::shared_ptr<Role> notifyRole)
{
	if (!notifyRole)
	{
		return;
	}

	DEFINE_BYTE_ARRAY(sendData, sizeof(MsgHeader) + sizeof(MsgRoleInfoUpdateSC));
	MsgHeader* header = (MsgHeader*)sendData;
	if (!header)
	{
		return;
	}
	header->m_nMsgLen = sizeof(sendData);
	header->m_nMsgType = MSG_TYPE_ROLE_INFO_UPDATE_SC;

	MsgRoleInfoUpdateSC sc;
	memmove(sc.m_targetRoleName, notifyRole->getRoleName().data(), sizeof(sc.m_targetRoleName));
	sc.m_roleX = notifyRole->getCurrentLocation().x();
	sc.m_roleY = notifyRole->getCurrentLocation().y();
	sc.m_roleZ = notifyRole->getCurrentLocation().z();
	
	boost::shared_ptr<Role> targetRole;
	std::map<std::string, boost::shared_ptr<Role>>::const_iterator cit = m_mapIdToRole.cbegin();
	for (; cit != m_mapIdToRole.cend(); ++cit)
	{
		if (!cit->second ||
			cit->second->getClientSeq() == notifyRole->getClientSeq())
		{
			continue;
		}

		header->m_nClientSrcSeq = cit->second->getClientSeq();
		memmove(sendData, (const char*)header, sizeof(MsgHeader));
		memmove(sendData + sizeof(MsgHeader), (const char*)&sc, sizeof(MsgRoleInfoUpdateSC));

		if (GLOBAL_LOGIC->getLogicServer())
		{
			GLOBAL_LOGIC->getLogicServer()->sendToClient(sendData, sizeof(sendData));
		}
	}
}

void RoleManager::deleteInstance()
{
	if (instanceObj)
	{
		delete instanceObj;
		instanceObj = NULL;
	}
}
