#include "user.h"
#include "gateserver.h"

#include <servercommon/commontool/msgtool/msgtool.h>
#include "boost/asio/bind_executor.hpp"
#include "boost/asio/basic_stream_socket.hpp"

User::User(CommonBoost::IOServer& ioserver)
	: m_nHasReadDataSize(0)
	, m_nSeq(0)
{
	memset(m_bytesReadBuffer, 0, sizeof(m_bytesReadBuffer));
	memset(m_bytesOnceMsg, 0, sizeof(m_bytesOnceMsg));
	m_pSocket = boost::make_shared<CommonBoost::Socket>(ioserver);
	m_pStrand = boost::make_shared<CommonBoost::Strand>(ioserver);
	assert(m_pSocket != NULL);
	assert(m_pStrand != NULL);
}

User::~User()
{
	closeSocket();
}

void User::ayncRead()
{
	if (!m_pSocket || !m_pStrand)
	{
		LOG_GATESERVER.printLog("m_pSocket == NULL || m_pStrand == NULL");
		return;
	}

	memset(m_bytesReadBuffer, 0, sizeof(m_bytesReadBuffer));
	m_pSocket->async_read_some(
		MSG_BUFFER(m_bytesReadBuffer, sizeof(m_bytesReadBuffer)),
		m_pStrand->wrap(BIND(&User::onAyncRead, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2))
	);
}

void User::ayncSend(const byte* data, uint size)
{
	if (!m_pSocket || !m_pStrand)
	{
		LOG_GATESERVER.printLog("m_pSocket == NULL || m_pStrand == NULL");
		return;
	}

	m_pSocket->async_write_some(
		MSG_BUFFER(data, size),
		m_pStrand->wrap(BIND(&User::onAyncSend, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2))
	);
}

void User::onAyncRead(
	const CommonBoost::ErrorCode& ec,
	uint readSize
	)
{
	if (ec)
	{
		sigError(shared_from_this(), ec);
		return;
	}
	if (readSize <= 0 || readSize > MsgBuffer::g_nReadBufferSize)
	{
		LOG_GATESERVER.printLog("size error,readSize[%d],g_nReadBufferSize[%d]", readSize, MsgBuffer::g_nReadBufferSize);
		return;
	}

	m_msgHeader.reset();
	m_msgEnder.reset();
	m_nHasReadDataSize = 0;


	/*
		Only sticky packets are processed here, and half packets are controlled by the client. 
		For example, the client starts the timer after requesting the protocol. If there is no response for a period of time, it will re-request.
		Then you can also specify the maximum number of requests, which is determined by the client
	*/
	while(m_nHasReadDataSize < readSize)
	{
		// Analysis Protocol
		m_msgHeader = *(MsgHeader*)(m_bytesReadBuffer + m_nHasReadDataSize);

		// Judgment of the maximum length of a protocol
		if(m_msgHeader.m_nMsgLen > MsgBuffer::g_nOnceMsgSize ||
			m_msgHeader.m_nMsgLen <= 0)		
		{
			LOG_GATESERVER.printLog("msgtype[%d] size[%d] error, read buff[%s]",
				m_msgHeader.m_nMsgType, 
				m_msgHeader.m_nMsgLen, 
				m_bytesReadBuffer);
			m_nHasReadDataSize++;
			continue;
		}

		if (m_msgHeader.m_nMsgLen <= (sizeof(MsgHeader) + sizeof(MsgEnder)))
		{
			// There is a high possibility that the protocol format of the client and the server is inconsistent.
			LOG_GATESERVER.printLog("m_msgHeader.m_nMsgLen[%d] <= (sizeof(MsgHeader) + sizeof(MsgEnder)%d)", 
				m_msgHeader.m_nMsgLen, sizeof(MsgHeader) + sizeof(MsgEnder));
			break;
		}

		memmove(m_bytesOnceMsg, m_bytesReadBuffer + m_nHasReadDataSize, m_msgHeader.m_nMsgLen);
		ushort userDataSize = m_msgHeader.m_nMsgLen - sizeof(MsgHeader) - sizeof(MsgEnder);
		m_msgEnder = *(MsgEnder*)(m_bytesOnceMsg + sizeof(MsgHeader) + userDataSize);

		// MD5 check protocol
		DEFINE_BYTE_ARRAY(md5,16);
		CommonTool::MsgTool::data2Md5(m_bytesOnceMsg, sizeof(MsgHeader) + userDataSize, md5);
		if(!CommonTool::MsgTool::isBytesMd5EQ(md5, m_msgEnder.m_bytesMD5) || 
			(m_msgHeader.m_nMsgType < MSG_TYPE_HEART_CS || 
				m_msgHeader.m_nMsgType >= MSG_CODE_MAX))
		{
			// drop this msg
			LOG_GATESERVER.printLog("msgtype[%d] md5 not eq or msgtype error", m_msgHeader.m_nMsgType);
			m_nHasReadDataSize += m_msgHeader.m_nMsgLen;
			continue;
		}

		// Reassemble the message : MsgHeader + m_bytesOnceMsg
		// Need not md5 check
		forwardToProxy(m_bytesOnceMsg, sizeof(MsgHeader) + userDataSize);

		m_nHasReadDataSize += m_msgHeader.m_nMsgLen;
	}

	ayncRead();
}


CommonBoost::SocketPtr & User::getSocket()
{
	return m_pSocket;
}

void User::getLinkIP(std::string& outIp)
{
	if (m_pSocket)
	{
		outIp = m_pSocket->remote_endpoint().address().to_string();
	}
}

void User::getLinkPort(ushort& outPort)
{
	if (m_pSocket)
	{
		outPort = m_pSocket->remote_endpoint().port();
	}
}

void User::closeSocket()
{
	if (!m_pSocket)
	{
		LOG_GATESERVER.printLog("socket close error!");
		return;
	}
	m_pSocket->close();
}

int User::slotConnect(GateServer* gateServer)
{
	if(!gateServer)
	{
		LOG_GATESERVER.printLog("gateServer == NULL, connect slot error!");
		return CONNECT_ERROR;
	}

	sigError.connect(BIND(
		&GateServer::onUserError,
		gateServer,
		boost::placeholders::_1,
		boost::placeholders::_2));

	sigSendDataToProxy.connect(BIND(
		&GateServer::OnSendToProxySrvByUser,
		gateServer,
		boost::placeholders::_1,
		boost::placeholders::_2,
		boost::placeholders::_3));

	return CONNECT_OK;
}

void User::setSeq(int seq)
{
	m_nSeq = seq;
}

int User::getSeq()
{
	return m_nSeq;
}

void User::onAyncSend(const CommonBoost::ErrorCode & ec, uint readSize)
{
	if (ec)
	{
		LOG_GATESERVER.printLog("error value[%d],send size[%d], message[%s]",
			ec.value(),
			readSize,
			ec.message().data());
	}
}

void User::forwardToProxy(const byte* readOnceMsg, uint msgSize)
{
	MsgHeader* header = (MsgHeader*)readOnceMsg;
	if (!header)
	{
		return;
	}

	header->m_nMsgLen = msgSize;
	header->m_nSender = MsgHeader::F_GATESERVER;
	header->m_nReceiver = MsgHeader::F_LOGICSERVER;
	header->m_nProxyer = MsgHeader::F_PROXYSERVER;
	header->m_nClientSrcSeq = getSeq();


	sigSendDataToProxy(readOnceMsg, msgSize, getSeq());
}
