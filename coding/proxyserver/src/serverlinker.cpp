#include "serverlinker.h"
#include "proxyserver.h"

#include <servercommon/logmodule/logdef.h>
#include <servercommon/msgmodule/proxyservermsghandler.h>
#include "msgmodule/msgcommondef.h"
#include "commontool/msgtool/msgtool.h"

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
	m_pStrand = boost::make_shared<CommonBoost::Strand>(ioserver);
	assert(m_pSocket != NULL);
	assert(m_pStrand != NULL);
	memset(m_bytesOnceMsg, 0, sizeof(m_bytesOnceMsg));
}

ServerLinker::~ServerLinker()
{
}

CommonBoost::SocketPtr& ServerLinker::getSocket()
{
	return m_pSocket;
}

void ServerLinker::ayncRead(bool bFirstConnect)
{
	if(!m_pSocket || !m_pStrand)
	{
		LOG_PROXYSERVER.printLog("m_pSocket == NULL || m_pStrand == NULL");
		return;
	}

	memset(m_bytesReadBuffer, 0, sizeof(m_bytesReadBuffer));
	m_pSocket->async_read_some(
		MSG_BUFFER(m_bytesReadBuffer, sizeof(m_bytesReadBuffer)),
		m_pStrand->wrap(BIND(&ServerLinker::onAyncRead, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2, bFirstConnect))
	);
}

void ServerLinker::ayncSend(const byte* data, uint size)
{
	if(!m_pSocket || !m_pStrand)
	{
		LOG_PROXYSERVER.printLog("m_pSocket == NULL || m_pStrand == NULL");
		return;
	}

	m_pSocket->async_write_some(
		MSG_BUFFER(data, size),
		m_pStrand->wrap(BIND(&ServerLinker::onAyncSend, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2))
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
	sigFirstConnect.connect(BIND(
		&ProxyServer::onLinkerFirstConnect,
		proxyServer,
		boost::placeholders::_1,
		boost::placeholders::_2));
	sigSendToDstServer.connect(BIND(
		&ProxyServer::onSendToDstServer,
		proxyServer,
		boost::placeholders::_1,
		boost::placeholders::_2,
		boost::placeholders::_3));

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
	uint readSize,
	bool bFirstConnect
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

	// first link client send a byte, tell me his identity
	if (bFirstConnect)
	{
		if (readSize != 1)
		{
			LOG_PROXYSERVER.printLog("readSize != 1");
			return;
		}
		int listIndex = *(int *)m_bytesReadBuffer;
		std::string strLinkerServerInfo = CommonTool::MsgTool::getMsgHeaderFlagString(listIndex);
		LOG_PROXYSERVER.printLog("A inner server connected!! server type[%s]", strLinkerServerInfo.data());
		sigFirstConnect(shared_from_this(), listIndex);
		ayncRead();
		return;
	}

	m_msgHeader.reset();
	m_nHasReadDataSize = 0;

	while(m_nHasReadDataSize < readSize)
	{
		THREAD_SLEEP(1);
		memset(m_bytesOnceMsg, 0, sizeof(m_bytesOnceMsg));

		// Judgment of the maximum length of a protocol
		m_msgHeader = *(MsgHeader*)(m_bytesReadBuffer + m_nHasReadDataSize);

		// 一条协议最大长度判断
		if(m_msgHeader.m_nMsgLen > MsgBuffer::g_nOnceMsgSize ||
			m_msgHeader.m_nMsgLen <= 0)
		{
			LOG_PROXYSERVER.printLog("msgtype[%d] size[%d] error, read buff[%s]",
				m_msgHeader.m_nMsgType,
				m_msgHeader.m_nMsgLen,
				m_bytesReadBuffer);
			m_nHasReadDataSize++;
			continue;
		}
		memmove(m_bytesOnceMsg, m_bytesReadBuffer + m_nHasReadDataSize, m_msgHeader.m_nMsgLen);

		if(m_msgHeader.m_nProxyer != MsgHeader::F_PROXYSERVER)
		{
			LOG_PROXYSERVER.printLog("m_msgHeader.m_nProxyer != MsgHeader::F_PROXYSERVER");
			PROXY_SERVER_READ_MSG_CONTINUE;
		}

		// check sender
		if (m_msgHeader.m_nSender < MsgHeader::F_DEFAULT ||
			m_msgHeader.m_nSender >= MsgHeader::F_MAX)
		{
			LOG_PROXYSERVER.printLog("m_msgHeader.m_nSender error");
			PROXY_SERVER_READ_MSG_CONTINUE;
		}

		// check receiver
		if(m_msgHeader.m_nReceiver < MsgHeader::F_DEFAULT ||
			m_msgHeader.m_nReceiver >= MsgHeader::F_MAX)
		{
			LOG_PROXYSERVER.printLog("m_msgHeader.m_nReceiver error");
			PROXY_SERVER_READ_MSG_CONTINUE;
		}

		// if this msg is heart with proxy server

		if(m_msgHeader.m_nReceiver == MsgHeader::F_PROXYSERVER)
		{
			ProxyServerMsgHandler::callHandler(
				m_msgHeader.m_nMsgType,
				shared_from_this(),
				m_bytesOnceMsg + sizeof(MsgHeader),
				m_msgHeader.m_nMsgLen - sizeof(MsgHeader));
			PROXY_SERVER_READ_MSG_CONTINUE;
		}

		// forward to the target server
		sigSendToDstServer(m_msgHeader.m_nReceiver,
			m_bytesOnceMsg,
			m_msgHeader.m_nMsgLen);

		m_nHasReadDataSize += m_msgHeader.m_nMsgLen;
	}

	

	ayncRead();
}
