#include "logicserver.h"

#include <configmodule/configmanager.h>
#include <configmodule/proxyserverconfig/proxyserverconfig.h>
#include <logmodule/logdef.h>
#include <msgmodule/msgcommondef.h>

LogicServer::LogicServer()
	: m_bConnectProxySrv(false)
	, m_bInnerRunOnce(false)
{
	const ProxyServerConfigInfo info = *(CONFIG_MGR->GetProxyServerConfig()->getConfigInfo());
	m_innerSrvHeart.setLogicServer(this);
	m_innerSrvHeart.setInterval(info.heart_time);

	initInnerClient();
}

LogicServer::~LogicServer()
{
	closeInnerSocket();
}

void LogicServer::start()
{
	boost::thread tConnect(BIND(&LogicServer::onRunInnnerIOServerOnce, this));
	tConnect.detach();
	while (1);
}

void LogicServer::sendToProxySrv(const byte* data, uint size)
{
	if (!m_pInnerSocket)
	{
		LOG_LOGICSERVER.printLog("m_pInnerSocket NULL");
		return;
	}

	m_pInnerSocket->async_write_some(
		MSG_BUFFER(data, size),
		BIND(&LogicServer::onProxySrvSend, this, boost::placeholders::_1, boost::placeholders::_2)
	);
}

void LogicServer::onRunInnnerIOServerOnce()
{
	if (!m_bInnerRunOnce)
	{
		CommonBoost::WorkPtr work(new CommonBoost::IOServer::work(m_innerServer));
		m_bInnerRunOnce = true;
		while (1)
		{
			try
			{
				m_innerServer.run();
				break;
			} catch (std::exception& e)
			{
				LOG_LOGICSERVER.printLog("m_innerServer run exception!! info[%s] server will re-start!!", e.what());
			}
		}
	}
}

void LogicServer::onProxySrvSend(const CommonBoost::ErrorCode& ec, uint readSize)
{
	if (ec)
	{
		LOG_LOGICSERVER.printLog("error value[%d],send size[%d], message[%s]",
			ec.value(),
			readSize,
			ec.message().data());

		// 异常处理
	}
}

void LogicServer::initInnerClient()
{
	if (!CONFIG_MGR->GetProxyServerConfig())
	{
		return;
	}
	const ProxyServerConfigInfo info = *(CONFIG_MGR->GetProxyServerConfig()->getConfigInfo());

	m_pInnerSocket = boost::make_shared<CommonBoost::Socket>(m_innerServer);
	m_innerEndpoint = CommonBoost::Endpoint(
		boost::asio::ip::address::from_string(info.ip), info.port
	);
	connectInnerServer();
}

void LogicServer::connectInnerServer()
{
	if (!m_pInnerSocket)
	{
		LOG_LOGICSERVER.printLog("m_pInnerSocket is NULL");
		return;
	}

	if (m_bConnectProxySrv)
	{
		closeInnerSocket();
	}
	m_bConnectProxySrv = false;
	m_pInnerSocket->async_connect(m_innerEndpoint, BIND(&LogicServer::onConnectInnerServer, this, boost::placeholders::_1));

}

void LogicServer::closeInnerSocket()
{
	if (!m_pInnerSocket)
	{
		LOG_LOGICSERVER.printLog("m_pInnerSocket is NULL");
		return;
	}
	m_pInnerSocket->close();
}

void LogicServer::onConnectInnerServer(const CommonBoost::ErrorCode& err)
{
	if (err)
	{
		LOG_LOGICSERVER.printLog("Please run ProxyServer first.......");
		printf_color(PRINTF_RED, "\nPlease run ProxyServer first.......\n");
		return;
	}

	if (m_pInnerSocket.get() == NULL)
	{
		LOG_LOGICSERVER.printLog("m_pInnerSocket is NULL");
		return;
	}

	LOG_LOGICSERVER.printLog("link proxy server succ");
	printf_color(PRINTF_GREEN, "\nlink proxy server succ\n");
	m_bConnectProxySrv = true;

	m_innerSrvHeart.start();

	// 发送一个字节，告诉代理服自己的身份
	DEFINE_BYTE_ARRAY(firstData, 1);
	firstData[0] = MsgHeader::F_LOGICSERVER;
	sendToProxySrv(firstData, 1);
}

