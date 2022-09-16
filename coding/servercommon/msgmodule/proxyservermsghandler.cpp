#include "proxyservermsghandler.h"
#include "logmodule/logdef.h"
#include "commontool/msgtool/msgtool.h"
#include "innerserverheartmsg.h"

#include <proxyserver/src/serverlinker.h>

#define PROXY_CHECK_SUB_SERVER_HEART											\
	MsgInHeartCS* msg = (MsgInHeartCS*)data;									\
	if (!msg || !CommonTool::MsgTool::isBytesDataEQ(msg->m_bytesHeart,			\
		(const byte*)I_MSG_HEART_CS, sizeof(msg->m_bytesHeart)))				\
	{																			\
		LOG_PROXYSERVER.printLog("msg data error");								\
		return;																	\
	}

namespace ProxyServerMsgHandler
{

void onHandlerGateHeartCS(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize)
{
	PROXY_CHECK_SUB_SERVER_HEART

	callHandler(MSG_TYPE_GATE_PROXY_HEART_PG, linker, data, dataSize);
}

void onHandlerGateHeartSC(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize)
{
	MsgInHeartSC sendMsg;
	memmove(sendMsg.m_bytesHeart, I_MSG_HEART_SC, sizeof(sendMsg.m_bytesHeart));

	MsgHeader header;
	header.m_nMsgLen = sizeof(MsgHeader) + sizeof(sendMsg.m_bytesHeart);
	header.m_nMsgType = MSG_TYPE_GATE_PROXY_HEART_PG;
	header.m_nSender = MsgHeader::F_PROXYSERVER;
	header.m_nReceiver = MsgHeader::F_GATESERVER;
	header.m_nProxyer = MsgHeader::F_PROXYSERVER;

	DEFINE_BYTE_ARRAY(sendInfo, sizeof(MsgHeader) + sizeof(sendMsg.m_bytesHeart));
	memmove(sendInfo, (const char*)&header, sizeof(MsgHeader));
	memmove(sendInfo + sizeof(MsgHeader), (const char*)&sendMsg, sizeof(MsgInHeartCS));

	if (linker.get())
	{
		linker->ayncSend(sendInfo, sizeof(sendInfo));
	}
}

void onHandlerLogicHeartCS(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize)
{
	PROXY_CHECK_SUB_SERVER_HEART

	callHandler(MSG_TYPE_LOGIC_PROXY_HEART_PL, linker, data, dataSize);
}

void onHandlerLogicHeartSC(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize)
{
	MsgInHeartSC sendMsg;
	memmove(sendMsg.m_bytesHeart, I_MSG_HEART_SC, sizeof(sendMsg.m_bytesHeart));

	MsgHeader header;
	header.m_nMsgLen = sizeof(MsgHeader) + sizeof(sendMsg.m_bytesHeart);
	header.m_nMsgType = MSG_TYPE_LOGIC_PROXY_HEART_PL;
	header.m_nSender = MsgHeader::F_PROXYSERVER;
	header.m_nReceiver = MsgHeader::F_LOGICSERVER;
	header.m_nProxyer = MsgHeader::F_PROXYSERVER;

	DEFINE_BYTE_ARRAY(sendInfo, sizeof(MsgHeader) + sizeof(sendMsg.m_bytesHeart));
	memmove(sendInfo, (const char*)&header, sizeof(MsgHeader));
	memmove(sendInfo + sizeof(MsgHeader), (const char*)&sendMsg, sizeof(MsgInHeartCS));

	if (linker.get())
	{
		linker->ayncSend(sendInfo, sizeof(sendInfo));
	}
}

void onHandlerCacheHeartCS(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize)
{
	PROXY_CHECK_SUB_SERVER_HEART

	callHandler(MSG_TYPE_CACHE_PROXY_HEART_PC, linker, data, dataSize);

}

void onHandlerCacheHeartSC(const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize)
{
	MsgInHeartSC sendMsg;
	memmove(sendMsg.m_bytesHeart, I_MSG_HEART_SC, sizeof(sendMsg.m_bytesHeart));

	MsgHeader header;
	header.m_nMsgLen = sizeof(MsgHeader) + sizeof(sendMsg.m_bytesHeart);
	header.m_nMsgType = MSG_TYPE_CACHE_PROXY_HEART_PC;
	header.m_nSender = MsgHeader::F_PROXYSERVER;
	header.m_nReceiver = MsgHeader::F_CACHESERVER;
	header.m_nProxyer = MsgHeader::F_PROXYSERVER;

	DEFINE_BYTE_ARRAY(sendInfo, sizeof(MsgHeader) + sizeof(sendMsg.m_bytesHeart));
	memmove(sendInfo, (const char*)&header, sizeof(MsgHeader));
	memmove(sendInfo + sizeof(MsgHeader), (const char*)&sendMsg, sizeof(MsgInHeartCS));

	if (linker.get())
	{
		linker->ayncSend(sendInfo, sizeof(sendInfo));
	}
}


// 非handler跳转部分
HandlerFunc g_handlerList[EnMsgType::MSG_IN_TYPE_MAX] =
{
	onHandlerGateHeartCS,
	onHandlerGateHeartSC,
	onHandlerLogicHeartCS,
	onHandlerLogicHeartSC,
	onHandlerCacheHeartCS,
	onHandlerCacheHeartSC,
};

void callHandler(int msgType, const boost::shared_ptr<ServerLinker>& linker, byte* data, uint dataSize)
{
	if (!linker.get())
	{
		LOG_PROXYSERVER.printLog("linker NULL");
		return;
	}

	if(msgType < MSG_TYPE_GATE_PROXY_HEART_GP || msgType >= MSG_IN_TYPE_MAX)
	{
		LOG_PROXYSERVER.printLog("msgtype error");
		return;
	}

	if(g_handlerList[msgType] == NULL)
	{
		LOG_PROXYSERVER.printLog("g_handlerList[%d] == NULL", msgType);
		return;
	}

	g_handlerList[msgType](linker, data, dataSize);
}


}