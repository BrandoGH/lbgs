#include "proxyserver.h"

#include <configmodule/proxyserverconfig/proxyserverconfig.h>
#include <servercommon/logmodule/logdef.h>
#include <servercommon/configmodule/configmanager.h>


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
	while(1);
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
		LOG_PROXYSERVER.printLog("m_pAcceptor is NULL");		// 这里都能跑到，还写什么代码，不如去搬砖
		return;
	}

	boost::shared_ptr<ServerLinker> linker = boost::make_shared<ServerLinker>(m_server);
	if(linker->getSocket().get() == NULL)
	{
		LOG_PROXYSERVER.printLog("linker->getSocket().get() == NULL");			// 都跑到这里了，服务器是不是有问题
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
      break;
		}
		catch(std::exception& e)
		{
			LOG_PROXYSERVER.printLog("m_server run exception!! info[%s] server will re-start!!", e.what());
		}
	}
}

void ProxyServer::onLinkerFirstConnect(boost::shared_ptr<ServerLinker> linker, int listIndex)
{
	if (!linker.get() || 
		(listIndex <= MsgHeader::F_DEFAULT || listIndex >= MsgHeader::F_MAX))
	{
		LOG_PROXYSERVER.printLog("error");
		return;
	}

	CommonBoost::UniqueLock lock(m_mtxLinkerList);
	m_linkerList[listIndex] = linker;
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
		return;
	}
	pMsgHeader->m_nSender = MsgHeader::F_PROXYSERVER;
	pMsgHeader->m_nProxyer = MsgHeader::F_PROXYSERVER;
	pMsgHeader->m_nReceiver = listIndex;

	// 其他操作 TODO
	lock.lock();
	m_linkerList[listIndex]->ayncSend(data, dataSize);
}

void ProxyServer::onLinkerError(
	boost::shared_ptr<ServerLinker> linker,
	const CommonBoost::ErrorCode& ec)
{
	bool bLinkerValid = false;
	if(linker.get() != NULL)
	{
		bLinkerValid = true;
	}
	
	// 服务器正常退出
	if(ec.value() == ProxyServer::LOGOUT)
	{
		LOG_PROXYSERVER.printLog("once server has logout");
		linker->closeSocket();
		return;
	}
	else
	{
		// 其他错误
		if(bLinkerValid)
		{
			LOG_PROXYSERVER.printLog("other error! ecode[%d],messages[%s]",
				ec.value(),
				ec.message().data());
			linker->closeSocket();
		}
		else
		{
			LOG_PROXYSERVER.printLog("ecode[%d],messages[%s]", ec.value(), ec.message().data());
		}

	}

}

void ProxyServer::onAcceptHandler(
	const CommonBoost::ErrorCode& err,
	const boost::shared_ptr<ServerLinker>& linker
)
{
	if(err)
	{
		LOG_PROXYSERVER.printLog("connect error value[%d],message[%s]", err.value(), err.message().data());
		accept();
		return;
	}
	if(!linker)
	{
		LOG_PROXYSERVER.printLog("linking linker is NULL");
		accept();
		return;
	}

	linker->ayncRead(true);
	accept();
}