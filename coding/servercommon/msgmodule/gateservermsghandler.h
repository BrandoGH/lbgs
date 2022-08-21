#ifndef __GATE_SERVER_MSG_HANDLER_H__
#define __GATE_SERVER_MSG_HANDLER_H__

#include "basedef.h"
#include "msgcommondef.h"
#include "boostmodule/basedef.h"
#include "gateserver/src/gateserver.h"

namespace GateServerMsgHandler
{
typedef void(*HandlerFunc)(const GateServer* gateServer, byte* data, uint dataSize);

void onHandlerHeartSC(const GateServer* gateServer, byte* data, uint dataSize);
void onHandlerHeartCS(const GateServer* gateServer, byte* data, uint dataSize);

void callHandler(int msgType, const GateServer* gateServer, byte* data, uint dataSize);
}

#endif // !__GATE_SERVER_MSG_HANDLER_H__
