#ifndef __ROLE_MANAGER_H__
#define __ROLE_MANAGER_H__

#include <servercommon/basedef.h>

class RoleManager
{
public:
	RoleManager();
	~RoleManager();

	static RoleManager* instance();

	void loginCheck(byte* msgData, uint dataSize);

private:
	void deleteInstance();

private:
	static RoleManager* instanceObj;
};

#define ROLE_MGR RoleManager::instance()

#endif // !__ROLE_MANAGER_H__
