#include "logicserver_aid.h"
#include "logicserver.h"

#include <logmodule/logdef.h>
#include <msgmodule/singletoproxymsghandler.h>
#include <msgmodule/msgcommondef.h>

TimerLogicProxySrvHeart::TimerLogicProxySrvHeart()
	: m_pLogicServer(NULL)
{
}

TimerLogicProxySrvHeart::~TimerLogicProxySrvHeart()
{
}

void TimerLogicProxySrvHeart::setLogicServer(LogicServer* pLogicServer)
{
	m_pLogicServer = pLogicServer;
}

void TimerLogicProxySrvHeart::timeoutRun()
{
	if (!m_pLogicServer)
	{
		LOG_LOGICSERVER.printLog("m_pLogicServer NULL");
		return;
	}
	SingleToProxyMsgHandler::callHandler(
	MSG_TYPE_GATE_PROXY_HEART_LP,
	(const byte*)m_pLogicServer,
	NULL,
	0);
	m_pLogicServer->sendToProxySrv(SingleToProxyMsgHandler::g_LogicSendProxy, sizeof(SingleToProxyMsgHandler::g_LogicSendProxy));
}
