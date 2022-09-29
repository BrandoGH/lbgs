#include "cachemsghandler.h"
#include "redis/rediscluster.h"
#include "src/cacheserver.h"

#include <logmodule/logdef.h>
#include <msgmodule/msgcommondef.h>
#include <logicserver/communicationmsg/msglogin.h>
#include <iosfwd>
#include <commontool/commontool.h>

namespace CacheMsgHandler
{


void onLoginLC(CacheServer* pCacheServer, byte* data, uint dataSize)
{
	if (!pCacheServer || !data)
	{
		LOG_CACHESERVER.printLog("Pointer NULL");
		return;
	}
	MsgHeader* heeder = (MsgHeader*)data;
	if (!heeder)
	{
		LOG_CACHESERVER.printLog("heeder NULL");
		return;
	}

	MsgLoginCS* msg = (MsgLoginCS*)(data + sizeof(MsgHeader));
	if (!msg || msg->m_cLoginFlag != MsgLoginCS::LF_LOGIN)
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

	DEFINE_BYTE_ARRAY(sendDataArr, sizeof(MsgHeader) + sizeof(MsgLoginSC));
	memset(sendDataArr, 0, sizeof(MsgHeader) + sizeof(MsgLoginSC));
	memmove(sendDataArr, (const char*)data, sizeof(MsgHeader));

	BaseRedis::RedisReturnST value = redis->get(CommonTool::genRoleIdByUserName(msg->m_strRoleName));
	if (value.m_len == 0)		// new role
	{
		// TODO search db
		pCacheServer->sendToDBServer(data, dataSize);

		// TEST
		/*MsgLoginSC loginSc;
		loginSc.m_cLoginStatus = MsgLoginSC::LS_LOGIN_ERROR;
		loginSc.m_cErrorReason = MsgLoginSC::ER_UNREGISTERED;
		memmove(sendDataArr + sizeof(MsgHeader), (const char*)&loginSc, sizeof(MsgLoginSC));
		callHandler(MSG_TYPE_LOGIN_REGISTER_SC, pCacheServer, sendDataArr, sizeof(MsgHeader) + sizeof(MsgLoginSC));*/

	} else
	{
		// TODO serialize, save key
	}
	
	
}

void onLoginCL(CacheServer* pCacheServer, byte* data, uint dataSize)
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
	header->m_nMsgLen = sizeof(MsgHeader) + sizeof(MsgLoginSC);
	header->m_nMsgType = MSG_TYPE_LOGIN_REGISTER_SC;
	header->m_nSender = MsgHeader::F_CACHESERVER;
	header->m_nReceiver = MsgHeader::F_LOGICSERVER;
	header->m_nProxyer = MsgHeader::F_PROXYSERVER;

	MsgLoginSC* loginSc = (MsgLoginSC*)(data + sizeof(MsgHeader));

	pCacheServer->sendToProxySrv(data, header->m_nMsgLen);

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