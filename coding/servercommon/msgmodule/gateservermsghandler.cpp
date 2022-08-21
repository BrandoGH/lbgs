#include "proxyservermsghandler.h"
#include "logmodule/logdef.h"
#include "commontool/msgtool/msgtool.h"

#include <proxyserver/src/serverlinker.h>
#include <gateserver/gateservermsg/gateservermsg.h>
#include "gateservermsghandler.h"

namespace GateServerMsgHandler
{
void onHandlerHeartSC(const GateServer* gateServer, byte* data, uint dataSize)
{
	if (!gateServer || !data)
	{
		LOG_GATESERVER.printLog("call handler error");
	}
	MsgInHeartSC* msg = (MsgInHeartSC*)data;
	if (!msg || !CommonTool::MsgTool::isBytesDataEQ(msg->m_bytesHeart, (const byte*)"\x53\x47\x42\x4C", sizeof(msg->m_bytesHeart)))
	{
		LOG_PROXYSERVER.printLog("msg data error");
		return;
	}
}

void onHandlerHeartCS(const GateServer* gateServer, byte* data, uint dataSize)
{
	// 暂无实现
}


// 非handler跳转部分
HandlerFunc g_handlerList[EnMsgType::MSG_IN_TYPE_MAX] =
{
	onHandlerHeartCS,
	onHandlerHeartSC,
};

void callHandler(int msgType, const GateServer* gateServer, byte* data, uint dataSize)
{
	if (!gateServer)
	{
		LOG_GATESERVER.printLog("gateServer NULL");
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

	g_handlerList[msgType](gateServer, data, dataSize);

}

}