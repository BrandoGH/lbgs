#include "logicmsghandler.h"
#include "src/logicserver.h"
#include "src/globallogicserver.h"
#include "communicationmsg/msgheart.h"
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

	if (msg->m_cLoginFlag == MsgLoginCS::LF_LOGIN)
	{
		// try get redis value
		pLogicServer->sendToCache(data, dataSize);
	}
	else if (msg->m_cLoginFlag == MsgLoginCS::LF_REGISTER)
	{
		// TODO register role
	}
	
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

	pLogicServer->sendToClient(data, dataSize);
}


// Non-handler jump part
HandlerFunc g_handlerList[EnMsgType::MSG_TYPE_CLIENT_SIZE] =
{
	onClientHeartCS,
	onClientHeartSC,
	onClientLoginCS,
	onClientLoginSC,
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