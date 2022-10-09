#include "cacheserverconfig.h"
#include "commontool/commontool.h"
#include "logmodule/logdef.h"


CacheServerConfig::CacheServerConfig()
	: m_nCurMaxClusterCount(0)
{
}

CacheServerConfig::~CacheServerConfig()
{
}

int CacheServerConfig::init(const std::string & path)
{
	LOAD_NODE("config.cacheserver.baseconfig", initBaseInfoCfg);
	LOAD_NODE("config.cacheserver.cluster_server_list", initClusterInfoCfg);
	return INIT_OK;
}

int CacheServerConfig::getCurClusterCount()
{
	return m_nCurMaxClusterCount;
}

const CacheServerConnectBaseCfgInfo* CacheServerConfig::getBaseCacheCfg()
{
	return &m_cfgBase;
}

const CacheClusterConfigInfo* CacheServerConfig::getClusterConfigByIndex(int idx)
{
	if (idx < 0 || idx >= g_nRedisClusterMaxNode)
	{
		return NULL;
	}
	return &m_arrRedisClusterCfgInfo[idx];
}

int CacheServerConfig::getLogoutAfterExpire()
{
	return m_cfgBase.m_nLogoutAfExpire;
}

int CacheServerConfig::initBaseInfoCfg(CommonBoost::PTree& rootNode)
{
	CommonBoost::PTree::iterator it = rootNode.begin();
	for (; it != rootNode.end(); ++it)
	{
		CommonBoost::PTree dataNode = it->second;
		
		m_cfgBase.m_nConnectTimeoutSec = CAST_TO(int, dataNode.get_child("connect_timeout_sec").data());
		m_cfgBase.m_nConnectTimeoutMicrosec = CAST_TO(long long, dataNode.get_child("connect_timeout_microsec").data());
		m_cfgBase.m_nLogoutAfExpire = CAST_TO(int, dataNode.get_child("logout_after_expire").data());

		if (m_cfgBase.m_nConnectTimeoutSec < 0 ||
			m_cfgBase.m_nConnectTimeoutMicrosec < 0 ||
			m_cfgBase.m_nLogoutAfExpire <= 0)
		{
			LOG_CACHESERVER.printLog("config init error! m_cfgBase.m_nConnectTimeoutSec[%d]," 
				"m_cfgBase.m_nConnectTimeoutMicrosec[%ld]"
				"m_cfgBase.m_nLogoutAfExpire[%d]",
				m_cfgBase.m_nConnectTimeoutSec, 
				m_cfgBase.m_nConnectTimeoutMicrosec, 
				m_cfgBase.m_nLogoutAfExpire);
			return INIT_ERROR;
		}
	}

	return INIT_OK;
}

int CacheServerConfig::initClusterInfoCfg(CommonBoost::PTree& rootNode)
{
	CommonBoost::PTree::iterator it = rootNode.begin();

	CacheClusterConfigInfo info;
	for (; it != rootNode.end(); ++it)
	{
		info.reset();
		CommonBoost::PTree dataNode = it->second;

		info.m_nSeq = CAST_TO(int, dataNode.get_child("seq").data());
		info.m_strIp = dataNode.get_child("ip").data();
		info.m_nPort = CAST_TO(ushort, dataNode.get_child("port").data());
		info.m_strPassword = dataNode.get_child("auth_password").data();

		if (!CommonTool::isIpFormat(info.m_strIp) ||
			(info.m_nPort <= 0 || info.m_nPort > 65535) ||
			(info.m_nSeq < 0 || info.m_nSeq >= g_nRedisClusterMaxNode))
		{
			LOG_CACHESERVER.printLog("read xml error!");
			return INIT_ERROR;
		}
		m_nCurMaxClusterCount = info.m_nSeq + 1;
		m_arrRedisClusterCfgInfo[info.m_nSeq] = info;
	}

	return INIT_OK;
}
