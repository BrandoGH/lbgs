#include "rolemanager.h"
#include "src/globallogicserver.h"
#include "src/logicserver.h"

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

void RoleManager::loginCheck(byte* msgData, uint dataSize)
{
	MsgHeader* header = (MsgHeader*)msgData;
	if (!header)
	{
		LOG_ROLE.printLog("login header error");
		return;
	}

	if (header->m_nMsgType != MSG_TYPE_LOGIN_REGISTER_CS)
	{
		LOG_ROLE.printLog("login msg type error");
		return;
	}

	header->m_nSender = MsgHeader::F_LOGICSERVER;
	header->m_nReceiver = MsgHeader::F_CACHESERVER;

	MsgHeader* header2 = (MsgHeader*)msgData;

	if (GLOBAL_LOGIC->getLogicServer())
	{
		GLOBAL_LOGIC->getLogicServer()->sendToProxySrv(msgData, dataSize);
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
