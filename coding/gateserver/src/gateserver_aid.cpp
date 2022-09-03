#include "gateserver_aid.h"
#include "gateserver.h"
#include "gateservermsg/gateservermsg.h"

#include <servercommon/logmodule/logdef.h>
#include "msgmodule/gateservermsghandler.h"

TimerProxySrvHeart::TimerProxySrvHeart()
	: m_pGateServer(NULL)
{
}

TimerProxySrvHeart::~TimerProxySrvHeart()
{
}

void TimerProxySrvHeart::setGateServer(GateServer* gateserver)
{
	m_pGateServer = gateserver;
}

void TimerProxySrvHeart::timeoutRun()
{
	GateServerMsgHandler::callHandler(
		MSG_TYPE_GATE_PROXY_HEART_GP,
		m_pGateServer,
		NULL,
		0);
}
