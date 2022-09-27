#ifndef __ROLE_MANAGER_H__
#define __ROLE_MANAGER_H__

class RoleManager
{
public:
	RoleManager();
	~RoleManager();

	static RoleManager* instance();

private:
	void deleteInstance();

private:
	static RoleManager* instanceObj;
};

#define ROLE_MGR RoleManager::instance()

#endif // !__ROLE_MANAGER_H__
