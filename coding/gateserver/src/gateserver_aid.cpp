#include "gateserver_aid.h"
#include "gateserver.h"
#include "gateservermsg/gateservermsg.h"

#include <servercommon/logmodule/logdef.h>
#include <msgmodule/singletoproxymsghandler.h>
#include <msgmodule/msgcommondef.h>

TimerGateProxySrvHeart::TimerGateProxySrvHeart()
	: m_pGateServer(NULL)
{
}

TimerGateProxySrvHeart::~TimerGateProxySrvHeart()
{
}

void TimerGateProxySrvHeart::setGateServer(GateServer* gateserver)
{
	m_pGateServer = gateserver;
}

void TimerGateProxySrvHeart::timeoutRun()
{
	if (!m_pGateServer)
	{
		LOG_GATESERVER.printLog("m_pGateServer NULL");
		return;
	}
	SingleToProxyMsgHandler::callHandler(
		MSG_TYPE_GATE_PROXY_HEART_GP,
		(const byte*)m_pGateServer,
		NULL,
		0);
	m_pGateServer->sendToProxySrv(SingleToProxyMsgHandler::g_GateSendProxy, sizeof(SingleToProxyMsgHandler::g_GateSendProxy));
}
