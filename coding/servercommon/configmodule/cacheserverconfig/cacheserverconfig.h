#ifndef __CACHE_SERVER_CONFIG_H__
#define __CACHE_SERVER_CONFIG_H__

#include <configmodule/configinterface.h>

namespace
{
// Redis cluster support max slot value(max nodes size) are 16384
// Here a cache server setting can connect up to 100 redis
const int g_nRedisClusterMaxNode = 100;
}

struct CacheServerConnectBaseCfgInfo
{
	CacheServerConnectBaseCfgInfo()
	{
		m_nConnectTimeoutSec = 0;
		m_nConnectTimeoutMicrosec = 0LL;
	}
	int m_nConnectTimeoutSec;
	long long m_nConnectTimeoutMicrosec;
};

struct CacheClusterConfigInfo
{
	CacheClusterConfigInfo()
	{
		reset();
	}

	void reset()
	{
		m_nSeq = 0;
		m_strIp = "";
		m_strPassword = "";
		m_nPort = 0;
	}

	int m_nSeq;
	std::string m_strIp;
	std::string m_strPassword;
	ushort m_nPort;
};

class CacheServerConfig : public ConfigInterface
{
public:
	enum EnErrorCode
	{
		INIT_ERROR = 1,
	};

public:
	CacheServerConfig();
	virtual ~CacheServerConfig();

	virtual int init(const std::string& path) override;

	int getCurClusterCount();
	const CacheServerConnectBaseCfgInfo* getBaseCacheCfg();
	const CacheClusterConfigInfo* getClusterConfigByIndex(int idx);

private:
	int initBaseInfoCfg(CommonBoost::PTree& rootNode);
	int initClusterInfoCfg(CommonBoost::PTree& rootNode);

private:
	CacheClusterConfigInfo m_arrRedisClusterCfgInfo[g_nRedisClusterMaxNode];
	CacheServerConnectBaseCfgInfo m_cfgBase;

	int m_nCurMaxClusterCount;
};

#endif // !__CACHE_SERVER_CONFIG_H__
