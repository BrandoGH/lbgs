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
			boost::placeholders::_3, boost::placeholders::_4, boost::placeholders::_5, boost::placeholders::_6, boost::placeholders::_7));
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

BaseRedis::GetValueST RedisCluster::get(const std::string& key)
{
	CommonBoost::UniqueLock lock(m_mtxRedisOp);
	BaseRedis::GetValueST retSt;
	int randIdx = CommonTool::getRandom(0, m_CfgCache->getCurClusterCount() - 1);
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[randIdx];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", randIdx);
		return retSt;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();

	retSt = opRedis->get(key);
	return clusterDataCheck_Get(key, retSt);
}

void RedisCluster::OnOpResult(
	int opType,
	const char* opKey,
	const char* opKeySetVal,
	uint keySize,
	uint valSize,
	bool ok,
	const char* str
)
{
	LOG_CACHESERVER.printLog("opType[%d] opKey[%s] opKeySetVal[%s] keySize[%d] valSize[%d] ok[%d] str[%s]", 
		opType, opKey, opKeySetVal, keySize, valSize ,ok, str);

	clusterDataCheck_Set(opType, opKey, opKeySetVal, ok, str, keySize, valSize);
}

BaseRedis::GetValueST RedisCluster::clusterDataCheck_Get(const std::string& checkKey, const BaseRedis::GetValueST& checkRetValue)
{
	BaseRedis::GetValueST inputSt = checkRetValue;
	std::string checkStr = checkRetValue.m_getData ? checkRetValue.m_getData : "";
	if (checkStr.substr(0, 5) != "MOVED")
	{
		return inputSt;
	}

	std::vector<std::string> vecSplit;
	boost::split(vecSplit, checkStr, boost::is_any_of(" "), boost::token_compress_on);

	int index = m_mapClusterInfoIndex[vecSplit[vecSplit.size() - 1]];
	boost::weak_ptr<BaseRedis> weakRedis = m_vecRedisCluster[index];
	if (weakRedis.expired())
	{
		LOG_CACHESERVER.printLog("m_vecRedisCluster[%d] expired", index);
		return inputSt;
	}
	boost::shared_ptr<BaseRedis> opRedis = weakRedis.lock();
	return opRedis->get(checkKey);
}

void RedisCluster::clusterDataCheck_Set(
	int opType,
	const char* opKey,
	const char* opKeySetVal,
	bool ok,
	const char* str,
	uint keySize,
	uint valSize)
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
		if (!m_cbClusterConnected.empty())
		{
			m_cbClusterConnected(true);
		}
		LOG_CACHESERVER.printLog("m_nRedisConnectCount[%d]", m_nRedisConnectCount.load());
	}
}
