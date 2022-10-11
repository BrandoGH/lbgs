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
	void removeRole(int roleSeq, int errCode);
	bool isRoleExists(const std::string& roleId);

	Role* findRoleByClientSeq(int clientSeq);

private:
	void deleteInstance();

private:
	static RoleManager* instanceObj;

	std::map<std::string, boost::shared_ptr<Role>> m_mapIdToRole;
	std::map<int, boost::shared_ptr<Role>> m_mapSeqToRole;
	CommonBoost::Mutex m_mtxMap;
};

#define ROLE_MGR RoleManager::instance()

#endif // !__ROLE_MANAGER_H__
