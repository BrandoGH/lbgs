#include "cacheserverconfig.h"
#include "commontool/commontool.h"
#include "logmodule/logdef.h"

CacheServerConfig::CacheServerConfig()
{
}

CacheServerConfig::~CacheServerConfig()
{
}

int CacheServerConfig::init(const std::string & path)
{
	LOAD_NODE("config.cacheserver.baseconfig", initBaseInfoCfg);
	LOAD_NODE("config.cacheserver.replicat_server_list", initReplicatInfoCfg);
	return INIT_OK;
}

const CacheReplicatConfigInfo* CacheServerConfig::getSingleMasterReidsCfg()
{
	if (m_vecMasterRedis.size() <= 0)
	{
		LOG_CACHESERVER.printLog("get error: m_vecMasterRedis.size = [%d]", m_vecMasterRedis.size());
		return NULL;
	}
	return &m_vecMasterRedis[0];
}

const CacheServerConnectBaseCfgInfo* CacheServerConfig::getBaseCacheCfg()
{
	return &m_cfgBase;
}

int CacheServerConfig::initBaseInfoCfg(CommonBoost::PTree& rootNode)
{
	CommonBoost::PTree::iterator it = rootNode.begin();
	for (; it != rootNode.end(); ++it)
	{
		CommonBoost::PTree dataNode = it->second;
		
		m_cfgBase.m_nConnectTimeoutSec = CAST_TO(int, dataNode.get_child("connect_timeout_sec").data());
		m_cfgBase.m_nConnectTimeoutMicrosec = CAST_TO(long long, dataNode.get_child("connect_timeout_microsec").data());

		if (m_cfgBase.m_nConnectTimeoutSec < 0 ||
			m_cfgBase.m_nConnectTimeoutMicrosec < 0)
		{
			LOG_CACHESERVER.printLog("config init error! m_cfgBase.m_nConnectTimeoutSec[%d], m_cfgBase.m_nConnectTimeoutMicrosec[%ld]",
				m_cfgBase.m_nConnectTimeoutSec, m_cfgBase.m_nConnectTimeoutMicrosec);
			return INIT_ERROR;
		}
	}

	return INIT_OK;
}

int CacheServerConfig::initReplicatInfoCfg(CommonBoost::PTree& rootNode)
{
	CommonBoost::PTree::iterator it = rootNode.begin();

	CacheReplicatConfigInfo info;
	for (; it != rootNode.end(); ++it)
	{
		info.reset();
		CommonBoost::PTree dataNode = it->second;

		info.m_strIp = dataNode.get_child("ip").data();
		info.m_strPassword = dataNode.get_child("auth_password").data();
		info.m_nPort = CAST_TO(ushort, dataNode.get_child("port").data());
		info.m_nRole = CAST_TO(ushort, dataNode.get_child("role").data());

		if (!CommonTool::isIpFormat(info.m_strIp))
		{
			LOG_CACHESERVER.printLog("ip format error!");
			return INIT_ERROR;
		}
		if (info.m_nPort <= 0 || info.m_nPort > 65535)
		{
			LOG_PROXYSERVER.printLog("port read error");
			return INIT_ERROR;
		}

		if (info.m_nRole == CacheReplicatConfigInfo::ROLE_MASTER)
		{
			m_vecMasterRedis.push_back(info);
		}
		else if (info.m_nRole == CacheReplicatConfigInfo::ROLE_SLAVE)
		{
			m_vecSlaveRedis.push_back(info);
		}
		else
		{
			LOG_PROXYSERVER.printLog("info.m_nRole read error, cur info.m_nRole[%d]", info.m_nRole);
			return INIT_ERROR;
		}
	}


	return INIT_OK;
}
