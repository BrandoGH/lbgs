#include "cachemsghandler.h"
#include "redis/rediscluster.h"
#include "src/cacheserver.h"

#include <logmodule/logdef.h>
#include <msgmodule/msgcommondef.h>
#include <logicserver/communicationmsg/msglogin.h>
#include <iosfwd>
#include <commontool/commontool.h>

namespace
{
// roleSeq = clientSeq of Gateserver, format: [roleId]_login
std::string genRoleLoginStatusKey(int roleSeq)
{
	std::stringstream fm;
	fm << CAST_TO(std::string, roleSeq) << "_login";
	return fm.str();
}
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

	// role login
	if (msg->m_cLoginFlag == MsgLoginCS::LF_LOGIN)
	{
		BaseRedis::RedisReturnST value = redis->get(CommonTool::genRoleIdByUserName(msg->m_strRoleName));
		if (value.m_len == 0)		// new role
		{
			// key = [roleSeq]_login value = uuid
			std::string key = genRoleLoginStatusKey(header->m_nClientSrcSeq);
			std::string roleId = CommonTool::genRoleIdByUserName(msg->m_strRoleName);
			redis->setex(key, roleId.data(), key.size(), roleId.size(), redis->getKeyStatusExpireSec());
			// key = uuid value = status
			redis->setKeyStatus(roleId, RedisCluster::RCS_UNCONFIRMED);

			//  search db
			pCacheServer->sendToDBServer(data, dataSize);

		} 
		else if (value.m_len > 0 && CAST_TO(int, std::string(value.m_getData)) == RedisCluster::RCS_DB_NOT_EXISTS)
		{
			DEFINE_BYTE_ARRAY(dataArr, sizeof(MsgHeader) + sizeof(MsgLoginSC));
			memset(dataArr, 0, sizeof(dataArr));

			header->m_nMsgLen = sizeof(MsgHeader) + sizeof(MsgLoginSC);
			header->m_nMsgType = MSG_TYPE_LOGIN_REGISTER_SC;

			MsgLoginSC sc;
			sc.m_cLoginStatus = MsgLoginSC::LS_LOGIN_ERROR;
			sc.m_cErrorReason = MsgLoginSC::ER_UNREGISTERED;

			memmove(dataArr, data, sizeof(MsgHeader));
			memmove(dataArr + sizeof(MsgHeader), (const char*)&sc, sizeof(MsgLoginSC));
			callHandler(MSG_TYPE_LOGIN_REGISTER_SC, pCacheServer, dataArr, sizeof(dataArr));
		}
	}
	else if (msg->m_cLoginFlag == MsgLoginCS::LF_REGISTER)
	{
		// TODO register role
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

	// set cache data, if role is not exists
	if (msg->m_cLoginStatus == MsgLoginSC::LS_LOGIN_ERROR &&
		msg->m_cErrorReason == MsgLoginSC::ER_UNREGISTERED)
	{
		BaseRedis::RedisReturnST value = redis->get(
			genRoleLoginStatusKey(header->m_nClientSrcSeq));
		std::string roleId = value.m_len > 0 ? value.m_getData : "";
		BaseRedis::RedisReturnST roldIdVal = redis->get(roleId);
		if (roldIdVal.m_len > 0 && CAST_TO(int, std::string(roldIdVal.m_getData)) == RedisCluster::RCS_UNCONFIRMED)
		{
			redis->setKeyStatus(roleId, RedisCluster::RCS_DB_NOT_EXISTS);
		}

	}

	pCacheServer->sendToLogicServer(data, dataSize);

}


// Non-handler jump part
HandlerFunc g_handlerList[EnMsgType::MSG_TYPE_CLIENT_SIZE] =
{
	NULL,
	NULL,
	onLoginLC,
	onLoginCL,
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