#include "logicserver.h"
#include "msghandler/logicmsghandler.h"

#include <configmodule/configmanager.h>
#include <configmodule/proxyserverconfig/proxyserverconfig.h>
#include <logmodule/logdef.h>
#include <msgmodule/msgcommondef.h>
#include <msgmodule/singletoproxymsghandler.h>

#define LOGIC_SERVER_READ_MSG_CONTINUE \
	m_nHasReadProxyDataSize += m_msgHeader.m_nMsgLen; \
	continue

LogicServer::LogicServer()
	: m_bConnectProxySrv(false)
	, m_bInnerRunOnce(false)
	, m_nHasReadProxyDataSize(0)
{
	const ProxyServerConfigInfo info = *(CONFIG_MGR->GetProxyServerConfig()->getConfigInfo());
	m_innerSrvHeart.setLogicServer(this);
	m_innerSrvHeart.setInterval(info.heart_time);
	memset(m_bytesInnerSrvBuffer, 0, MsgBuffer::g_nReadBufferSize);

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

void LogicServer::onProxySrvRead(const CommonBoost::ErrorCode& ec, uint readSize)
{
	if (ec)
	{
		LOG_LOGICSERVER.printLog("ecode[%d],messages[%s]",
			ec.value(),
			ec.message().data());
		connectInnerServer();
		return;
	}
	if (readSize <= 0 || readSize > MsgBuffer::g_nReadBufferSize)
	{
		LOG_LOGICSERVER.printLog("size error,readSize[%d],g_nReadBufferSize[%d]", readSize, MsgBuffer::g_nReadBufferSize);
		return;
	}

	MsgHeader m_msgHeader;
	m_nHasReadProxyDataSize = 0;

	while (m_nHasReadProxyDataSize < readSize)
	{
		memset(m_bytesInnerSrvOnceMsg, 0, sizeof(m_bytesInnerSrvOnceMsg));

		m_msgHeader = *(MsgHeader*)(m_bytesInnerSrvBuffer + m_nHasReadProxyDataSize);
		// 一条协议最大长度判断
		if (m_msgHeader.m_nMsgLen > MsgBuffer::g_nOnceMsgSize ||
			m_msgHeader.m_nMsgLen <= 0)
		{
			LOG_LOGICSERVER.printLog("msgtype[%d] size[%d] error, read buff[%s]",
				m_msgHeader.m_nMsgType,
				m_msgHeader.m_nMsgLen,
				m_bytesInnerSrvBuffer);
			break;
		}
		memmove(m_bytesInnerSrvOnceMsg, m_bytesInnerSrvBuffer + m_nHasReadProxyDataSize, m_msgHeader.m_nMsgLen);

		if (m_msgHeader.m_nProxyer != MsgHeader::F_PROXYSERVER ||
			m_msgHeader.m_nReceiver != MsgHeader::F_LOGICSERVER
			)
		{
			LOG_LOGICSERVER.printLog("MsgHeader Info Error");
			LOGIC_SERVER_READ_MSG_CONTINUE;
		}

		// 如果此条协议是只和网关通信的
		if (m_msgHeader.m_nMsgType >= MSG_TYPE_GATE_PROXY_HEART_GP &&
			m_msgHeader.m_nMsgType < MSG_IN_TYPE_MAX
			)
		{
			SingleToProxyMsgHandler::callHandler(
				m_msgHeader.m_nMsgType,
				(const byte*)this,
				m_bytesInnerSrvOnceMsg + sizeof(MsgHeader),
				m_msgHeader.m_nMsgLen - sizeof(MsgHeader));
			LOGIC_SERVER_READ_MSG_CONTINUE;
		}
		// 和逻辑服通信的
		else if (m_msgHeader.m_nMsgType >= MSG_TYPE_CLIENT_START &&
			m_msgHeader.m_nMsgType < MSG_CODE_MAX)
		{
			LogicMsgHandler::callHandler(
				m_msgHeader.m_nMsgType,
				this,
				m_bytesInnerSrvOnceMsg + sizeof(MsgHeader),
				m_msgHeader.m_nMsgLen - sizeof(MsgHeader)
			);
			LOGIC_SERVER_READ_MSG_CONTINUE;
		}

		

		m_nHasReadProxyDataSize += m_msgHeader.m_nMsgLen;
	}


	readFromProxySrv();
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

void LogicServer::readFromProxySrv()
{
	if (!m_pInnerSocket)
	{
		LOG_LOGICSERVER.printLog("m_pInnerSocket == NULL");
		return;
	}

	memset(m_bytesInnerSrvBuffer, 0, sizeof(m_bytesInnerSrvBuffer));
	m_pInnerSocket->async_read_some(
		MSG_BUFFER(m_bytesInnerSrvBuffer, sizeof(m_bytesInnerSrvBuffer)),
		BIND(&LogicServer::onProxySrvRead, this, boost::placeholders::_1, boost::placeholders::_2)
	);
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

	readFromProxySrv();
}

