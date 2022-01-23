#ifndef __PROXYSERVER_EXPORT_H__
#define __PROXYSERVER_EXPORT_H__

#include "proxyserver/config/proxyserverconfig.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	PROXYSERVER_EXPORT ProxyServerConfig* getProxyServerConfig();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ! __PROXYSERVER_EXPORT_H__
