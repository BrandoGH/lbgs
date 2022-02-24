#include "serverlinker.h"
#include "proxyserver.h"

#include <servercommon/logmodule/logdef.h>

ServerLinker::ServerLinker(CommonBoost::IOServer& ioserver)
{
	m_pSocket = boost::make_shared<CommonBoost::Socket>(ioserver);
	assert(m_pSocket != NULL);
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

	// TODO 协议解析
	// TODO 转发到指定服务器
	//ayncSend(m_bytesOnceMsg, m_msgHeader.m_nMsgLen);
	printf("get inner server msg: %s\n", m_bytesReadBuffer);
	ayncSend((byte*)"123456789", 9);

	ayncRead();
}
