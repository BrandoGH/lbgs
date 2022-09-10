#ifndef __SINGLE_TO_PROXY_MSG_HANDLER_H__
#define __SINGLE_TO_PROXY_MSG_HANDLER_H__

#include "basedef.h"
#include "msgcommondef.h"
#include "boostmodule/basedef.h"

#include <gateserver/gateservermsg/gateservermsg.h>

namespace SingleToProxyMsgHandler
{
extern byte g_GateSendProxy[sizeof(MsgHeader) + sizeof(MsgInHeartCS)];
extern byte g_LogicSendProxy[sizeof(MsgHeader) + sizeof(MsgInHeartCS)];
typedef void(*HandlerFunc)(const byte* objServer, byte* data, uint dataSize);

void onHandlerGPHeartCS(const byte* objServer, byte* data, uint dataSize);
void onHandlerPGHeartSC(const byte* objServer, byte* data, uint dataSize);
void onHandlerLPHeartCS(const byte* objServer, byte* data, uint dataSize);
void onHandlerPLHeartSC(const byte* objServer, byte* data, uint dataSize);

void callHandler(int msgType, const byte* objServer, byte* data, uint dataSize);
}

#endif // !__SINGLE_TO_PROXY_MSG_HANDLER_H__
