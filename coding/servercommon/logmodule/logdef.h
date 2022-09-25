#ifndef __LOG_DEF_H__
#define __LOG_DEF_H__

#include "log.h"

#define LOG_GATESERVER					CommonLog::g_logGateServer.setLogHeader(__FUNCTION__, __FILE__, __LINE__)
#define LOG_SERVER_COMMON_CONFIG		CommonLog::g_logServerCommonConfig.setLogHeader(__FUNCTION__, __FILE__, __LINE__)
#define LOG_PROXYSERVER					CommonLog::g_logProxyServer.setLogHeader(__FUNCTION__, __FILE__, __LINE__)
#define LOG_LOGICSERVER					CommonLog::g_logLogicServer.setLogHeader(__FUNCTION__, __FILE__, __LINE__)
#define LOG_CACHESERVER					CommonLog::g_logCacheServer.setLogHeader(__FUNCTION__, __FILE__, __LINE__)
#define LOG_DBSERVER					CommonLog::g_logDBServer.setLogHeader(__FUNCTION__, __FILE__, __LINE__)

#endif // !__LOG_DEF_H__
