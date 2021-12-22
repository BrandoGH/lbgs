#include "gateserver.h"
#include "config/config.h"

#include <configmodule/configmanager.h>
#include <servercommon/logmodule/logdef.h>
#include <servercommon/sysinfomodule/sysinfo.h>
#include <exception>

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
	if (newUser->getSocket().get() == NULL)
	{
		LOG_GATESERVER.printLog("newUser->getSocket().get() == NULL");
		return;
	}
	m_pAcceptor->async_accept(*(newUser->getSocket()), BIND(&GateServer::onAcceptHandler, this, boost::placeholders::_1, newUser));
}

void GateServer::initData()
{
	m_pAcceptor = NULL;
	m_nConnectCount = 0;
	m_nPort = 0;
}

void GateServer::onUserError(
	boost::shared_ptr<User> user,
	const CommonBoost::ErrorCode& ec)
{
	--m_nConnectCount;
	LOG_GATESERVER.printLog("current connect count: [%d]", m_nConnectCount.load());

	std::string getIp;
	ushort getPort = 0;

	bool bUserValid = false;
	if (user.get() != NULL)
	{
		bUserValid = true;
		user->getLinkIP(getIp);
		user->getLinkPort(getPort);
	}

	// 客户端正常关闭
	if (ec.value() == GateServer::LOGOUT)
	{
		LOG_GATESERVER.printLog("client[%s : %d] closed",
			getIp.data(), 
			getPort);
 		user->closeSocket();
		return;
	}
	else
	{
		// 其他错误
		if (bUserValid)
		{
			LOG_GATESERVER.printLog("client[%s : %d] error! ecode[%d],messages[%s]",
				getIp.data(), 
				getPort, 
				ec.value(),
				ec.message().data());
		}
		else
		{
			LOG_GATESERVER.printLog("ecode[%d],messages[%s]", ec.value(),ec.message().data());
		}

	}

}

void GateServer::onThreadRunAcceptorIOServer()
{
	LOG_GATESERVER.printLog("server has run");
	CommonBoost::WorkPtr work(new CommonBoost::IOServer::work(m_server));
 
  /*
    捕获异常，可能会出现一个错误，这个错误的原因在于客户端建立连接以后一瞬间，服务端调用remote_endpoint前，就断开了链接，导致返回失败
    错误信息如下：
      terminate called after throwing an instance of 'boost::wrapexcept<boost::system::system_error>'
      what():  remote_endpoint: Transport endpoint is not connected
      Aborted
  */
  while(1)
  {
    try
    {
      m_server.run();
    }
     catch (std::exception& e)
    {
      // 如果出现这个情况，建议这台网关服重启,因为有可能会影响到一些数据不正确，比如客户端连接数（当连接数万级以上时）
      LOG_GATESERVER.printLog("m_server run exception!! server will re-start!!");
    }
  }
	
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

	std::string ip;
	ushort port = 0;
	user->getLinkIP(ip);
	user->getLinkPort(port);
	if (!ip.empty() && port != 0)
	{
		LOG_GATESERVER.printLog("new client[%s : %d] connect succ, client has link count[%d]",
			ip.data(),
			port,
			m_nConnectCount.load());
	}
	

	accept();
}


