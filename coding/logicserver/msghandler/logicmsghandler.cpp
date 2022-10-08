#include "logicmsghandler.h"
#include "src/logicserver.h"
#include "src/globallogicserver.h"
#include "communicationmsg/msgheart.h"
#include "communicationmsg/msglogout.h"
#include "communicationmsg/msglogin.h"
#include "rolemanager/rolemanager.h"

#include <logmodule/logdef.h>
#include <commontool/msgtool/msgtool.h>

namespace LogicMsgHandler
{
void onClientHeartCS(LogicServer* pLogicServer, byte* data, uint dataSize)
{
	if (!pLogicServer || !data)
	{
		LOG_LOGICSERVER.printLog("Pointer NULL");
		return;
	}
	MsgHeartCS* msg = (MsgHeartCS*)(data + sizeof(MsgHeader));
	if (!msg || !CommonTool::MsgTool::isBytesDataEQ(msg->m_bytesHeart, (const byte*)I_MSG_HEART_CS, sizeof(msg->m_bytesHeart)))
	{
		LOG_LOGICSERVER.printLog("msg data error");
		return;
	}
	
	callHandler(MSG_TYPE_HEART_SC, pLogicServer, data, dataSize);
}

void onClientHeartSC(LogicServer* pLogicServer, byte* data, uint dataSize)
{
	if (!pLogicServer || !data)
	{
		LOG_LOGICSERVER.printLog("pLogicServer NULL");
		return;
	}
	MsgHeartSC msg;
	memmove(msg.m_bytesHeart, I_MSG_HEART_SC, sizeof(msg.m_bytesHeart));
	MsgHeader* header = (MsgHeader*)data;
	if (!header)
	{
		LOG_LOGICSERVER.printLog("header NULL");
		return;
	}
	header->m_nMsgLen = sizeof(MsgHeader) + sizeof(MsgHeartSC);
	header->m_nMsgType = MSG_TYPE_HEART_SC;
	header->m_nSender = MsgHeader::F_LOGICSERVER;
	header->m_nReceiver = MsgHeader::F_GATESERVER;
	header->m_nProxyer = MsgHeader::F_PROXYSERVER;

	DEFINE_BYTE_ARRAY(sendDataArr, sizeof(MsgHeader) + sizeof(MsgHeartSC));
	memset(sendDataArr, 0, sizeof(MsgHeader) + sizeof(MsgHeartSC));
	memmove(sendDataArr, (const char*)header, sizeof(MsgHeader));
	memmove(sendDataArr + sizeof(MsgHeader), (const char*)&msg, sizeof(MsgHeartSC));
	pLogicServer->sendToProxySrv(sendDataArr, sizeof(MsgHeader) + sizeof(MsgHeartSC));
}

void onClientLoginCS(LogicServer* pLogicServer, byte* data, uint dataSize)
{
	if (!pLogicServer || !data)
	{
		LOG_LOGICSERVER.printLog("Pointer NULL");
		return;
	}
	MsgLoginCS* msg = (MsgLoginCS*)(data + sizeof(MsgHeader));
	if (!msg)
	{
		LOG_LOGICSERVER.printLog("msg NULL");
		return;
	}

	pLogicServer->sendToCache(data, dataSize);
}

void onClientLoginSC(LogicServer* pLogicServer, byte* data, uint dataSize)
{
	if (!pLogicServer || !data)
	{
		LOG_LOGICSERVER.printLog("pLogicServer NULL");
		return;
	}
	MsgHeader* header = (MsgHeader*)data;
	if (!header)
	{
		LOG_LOGICSERVER.printLog("header NULL");
		return;
	}
	MsgLoginSC* msg = (MsgLoginSC*)(data + sizeof(MsgHeader));
	if (!msg)
	{
		LOG_LOGICSERVER.printLog("msg NULL");
		return;
	}

	if (msg->m_cLoginStatus == MsgLoginSC::LS_LOGIN_OK && msg->m_cErrorReason == MsgLoginSC::ER_NO_ERROR)
	{
		// create role
		std::string roleId = CommonTool::genRoleIdByUserName(msg->m_strRoleName);
		RoleManager::CreateRoleInput input;
		RoleLoginInfoParam param;
		memmove(param.m_strRoleId, roleId.data(), sizeof(param.m_strRoleId));
		memmove(param.m_strRoleName, msg->m_strRoleName, sizeof(param.m_strRoleName));
		memmove(param.m_strPassword, msg->m_strPassword, sizeof(param.m_strPassword));

		input.m_nClientSeq = header->m_nClientSrcSeq;
		input.m_param = param;
		ROLE_MGR->createRole(input);
	}

	pLogicServer->sendToClient(data, dataSize);
}

void onClientLogoutCS(LogicServer* pLogicServer, byte* data, uint dataSize)
{
	if (!pLogicServer || !data)
	{
		LOG_LOGICSERVER.printLog("pLogicServer NULL");
		return;
	}
	MsgHeader* header = (MsgHeader*)data;
	if (!header)
	{
		LOG_LOGICSERVER.printLog("header NULL");
		return;
	}
	MsgLogoutCS* msg = (MsgLogoutCS*)(data + sizeof(MsgHeader));
	if (!msg)
	{
		LOG_LOGICSERVER.printLog("msg NULL");
		return;
	}

	ROLE_MGR->removeRole(header->m_nClientSrcSeq, msg->m_nErrorCode);
}


// Non-handler jump part
HandlerFunc g_handlerList[EnMsgType::MSG_TYPE_CLIENT_SIZE] =
{
	onClientHeartCS,
	onClientHeartSC,
	onClientLoginCS,
	onClientLoginSC,
	onClientLogoutCS,
};

void callHandler(int msgType, LogicServer* pLogicServer, byte* data, uint dataSize)
{
	if (!pLogicServer)
	{
		LOG_LOGICSERVER.printLog("pLogicServer NULL");
		return;
	}

	if (msgType < MSG_TYPE_CLIENT_START || msgType >= MSG_CODE_MAX)
	{
		LOG_LOGICSERVER.printLog("msgtype error");
		return;
	}

	if (g_handlerList[msgType - MSG_TYPE_CLIENT_START] == NULL)
	{
		LOG_LOGICSERVER.printLog("g_handlerList[%d] == NULL", msgType);
		return;
	}

	g_handlerList[msgType - MSG_TYPE_CLIENT_START](pLogicServer, data, dataSize);
}

}