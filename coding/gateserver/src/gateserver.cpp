#include "gateserver.h"
#include "config/config.h"

#include <configmodule/configmanager.h>
#include <servercommon/logmodule/logdef.h>
#include <servercommon/sysinfomodule/sysinfo.h>

namespace 
{
	// 单台服务器最大连接数可用内存的最大连接数的3/4
	const int g_nConnectMaxCount = 
		int(SystemInfo::getAvailableMemory(SystemInfo::UNIT_B) / UserBuffer::g_nReadBufferSize * 1.0) * 3 / 4; 
}

using CommonBoost::Endpoint;
using CommonBoost::TCP;
GateServer::GateServer()

{
	initData();
	const ConfigInfo* pCfgInfo = CONFIG_MGR->GetGateServerConfig()->getConfigInfo();
	if (CONFIG_MGR->GetGateServerConfig() && pCfgInfo)
	{
		m_nPort = pCfgInfo->port;
		m_pAcceptor = new CommonBoost::Acceptor(
			m_server,
			CommonBoost::Endpoint(CommonBoost::TCP::v4(), m_nPort));
		accept();
		LOG_GATESERVER.printLog("has run gateserver succ");
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
	LOG_GATESERVER.printLog("has run gateserver succ");
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
	LOG_GATESERVER.printLog("gateserver has start,port[%d],max link count[%d]",
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
	newUser->slotConnect(this);
	m_pAcceptor->async_accept(*(newUser->getSocket()), BIND(&GateServer::onAcceptHandler, this, boost::placeholders::_1, newUser));
}

void GateServer::initData()
{
	m_pAcceptor = NULL;
	m_nConnectCount = 0;
	m_nPort = 0;
}

void GateServer::onUserError(const std::string& ip, ushort port)
{
	--m_nConnectCount;
	LOG_GATESERVER.printLog("client[%s : %d] closed,current connect[%d]",
		ip.data(), port, m_nConnectCount.load()
		);

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

	user->ayncRead();

	++m_nConnectCount;
	LOG_GATESERVER.printLog("new client[%s : %d] connect succ, client has link count[%d]",
		user->getLinkIP().data(),
		user->getLinkPort(),
		m_nConnectCount.load());

	accept();
}


