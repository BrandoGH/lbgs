#include "cachemsghandler.h"
#include "redis/rediscluster.h"
#include "src/cacheserver.h"

#include <logmodule/logdef.h>
#include <msgmodule/msgcommondef.h>
#include <logicserver/communicationmsg/msglogin.h>
#include <logicserver/communicationmsg/msglogout.h>
#include <iosfwd>
#include <commontool/commontool.h>
#include <boost/algorithm/string.hpp>


namespace
{

}

namespace CacheMsgHandler
{

void onLoginLC(CacheServer* pCacheServer, byte* data, uint dataSize)
{
	if (!pCacheServer || !data)
	{
		LOG_CACHESERVER.printLog("Pointer NULL");
		return;
	}
	MsgHeader* header = (MsgHeader*)data;
	if (!header)
	{
		LOG_CACHESERVER.printLog("header NULL");
		return;
	}

	MsgLoginCS* msg = (MsgLoginCS*)(data + sizeof(MsgHeader));
	if (!msg)
	{
		LOG_CACHESERVER.printLog("msg NULL");
		return;
	}

	RedisCluster* redis = pCacheServer->getRedisCluster();
	if (!redis)
	{
		LOG_CACHESERVER.printLog("getRedisCluster NULL");
		return;
	}

	std::string roleId = CommonTool::genRoleIdByUserName(msg->m_strRoleName);
	bool bLoginStatusKeyExists = redis->existsKey(redis->getLoginStatusCacheKey(roleId));
	bool bRoleIdExists = redis->existsKey(roleId);
	bool bLogin = redis->getLoginStatusCache(roleId);

	if (!bLoginStatusKeyExists || !bRoleIdExists)
	{
		//  search db
		pCacheServer->sendToDBServer(data, dataSize);
	} 
	else
	{
		header->m_nMsgLen = sizeof(MsgHeader) + sizeof(MsgLoginSC);
		header->m_nMsgType = MSG_TYPE_LOGIN_SC;
		DEFINE_BYTE_ARRAY(dataArr, sizeof(MsgHeader) + sizeof(MsgLoginSC));
		memset(dataArr, 0, sizeof(dataArr));
		MsgLoginSC sc;
		sc.m_cLoginStatus = MsgLoginSC::LS_LOGIN_OK;
		sc.m_cErrorReason = MsgLoginSC::ER_NO_ERROR;
		do 
		{
			if (bLogin)
			{
				sc.m_cLoginStatus = MsgLoginSC::LS_LOGIN_ERROR;
				sc.m_cErrorReason = MsgLoginSC::ER_HAS_LOGIN_ERROR;
				break;
			} else if (bRoleIdExists)
			{
				RoleLoginInfoParam param;
				redis->getLoginParam(roleId, param);
				if (strcmp(msg->m_strRoleName, param.m_strRoleName) == 0 &&
					strcmp(msg->m_strPassword, param.m_strPassword) != 0)
				{
					sc.m_cLoginStatus = MsgLoginSC::LS_LOGIN_ERROR;
					sc.m_cErrorReason = MsgLoginSC::ER_PASSWORD_ERROR;
				}
			}
		} while (0);

		memmove(sc.m_strRoleName, msg->m_strRoleName, sizeof(sc.m_strRoleName));
		memmove(sc.m_strPassword, msg->m_strPassword, sizeof(sc.m_strPassword));
		memmove(dataArr, data, sizeof(MsgHeader));
		memmove(dataArr + sizeof(MsgHeader), (const char*)&sc, sizeof(MsgLoginSC));
		callHandler(MSG_TYPE_LOGIN_SC, pCacheServer, dataArr, sizeof(dataArr));
	}
}

void onLoginCL(CacheServer* pCacheServer, byte* data, uint dataSize)
{
	if (!pCacheServer || !data)
	{
		LOG_CACHESERVER.printLog("Pointer NULL");
		return;
	}

	RedisCluster* redis = pCacheServer->getRedisCluster();
	if (!redis)
	{
		LOG_CACHESERVER.printLog("getRedisCluster NULL");
		return;
	}

	MsgHeader* header = (MsgHeader*)data;
	if (!header)
	{
		LOG_CACHESERVER.printLog("header NULL");
		return;
	}

	MsgLoginSC* msg = (MsgLoginSC*)(data + sizeof(MsgHeader));
	if (!msg)
	{
		LOG_CACHESERVER.printLog("msg NULL");
		return;
	}

	std::string roleId = CommonTool::genRoleIdByUserName(msg->m_strRoleName);

	
	if ((msg->m_cLoginStatus == MsgLoginSC::LS_LOGIN_OK && msg->m_cErrorReason == MsgLoginSC::ER_NO_ERROR))
	{
		RoleLoginInfoParam param;
		memmove(param.m_strRoleId, roleId.data(), sizeof(param.m_strRoleId));
		memmove(param.m_strRoleName, msg->m_strRoleName, sizeof(param.m_strRoleName));
		memmove(param.m_strPassword, msg->m_strPassword, sizeof(param.m_strPassword));
		redis->setLoginParam(roleId, param);
		redis->setLoginStatusCache(roleId, true);
	}
	else if (msg->m_cErrorReason == MsgLoginSC::ER_HAS_LOGIN_ERROR)
	{
		RoleLoginInfoParam param;
		redis->getLoginParam(roleId, param);
		redis->setLoginParam(roleId, param);
		redis->setLoginStatusCache(roleId, true);
	}
	else
	{
		redis->setLoginStatusCache(roleId, false);
	}

	pCacheServer->sendToLogicServer(data, dataSize);

}

void onLogoutLC(CacheServer* pCacheServer, byte* data, uint dataSize)
{
	if (!pCacheServer || !data)
	{
		LOG_CACHESERVER.printLog("Pointer NULL");
		return;
	}

	RedisCluster* redis = pCacheServer->getRedisCluster();
	if (!redis)
	{
		LOG_CACHESERVER.printLog("getRedisCluster NULL");
		return;
	}

	MsgHeader* header = (MsgHeader*)data;
	if (!header)
	{
		LOG_CACHESERVER.printLog("header NULL");
		return;
	}

	MsgLogoutCS* msg = (MsgLogoutCS*)(data + sizeof(MsgHeader));
	if (!msg)
	{
		LOG_CACHESERVER.printLog("msg NULL");
		return;
	}
	// delete cache
	redis->setLoginStatusCache(msg->m_roleId, false);
}


// Non-handler jump part
HandlerFunc g_handlerList[EnMsgType::MSG_TYPE_CLIENT_SIZE] =
{
	NULL,
	NULL,
	onLoginLC,
	onLoginCL,
	onLogoutLC,
};


void callHandler(int msgType, CacheServer* pCacheServer, byte* data, uint dataSize)
{
	if (!pCacheServer)
	{
		LOG_CACHESERVER.printLog("pCacheServer NULL");
		return;
	}

	if (msgType < MSG_TYPE_CLIENT_START || msgType >= MSG_CODE_MAX)
	{
		LOG_CACHESERVER.printLog("msgtype error");
		return;
	}

	if (g_handlerList[msgType - MSG_TYPE_CLIENT_START] == NULL)
	{
		LOG_CACHESERVER.printLog("g_handlerList[%d] == NULL", msgType);
		return;
	}

	g_handlerList[msgType - MSG_TYPE_CLIENT_START](pCacheServer, data, dataSize);
}

}