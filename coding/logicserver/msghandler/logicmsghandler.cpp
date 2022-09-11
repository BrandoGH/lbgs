#include "logicmsghandler.h"
#include "src/logicserver.h"
#include "communicationmsg/msgheart.h"

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
	MsgHeartCS* msg = (MsgHeartCS*)data;
	if (!msg || !CommonTool::MsgTool::isBytesDataEQ(msg->m_bytesHeart, (const byte*)"\x4C\x42\x47\x53", sizeof(msg->m_bytesHeart)))
	{
		LOG_GATESERVER.printLog("msg data error");
		return;
	}
	
	callHandler(MSG_TYPE_HEART_SC, pLogicServer, data, dataSize);
}

void onClientHeartSC(LogicServer* pLogicServer, byte* data, uint dataSize)
{
	if (!pLogicServer)
	{
		LOG_GATESERVER.printLog("pLogicServer NULL");
		return;
	}
	MsgHeartSC msg;
	memmove(msg.m_bytesHeart, "\x53\x47\x42\x4C", sizeof(msg.m_bytesHeart));
	MsgHeader header;
	header.m_nMsgLen = sizeof(MsgHeader) + sizeof(MsgHeartSC);
	header.m_nMsgType = MSG_TYPE_HEART_SC;
	header.m_nSender = MsgHeader::F_LOGICSERVER;
	header.m_nReceiver = MsgHeader::F_GATESERVER;
	header.m_nProxyer = MsgHeader::F_PROXYSERVER;

	DEFINE_BYTE_ARRAY(sendDataArr, sizeof(MsgHeader) + sizeof(MsgHeartSC));
	memset(sendDataArr, 0, sizeof(MsgHeader) + sizeof(MsgHeartSC));
	memmove(sendDataArr, (const char*)&header, sizeof(MsgHeader));
	memmove(sendDataArr + sizeof(MsgHeader), (const char*)&msg, sizeof(MsgHeartSC));
	pLogicServer->sendToProxySrv(sendDataArr, sizeof(MsgHeader) + sizeof(MsgHeartSC));
}


// 非handler跳转部分
HandlerFunc g_handlerList[EnMsgType::MSG_TYPE_CLIENT_SIZE] =
{
	onClientHeartCS,
	onClientHeartSC,
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