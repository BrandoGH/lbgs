#ifndef __PROXY_SERVER_CONFIG_H__
#define __PROXY_SERVER_CONFIG_H__
 
#include <configmodule/configinterface.h>

struct ProxyServerConfigInfo
{
	std::string ip;
	ushort port;
	ushort heart_time;	// 和代理服的心跳时间
};

class ProxyServerConfig : public ConfigInterface
{
public:
	enum EnErrorCode
	{
		ERROR_PORT = 1,
		ERROR_IP,
		ERROR_HEART_TIME,
	};

public:
	ProxyServerConfig();
	virtual ~ProxyServerConfig();

	virtual int init(const std::string& path) override;

	const ProxyServerConfigInfo* getConfigInfo() { return &m_info; }


private:
	int initInfoCfg(CommonBoost::PTree& rootNode);

private:
	ProxyServerConfigInfo m_info;
};


#endif // ! __PROXY_SERVER_CONFIG_H__
