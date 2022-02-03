#include "serverlinker.h"

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

	ayncRead();
}
