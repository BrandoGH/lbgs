#include "gateserver.h"
#include "config/config.h"

#include <configmodule/configmanager.h>
#include <servercommon/logmodule/logdef.h>
#include <servercommon/sysinfomodule/sysinfo.h>

namespace 
{
	const int g_nConnectMaxCount = 
		int(SystemInfo::getAvailableMemory(SystemInfo::UNIT_B) / UserBuffer::g_nReadBufferSize * 1.0); // 单台服务器最大连接数
}

using CommonBoost::Endpoint;
using CommonBoost::TCP;
GateServer::GateServer()
{
	initData();
	if (CONFIG_MGR->GetGateServerConfig() &&
		(m_pCfgInfo = const_cast<ConfigInfo*>(CONFIG_MGR->GetGateServerConfig()->getConfigInfo()))
		)
	{
		m_nPort = m_pCfgInfo->port;
		m_pAcceptor = new CommonBoost::Acceptor(
			m_server,
			CommonBoost::Endpoint(CommonBoost::TCP::v4(), m_nPort));
		accept();
		LOG_GATESERVER.printLog("run GateServer()");
	}

}

GateServer::GateServer(int port)
{
	initData();
	m_nPort = port;
	m_pAcceptor = new CommonBoost::Acceptor(
		m_server,
		CommonBoost::Endpoint(CommonBoost::TCP::v4(), m_nPort));
	accept();
	LOG_GATESERVER.printLog("run GateServer()");
}

GateServer::~GateServer()
{
	if (m_pAcceptor)
	{
		delete m_pAcceptor;
		m_pAcceptor = NULL;
	}
	LOG_GATESERVER.printLog("run ~GateServer()");

}

void GateServer::start()
{
	LOG_GATESERVER.printLog("gateserver has start,run sub thread count[%d]", CPU_MAX_THREAD - 1);
	printf("----------------------gateserver has start,port[%d],max link count[%d]----------------------\n",
		m_nPort, 
		g_nConnectMaxCount
	);
	for (int i = 0; i < CPU_MAX_THREAD - 1; ++i)
	{
		boost::thread tAccServer(BIND(&GateServer::onThreadRunAcceptorIOServer, this));
		tAccServer.detach();
	}
	while (1);
}

void GateServer::accept()
{
	if (!m_pAcceptor)
	{
		LOG_GATESERVER.printLog("m_pAcceptor is NULL");
		return;
	}

	if (m_nConnectCount > g_nConnectMaxCount)
	{
		LOG_GATESERVER.printLog("connect size has over [%d]", g_nConnectMaxCount);
		printf("connect size has over [%d]\n", g_nConnectMaxCount);
		return;
	}

	boost::shared_ptr<User> newUser = boost::make_shared<User>(m_server);
	m_pAcceptor->async_accept(*(newUser->getSocket()), BIND(&GateServer::onAcceptHandler, this, boost::placeholders::_1, newUser));

}

void GateServer::initData()
{
	m_pAcceptor = NULL;
	m_pCfgInfo = NULL;
	m_nConnectCount = 1;
	m_nPort = 0;
}

void GateServer::onThreadRunAcceptorIOServer()
{
	LOG_GATESERVER.printLog("server has run");
	CommonBoost::WorkPtr work(new CommonBoost::IOServer::work(m_server));
	m_server.run();
}

void GateServer::onAcceptHandler(
	const CommonBoost::ErrorCode& err,
	const boost::shared_ptr<User>& user
	)
{
	if (err)
	{
		LOG_GATESERVER.printLog("new client connect error value[%d],message[%s]", err.value(), err.message().data());
		return;
	}
	if (!user)
	{
		LOG_GATESERVER.printLog("linking user is NULL");
		return;
	}

	LOG_GATESERVER.printLog("new client connect succ,count[%d]", m_nConnectCount);
	++m_nConnectCount;

	user->ayncRead();
	accept();
}


