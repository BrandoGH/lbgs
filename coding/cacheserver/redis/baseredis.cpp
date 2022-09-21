#include "baseredis.h"

#include <servercommon/logmodule/logdef.h>
#include <servercommon/cacheserverconfig.h>

#define REDIS_OP_CALLBACK(opType, opKeyName, opSetKeyValue,opKeySize,opValSize)		\
if (m_redisRep)																		\
{																					\
	switch (m_redisRep->type)														\
	{																				\
	case REDIS_REPLY_INTEGER:														\
		m_callbackOp(opType, opKeyName, opSetKeyValue,								\
			opKeySize,opValSize, m_redisRep->integer, m_redisRep->str);				\
		break;																		\
	case REDIS_REPLY_ERROR:															\
	case REDIS_REPLY_NIL:															\
		m_callbackOp(opType, opKeyName, opSetKeyValue, opKeySize, opValSize,		\
			false, m_redisRep->str);												\
		break;																		\
	default:																		\
		m_callbackOp(opType, opKeyName,  opSetKeyValue,								\
			opKeySize, opValSize,true, m_redisRep->str);							\
		break;																		\
	}																				\
}																					\
else																				\
{																					\
	m_callbackOp(opType, opKeyName,  opSetKeyValue, opKeySize, opValSize,			\
		false,"reply handle NULL");													\
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

void BaseRedis::start(const std::string& ip, ushort port, const CacheServerConnectBaseCfgInfo* timeoutInfo, const std::string password)
{
	boost::thread tStart(
		BIND(&BaseRedis::onThreadStart, this, ip, port, timeoutInfo, password));
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
	REDIS_OP_CALLBACK(OP_SET, key.data(), val, keySize, valSize);
	freeReplyObject(m_redisRep);
}

void BaseRedis::setnx(const std::string& key, const char* val, uint keySize, uint valSize)
{
	m_redisRep = (redisReply*)redisCommand(m_redisCont, "SETNX %b %b", key.data(), (size_t)keySize, val, (size_t)valSize);
	REDIS_OP_CALLBACK(OP_SETNX, key.data(), val, keySize, valSize);
	freeReplyObject(m_redisRep);
}

BaseRedis::GetValueST BaseRedis::get(const std::string& key)
{
	GetValueST retSt;
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
	REDIS_OP_CALLBACK(OP_GET, key.data(), "", key.length(), retSt.m_len);
	freeReplyObject(m_redisRep);
	return retSt;
}

void BaseRedis::onThreadStart(
	const std::string& ip,
	ushort port,
	const CacheServerConnectBaseCfgInfo* timeoutInfo = NULL,
	const std::string password = std::string())
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


