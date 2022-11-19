#ifndef __ROLE_MANAGER_H__
#define __ROLE_MANAGER_H__
#include "communicationmsg/msglogin.h"

#include <servercommon/basedef.h>
#include <boostmodule/basedef.h>

class Role;
class RoleManager
{
public:
	struct CreateRoleInput
	{
		ullong m_nClientSeq = 0;
		RoleLoginInfoParam m_param;
	};

public:
	RoleManager();
	~RoleManager();

	static RoleManager* instance();

	void createRole(const CreateRoleInput& input);
	void removeRole(const std::string& roleId, int errCode);
	void tryRemoveRole(ullong roleSeq, int errCode);
	bool isRoleExists(const std::string& roleId);
	bool tryToDetermineIfExists(ullong roleSeq);

	boost::shared_ptr<Role> findRoleByClientSeq(ullong clientSeq);

	void createRoleModel(boost::shared_ptr<Role> myself);
	void notifyRoleInfoChange(boost::shared_ptr<Role> notifyRole);

	const std::map<std::string, boost::shared_ptr<Role>>&
		getRoleMap() { return m_mapIdToRole; }
	
private:
	void deleteInstance();

private:
	static RoleManager* instanceObj;

	std::map<std::string, boost::shared_ptr<Role>> m_mapIdToRole;
	CommonBoost::Mutex m_mtxMap;
};

#define ROLE_MGR RoleManager::instance()

#endif // !__ROLE_MANAGER_H__
