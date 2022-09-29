#include "dbmsghandler.h"
#include "src/dbserver.h"

#include <logmodule/logdef.h>
#include <msgmodule/msgcommondef.h>
#include <logicserver/communicationmsg/msglogin.h>
#include <iosfwd>
#include <commontool/commontool.h>

namespace DBMsgHandler
{


void onLoginCD(DBServer* pDBServer, byte* data, uint dataSize)
{
	if (!pDBServer || !data)
	{
		LOG_DBSERVER.printLog("Pointer NULL");
		return;
	}
	MsgHeader* heeder = (MsgHeader*)data;
	if (!heeder)
	{
		LOG_DBSERVER.printLog("heeder NULL");
		return;
	}

	MsgLoginCS* msg = (MsgLoginCS*)(data + sizeof(MsgHeader));
	if (!msg || msg->m_cLoginFlag != MsgLoginCS::LF_LOGIN)
	{
		LOG_DBSERVER.printLog("msg NULL");
		return;
	}

	
}

void onLoginDC(DBServer* pDBServer, byte* data, uint dataSize)
{
	
}


// Non-handler jump part
HandlerFunc g_handlerList[EnMsgType::MSG_TYPE_CLIENT_SIZE] =
{
	NULL,
	NULL,
	onLoginCD,
	onLoginDC,
};

void callHandler(int msgType, DBServer* pDBServer, byte* data, uint dataSize)
{
	if (!pDBServer)
	{
		LOG_DBSERVER.printLog("pDBServer NULL");
		return;
	}

	if (msgType < MSG_TYPE_CLIENT_START || msgType >= MSG_CODE_MAX)
	{
		LOG_DBSERVER.printLog("msgtype error");
		return;
	}

	if (g_handlerList[msgType - MSG_TYPE_CLIENT_START] == NULL)
	{
		LOG_DBSERVER.printLog("g_handlerList[%d] == NULL", msgType);
		return;
	}

	g_handlerList[msgType - MSG_TYPE_CLIENT_START](pDBServer, data, dataSize);
}

}