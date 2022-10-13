#include "proxyserver.h"

#include <configmodule/proxyserverconfig/proxyserverconfig.h>
#include <servercommon/logmodule/logdef.h>
#include <servercommon/configmodule/configmanager.h>
#include <servercommon/basedef.h>

ProxyServer::ProxyServer()
{
	initData();
	const ProxyServerConfigInfo* pCfgInfo = CONFIG_MGR->GetProxyServerConfig()->getConfigInfo();
	if(CONFIG_MGR->GetProxyServerConfig() && pCfgInfo)
	{
		m_nPort = pCfgInfo->port;
		m_pAcceptor = new CommonBoost::Acceptor(
			m_server,
			CommonBoost::Endpoint(CommonBoost::TCP::v4(), m_nPort));
		accept();
		LOG_PROXYSERVER.printLog("has run gateserver succ");
	}
}

ProxyServer::ProxyServer(int port)
{
	initData();
	m_nPort = port;
	m_pAcceptor = new CommonBoost::Acceptor(
		m_server,
		CommonBoost::Endpoint(CommonBoost::TCP::v4(), m_nPort));
	accept();
	LOG_PROXYSERVER.printLog("has run gateserver succ");
}

ProxyServer::~ProxyServer()
{
}

void ProxyServer::start()
{
	LOG_PROXYSERVER.printLog("ProxyServer has start,run sub thread count[%d]", CPU_MAX_THREAD);
	LOG_PROXYSERVER.printLog("ProxyServer has start,port[%d]", m_nPort);
	for(int i = 0; i < CPU_MAX_THREAD; ++i)
	{
		boost::thread tAccServer(BIND(&ProxyServer::onThreadRunAcceptorIOServer, this));
		tAccServer.detach();
	}
	printf_color(PRINTF_GREEN, "--ProxyServer start successed!!!!!!!!!!,port[%d]\n", m_nPort);
	while (1) { THREAD_SLEEP(1); }
}

void ProxyServer::initData()
{
	m_pAcceptor = NULL;
	m_nPort = 0;
}

void ProxyServer::accept()
{
	if(!m_pAcceptor)
	{
		LOG_PROXYSERVER.printLog("m_pAcceptor is NULL");		// I've come here, it seems that I'm not suitable for writing code
		return;
	}

	boost::shared_ptr<ServerLinker> linker = boost::make_shared<ServerLinker>(m_server);
	if(linker->getSocket().get() == NULL)
	{
		LOG_PROXYSERVER.printLog("linker->getSocket().get() == NULL");			// I've come here, this server like a shi--
		return;
	}
	linker->slotConnect(this);
	m_pAcceptor->async_accept(*(linker->getSocket()), BIND(&ProxyServer::onAcceptHandler, this, boost::placeholders::_1, linker));
}

void ProxyServer::onThreadRunAcceptorIOServer()
{
	LOG_PROXYSERVER.printLog("server has run");
	CommonBoost::WorkPtr work(new CommonBoost::IOServer::work(m_server));

	/*
	If an exception is caught, an error may occur. The reason for this error is that the connection is disconnected immediately after the client establishes the connection,
	before the server calls remote_endpoint, resulting in a return failure.

	Error info£º
		terminate called after throwing an instance of 'boost::wrapexcept<boost::system::system_error>'
		what():  remote_endpoint: Transport endpoint is not connected
		Aborted
	*/
	while(1)
	{
		THREAD_SLEEP(1);
		try
		{
			m_server.run();
			break;
		}
		catch(std::exception& e)
		{
			LOG_PROXYSERVER.printLog("m_server run exception!! info[%s] server will re-start!!", e.what());
		}
	}
}

void ProxyServer::onLinkerFirstConnect(const boost::weak_ptr<ServerLinker>& linker, int listIndex)
{
	if (linker.expired() || 
		(listIndex <= MsgHeader::F_DEFAULT || listIndex >= MsgHeader::F_MAX))
	{
		LOG_PROXYSERVER.printLog("error");
		return;
	}

	CommonBoost::UniqueLock lock(m_mtxLinkerList);
	m_linkerList[listIndex] = linker.lock();
}

void ProxyServer::onSendToDstServer(int listIndex, const byte* data, uint dataSize)
{
	if ((listIndex <= MsgHeader::F_DEFAULT || listIndex >= MsgHeader::F_MAX) ||
		!data || dataSize == 0)
	{
		LOG_PROXYSERVER.printLog("error");
		return;
	}

	CommonBoost::UniqueLock lock(m_mtxLinkerList);
	if (!m_linkerList[listIndex])
	{
		LOG_PROXYSERVER.printLog("m_linkerList[%d] NULL", listIndex);
		return;
	}
	lock.unlock();

	MsgHeader* pMsgHeader = (MsgHeader*)(data);
	if (!pMsgHeader)
	{
		LOG_PROXYSERVER.printLog("pMsgHeader NULL");
		return;
	}
	
	// Do somthing.....[nothing now]
	lock.lock();
	m_linkerList[listIndex]->ayncSend(data, dataSize);
}

void ProxyServer::onLinkerError(
	const boost::weak_ptr<ServerLinker>& linker,
	const CommonBoost::ErrorCode& ec)
{
	// Client shuts down gracefully
	if(ec.value() == ProxyServer::LOGOUT)
	{
		LOG_PROXYSERVER.printLog("once server has logout");
		return;
	}
	else
	{
		// Ohter error
		if(!linker.expired())
		{
			LOG_PROXYSERVER.printLog("other error! ecode[%d],messages[%s]",
				ec.value(),
				ec.message().data());
		}
		else
		{
			LOG_PROXYSERVER.printLog("ecode[%d],messages[%s]", ec.value(), ec.message().data());
		}

	}

}

void ProxyServer::onAcceptHandler(
	const CommonBoost::ErrorCode& err,
	const boost::weak_ptr<ServerLinker>& linker
)
{
	if(err)
	{
		LOG_PROXYSERVER.printLog("connect error value[%d],message[%s]", err.value(), err.message().data());
		accept();
		return;
	}
	if(linker.expired())
	{
		LOG_PROXYSERVER.printLog("linking linker is NULL");
		accept();
		return;
	}

	boost::shared_ptr<ServerLinker> sLinker = linker.lock();

	sLinker->ayncRead(true);
	accept();
}