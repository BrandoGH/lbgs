#ifndef __LOGIC_MSG_HANDLER_H__
#define __LOGIC_MSG_HANDLER_H__

#include <servercommon/basedef.h>
#include <servercommon/msgmodule/msgcommondef.h>
#include <boostmodule/basedef.h>

class LogicServer;
class Role;
namespace LogicMsgHandler
{
typedef void(*HandlerFunc)(LogicServer* pLogicServer, boost::weak_ptr<Role> role, byte* data, uint dataSize);

void onClientHeartCS(LogicServer* pLogicServer, boost::weak_ptr<Role> role, byte* data, uint dataSize);
void onClientHeartSC(LogicServer* pLogicServer, boost::weak_ptr<Role> role, byte* data, uint dataSize);
void onClientLoginCS(LogicServer* pLogicServer, boost::weak_ptr<Role> role, byte* data, uint dataSize);
void onClientLoginSC(LogicServer* pLogicServer, boost::weak_ptr<Role> role, byte* data, uint dataSize);
void onClientLogoutCS(LogicServer* pLogicServer, boost::weak_ptr<Role> role, byte* data, uint dataSize);

void callHandler(int msgType, LogicServer* pLogicServer, boost::weak_ptr<Role> role, byte* data, uint dataSize);
}

#endif // !__LOGIC_MSG_HANDLER_H__
