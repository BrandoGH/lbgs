#include "logicserver_aid.h"
#include <logmodule/logdef.h>

TimerProxySrvHeart::TimerProxySrvHeart()
	: m_pLogicServer(NULL)
{
}

TimerProxySrvHeart::~TimerProxySrvHeart()
{
}

void TimerProxySrvHeart::setLogicServer(LogicServer* pLogicServer)
{
	m_pLogicServer = pLogicServer;
}

void TimerProxySrvHeart::timeoutRun()
{
}
