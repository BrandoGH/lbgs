#ifndef __CACHE_MSG_HANDLER_H__
#define __CACHE_MSG_HANDLER_H__

#include <servercommon/basedef.h>

class DBServer;
namespace DBMsgHandler
{
typedef void(*HandlerFunc)(DBServer* pCacheServer, byte* data, uint dataSize);

void onLoginCD(DBServer* pCacheServer, byte* data, uint dataSize);
void onLoginDC(DBServer* pCacheServer, byte* data, uint dataSize);

void callHandler(int msgType, DBServer* pCacheServer, byte* data, uint dataSize);
}

#endif // !__CACHE_MSG_HANDLER_H__
