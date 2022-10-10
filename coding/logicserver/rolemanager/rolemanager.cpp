#include "rolemanager.h"
#include "src/globallogicserver.h"
#include "src/logicserver.h"
#include "role.h"

#include <logmodule/logdef.h>
#include <msgmodule/msgcommondef.h>

RoleManager::RoleManager()
{
}

RoleManager::~RoleManager()
{
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
	std::map<std::string, boost::shared_ptr<Role>>::const_iterator cit = 
		m_mapIdToRole.find(input.m_param.m_strRoleId);
	if (cit == m_mapIdToRole.cend())
	{
		m_mapIdToRole[input.m_param.m_strRoleId] = newRole;
	}
	lock.unlock();

	newRole->login();

}

void RoleManager::removeRole(int roleSeq, int errCode)
{
	if (m_mapIdToRole.empty())
	{
		LOG_ROLE.printLog("m_mapIdToRole empty");
		return;
	}

	typedef std::map<std::string, boost::shared_ptr<Role>>::const_iterator MapRoleCIT;

	CommonBoost::UniqueLock lock(m_mtxMap);
	
	boost::shared_ptr<Role> onceRole;
	for (MapRoleCIT cit = m_mapIdToRole.begin(); cit != m_mapIdToRole.end();)
	{
		onceRole = cit->second;
		int seq = onceRole->getClientSeq();
		std::string id = onceRole->getRoleId();
		if (onceRole.get() &&
			(seq == roleSeq) &&
			(id.length() > 0))
		{
			onceRole->setLogoutErrorCode(errCode);
			onceRole->logout();
			m_mapIdToRole.erase(cit++);
			break;
		}
		else
		{
			cit++;
		}
		
	}
}

bool RoleManager::isRoleExists(const std::string& roleId)
{
	std::map<std::string, boost::shared_ptr<Role>>::const_iterator cit = 
		m_mapIdToRole.find(roleId);

	return (cit != m_mapIdToRole.end());
}

void RoleManager::deleteInstance()
{
	if (instanceObj)
	{
		delete instanceObj;
		instanceObj = NULL;
	}
}
