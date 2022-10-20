#include "baseredis.h"

#include <servercommon/logmodule/logdef.h>
#include <servercommon/cacheserverconfig.h>

#define REDIS_OP_CALLBACK(opType, opKeyName, opSetKeyValue, opKeySize, opValSize, keyExpireTime)		\
if (m_redisRep)																							\
{																										\
	switch (m_redisRep->type)																			\
	{																									\
	case REDIS_REPLY_INTEGER:																			\
		m_callbackOp(opType, opKeyName, opSetKeyValue,													\
			opKeySize,opValSize, keyExpireTime, m_redisRep->integer, m_redisRep->str);					\
		break;																							\
	case REDIS_REPLY_ERROR:																				\
	case REDIS_REPLY_NIL:																				\
		m_callbackOp(opType, opKeyName, opSetKeyValue, opKeySize, opValSize,							\
			keyExpireTime, false, m_redisRep->str);														\
		break;																							\
	default:																							\
		m_callbackOp(opType, opKeyName,  opSetKeyValue,													\
			opKeySize, opValSize,keyExpireTime,true, m_redisRep->str);									\
		break;																							\
	}																									\
}																										\
else																									\
{																										\
	m_callbackOp(opType, opKeyName, opSetKeyValue, opKeySize, opValSize,								\
		keyExpireTime, false,"reply handle NULL");														\
}


BaseRedis::BaseRedis()
	: m_redisCont(NULL)
	, m_redisRep(NULL)
	, m_nSeq(0)
{
}

BaseRedis::~BaseRedis()
{
	end();
}

void BaseRedis::start(const std::string& ip, ushort port, const CacheServerConnectBaseCfgInfo* timeoutInfo, const std::string& password)
{
	boost::thread tStart(
		BIND(&BaseRedis::onThreadStart, shared_from_this(), ip, port, timeoutInfo, password));
	tStart.detach();
}

void BaseRedis::end()
{
	CloseAllHandle();
}

const std::string BaseRedis::getRedisServerIp()
{
	return m_strConnectIp;
}

ushort BaseRedis::getRedisServerPort()
{
	return m_nConnectPort;
}

void BaseRedis::setStartCallback(CallbackStart callback)
{
	m_callbackStart = callback;
}

void BaseRedis::setOpCallback(CallbackOp callback)
{
	m_callbackOp = callback;
}

void BaseRedis::setCurServiceSeq(int seq)
{
	m_nSeq = seq;
}

int BaseRedis::getCurServiceSeq()
{
	return m_nSeq;
}

void BaseRedis::set(const std::string& key, const char* val, uint keySize, uint valSize)
{
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "SET %b %b", key.data(), (size_t)keySize, val, (size_t)valSize);
	REDIS_OP_CALLBACK(OP_SET, key.data(), val, keySize, valSize, EN_EXPIRE_NULL);
	freeReplyObject(m_redisRep);
}

void BaseRedis::setnx(const std::string& key, const char* val, uint keySize, uint valSize)
{
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "SETNX %b %b", key.data(), (size_t)keySize, val, (size_t)valSize);
	REDIS_OP_CALLBACK(OP_SETNX, key.data(), val, keySize, valSize, EN_EXPIRE_NULL);
	freeReplyObject(m_redisRep);
}

void BaseRedis::setxx(const std::string& key, const char* val, uint keySize, uint valSize)
{
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "SET %b %b XX", key.data(), (size_t)keySize, val, (size_t)valSize);
	REDIS_OP_CALLBACK(OP_SETXX, key.data(), val, keySize, valSize, EN_EXPIRE_NULL);
	freeReplyObject(m_redisRep);
}

void BaseRedis::setex(const std::string& key, const char* val, uint keySize, uint valSize, int expireSec)
{
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "SET %b %b EX %d",
		key.data(), (size_t)keySize, val, (size_t)valSize, expireSec);
	REDIS_OP_CALLBACK(OP_SETEX, key.data(), val, keySize, valSize, expireSec);
	freeReplyObject(m_redisRep);
}

void BaseRedis::setex_nx(const std::string& key, const char* val, uint keySize, uint valSize, int expireSec)
{
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "SET %b %b NX EX %d",
		key.data(), (size_t)keySize, val, (size_t)valSize, expireSec);
	REDIS_OP_CALLBACK(OP_SETEX_NX, key.data(), val, keySize, valSize, expireSec);
	freeReplyObject(m_redisRep);
}

void BaseRedis::expireKey(const std::string& key, int expireSec)
{
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "EXPIRE %s %d", key.data(), expireSec);
	REDIS_OP_CALLBACK(OP_EXPIRE, key.data(), "", key.length(), 0, expireSec);
	freeReplyObject(m_redisRep);
}

BaseRedis::RedisReturnST BaseRedis::get(const std::string& key)
{
	RedisReturnST retSt;
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "GET %s", key.data());
	if (m_redisRep && m_redisRep->str)
	{
		if (m_redisRep->len >= g_nGetValueMaxSize - 1)
		{
			LOG_CACHESERVER.printLog("redis get data size large!!");
			return retSt;
		}
		memmove(retSt.m_getData, m_redisRep->str, m_redisRep->len);
		retSt.m_len = m_redisRep->len;
	}
	REDIS_OP_CALLBACK(OP_GET, key.data(), "", key.length(), retSt.m_len, EN_EXPIRE_NULL);
	freeReplyObject(m_redisRep);
	return retSt;
}

BaseRedis::RedisReturnST BaseRedis::delKey(const std::string& key, bool delByAync)
{
	RedisReturnST retSt;
	if (delByAync)
	{
		m_redisRep = (redisReply*)redisCommand(m_redisCont, "UNLINK %s", key.data());
	}
	else
	{
		m_redisRep = (redisReply*)redisCommand(m_redisCont, "DEL %s", key.data());
	}
	if (m_redisRep)
	{
		if (m_redisRep->len >= g_nGetValueMaxSize - 1)
		{
			LOG_CACHESERVER.printLog("redis get data size large!!");
			return retSt;
		}
		if (m_redisRep->str)
		{
			memmove(retSt.m_getData, m_redisRep->str, m_redisRep->len);
			retSt.m_len = m_redisRep->len;
		}
		retSt.m_nInteger = m_redisRep->integer;
	}

	REDIS_OP_CALLBACK(OP_DEL, key.data(), "", key.length(), 0, EN_EXPIRE_NULL);
	freeReplyObject(m_redisRep);
	return retSt;
}

BaseRedis::RedisReturnST BaseRedis::ttl(const std::string& key)
{
	RedisReturnST retSt;
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "TTL %s", key.data());
	if (m_redisRep)
	{
		if (m_redisRep->len >= g_nGetValueMaxSize - 1)
		{
			LOG_CACHESERVER.printLog("redis get data size large!!");
			return retSt;
		}
		if (m_redisRep->str)
		{
			memmove(retSt.m_getData, m_redisRep->str, m_redisRep->len);
			retSt.m_len = m_redisRep->len;
		}
		retSt.m_nInteger = m_redisRep->integer;
	}

	REDIS_OP_CALLBACK(OP_TTL, key.data(), "", key.length(), 0, EN_EXPIRE_NULL);
	freeReplyObject(m_redisRep);
	return retSt;
}

BaseRedis::RedisReturnST BaseRedis::incr(const std::string& key)
{
	RedisReturnST retSt;
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "INCR %s", key.data());
	if (m_redisRep)
	{
		if (m_redisRep->len >= g_nGetValueMaxSize - 1)
		{
			LOG_CACHESERVER.printLog("redis get data size large!!");
			return retSt;
		}
		if (m_redisRep->str)
		{
			memmove(retSt.m_getData, m_redisRep->str, m_redisRep->len);
			retSt.m_len = m_redisRep->len;
		}
		retSt.m_nInteger = m_redisRep->integer;
	}

	REDIS_OP_CALLBACK(OP_INCR, key.data(), "", key.length(), 0, EN_EXPIRE_NULL);
	freeReplyObject(m_redisRep);
	return retSt;
}

BaseRedis::RedisReturnST BaseRedis::decr(const std::string& key)
{
	RedisReturnST retSt;
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "DECR %s", key.data());
	if (m_redisRep)
	{
		if (m_redisRep->len >= g_nGetValueMaxSize - 1)
		{
			LOG_CACHESERVER.printLog("redis get data size large!!");
			return retSt;
		}
		if (m_redisRep->str)
		{
			memmove(retSt.m_getData, m_redisRep->str, m_redisRep->len);
			retSt.m_len = m_redisRep->len;
		}
		retSt.m_nInteger = m_redisRep->integer;
	}

	REDIS_OP_CALLBACK(OP_DECR, key.data(), "", key.length(), 0, EN_EXPIRE_NULL);
	freeReplyObject(m_redisRep);
	return retSt;
}

void BaseRedis::onThreadStart(
	const std::string& ip,
	ushort port,
	const CacheServerConnectBaseCfgInfo* timeoutInfo = NULL,
	const std::string& password = std::string())
{
	m_strConnectIp = ip;
	m_nConnectPort = port;

	bool ok = connect(ip, port, timeoutInfo) && auth(password);
	if (!m_callbackStart.empty())
	{
		m_callbackStart(ok, getCurServiceSeq());
	}
}

bool BaseRedis::connect(const std::string& ip, ushort port, const CacheServerConnectBaseCfgInfo* timeoutInfo)
{
	if (!timeoutInfo)
	{
		m_redisCont = redisConnect(ip.data(), port);
	}
	else
	{
		timeval tv =
		{
			timeoutInfo->m_nConnectTimeoutSec,
			timeoutInfo->m_nConnectTimeoutMicrosec
		};
		m_redisCont = redisConnectWithTimeout(ip.data(), port, tv);
	}
	
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

bool BaseRedis::auth(const std::string& password)
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

	freeReplyObject(m_redisRep);

	return true;
}

void BaseRedis::CloseAllHandle()
{
	if (!m_redisRep)
	{
		freeReplyObject(m_redisRep);
	}

	if (!m_redisCont)
	{
		redisFree(m_redisCont);
	}
}


