#include "rolemanager.h"

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

void RoleManager::deleteInstance()
{
	if (instanceObj)
	{
		delete instanceObj;
		instanceObj = NULL;
	}
}
