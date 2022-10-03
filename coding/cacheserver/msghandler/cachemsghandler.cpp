#include "cachemsghandler.h"
#include "redis/rediscluster.h"
#include "src/cacheserver.h"

#include <logmodule/logdef.h>
#include <msgmodule/msgcommondef.h>
#include <logicserver/communicationmsg/msglogin.h>
#include <iosfwd>
#include <commontool/commontool.h>
#include <boost/algorithm/string.hpp>


namespace
{
// roleSeq = clientSeq of Gateserver, format: [roleSeq]_login
std::string genRoleLoginStatusKey(int roleSeq)
{
	std::stringstream fm;
	fm << CAST_TO(std::string, roleSeq) << "_login";
	return fm.str();
}

// [roleSeq]_login = [roleId]_[status]
std::string genRoleLoginStatusValue(const std::string& roleId, int status)
{
	std::stringstream fm;
	fm << roleId << "__" << status;
	return fm.str();
}

// vecSplit[0] = roleId vecSplit[1] = status 
std::string getRoleLoginStatusValue_RoleId(const std::string& val)
{
	std::vector<std::string> vecSplit;
	boost::split(vecSplit, val, boost::is_any_of("__"), boost::token_compress_on);
	return vecSplit[0];
}

// vecSplit[0] = roleId vecSplit[1] = status 
int getRoleLoginStatusValue_Status(const std::string& val)
{
	std::vector<std::string> vecSplit;
	boost::split(vecSplit, val, boost::is_any_of("__"), boost::token_compress_on);
	return CAST_TO(int, vecSplit[1]);
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
		std::string key = genRoleLoginStatusKey(header->m_nClientSrcSeq);
		BaseRedis::RedisReturnST value = redis->get(key);
		if (value.m_len == 0)		// new role
		{
			std::string roleId = CommonTool::genRoleIdByUserName(msg->m_strRoleName);
			std::string saveVal = genRoleLoginStatusValue(roleId, RedisCluster::RCS_UNCONFIRMED);
			// [roleSeq]_login = [roleId]_[status]
			redis->setex(key, saveVal.data(), key.size(), saveVal.size(), redis->getKeyStatusExpireSec());

			//  search db
			pCacheServer->sendToDBServer(data, dataSize);

		} 
		else if (value.m_len > 0 && 
			getRoleLoginStatusValue_Status(std::string(value.m_getData)) == RedisCluster::RCS_DB_NOT_EXISTS)
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
		std::string key = genRoleLoginStatusKey(header->m_nClientSrcSeq);
		BaseRedis::RedisReturnST value = redis->get(key);
		if (value.m_len > 0)
		{
			if (getRoleLoginStatusValue_Status(std::string(value.m_getData)) == RedisCluster::RCS_UNCONFIRMED)
			{
				std::string roleId = getRoleLoginStatusValue_RoleId(std::string(value.m_getData));
				std::string saveVal = genRoleLoginStatusValue(roleId, RedisCluster::RCS_DB_NOT_EXISTS);
				redis->setex(key, saveVal.data(), key.size(), saveVal.size(), redis->getKeyStatusExpireSec());
			}
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