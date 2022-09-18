#include "baseredis.h"

#include <servercommon/logmodule/logdef.h>
#include <servercommon/cacheserverconfig.h>

BaseRedis::BaseRedis()
	: m_redisCont(NULL)
	, m_redisRep(NULL)
{
}

BaseRedis::~BaseRedis()
{
}

bool BaseRedis::connect(const std::string& ip, ushort port, const CacheServerConnectBaseCfgInfo* timeoutInfo)
{
	if (!timeoutInfo)
	{
		LOG_CACHESERVER.printLog("Param timeoutInfo NULL");
		return false;
	}

	m_strConnectIp = ip;
	m_nConnectPort = port;

	timeval tv = 
	{ 
		timeoutInfo->m_nConnectTimeoutSec,
		timeoutInfo->m_nConnectTimeoutMicrosec 
	};

	m_redisCont = redisConnectWithTimeout(ip.data(), port , tv);
	if (!m_redisCont)
	{
		LOG_CACHESERVER.printLog("m_redisCont NULL");
		return false;
	}

	if (m_redisCont->err)
	{
		LOG_CACHESERVER.printLog("Connect redis server error!!! error str = [%s]", m_redisCont->errstr);
		return false;
	}
	return true;
}

bool BaseRedis::auth(const std::string password)
{
	if (!m_redisCont)
	{
		LOG_CACHESERVER.printLog("m_redisCont NULL");
		return false;
	}
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "AUTH default %s", password.data());
	if (!m_redisRep)
	{
		LOG_CACHESERVER.printLog("m_redisRep NULL");
		return false;
	}
	if (m_redisRep->type == REDIS_REPLY_ERROR)
	{
		LOG_CACHESERVER.printLog("%s", m_redisRep->str);
		freeReplyObject(m_redisRep);
		return false;
	} 

	LOG_CACHESERVER.printLog("%s", m_redisRep->str);
	freeReplyObject(m_redisRep);

	return true;
}


