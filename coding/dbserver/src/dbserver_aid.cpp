#include "dbserver_aid.h"
#include "dbserver.h"

#include <logmodule/logdef.h>
#include <msgmodule/singletoproxymsghandler.h>
#include <msgmodule/msgcommondef.h>

TimerDBProxySrvHeart::TimerDBProxySrvHeart()
	: m_pDBServer(NULL)
{
}

TimerDBProxySrvHeart::~TimerDBProxySrvHeart()
{
}

void TimerDBProxySrvHeart::setDBServer(DBServer* pDBServer)
{
	m_pDBServer = pDBServer;
}

void TimerDBProxySrvHeart::timeoutRun()
{
	if (!m_pDBServer)
	{
		LOG_DBSERVER.printLog("m_pDBServer NULL");
		return;
	}
	SingleToProxyMsgHandler::callHandler(
		MSG_TYPE_DB_PROXY_HEART_DP,
	(const byte*)m_pDBServer,
	NULL,
	0);
	m_pDBServer->sendToProxySrv(SingleToProxyMsgHandler::g_DBSendProxy, sizeof(SingleToProxyMsgHandler::g_DBSendProxy));
}
