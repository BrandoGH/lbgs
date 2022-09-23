#include "rediscluster.h"

#include <servercommon/configmodule/configmanager.h>
#include <servercommon/cacheserverconfig.h>
#include <servercommon/basedef.h>
#include <servercommon/logmodule/logdef.h>
#include <servercommon/commontool.h>

#include <boost/smart_ptr/weak_ptr.hpp>
#include <boost/algorithm/string.hpp>

#include <iosfwd>

RedisCluster::RedisCluster()
	: m_CfgCache(CONFIG_MGR->GetCacheServerConfig())
	, m_nRedisConnectCount(0)
	, m_nTTLIndex(-1)
	, m_bCallbackStartResultOnce(false)
{
	if (!m_CfgCache)
	{
		assert(0);
	}
}

RedisCluster::~RedisCluster()
{
}

void RedisCluster::startConnectCluster()
{
	CacheClusterConfigInfo* clusterCfg = NULL;
	CacheServerConnectBaseCfgInfo* baseCfg = const_cast<CacheServerConnectBaseCfgInfo*>(m_CfgCache->getBaseCacheCfg());
	std::string strMapKey;
	for (int i = 0; i < m_CfgCache->getCurClusterCount(); ++i)
	{
		clusterCfg = const_cast<CacheClusterConfigInfo*>(m_CfgCache->getClusterConfigByIndex(i));
		if (!clusterCfg || !baseCfg)
		{
			continue;
		}
		boost::shared_ptr<BaseRedis> onceRedis = boost::make_shared<BaseRedis>();
		onceRedis->setStartCallback(BIND(&RedisCluster::OnStartConnectResult, this, boost::placeholders::_1, boost::placeholders::_2));
		onceRedis->setOpCallback(BIND(&RedisCluster::OnOpResult, this, boost::placeholders::_1, boost::placeholders::_2,
			boost::placeholders::_3, boost::placeholders::_4, boost::placeholders::_5, boost::placeholders::_6, boost::placeholders::_7, boost::placeholders::_8));
		onceRedis->setCurServiceSeq(i);
		onceRedis->start(
			clusterCfg->m_strIp, 
			clusterCfg->m_nPort,
			baseCfg,
			clusterCfg->m_strPassword);

		m_vecRedisCluster.push_back(onceRedis);
		
		// format key
		strMapKey.clear();
		std::stringstream fm;
		fm << clusterCfg->m_strIp << ":" << clusterCfg->m_nPort;
		m_mapClusterInfoIndex[fm.str()] = i;

	}
}

void RedisCluster::setConnectedCallback(CallbackClusterConnected callback)
{
	m_cbClusterConnected = callback;
}

void RedisCluster::set(const std::string& key, const char* val, uint keySize, uint valSize)
{
	CommonBoost::UniqueLock lock(m_mtxRedisOp);
	int randIdx = CommonTool::getRandom(0, m_CfgCache->getCurClusterCount() - 1);
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[randIdx];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", randIdx);
		return;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();
	opRedis->set(key, val, keySize, valSize);
}

void RedisCluster::setnx(const std::string& key, const char* val, uint keySize, uint valSize)
{
	CommonBoost::UniqueLock lock(m_mtxRedisOp);
	int randIdx = CommonTool::getRandom(0, m_CfgCache->getCurClusterCount() - 1);
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[randIdx];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", randIdx);
		return;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();
	opRedis->setnx(key, val, keySize, valSize);
}

void RedisCluster::setxx(const std::string& key, const char* val, uint keySize, uint valSize)
{
	CommonBoost::UniqueLock lock(m_mtxRedisOp);
	int randIdx = CommonTool::getRandom(0, m_CfgCache->getCurClusterCount() - 1);
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[randIdx];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", randIdx);
		return;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();
	opRedis->setxx(key, val, keySize, valSize);
}

void RedisCluster::setex(const std::string& key, const char* val, uint keySize, uint valSize, int expireSec)
{
	CommonBoost::UniqueLock lock(m_mtxRedisOp);
	int randIdx = CommonTool::getRandom(0, m_CfgCache->getCurClusterCount() - 1);
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[randIdx];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", randIdx);
		return;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();
	opRedis->setex(key, val, keySize, valSize, expireSec);
}

BaseRedis::RedisReturnST RedisCluster::get(const std::string& key)
{
	CommonBoost::UniqueLock lock(m_mtxRedisOp);
	BaseRedis::RedisReturnST retSt;
	int randIdx = CommonTool::getRandom(0, m_CfgCache->getCurClusterCount() - 1);
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[randIdx];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", randIdx);
		return retSt;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();

	retSt = opRedis->get(key);
	return clusterDataCheck_MOVED(key, retSt, BaseRedis::OP_GET);
}

bool RedisCluster::existsKey(const std::string& key)
{
	BaseRedis::RedisReturnST st = get(key);
	return ((st.m_len > 0) && (strlen(st.m_getData) > 0));
}

BaseRedis::RedisReturnST RedisCluster::delkey(const std::string& key, bool delByAync)
{
	CommonBoost::UniqueLock lock(m_mtxRedisOp);
	BaseRedis::RedisReturnST retSt;
	int randIdx = CommonTool::getRandom(0, m_CfgCache->getCurClusterCount() - 1);
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[randIdx];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", randIdx);
		return retSt;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();
	retSt = opRedis->delKey(key, delByAync);
	return clusterDataCheck_MOVED(key, retSt, BaseRedis::OP_DEL);
}

BaseRedis::RedisReturnST RedisCluster::ttl(const std::string& key)
{
	CommonBoost::UniqueLock lock(m_mtxRedisOp);
	BaseRedis::RedisReturnST retSt;
	int randIdx = (m_nTTLIndex > -1 ? 
		m_nTTLIndex : 
		CommonTool::getRandom(0, m_CfgCache->getCurClusterCount() - 1));
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[randIdx];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", randIdx);
		return retSt;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();
	retSt = opRedis->ttl(key);
	return clusterDataCheck_MOVED(key, retSt, BaseRedis::OP_TTL);
}

BaseRedis::RedisReturnST RedisCluster::incr(const std::string& key)
{
	CommonBoost::UniqueLock lock(m_mtxRedisOp);
	BaseRedis::RedisReturnST retSt;
	int randIdx = CommonTool::getRandom(0, m_CfgCache->getCurClusterCount() - 1);
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[randIdx];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", randIdx);
		return retSt;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();
	retSt = opRedis->incr(key);
	return clusterDataCheck_MOVED(key, retSt, BaseRedis::OP_INCR);
}

BaseRedis::RedisReturnST RedisCluster::decr(const std::string& key)
{
	CommonBoost::UniqueLock lock(m_mtxRedisOp);
	BaseRedis::RedisReturnST retSt;
	int randIdx = CommonTool::getRandom(0, m_CfgCache->getCurClusterCount() - 1);
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[randIdx];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", randIdx);
		return retSt;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();
	retSt = opRedis->decr(key);
	return clusterDataCheck_MOVED(key, retSt, BaseRedis::OP_DECR);
}

void RedisCluster::expireKey(const std::string& key, int expireSec)
{
	CommonBoost::UniqueLock lock(m_mtxRedisOp);
	int randIdx = CommonTool::getRandom(0, m_CfgCache->getCurClusterCount() - 1);
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[randIdx];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", randIdx);
		return;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();
	opRedis->expireKey(key, expireSec);
}

void RedisCluster::OnOpResult(
	int opType,
	const char* opKey,
	const char* opKeySetVal,
	uint keySize,
	uint valSize,
	uint expireSecSet,
	bool ok,
	const char* str
)
{
	LOG_CACHESERVER.printLog("opType[%d] opKey[%s] opKeySetVal[%s] keySize[%d] valSize[%d] ok[%d] str[%s]", 
		opType, opKey, opKeySetVal, keySize, valSize ,ok, str);

	clusterDataCheck_Set(opType, opKey, opKeySetVal, keySize, valSize, expireSecSet, ok, str);
}

BaseRedis::RedisReturnST RedisCluster::clusterDataCheck_MOVED(const std::string& checkKey, const BaseRedis::RedisReturnST& checkRetValue, int opRedisFlag)
{
	BaseRedis::RedisReturnST inputSt = checkRetValue;
	std::string checkStr = checkRetValue.m_getData ? checkRetValue.m_getData : "";
	if (checkStr.substr(0, 5) != "MOVED")
	{
		return inputSt;
	}

	std::vector<std::string> vecSplit;
	boost::split(vecSplit, checkStr, boost::is_any_of(" "), boost::token_compress_on);

	m_nTTLIndex = m_mapClusterInfoIndex[vecSplit[vecSplit.size() - 1]];
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[m_nTTLIndex];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", m_nTTLIndex);
		return inputSt;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();

	BaseRedis::RedisReturnST retSt;
	switch (opRedisFlag)
	{
	case BaseRedis::OP_GET:
		retSt = opRedis->get(checkKey);
		break;
	case BaseRedis::OP_DEL:
		retSt = opRedis->delKey(checkKey);
		break;
	case BaseRedis::OP_TTL:
		retSt = opRedis->ttl(checkKey);
		break;
	case BaseRedis::OP_INCR:
		retSt = opRedis->incr(checkKey);
		break;
	case BaseRedis::OP_DECR:
		retSt = opRedis->decr(checkKey);
		break;
	}

	return retSt;
}

void RedisCluster::clusterDataCheck_Set(
	int opType,
	const char* opKey,
	const char* opKeySetVal,
	uint keySize,
	uint valSize,
	uint expireSecSet,
	bool ok,
	const char* str
)
{
	if (opType <= BaseRedis::OP_SET_START || opType >= BaseRedis::OP_SET_END)
	{
		return;
	}
	if (ok || (str == NULL))
	{
		return;
	}

	std::string strDeal = str;
	if (strDeal.substr(0, 5) != "MOVED")
	{
		return;
	}
	std::vector<std::string> vecSplit;
	boost::split(vecSplit, strDeal, boost::is_any_of(" "), boost::token_compress_on);

	int index = m_mapClusterInfoIndex[vecSplit[vecSplit.size() - 1]];
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[index];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", index);
		return;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();

	switch (opType)
	{
	case BaseRedis::OP_SET:
		opRedis->set(opKey, opKeySetVal, keySize, valSize);
		break;
	case BaseRedis::OP_SETNX:
		opRedis->setnx(opKey, opKeySetVal, keySize, valSize);
		break;
	case BaseRedis::OP_SETXX:
		opRedis->setxx(opKey, opKeySetVal, keySize, valSize);
		break;
	case BaseRedis::OP_SETEX:
		opRedis->setex(opKey, opKeySetVal, keySize, valSize, expireSecSet);
		break;
	case BaseRedis::OP_EXPIRE:
		opRedis->expireKey(opKey,expireSecSet);
		break;
	}
}

void RedisCluster::OnStartConnectResult(bool ok, int curRedisSeq)
{
	if (!ok)
	{
		LOG_CACHESERVER.printLog("m_redisCluster[%d] connect error", curRedisSeq);
		if (!m_cbClusterConnected.empty())
		{
			m_cbClusterConnected(false);
		}
		return;
	}
	++m_nRedisConnectCount;
	if (m_nRedisConnectCount.load() == m_CfgCache->getCurClusterCount())
	{
		if (!m_cbClusterConnected.empty() && !m_bCallbackStartResultOnce)
		{
			m_bCallbackStartResultOnce = true;
			LOG_CACHESERVER.printLog("m_nRedisConnectCount[%d]", m_nRedisConnectCount.load());
			m_cbClusterConnected(true);
		}
	}
}
