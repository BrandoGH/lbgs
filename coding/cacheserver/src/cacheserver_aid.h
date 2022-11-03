#ifndef __CACHE_SERVER_AID_H__
#define __CACHE_SERVER_AID_H__

#include <servercommon/timermodule/timer.h>

class CacheServer;
class [[deprecated]] TimerCacheProxySrvHeart : public Timer
{
public:
	TimerCacheProxySrvHeart();
	~TimerCacheProxySrvHeart();

	void setCacheServer(CacheServer* pCacheServer);
	virtual void timeoutRun();

private:
	CacheServer* m_pCacheServer;
};

#endif // !__CACHE_SERVER_AID_H__
