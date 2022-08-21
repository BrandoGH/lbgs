#include "serverlinker.h"
#include "proxyserver.h"

#include <servercommon/logmodule/logdef.h>
#include <servercommon/msgmodule/proxyservermsghandler.h>

namespace
{
#define PROXY_SERVER_READ_MSG_CONTINUE \
	m_nHasReadDataSize += m_msgHeader.m_nMsgLen; \
	continue
}

ServerLinker::ServerLinker(CommonBoost::IOServer& ioserver)
	: m_nHasReadDataSize(0)
{
	m_pSocket = boost::make_shared<CommonBoost::Socket>(ioserver);
	assert(m_pSocket != NULL);
	memset(m_bytesOnceMsg, 0, sizeof(m_bytesOnceMsg));
}

ServerLinker::~ServerLinker()
{
}

CommonBoost::SocketPtr& ServerLinker::getSocket()
{
	return m_pSocket;
}

void ServerLinker::ayncRead()
{
	if(!m_pSocket)
	{
		LOG_PROXYSERVER.printLog("m_pSocket == NULL");
		return;
	}

	memset(m_bytesReadBuffer, 0, sizeof(m_bytesReadBuffer));
	m_pSocket->async_read_some(
		MSG_BUFFER(m_bytesReadBuffer, sizeof(m_bytesReadBuffer)),
		BIND(&ServerLinker::onAyncRead, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2)
	);
}

void ServerLinker::ayncSend(const byte* data, uint size)
{
	if(!m_pSocket)
	{
		LOG_PROXYSERVER.printLog("m_pSocket == NULL");
		return;
	}

	m_pSocket->async_write_some(
		MSG_BUFFER(data, size),
		BIND(&ServerLinker::onAyncSend, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2)
	);
}

int ServerLinker::slotConnect(ProxyServer* proxyServer)
{
	if(!proxyServer)
	{
		LOG_PROXYSERVER.printLog("gateServer == NULL, connect slot error!");
		return CONNECT_ERROR;
	}

	sigError.connect(BIND(
		&ProxyServer::onLinkerError,
		proxyServer,
		boost::placeholders::_1,
		boost::placeholders::_2));

	return CONNECT_OK;
}

void ServerLinker::onAyncSend(const CommonBoost::ErrorCode& ec, uint readSize)
{
	if(ec)
	{
		LOG_PROXYSERVER.printLog("error value[%d],send size[%d], message[%s]",
			ec.value(),
			readSize,
			ec.message().data());
	}
}

void ServerLinker::closeSocket()
{
	if(!m_pSocket)
	{
		LOG_PROXYSERVER.printLog("socket close error!");
		return;
	}
	m_pSocket->close();
}

void ServerLinker::onAyncRead(
	const CommonBoost::ErrorCode& ec,
	uint readSize
)
{
	if(ec)
	{
		sigError(shared_from_this(), ec);
		return;
	}
	if(readSize <= 0 || readSize > MsgBuffer::g_nReadBufferSize)
	{
		LOG_PROXYSERVER.printLog("size error,readSize[%d],g_nReadBufferSize[%d]", readSize, MsgBuffer::g_nReadBufferSize);
		return;
	}

	m_msgHeader.reset();
	m_nHasReadDataSize = 0;

	while(m_nHasReadDataSize < readSize)
	{
		memset(m_bytesOnceMsg, 0, sizeof(m_bytesOnceMsg));

		// 分析协议
		m_msgHeader = *(MsgHeader*)(m_bytesReadBuffer + m_nHasReadDataSize);

		// 一条协议最大长度判断
		if(m_msgHeader.m_nMsgLen > MsgBuffer::g_nOnceMsgSize ||
			m_msgHeader.m_nMsgLen <= 0)
		{
			LOG_PROXYSERVER.printLog("msgtype[%d] size[%d] error, read buff[%s]",
				m_msgHeader.m_nMsgType,
				m_msgHeader.m_nMsgLen,
				m_bytesReadBuffer);
			break;
		}
		memmove(m_bytesOnceMsg, m_bytesReadBuffer + m_nHasReadDataSize, m_msgHeader.m_nMsgLen);

		if(m_msgHeader.m_nProxyer != MsgHeader::F_PROXYSERVER)
		{
			LOG_PROXYSERVER.printLog("m_msgHeader.m_nProxyer != MsgHeader::F_PROXYSERVER");
			PROXY_SERVER_READ_MSG_CONTINUE;
		}

		// 发送方验证
		if (m_msgHeader.m_nSender < MsgHeader::F_DEFAULT ||
			m_msgHeader.m_nSender >= MsgHeader::F_MAX)
		{
			LOG_PROXYSERVER.printLog("m_msgHeader.m_nSender error");
			PROXY_SERVER_READ_MSG_CONTINUE;
		}

		// 接收方验证
		if(m_msgHeader.m_nReceiver < MsgHeader::F_DEFAULT ||
			m_msgHeader.m_nReceiver >= MsgHeader::F_MAX)
		{
			LOG_PROXYSERVER.printLog("m_msgHeader.m_nReceiver error");
			PROXY_SERVER_READ_MSG_CONTINUE;
		}

		// 和代理服通信的协议
		if(m_msgHeader.m_nReceiver == MsgHeader::F_PROXYSERVER)
		{
			ProxyServerMsgHandler::callHandler(
				m_msgHeader.m_nMsgType,
				shared_from_this(),
				m_bytesOnceMsg + sizeof(MsgHeader),
				m_msgHeader.m_nMsgLen - sizeof(MsgHeader));
			PROXY_SERVER_READ_MSG_CONTINUE;
		}

		// TODO 转发到指定服务器
		//ayncSend(m_bytesOnceMsg, m_msgHeader.m_nMsgLen);
		printf("get inner server msg: %s\n", m_bytesOnceMsg);
		//ayncSend((byte*)"123456789", 9);

		m_nHasReadDataSize += m_msgHeader.m_nMsgLen;
	}

	

	ayncRead();
}
