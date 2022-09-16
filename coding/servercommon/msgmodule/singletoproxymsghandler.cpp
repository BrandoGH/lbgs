#include "singletoproxymsghandler.h"
#include "logmodule/logdef.h"
#include "commontool/msgtool/msgtool.h"

#include <proxyserver/src/serverlinker.h>

#include <gateserver/src/gateserver.h>

#define MSG_SC_CODE_MODE(log_obj)													\
	if (!objServer || !data)														\
	{																				\
		log_obj.printLog("call handler error");										\
		return;																		\
	}																				\
	MsgInHeartSC* msg = (MsgInHeartSC*)data;										\
	if (!msg || !CommonTool::MsgTool::isBytesDataEQ(msg->m_bytesHeart,				\
		(const byte*)I_MSG_HEART_SC, sizeof(msg->m_bytesHeart)))					\
	{																				\
		log_obj.printLog("msg data error");											\
		return;																		\
	}

namespace
{
// 发送心跳包
template<int len>
void sendProxyHeartInfo(byte(&targetInfoData)[len], ushort msgType, ushort sender)
{ 
	MsgInHeartCS msg;
	memmove(msg.m_bytesHeart, I_MSG_HEART_CS, sizeof(msg.m_bytesHeart));

	MsgHeader header;
	header.m_nMsgLen = sizeof(MsgHeader) + sizeof(msg.m_bytesHeart);
	header.m_nMsgType = msgType;
	header.m_nSender = sender;
	header.m_nReceiver = MsgHeader::F_PROXYSERVER;
	header.m_nProxyer = MsgHeader::F_PROXYSERVER;

	memset(targetInfoData, 0, sizeof(MsgHeader) + sizeof(MsgInHeartCS));
	memmove(targetInfoData, (const char*)&header, sizeof(MsgHeader));
	memmove(targetInfoData + sizeof(MsgHeader), (const char*)&msg, sizeof(MsgInHeartCS));
}

}

namespace SingleToProxyMsgHandler
{
byte g_GateSendProxy[sizeof(MsgHeader) + sizeof(MsgInHeartCS)];
byte g_LogicSendProxy[sizeof(MsgHeader) + sizeof(MsgInHeartCS)];
byte g_CacheSendProxy[sizeof(MsgHeader) + sizeof(MsgInHeartCS)];

void onHandlerGPHeartCS(const byte* objServer, byte* data, uint dataSize)
{
	sendProxyHeartInfo(g_GateSendProxy, MSG_TYPE_GATE_PROXY_HEART_GP, MsgHeader::F_GATESERVER);
}

void onHandlerPGHeartSC(const byte* objServer, byte* data, uint dataSize)
{
	MSG_SC_CODE_MODE(LOG_GATESERVER)
}

void onHandlerLPHeartCS(const byte* objServer, byte* data, uint dataSize)
{
	sendProxyHeartInfo(g_LogicSendProxy, MSG_TYPE_LOGIC_PROXY_HEART_LP, MsgHeader::F_LOGICSERVER);
}

void onHandlerPLHeartSC(const byte* objServer, byte* data, uint dataSize)
{
	MSG_SC_CODE_MODE(LOG_LOGICSERVER)
}

void onHandlerCPHeartCS(const byte* objServer, byte* data, uint dataSize)
{
	sendProxyHeartInfo(g_CacheSendProxy, MSG_TYPE_CACHE_PROXY_HEART_CP, MsgHeader::F_CACHESERVER);

	printf_color(PRINTF_YELLOW, "cache send to proxy\n");
}

void onHandlerPCHeartSC(const byte* objServer, byte* data, uint dataSize)
{
	MSG_SC_CODE_MODE(LOG_CACHESERVER)

	printf_color(PRINTF_YELLOW, "proxy send to cache\n");

}

// 非handler跳转部分
HandlerFunc g_handlerList[EnMsgType::MSG_IN_TYPE_MAX] =
{
	onHandlerGPHeartCS,
	onHandlerPGHeartSC,
	onHandlerLPHeartCS,
	onHandlerPLHeartSC,
	onHandlerCPHeartCS,
	onHandlerPCHeartSC,
};

void callHandler(int msgType, const byte* objServer, byte* data, uint dataSize)
{
	if (!objServer)
	{
		LOG_GATESERVER.printLog("objServer NULL");
		return;
	}

	if (msgType < MSG_TYPE_GATE_PROXY_HEART_GP || msgType >= MSG_IN_TYPE_MAX)
	{
		LOG_GATESERVER.printLog("msgtype error");
		return;
	}

	if (g_handlerList[msgType] == NULL)
	{
		LOG_GATESERVER.printLog("g_handlerList[%d] == NULL", msgType);
		return;
	}

	g_handlerList[msgType](objServer, data, dataSize);
}

}