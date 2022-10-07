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
		int m_nClientSeq = -1;
		RoleLoginInfoParam m_param;
	};

public:
	RoleManager();
	~RoleManager();

	static RoleManager* instance();

	void createRole(const CreateRoleInput& input);

private:
	void deleteInstance();

private:
	static RoleManager* instanceObj;

	std::map<std::string, boost::shared_ptr<Role>> m_mapIdToRole;
};

#define ROLE_MGR RoleManager::instance()

#endif // !__ROLE_MANAGER_H__
