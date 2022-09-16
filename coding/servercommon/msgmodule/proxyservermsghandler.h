#ifndef __PROXY_SERVER_MSG_HANDLER_H__
#define __PROXY_SERVER_MSG_HANDLER_H__

#include "basedef.h"
#include "msgcommondef.h"
#include "boostmodule/basedef.h"

class ServerLinker;
namespace ProxyServerMsgHandler
{
typedef void(*HandlerFunc)(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize);

void onHandlerGateHeartCS(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize);
void onHandlerGateHeartSC(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize);
void onHandlerLogicHeartCS(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize);
void onHandlerLogicHeartSC(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize);
void onHandlerCacheHeartCS(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize);
void onHandlerCacheHeartSC(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize);

void callHandler(int msgType, const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize);
}

#endif // !__PROXY_SERVER_MSG_HANDLER_H__
