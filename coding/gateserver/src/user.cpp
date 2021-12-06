#include "user.h"

#include <logmodule/logdef.h>
#include <boost/asio/socket_base.hpp>

User::User(CommonBoost::IOServer& ioserver)
{
	memset(m_readBuffer, 0, sizeof(m_readBuffer));
	m_pSocket = boost::make_shared<CommonBoost::Socket>(ioserver);
	assert(m_pSocket != NULL);

	CommonBoost::ErrorCode ec;
	boost::asio::ip::tcp::no_delay no_delay(true);
	m_pSocket->set_option(no_delay, ec);
	m_pSocket->set_option(boost::asio::socket_base::linger(true, 0), ec);
	m_pSocket->set_option(boost::asio::socket_base::reuse_address(true), ec);

}

User::~User()
{
	m_pSocket->close();
}

void User::ayncRead()
{
	if (!m_pSocket)
	{
		LOG_GATESERVER.printLog("m_pSocket == NULL");
		return;
	}

	memset(m_readBuffer, 0, sizeof(m_readBuffer));
	m_pSocket->async_read_some(
		MSG_BUFFER(m_readBuffer, sizeof(m_readBuffer)),
		BIND(&User::onAyncRead, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2)
		);
}

void User::ayncSend(const char * str, unsigned int size)
{
	m_pSocket->async_write_some(
		MSG_BUFFER(str, size),
		BIND(&User::onAyncSend, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2)
	);
}

void User::onAyncRead(
	const CommonBoost::ErrorCode& ec,
	unsigned int readSize
	)
{
	if (ec)
	{
		LOG_GATESERVER.printLog("error value[%d], message[%s]",ec.value(), ec.message().data());
		m_pSocket->close();
		return;
	}
	if (readSize <= 0 || readSize > UserBuffer::g_nReadBufferSize)
	{
		LOG_GATESERVER.printLog("size error,readSize[%d],g_nReadBufferSize[%d]", readSize, UserBuffer::g_nReadBufferSize);
		return;
	}
	
	LOG_GATESERVER.printLog("read msg[%s]", m_readBuffer);

	std::string str = "hello im server";
	ayncSend(str.data(),str.size());

	ayncRead();
}


CommonBoost::SocketPtr & User::getSocket()
{
	return m_pSocket;
}

void User::onAyncSend(const CommonBoost::ErrorCode & ec, unsigned int readSize)
{
	if (ec)
	{
		LOG_GATESERVER.printLog("error value[%d],send size[%d], message[%s]",
			ec.value(),
			readSize,
			ec.message().data());
	}
}
