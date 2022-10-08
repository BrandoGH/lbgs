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

	std::string roleId = CommonTool::genRoleIdByUserName(msg->m_strRoleName);

	// new role
	if (!DB_MGR->checkRoleExists(roleId))
	{
		RoleLoginInfoParam param;
		memmove(param.m_strRoleId, roleId.data(), sizeof(param.m_strRoleId));
		memmove(param.m_strRoleName, msg->m_strRoleName, sizeof(param.m_strRoleName));
		memmove(param.m_strPassword, msg->m_strPassword, sizeof(param.m_strPassword));
		DB_MGR->registerRoleLoginInfo(param, [&, pDBServer, data](const RoleLoginInfoParam& retParam)
		{
			MsgLoginSC sc;
			DEFINE_BYTE_ARRAY(dataArr, sizeof(MsgHeader) + sizeof(MsgLoginSC));
			memset(dataArr, 0, sizeof(dataArr));

			sc.m_cLoginStatus = MsgLoginSC::LS_LOGIN_OK;
			sc.m_cErrorReason = MsgLoginSC::ER_NO_ERROR;
			memmove(sc.m_strRoleName, retParam.m_strRoleName, sizeof(sc.m_strRoleName));
			memmove(sc.m_strPassword, retParam.m_strPassword, sizeof(sc.m_strPassword));

			memmove(dataArr, data, sizeof(MsgHeader));
			memmove(dataArr + sizeof(MsgHeader), (const char*)&sc, sizeof(MsgLoginSC));
			callHandler(MSG_TYPE_LOGIN_SC, pDBServer, dataArr, sizeof(dataArr));
		});
	}
	else
	{
		MsgLoginSC sc;
		DEFINE_BYTE_ARRAY(dataArr, sizeof(MsgHeader) + sizeof(MsgLoginSC));
		memset(dataArr, 0, sizeof(dataArr));

		sc.m_cLoginStatus = MsgLoginSC::LS_LOGIN_OK;
		sc.m_cErrorReason = MsgLoginSC::ER_NO_ERROR;
		memmove(sc.m_strRoleName, msg->m_strRoleName, sizeof(sc.m_strRoleName));
		memmove(sc.m_strPassword, msg->m_strPassword, sizeof(sc.m_strPassword));

		memmove(dataArr, data, sizeof(MsgHeader));
		memmove(dataArr + sizeof(MsgHeader), (const char*)&sc, sizeof(MsgLoginSC));
		callHandler(MSG_TYPE_LOGIN_SC, pDBServer, dataArr, sizeof(dataArr));
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
	h->m_nMsgType = MSG_TYPE_LOGIN_SC;

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