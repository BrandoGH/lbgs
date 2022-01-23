#ifndef __PROXY_SERVER_CONFIG_H__
#define __PROXY_SERVER_CONFIG_H__
 
#include "proxyserver/export/global.h"
#include <configmodule/configinterface.h>

struct ProxyServerConfigInfo
{
	ushort port;
};

class PROXYSERVER_EXPORT ProxyServerConfig : public ConfigInterface
{
public:
	enum EnErrorCode
	{
		ERROR_PORT = 1,
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
