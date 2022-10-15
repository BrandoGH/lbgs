#include "user.h"
#include "gateserver.h"

#include <servercommon/commontool/msgtool/msgtool.h>
#include <servercommon/msgmodule/msgcommondef.h>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/basic_stream_socket.hpp>
#include <boost/system/errc.hpp>
#include <boost/asio/socket_base.hpp>

#include <logicserver/communicationmsg/msglogout.h>

namespace
{
/*
* more than the heart time
* Disconnect if there is no message communication within the specified time
*/
const int g_nTimingCheckUserMillisec = 1000 * 120;

const CString64 g_nGateToClientErrorMsg = "Error Msg (From GateServer)";
}

User::User(CommonBoost::IOServer& ioserver, CommonBoost::IOServer& timerServe)
	: m_nHasReadDataSize(0)
	, m_nSeq(0)
	, m_ioserver(ioserver)
	, m_timerServer(timerServe)
	, m_bUserValid(false)
	, m_bHasSendError(false)
{
	memset(m_bytesReadBuffer, 0, sizeof(m_bytesReadBuffer));
	memset(m_bytesOnceMsg, 0, sizeof(m_bytesOnceMsg));
	m_pSocket = boost::make_shared<CommonBoost::Socket>(ioserver);
	m_pStrand = boost::make_shared<CommonBoost::Strand>(ioserver);
	m_pStrandTimer = boost::make_shared<CommonBoost::Strand>(timerServe);
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
	if (m_pSocket->is_open())
	{
		m_pSocket->async_read_some(
			MSG_BUFFER(m_bytesReadBuffer, sizeof(m_bytesReadBuffer)),
			m_pStrand->wrap(BIND(&User::onAyncRead, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2))
		);
	}
}

void User::ayncSend(const byte* data, uint size)
{
	if (!m_pSocket || !m_pStrand)
	{
		LOG_GATESERVER.printLog("m_pSocket == NULL || m_pStrand == NULL");
		return;
	}
	if (m_pSocket->is_open())
	{
		m_pSocket->async_write_some(
			MSG_BUFFER(data, size),
			m_pStrand->wrap(BIND(&User::onAyncSend, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2))
		);
	}
	
}

void User::onAyncRead(
	const CommonBoost::ErrorCode& ec,
	uint readSize
	)
{
	if (!m_pSocket || !m_pSocket->is_open())
	{
		LOG_GATESERVER.printLog("!m_pSocket || !m_pSocket->is_open()");
		return;
	}
	if (ec)
	{
		LOG_GATESERVER.printLog("error, will sendUserError, m_bHasSendError[%d]", m_bHasSendError.load());
		if (!m_bHasSendError)
		{
			LOG_GATESERVER.printLog("sendUserError");
			m_bHasSendError = sendUserError(ec);
		}
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
		THREAD_SLEEP(1);

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
			// ayncSend((const byte*)g_nGateToClientErrorMsg, strlen(g_nGateToClientErrorMsg)); // debug
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
			// ayncSend((const byte*)g_nGateToClientErrorMsg, strlen(g_nGateToClientErrorMsg));	 // debug
			continue;
		}

		// Reassemble the message : MsgHeader + m_bytesOnceMsg
		// Need not md5 check
		forwardToProxy(m_bytesOnceMsg, sizeof(MsgHeader) + userDataSize);

		m_nHasReadDataSize += m_msgHeader.m_nMsgLen;
	}

	ayncRead();
	checkUserValid();
}


CommonBoost::SocketPtr & User::getSocket()
{
	return m_pSocket;
}

void User::getLinkIP(std::string& outIp)
{
	if (m_pSocket && m_pSocket->is_open())
	{
		outIp = m_pSocket->remote_endpoint().address().to_string();
	}
}

void User::getLinkPort(ushort& outPort)
{
	if (m_pSocket && m_pSocket->is_open())
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
	if (m_pSocket->is_open())
	{
		m_pSocket->close();
	}

	LOG_GATESERVER.printLog("socket close ok! client seq[%lld]",getSeq());

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

void User::setSeq(ullong seq)
{
	m_nSeq = seq;
}

ullong User::getSeq()
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

		if (m_pSocket)
		{
			LOG_GATESERVER.printLog("send error, will shutdown send channel");
			m_pSocket->shutdown(boost::asio::socket_base::shutdown_send, const_cast<CommonBoost::ErrorCode&>(ec));
		}
	}
}

void User::onCheckUserValid()
{
	if (!m_pUesrCheckTimer || !m_pStrandTimer)
	{
		LOG_GATESERVER.printLog("!m_pUesrCheckTimer || !m_pStrandTimer");
		return;
	}
	if (m_bUserValid)
	{
		// printf_color(PRINTF_YELLOW, "%s (client seq=%lld): m_bUserValid = [%d]\n", __FUNCTION__,getSeq(), m_bUserValid.load());
		m_pUesrCheckTimer->expires_from_now(boost::posix_time::millisec(g_nTimingCheckUserMillisec));
		m_pUesrCheckTimer->async_wait(
			m_pStrandTimer->wrap(BIND(&User::onCheckUserValid, shared_from_this())));
		m_bUserValid = false;
	}
	else
	{
		// printf_color(PRINTF_YELLOW, "%s: No messaging, about to delete user, client seq[%lld]\n", __FUNCTION__, getSeq());
		LOG_GATESERVER.printLog("No messaging, about to delete user, client seq[%lld], will sendUserError, m_bHasSendError[%d]", getSeq(), m_bHasSendError.load());
		if (!m_bHasSendError)
		{
			LOG_GATESERVER.printLog("sendUserError");
			CommonBoost::ErrorCode ec = boost::system::errc::make_error_code(boost::system::errc::success);
			m_bHasSendError = sendUserError(ec);
		}
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

void User::sendLogoutProtocal(const CommonBoost::ErrorCode& ec)
{
	DEFINE_BYTE_ARRAY(sendData, sizeof(MsgHeader) + sizeof(MsgLogoutCS));

	MsgHeader header;
	header.m_nMsgType = MSG_TYPE_LOGOUT_CS;

	MsgLogoutCS cs;
	cs.m_nErrorCode = ec.value();

	memmove(sendData, (const char*)&header, sizeof(MsgHeader));
	memmove(sendData + sizeof(MsgHeader), (const char*)&cs, sizeof(MsgLogoutCS));

	forwardToProxy(sendData, sizeof(sendData));
}

bool User::sendUserError(const CommonBoost::ErrorCode& ec)
{
	sendLogoutProtocal(ec);
	sigError(shared_from_this(), ec);

	if (m_pSocket)
	{
		LOG_GATESERVER.printLog("will shutdown read channel");
		m_pSocket->shutdown(boost::asio::socket_base::shutdown_receive, const_cast<CommonBoost::ErrorCode&>(ec));
	}
	return true;
}

void User::checkUserValid()
{
	if (!m_pStrandTimer)
	{
		LOG_GATESERVER.printLog("m_pStrandTimer NULL");
		return;
	}
	if (!m_pUesrCheckTimer)
	{
		m_pUesrCheckTimer = boost::make_shared<CommonBoost::DeadlineTimer>(m_timerServer, boost::posix_time::millisec(g_nTimingCheckUserMillisec));
		m_pUesrCheckTimer->async_wait(
			m_pStrandTimer->wrap(BIND(&User::onCheckUserValid, shared_from_this())));
	}
	m_bUserValid = true;
}
