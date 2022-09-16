#include "cacheserver_aid.h"
#include "cacheserver.h"

#include <logmodule/logdef.h>
#include <msgmodule/singletoproxymsghandler.h>
#include <msgmodule/msgcommondef.h>

TimerCacheProxySrvHeart::TimerCacheProxySrvHeart()
	: m_pCacheServer(NULL)
{
}

TimerCacheProxySrvHeart::~TimerCacheProxySrvHeart()
{
}

void TimerCacheProxySrvHeart::setCacheServer(CacheServer* pCacheServer)
{
	m_pCacheServer = pCacheServer;
}

void TimerCacheProxySrvHeart::timeoutRun()
{
	if (!m_pCacheServer)
	{
		LOG_CACHESERVER.printLog("m_pCacheServer NULL");
		return;
	}
	SingleToProxyMsgHandler::callHandler(
		MSG_TYPE_CACHE_PROXY_HEART_CP,
	(const byte*)m_pCacheServer,
	NULL,
	0);
	m_pCacheServer->sendToProxySrv(SingleToProxyMsgHandler::g_CacheSendProxy, sizeof(SingleToProxyMsgHandler::g_CacheSendProxy));
}
