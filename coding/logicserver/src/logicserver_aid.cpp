#include "logicserver_aid.h"
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
	SingleToProxyMsgHandler::callHandler(
	MSG_TYPE_GATE_PROXY_HEART_LP,
	(const byte*)m_pLogicServer,
	NULL,
	0);

}
