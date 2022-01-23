#include "export.h"

PROXYSERVER_EXPORT ProxyServerConfig* getProxyServerConfig()
{
	return new ProxyServerConfig;
}
