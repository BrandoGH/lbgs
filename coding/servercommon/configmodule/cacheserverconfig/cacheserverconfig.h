#ifndef __CACHE_SERVER_CONFIG_H__
#define __CACHE_SERVER_CONFIG_H__

#include <configmodule/configinterface.h>

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

// m_nRole: 1-Master 0-Slave
struct CacheReplicatConfigInfo
{
	enum EnRoleFlag
	{
		ROLE_ERROR		= -1,
		ROLE_SLAVE		= 0,
		ROLE_MASTER		= 1,
	};

	CacheReplicatConfigInfo()
	{
		reset();
	}

	void reset()
	{
		m_strIp = "";
		m_strPassword = "";
		m_nPort = 0;
		m_nRole = ROLE_ERROR;
	}

	std::string m_strIp;
	std::string m_strPassword;
	ushort m_nPort;
	ushort m_nRole;
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

	// Get a master server configuration, if any (default first configuration)
	const CacheReplicatConfigInfo* getSingleMasterReidsCfg();
	const CacheServerConnectBaseCfgInfo* getBaseCacheCfg();

private:
	int initBaseInfoCfg(CommonBoost::PTree& rootNode);
	int initReplicatInfoCfg(CommonBoost::PTree& rootNode);

private:
	std::vector<CacheReplicatConfigInfo> m_vecMasterRedis;
	std::vector<CacheReplicatConfigInfo> m_vecSlaveRedis;

	CacheServerConnectBaseCfgInfo m_cfgBase;

};

#endif // !__CACHE_SERVER_CONFIG_H__
