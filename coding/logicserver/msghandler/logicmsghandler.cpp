#include "logicmsghandler.h"
#include "src/logicserver.h"
#include "src/globallogicserver.h"
#include "communicationmsg/msgheart.h"
#include "communicationmsg/msglogout.h"
#include "communicationmsg/msglogin.h"
#include "rolemanager/rolemanager.h"
#include "rolemanager/role.h"

#include <logmodule/logdef.h>
#include <commontool/msgtool/msgtool.h>
#include "communicationmsg/msgcreaterole.h"
#include "communicationmsg/msgroleinfoupdate.h"

#define LOGIC_REV_MSG_CHECK_MSG_STRUCT(msg_struct) \
if (!pLogicServer || !data)\
{\
	LOG_LOGICSERVER.printLog("pLogicServer NULL");\
	return;\
}\
MsgHeader* header = (MsgHeader*)data;\
if (!header)\
{\
	LOG_LOGICSERVER.printLog("header NULL");\
	return;\
}\
msg_struct* msg = (msg_struct*)(data + sizeof(MsgHeader));\
if (!msg)\
{\
	LOG_LOGICSERVER.printLog("msg NULL");\
	return;\
}

#define LOGIC_REV_MSG_CHECK \
if (!pLogicServer || !data)\
{\
	LOG_LOGICSERVER.printLog("pLogicServer NULL");\
	return;\
}\
MsgHeader* header = (MsgHeader*)data;\
if (!header)\
{\
	LOG_LOGICSERVER.printLog("header NULL");\
	return;\
}

namespace LogicMsgHandler
{
void onClientHeartCS(LogicServer* pLogicServer,boost::shared_ptr<Role> role, byte* data, uint dataSize)
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
	
	callHandler(MSG_TYPE_HEART_SC, pLogicServer, role, data, dataSize);
}

void onClientHeartSC(LogicServer* pLogicServer,boost::shared_ptr<Role> role, byte* data, uint dataSize)
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

void onClientLoginCS(LogicServer* pLogicServer,boost::shared_ptr<Role> role, byte* data, uint dataSize)
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

void onClientLoginSC(LogicServer* pLogicServer,boost::shared_ptr<Role> role, byte* data, uint dataSize)
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

	std::string roleId = CommonTool::genRoleIdByUserName(msg->m_strRoleName);

	if (msg->m_cLoginStatus == MsgLoginSC::LS_LOGIN_OK && msg->m_cErrorReason == MsgLoginSC::ER_NO_ERROR)
	{
		// create role
		RoleManager::CreateRoleInput input;
		RoleLoginInfoParam param;
		memmove(param.m_strRoleId, roleId.data(), sizeof(param.m_strRoleId));
		memmove(param.m_strRoleName, msg->m_strRoleName, sizeof(param.m_strRoleName));
		memmove(param.m_strPassword, msg->m_strPassword, sizeof(param.m_strPassword));

		input.m_nClientSeq = header->m_nClientSrcSeq;
		input.m_param = param;
		ROLE_MGR->createRole(input);
	}
	else if (msg->m_cErrorReason == MsgLoginSC::ER_HAS_LOGIN_ERROR && 
		!ROLE_MGR->isRoleExists(roleId))
	{
		/*
		*	Due to network delay, the role is not online but the loginstatus cache is still there
		*	This time back ER_HAS_LOGIN_ERROR,a second click to log in is required to log in normally
		*/
		DEFINE_BYTE_ARRAY(sendData, sizeof(MsgHeader) + sizeof(MsgLogoutCS));

		MsgLogoutCS cs;
		memmove(cs.m_roleId, roleId.data(), sizeof(cs.m_roleId));

		MsgHeader header;
		header.m_nMsgLen = sizeof(MsgHeader) + sizeof(MsgLogoutCS);
		header.m_nMsgType = MSG_TYPE_LOGOUT_CS;

		memmove(sendData, (const char*)&header, sizeof(MsgHeader));
		memmove(sendData + sizeof(MsgHeader), (const char*)&cs, sizeof(MsgLogoutCS));

		if (pLogicServer)
		{
			pLogicServer->sendToCache(sendData, sizeof(sendData));
		}

		msg->m_cErrorReason = MsgLoginSC::ER_RELOGIN_ERROR;
	}

	// client is not need password
	memset(msg->m_strPassword, 0, sizeof(msg->m_strPassword));

	// send msg to client
	pLogicServer->sendToClient(data, dataSize);
}

void onClientLogoutCS(LogicServer* pLogicServer,boost::shared_ptr<Role> role, byte* data, uint dataSize)
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

	if (role)
	{
		if (!ROLE_MGR->isRoleExists(role->getRoleId()))
		{
			LOG_LOGICSERVER.printLog("role[%s],id[%s] is not exists, remove role fail!", role->getRoleName().data(),role->getRoleId().data());
			return;
		}
		ROLE_MGR->removeRole(role->getRoleId(), msg->m_nErrorCode);
		return;
	}

	if (!ROLE_MGR->tryToDetermineIfExists(header->m_nClientSrcSeq))
	{
		LOG_LOGICSERVER.printLog("clientSeq[%lld] is not exists, remove role fail!", header->m_nClientSrcSeq);
		return;
	}
	ROLE_MGR->tryRemoveRole(header->m_nClientSrcSeq, msg->m_nErrorCode);
}

void onCreateRoleCS(LogicServer* pLogicServer, boost::shared_ptr<Role> role, byte* data, uint dataSize)
{
	LOGIC_REV_MSG_CHECK_MSG_STRUCT(MsgCreateRoleCS);

	callHandler(MSG_TYPE_CREATE_ROLE_SC, pLogicServer, role, data, dataSize);
}

void onCreateRoleSC(LogicServer* pLogicServer, boost::shared_ptr<Role> role, byte* data, uint dataSize)
{
	LOGIC_REV_MSG_CHECK;

	// create my role model
	if (ROLE_MGR)
	{
		ROLE_MGR->createRoleModel(role);
	}
}

void onRoleInfoUpdateCS(LogicServer* pLogicServer, boost::shared_ptr<Role> role, byte* data, uint dataSize)
{
	LOGIC_REV_MSG_CHECK_MSG_STRUCT(MsgRoleInfoUpdateCS);

	if (role)
	{
		role->updateInfoWhenRoleOperation(msg);
	}

	callHandler(MSG_TYPE_ROLE_INFO_UPDATE_SC, pLogicServer, role, data, dataSize);
}

void onRoleInfoUpdateSC(LogicServer* pLogicServer, boost::shared_ptr<Role> role, byte* data, uint dataSize)
{
	LOGIC_REV_MSG_CHECK;

	if (ROLE_MGR)
	{
		ROLE_MGR->notifyRoleInfoChange(role);
	}
}


// Non-handler jump part
HandlerFunc g_handlerList[EnMsgType::MSG_TYPE_CLIENT_SIZE] =
{
	onClientHeartCS,		// 150
	onClientHeartSC,		// 151
	onClientLoginCS,		// 152
	onClientLoginSC,		// 153
	onClientLogoutCS,		// 154
	onCreateRoleCS,			// 155
	onCreateRoleSC,			// 156
	onRoleInfoUpdateCS,		// 157
	onRoleInfoUpdateSC,		// 158
};

void callHandler(int msgType, LogicServer* pLogicServer,boost::shared_ptr<Role> role, byte* data, uint dataSize)
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

	g_handlerList[msgType - MSG_TYPE_CLIENT_START](pLogicServer, role, data, dataSize);
}

}