#ifndef __CACHE_MSG_HANDLER_H__
#define __CACHE_MSG_HANDLER_H__

#include <servercommon/basedef.h>

class CacheServer;
namespace CacheMsgHandler
{
typedef void(*HandlerFunc)(CacheServer* pCacheServer, byte* data, uint dataSize);

void onLoginLC(CacheServer* pCacheServer, byte* data, uint dataSize);
void onLoginCL(CacheServer* pCacheServer, byte* data, uint dataSize);
void onLogoutLC(CacheServer* pCacheServer, byte* data, uint dataSize);

void callHandler(int msgType, CacheServer* pCacheServer, byte* data, uint dataSize);
}

#endif // !__CACHE_MSG_HANDLER_H__
