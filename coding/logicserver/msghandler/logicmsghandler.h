#ifndef __LOGIC_MSG_HANDLER_H__
#define __LOGIC_MSG_HANDLER_H__

#include <servercommon/basedef.h>
#include <servercommon/msgmodule/msgcommondef.h>
#include <boostmodule/basedef.h>

class LogicServer;
namespace LogicMsgHandler
{
typedef void(*HandlerFunc)(LogicServer* pLogicServer, byte* data, uint dataSize);

void onClientHeartCS(LogicServer* pLogicServer, byte* data, uint dataSize);
void onClientHeartSC(LogicServer* pLogicServer, byte* data, uint dataSize);

void callHandler(int msgType, LogicServer* pLogicServer, byte* data, uint dataSize);
}

#endif // !__LOGIC_MSG_HANDLER_H__
