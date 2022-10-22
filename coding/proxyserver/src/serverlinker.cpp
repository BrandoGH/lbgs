#include "serverlinker.h"
#include "proxyserver.h"

#include <servercommon/logmodule/logdef.h>
#include <servercommon/msgmodule/proxyservermsghandler.h>
#include "msgmodule/msgcommondef.h"
#include "commontool/msgtool/msgtool.h"

namespace
{
	// Something...
}

#define DO_PROXYSERVER_MSG_CHECK_HEADER \
if (m_msgHeader.m_nSender < MsgHeader::F_DEFAULT ||\
m_msgHeader.m_nSender >= MsgHeader::F_MAX ||\
m_msgHeader.m_nReceiver < MsgHeader::F_DEFAULT ||\
	m_msgHeader.m_nReceiver >= MsgHeader::F_MAX ||\
	m_msgHeader.m_nProxyer != MsgHeader::F_PROXYSERVER)\
{\
	LOG_PROXYSERVER.printLog("m_msgHeader error");\
	m_bHeaderIntegrated = true;\
	m_nLastHasReadSize = 0;\
	m_nNextNeedReadSize = 0;\
	break;\
}

#define DO_PROXYSERVER_MSG_TO_DST_SERVER(callHandlerEndCode) \
if (m_msgHeader.m_nReceiver == MsgHeader::F_PROXYSERVER)\
{\
	ProxyServerMsgHandler::callHandler(\
		m_msgHeader.m_nMsgType,\
		shared_from_this(),\
		m_bytesOnceMsg + sizeof(MsgHeader),\
		m_msgHeader.m_nMsgLen - sizeof(MsgHeader));\
		callHandlerEndCode;\
}\
sigSendToDstServer(m_msgHeader.m_nReceiver,\
	m_bytesOnceMsg,\
	m_msgHeader.m_nMsgLen);


ServerLinker::ServerLinker(CommonBoost::IOServer& ioserver)
	: m_nHasReadDataSize(0)
	, m_nNextNeedReadSize(0)
	, m_nLastHasReadSize(0)
	, m_bHeaderIntegrated(true)
{
	m_pSocket = boost::make_shared<CommonBoost::Socket>(ioserver);
	m_pStrand = boost::make_shared<CommonBoost::Strand>(ioserver);
	assert(m_pSocket != NULL);
	assert(m_pStrand != NULL);
	memset(m_bytesOnceMsg, 0, sizeof(m_bytesOnceMsg));
}

ServerLinker::~ServerLinker()
{
	closeSocket();
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

		if (m_pSocket)
		{
			LOG_PROXYSERVER.printLog("error, socket send channel will close!");
			m_pSocket->shutdown(boost::asio::socket_base::shutdown_send, const_cast<CommonBoost::ErrorCode&>(ec));
		}
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
		if (m_pSocket)
		{
			LOG_PROXYSERVER.printLog("error, socket read channel will close!");
			m_pSocket->shutdown(boost::asio::socket_base::shutdown_receive, const_cast<CommonBoost::ErrorCode&>(ec));
		}
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

	int remainSize = 0;
	int remainBodySize = 0;

	while(m_nHasReadDataSize < readSize)
	{
		THREAD_SLEEP(1);

		if (m_nLastHasReadSize > 0 && m_nNextNeedReadSize > 0)
		{
			memmove(m_bytesOnceMsg + m_nLastHasReadSize, m_bytesReadBuffer, m_nNextNeedReadSize);
			m_msgHeader = *(MsgHeader*)(m_bytesOnceMsg + m_nHasReadDataSize);
			DO_PROXYSERVER_MSG_CHECK_HEADER;

			if (m_bHeaderIntegrated)
			{
				DO_PROXYSERVER_MSG_TO_DST_SERVER(
					m_nHasReadDataSize += m_nNextNeedReadSize;
					m_bHeaderIntegrated = true;
					m_nLastHasReadSize = 0;
					m_nNextNeedReadSize = 0;
					continue;
				);

				m_nHasReadDataSize += m_nNextNeedReadSize;
			} 
			else
			{
				remainBodySize = readSize - m_nNextNeedReadSize;
				if (remainBodySize >= m_msgHeader.m_nMsgLen - sizeof(MsgHeader))
				{
					memmove(m_bytesOnceMsg + sizeof(MsgHeader), m_bytesReadBuffer + m_nNextNeedReadSize, m_msgHeader.m_nMsgLen - sizeof(MsgHeader));

					DO_PROXYSERVER_MSG_TO_DST_SERVER(
						m_nHasReadDataSize += (m_nNextNeedReadSize + m_msgHeader.m_nMsgLen - sizeof(MsgHeader));
						m_bHeaderIntegrated = true;
						m_nLastHasReadSize = 0;
						m_nNextNeedReadSize = 0;
						continue;
					);

					m_nHasReadDataSize += (m_nNextNeedReadSize + m_msgHeader.m_nMsgLen - sizeof(MsgHeader));
				}
			}

			m_bHeaderIntegrated = true;
			m_nLastHasReadSize = 0;
			m_nNextNeedReadSize = 0;
			continue;
		}

		remainSize = readSize - m_nHasReadDataSize;
		if (remainSize >= sizeof(MsgHeader))
		{
			m_msgHeader = *(MsgHeader*)(m_bytesReadBuffer + m_nHasReadDataSize);
			DO_PROXYSERVER_MSG_CHECK_HEADER;

			remainBodySize = remainSize - sizeof(MsgHeader);
			if (remainBodySize >= m_msgHeader.m_nMsgLen - sizeof(MsgHeader))
			{
				memmove(m_bytesOnceMsg, m_bytesReadBuffer + m_nHasReadDataSize, m_msgHeader.m_nMsgLen);

				DO_PROXYSERVER_MSG_TO_DST_SERVER(
					m_nHasReadDataSize += m_msgHeader.m_nMsgLen;
					continue;
				);

				m_nHasReadDataSize += m_msgHeader.m_nMsgLen;
				continue;
			}
			m_nLastHasReadSize = remainSize;
			m_nNextNeedReadSize = (m_msgHeader.m_nMsgLen - remainBodySize - sizeof(MsgHeader));
			memmove(m_bytesOnceMsg, m_bytesReadBuffer + m_nHasReadDataSize, remainSize);
			m_nHasReadDataSize += remainSize;
			continue;
		}
		m_bHeaderIntegrated = false;
		m_nLastHasReadSize = remainSize;
		m_nNextNeedReadSize = (sizeof(MsgHeader) - m_nLastHasReadSize);
		memmove(m_bytesOnceMsg, m_bytesReadBuffer + m_nHasReadDataSize, remainSize);
		m_nHasReadDataSize += remainSize;

	}

	ayncRead();
}
