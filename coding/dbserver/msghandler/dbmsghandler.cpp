#include "dbmsghandler.h"
#include "src/dbserver.h"
#include "src/dbmanager.h"

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
	MsgHeader* header = (MsgHeader*)data;
	if (!header)
	{
		LOG_DBSERVER.printLog("header NULL");
		return;
	}

	MsgLoginCS* msg = (MsgLoginCS*)(data + sizeof(MsgHeader));
	if (!msg)
	{
		LOG_DBSERVER.printLog("msg NULL");
		return;
	}

	MsgLoginSC sc;
	DEFINE_BYTE_ARRAY(dataArr, sizeof(MsgHeader) + sizeof(MsgLoginSC));
	memset(dataArr, 0, sizeof(dataArr));
	if (msg->m_cLoginFlag == MsgLoginCS::LF_LOGIN)
	{
		// new role
		if (!DB_MGR->loginCheckRoleExists(std::string(msg->m_strRoleName)))
		{
			sc.m_cLoginStatus = MsgLoginSC::LS_LOGIN_ERROR;
			sc.m_cErrorReason = MsgLoginSC::ER_UNREGISTERED;

			memmove(dataArr, data, sizeof(MsgHeader));
			memmove(dataArr + sizeof(MsgHeader), (const char*)&sc, sizeof(MsgLoginSC));
			callHandler(MSG_TYPE_LOGIN_REGISTER_SC, pDBServer, dataArr, sizeof(dataArr));
		}
	}
	else if (msg->m_cLoginFlag == MsgLoginCS::LF_REGISTER)
	{
		// TODO register role
	}
	
}

void onLoginDC(DBServer* pDBServer, byte* data, uint dataSize)
{
	if (!pDBServer || !data)
	{
		LOG_DBSERVER.printLog("point NULL");
		return;
	}
	MsgHeader* h = (MsgHeader*)data;
	if (!h)
	{
		LOG_DBSERVER.printLog("hearder NULL");
		return;
	}

	h->m_nMsgLen = sizeof(MsgHeader) + sizeof(MsgLoginSC);
	h->m_nMsgType = MSG_TYPE_LOGIN_REGISTER_SC;

	pDBServer->sendToCacheServer(data, dataSize);
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